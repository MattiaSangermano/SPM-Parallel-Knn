#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <string>
#include <map>
#include <tuple>

typedef std::vector<std::pair<double,double>> pointsType;
typedef std::pair<double, int>  pi;
typedef std::map<int, std::vector<pi>> KnnType;

void readFile(pointsType& points,std::string filename);
void writeFile(std::vector<KnnType>& knns,std::string filename);
std::pair<int,double> findMax(std::vector< pi >& vec);
double computeDistance(std::pair<double, double> first, std::pair<double, double> second);

#endif