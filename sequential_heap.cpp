#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <queue>
#include <math.h> 
typedef std::pair<int, double>  pi;
typedef std::vector< std::pair< int, std::priority_queue<pi> >> knnType;


double computeDistance(std::pair<double, double> first, std::pair<double, double> second){
    return std::sqrt(std::pow((first.first - second.first),2) + std::pow((first.second - second.second),2));
}
void printKnn(knnType knn, int k){
    for (int i = 0; i < knn.size(); i ++){
        std::cout << i << ": " << std::endl;
        auto pq = knn[i].second;
        for(int j = 0; j < k; j++ ){
            pi el = pq.top();
            pq.pop();
            std::cout << el.second<< ", ";
        }
        std::cout << std::endl;
    }
}
void printVector(std::vector<std::pair<double,double>> vec){
    for(int i = 0; i < vec.size(); i++){
            //std::cout << std::setprecision(10)<< "x: " << vec[i].first << " y: " << vec[i].second << std::endl;
        }
}
int main(int argc, char **argv) {
    if(argc <= 1){
        std::cout << "Wrong input format. filename" << std::endl;
        std::exit(0);
    }
    std::ifstream inputFile;
    std::string filename = argv[1];
    double distance;
    int k = std::atoi(argv[2]);

    inputFile.open(filename);
    std::string line, token;
    std::vector<std::pair<double,double>> points;
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
    std::vector< std::pair< int, std::priority_queue<pi> >> knn;

    for(int i = 0; i < points.size(); i++){
        std::priority_queue<pi> neighbour;
        for(int j = 0; j < points.size(); j++){
            if(i != j){
                distance = computeDistance(points[i],points[j]);
                neighbour.push(std::make_pair(distance,j));
                if(neighbour.size() > k){
                    neighbour.pop();
                }
            }
        }
        knn.push_back(std::make_pair(i,neighbour));
    }

    // printKnn(knn, k);
    return 0;
}