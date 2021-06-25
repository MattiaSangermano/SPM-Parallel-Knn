#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
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

int main(int argc, char **argv) {
    if(argc != 5){
        std::cout << "Wrong input format!" << std::endl;
        std::cout << "Usage: " << argv[0] << " K nw inputFile outputFile" << std::endl;
        exit(-1);
    }

    int k = std::atoi(argv[1]);
    int nw = std::atoi(argv[2]);
    std::string filename = argv[3];
    std::string outFilename = argv[4];
    
    std::ofstream outputFile;
    double distance;

    auto t_start = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<double,double>> points;
    std::vector<std::future<KnnType*>> futures;

    readFile(points,filename);
    
    int taskLength = (int) (points.size() / nw);
    int remainingPoints = (points.size() % nw);

    int start = 0;
    int end = remainingPoints > 0 ? taskLength + 1 : taskLength;
    remainingPoints --;

    for(int i = 0; i < nw - 1; i++) {
        Task* t = new Task(start,end,k,&points);
        start = end;
        end = remainingPoints > 0 ? end + taskLength + 1 : end + taskLength;
        remainingPoints --;
        futures.push_back(
                std::async(std::launch::async, computeKNN, t)
        );
    }
    Task* t = new Task(start,end,k,&points);
    KnnType* my_knns;
    my_knns = computeKNN(t);

    std::vector<KnnType*> workers_knns;
    for (int j = 0; j < nw - 1; j++) {
        workers_knns.push_back(futures.at(j).get());
    }
    workers_knns.push_back(my_knns);

    writeFile(workers_knns,outFilename);
    
    auto t_end = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout << elapsed_time_ms << std::endl;

    return 0;
}