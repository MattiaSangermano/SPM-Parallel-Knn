#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <queue>
#include <math.h> 
#include <map>
#include <algorithm>
#include <future>
#include <chrono>
#include <thread>
#include <iomanip>
#include <float.h>
#include "utils.h"
typedef std::pair<double, int>  pi;
typedef std::map<int, std::vector<pi>> KnnType;
typedef std::vector<std::pair<double,double>> pointsType;
struct task {
    int unsigned start;
    int unsigned end;
    int unsigned k;
    std::vector<std::pair<double,double>>* points;
};

void printKnn(KnnType& knn){
    for (auto const& x : knn){
        std::cout << x.first << ": " << std::endl;
        auto neighbours = x.second;
        for(int i = 0; i < neighbours.size(); i++){
            std::cout << neighbours[i].second << " :" << neighbours[i].first << ", ";
        }
        std::cout << std::endl;
    }
}

KnnType computeKNN(task t){
    KnnType knn;
    for(int i = t.start; i < t.end; i++){
        std::vector<pi> neighbour(t.k,std::make_pair(DBL_MAX,0));
        double max = DBL_MAX;
        int index_max = 0;
        for(int j = 0; j < t.points->size(); j++){
            if(i != j){
                double distance = computeDistance(t.points->at(i),t.points->at(j));
                pi newPoint = std::make_pair(distance,j);
                if(neighbour.size() >= t.k){
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
void printVector(std::vector<std::pair<double,double>> vec){
    for(int i = 0; i < vec.size(); i++){
            std::cout << std::setprecision(10)<< "x: " << vec[i].first << " y: " << vec[i].second << std::endl;
        }
}

int main(int argc, char **argv) {
    if(argc <= 1){
        std::cout << "Wrong input format. filename" << std::endl;
        std::exit(0);
    }
    std::ofstream outputFile;
    double distance;
    int k = std::atoi(argv[1]);
    int nw = std::atoi(argv[2]);
    std::string filename = argv[3];
    std::string outFilename = argv[4];
    auto t_start = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<double,double>> points;
    
    readFile(points,filename);
    
    int taskLength = (int) (points.size() / nw);
    int remainingPoints = (points.size() % nw);

    std::vector<std::future<KnnType>> futures;
    int start = 0;
    int end = remainingPoints > 0 ? taskLength + 1 : taskLength;
    remainingPoints --;

    for(int i = 0; i < nw - 1; i++) {
        task t;
        t.k = k;
        t.start = start;
        t.end = end;
        t.points = &points;
        start = end;
        end = remainingPoints > 0 ? end + taskLength + 1 : end + taskLength;
        remainingPoints --;
        futures.push_back(
                std::async(std::launch::async, computeKNN, t)
        );
    }
    task t;
    t.k = k;
    t.start = start;
    t.end = end;
    t.points = &points;
    KnnType out;
    out = computeKNN(t);

    std::vector<KnnType> all_out;
    for (int j = 0; j < nw - 1; j++) {
        all_out.push_back(futures.at(j).get());
    }
    all_out.push_back(out);

    writeFile(all_out,outFilename);
    
    auto t_end = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout << elapsed_time_ms << std::endl;

    return 0;
}