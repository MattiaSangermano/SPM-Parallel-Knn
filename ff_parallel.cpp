#include <iostream>
#include <ff/ff.hpp>
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <math.h>
#include <chrono>
#include <float.h>
#include "utils.h"

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
           std::string outFilename, const int k):
    number_of_workers(number_of_workers),inFilename(inFilename),outFilename(outFilename),k(k){
        //all_primes = new std::vector<unsigned long long>();
        //results = new std::vector<unsigned long long>();
    }
    ~master() {
        //delete all_primes;
        //delete results;
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
};

struct worker: ff_node_t<my_task,KnnType> {
KnnType* svc(my_task * task) {
    KnnType* knn = new KnnType();
    KnnType res = computeKnn(task);
    *knn = res;
    delete task;
    return knn;
    }
};

int main(int argc, char *argv[]){
    if (argc<5) {
        std::cerr << "use: " << argv[0]  << " k nw inFilename outFilename";
        return -1;
    }
    int k = std::atoi(argv[1]);
    int number_of_workers = std::atoi(argv[2]);
    std::string inFilename= argv[3];
    std::string outFilename= argv[4];


    master emitter(number_of_workers,inFilename,outFilename,k);
    std::vector<std::unique_ptr<ff_node> > W;
    for(int i=0;i<number_of_workers-1;++i) W.push_back(make_unique<worker>());

    ff_Farm<float> farm(std::move(W),emitter);
    farm.wrap_around();

    ffTime(START_TIME);
    if (farm.run_and_wait_end()<0) {
        error("running farm");
        return -1;
    }
    ffTime(STOP_TIME);
    std::cout << ffTime(GET_TIME) << "\n";
    return 0;

}
