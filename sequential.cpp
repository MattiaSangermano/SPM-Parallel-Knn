#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <queue>
#include <math.h> 
#include <float.h>
#include <map>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include "utils.h"
typedef std::pair<double, int>  pi;

void writeFile(std::map<int, std::vector<pi> >& knn,std::string filename){
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

int main(int argc, char **argv) {
    if(argc != 4){
        std::cout << "Wrong input format!" << std::endl;
        std::cout << "Usage: " << argv[0] << " K inputFile outputFile" << std::endl;
        exit(-1);
    }
    
    //reading input values

    int k = std::atoi(argv[1]);
    std::string inFilename = argv[2];
    std::string outFilename = argv[3];
    std::ifstream inputFile;
    double distance;


    auto t_start = std::chrono::high_resolution_clock::now();

    std::vector<std::pair<double,double>> points;
    std::map<int, std::vector<pi> > knn;

    readFile(points,inFilename);

    // KNN computetion
    for(int i = 0; i < points.size(); i++){
        std::vector<pi> neighbours(k,std::make_pair(DBL_MAX,0));
        double max = DBL_MAX;
        int index_max = 0;
        for(int j = 0; j < points.size(); j++){
            if(i != j){
                distance = computeDistance(points[i],points[j]);
                pi newPoint = std::make_pair(distance,j);
                if(neighbours.size() >= k){
                    if(distance < max){
                        neighbours[index_max] = newPoint;
                        std::pair<int,double> max_point = findMax(neighbours);
                        max = max_point.second;
                        index_max = max_point.first;
                    }
                }
            }
        }
        std::sort(neighbours.begin(),neighbours.end());
        knn[i] = neighbours;
    }

    writeFile(knn,outFilename);

    auto t_end = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout << elapsed_time_ms << std::endl;

    return 0;
}