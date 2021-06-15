#include <iostream>
#include <ff/ff.hpp>
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <ff/parallel_for.hpp>
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

void writeFile(KnnType& knn,std::string filename){
    std::ofstream outFile;
    outFile.open(filename);

    for (auto const& x : knn){
        outFile << x.first << ": ";
        auto neighbours = x.second;
        for(int i = 0; i < neighbours.size(); i++){
            outFile << neighbours[i].second << " " ;
        }
        outFile << std::endl;
    }
    outFile.close();
}

static std::vector<pi> computeKnn(const int i, const int k, pointsType& points){
    std::vector<pi> neighbour(k,std::make_pair(DBL_MAX,0));
    double max = DBL_MAX;
    int index_max = 0;
    for(int j = 0; j < points.size(); j++){
        if(i != j){
            double distance = computeDistance(points.at(i), points.at(j));
            pi newPoint = std::make_pair(distance,j);
            if(neighbour.size() >= k){
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
    return neighbour;
}

int main(int argc, char *argv[]){
    if (argc<5) {
        std::cerr << "use: " << argv[0]  << " k nw inFilename outFilename";
        return -1;
    }
    int k = std::atoi(argv[1]);
    int number_of_workers = std::atoi(argv[2]);
    std::string inFilename= argv[3];
    std::string outFilename= argv[4];
    ffTime(START_TIME);
    std::vector<std::pair<double,double>> points;
    KnnType knn;
    ParallelForReduce<std::vector<KnnType>>* pf;
    pf = new ParallelForReduce<std::vector<KnnType>>(number_of_workers);
    // ParallelFor pf(number_of_workers);
    readFile(points, inFilename);
    std::vector<KnnType> res;
    auto compute = [&](const int i, std::vector<KnnType>& knn) {
            auto res = computeKnn(i,k,points);
            knn[0].insert(std::pair<int,std::vector<pi>>(i, res));
        };  
    auto reduce = [](std::vector<KnnType>& result, const std::vector<KnnType>& knn) {
            result.push_back(knn[0]);
        };
    pf->parallel_reduce(res, std::vector<KnnType>(1), 0, points.size(), compute, reduce, number_of_workers);
    writeFile(res,outFilename);
    ffTime(STOP_TIME);
    std::cout << ffTime(GET_TIME) << "\n";
    return 0;
}
