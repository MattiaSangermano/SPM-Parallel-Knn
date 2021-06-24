#include <iostream>
#include <ff/ff.hpp>
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <math.h>
#include <chrono>
#include <float.h>
#include "utils.h"
#include <string.h>

using namespace ff;

typedef std::pair<double, int>  pi;
typedef std::map<int, std::vector<pi>> KnnType;
typedef std::vector<std::pair<double,double>> pointsType;

struct my_task {
    int start;
    int end;
    int k;
    std::vector<std::pair<double,double>>* points;
};
static int get_thread_id(int nw, int i){
    int th_id = -1;
    if (nw + 1 >= 64){
            int mod = 1;
            int decr = 0;
            if (nw + 1 <= 128){
                mod = 2;
            }else if (nw + 1 <= 192){
                mod = 3;
            }else if(nw + 1 <= 256){
                mod = 4;
            }
            int row_lim = (nw + 1) % 64;
            int col_lim = mod - 2;
            int i_lim = (row_lim * mod) + col_lim + 1;
            if (i >= i_lim){
                i = i - i_lim;
                th_id = (i % (mod - 1)) * 64 + (int)(i / (mod - 1)) + row_lim + 1;
            }else{
                th_id = (i % mod) * 64 + (int)(i / mod);
            }
        }else{
            th_id = i;
        }
    return th_id;
}
static KnnType computeKnn(my_task* t){
    KnnType knn;
    for(int i = t->start; i < t->end; i++){
        std::vector<pi> neighbour(t->k,std::make_pair(DBL_MAX,0));
        double max = DBL_MAX;
        int index_max = 0;
        for(int j = 0; j < t->points->size(); j++){
            if(i != j){
                double distance = computeDistance(t->points->at(i),t->points->at(j));
                pi newPoint = std::make_pair(distance,j);
                if(neighbour.size() >= t->k){
                    if(distance < max){
                        neighbour[index_max] = newPoint;
                        std::pair<int,double> max_point = findMax(neighbour);
                        max = max_point.second;
                            index_max = max_point.first;
                    }
                }
            }
        }
        std::sort(neighbour.begin(),neighbour.end());
        knn[i] = neighbour;
    }
    return knn;
}
struct master: ff_monode_t<KnnType,my_task> {
    master(const int number_of_workers, const std::string inFilename,
           std::string outFilename, const int k,const bool pinning):
    number_of_workers(number_of_workers),inFilename(inFilename),outFilename(outFilename),k(k),pinning(pinning){
        th_id = get_thread_id(number_of_workers - 1, number_of_workers - 1);
    }

    int svc_init(){
        if (pinning) {
            if (ff_mapThreadToCpu(th_id)<0)
                error("mapping Emitter");
            printf("I'm the Emitter running on the core %d ---- > %ld\n",th_id, ff_getMyCore());
        }
    return 0;
    }

    my_task* svc(KnnType *single_knn) {
        if (single_knn == nullptr) {

            readFile(points,inFilename);

            int taskLength = (int) (points.size() / number_of_workers);
            int remainingPoints = (points.size() % number_of_workers);

            int start = 0;
            int end = remainingPoints > 0 ? taskLength + 1 : taskLength;
            remainingPoints --;

            for(int i = 0; i < number_of_workers - 1; i++) {
                my_task* t = new my_task();
                t->k = k;
                t->start = start;
                t->end = end;
                t->points = &points;
                start = end;
                end = remainingPoints > 0 ? end + taskLength + 1 : end + taskLength;
                remainingPoints --;
                ff_send_out(t);
            }
            my_task* t = new my_task();;
            t->k = k;
            t->start = start;
            t->end = end;
            t->points = &points;
            KnnType out;
            out = computeKnn(t);
            results.push_back(out);
            return GO_ON;
        }
        results.push_back(*single_knn);
        delete single_knn;
        if (++task_received == (number_of_workers - 1)) return EOS;
        return GO_ON;
    }
    void svc_end() {
        writeFile(results, outFilename);
    }

    std::vector<std::pair<double,double>> points;
    const std::string inFilename;
    const std::string outFilename;
    std::vector<KnnType> results;
    const int number_of_workers;
    int task_received = 0 ;
    const int k;
    const bool pinning;
    int th_id;
};

struct worker: ff_node_t<my_task,KnnType> {
    worker(const int i, const int nw, const bool pinning):
    pinning(pinning){
        th_id = get_thread_id(nw,i);
    }

    int svc_init(){
        if (pinning){
            if (ff_mapThreadToCpu(th_id)<0)
                error("mapping Emitter");
        }
        printf("I'm Worker%ld running on the core %d ----- > %ld\n", get_my_id(), th_id, ff_getMyCore());
        return 0;
    }
    KnnType* svc(my_task * task) {
        KnnType* knn = new KnnType();
        KnnType res = computeKnn(task);
        *knn = res;
        delete task;
        return knn;
    }

    int th_id;
    const bool pinning;
};

int main(int argc, char *argv[]){
    if (argc<5 || argc > 6) {
        std::cerr << "use: " << argv[0]  << " k nw inFilename outFilename [y|n]";
        return -1;
    }
    int k = std::atoi(argv[1]);
    int number_of_workers = std::atoi(argv[2]);
    std::string inFilename= argv[3];
    std::string outFilename= argv[4];
    
    bool pinning = false;
    if(argc == 6){
        pinning = std::strncmp(argv[5],"y",1) == 0;
    }

    master emitter(number_of_workers,inFilename,outFilename,k,pinning);
    std::vector<std::unique_ptr<ff_node> > W;
    for(int i=0;i<number_of_workers-1;++i) W.push_back(make_unique<worker>(i,number_of_workers - 1, pinning));

    ff_Farm<float> farm(std::move(W),emitter);
    farm.remove_collector();
    if(pinning && number_of_workers > 64){
        farm.no_mapping();
    }
    farm.wrap_around();

    auto t_start = std::chrono::high_resolution_clock::now();
    if (farm.run_and_wait_end()<0) {
        error("running farm");
        return -1;
    }
    auto t_end = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout << elapsed_time_ms << std::endl;
    return 0;
}
