#include "utils.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <tuple>
#include <cmath>

void readFile(pointsType& points,std::string filename){
    std::ifstream inputFile;
    inputFile.open(filename);
    std::string line, token;
    while(std::getline(inputFile, line)){ 
        std::stringstream lineStream(line);
        double x,y;
        bool isFirst;
        std::getline(lineStream, token,',');
        x = std::atof(token.c_str());
        std::getline(lineStream, token,',');
        y = std::atof(token.c_str());
        points.push_back(std::pair<double, double>(x,y));
    } 
    inputFile.close();
}

void writeFile(std::vector<KnnType>& knns,std::string filename){

    std::ofstream outFile;
    outFile.open(filename);
    for (int i = 0; i < knns.size(); i++){
        for (auto const& x : knns[i]){
            outFile << x.first << ": ";
            auto neighbours = x.second;
            for(int j = 0; j < neighbours.size(); j++){
                outFile << neighbours[j].second << " ";
            }
            outFile << std::endl;
        }
    }
    outFile.close();
}

std::pair<int,double> findMax(std::vector< pi >& vec){

    double max = vec[0].first;
    int index = 0;
    for(int i = 1; i < vec.size(); i++){
        if (vec[i].first > max){
            max = vec[i].first;
            index = i;
        }
    }
    return std::make_pair(index,max);
}

double computeDistance(std::pair<double, double> first, std::pair<double, double> second){
    return std::sqrt(std::pow((first.first - second.first),2) + std::pow((first.second - second.second),2));
}

