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

void printVector(std::vector<std::pair<double,double>> vec){
    for(int i = 0; i < vec.size(); i++){
            // std::cout << std::setprecision(10)<< "x: " < < vec[i].first << " y: " << vec[i].second << std::endl;
        }
}
bool internal2(int& index, double& max, double val, int i){
    max = val;
    index = i;
    return true;
}
std::pair<int,double> findMax(std::vector< pi >& vec){

    double max = vec[0].first;
    int index = 0;
    for(int i = 1; i < vec.size(); i++){
        vec[i].first > max && internal2(index, max, vec[i].first, i);
    }
    return std::make_pair(index,max);
}
bool internal(std::vector<pi>& neighbour, double& max, int& index_max,double distance, int j){
    pi newPoint = std::make_pair(distance,j);
    neighbour[index_max] = newPoint;
    std::pair<int,double> max_point = findMax(neighbour);
    max = max_point.second;
    index_max = max_point.first;
    return true;
}
int main(int argc, char **argv) {
    if(argc <= 1){
        std::cout << "Wrong input format. filename" << std::endl;
        std::exit(0);
    }
    std::ifstream inputFile;
    double distance;
    int k = std::atoi(argv[1]);
    std::string inFilename = argv[2];
    std::string outFilename = argv[3];
    auto t_start = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<double,double>> points;
    readFile(points,inFilename);
    std::map<int, std::vector<pi> > knn;
    for(int i = 0; i < points.size(); i++){
        std::vector<pi> neighbour(k,std::make_pair(DBL_MAX,0));
        double max = DBL_MAX;
        int index_max = 0;
        for(int j = 0; j < points.size(); j++){
            distance = std::sqrt(std::pow((points[i].first - points[j].first),2) + std::pow((points[i].second - points[j].second),2));
            i != j && neighbour.size() >= k && (distance < max) && internal(neighbour, max, index_max, distance, j);
        }
        std::sort(neighbour.begin(),neighbour.end());
        knn[i] = neighbour;
    }

    writeFile(knn,outFilename);
    auto t_end = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout << elapsed_time_ms << std::endl;

    return 0;
}