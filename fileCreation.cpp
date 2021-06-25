#include <iostream>
#include <fstream>
#include <random>
#include<iomanip>

int main(int argc, char **argv) {
    if(argc < 5 || argc > 6){
        std::cout << "Wrong input format!" << std::endl;
        std::cout << "Usage: " << argv[0] << " noPoints minVal maxVal seed [fileName]" << std::endl;
        exit(-1);
    }
    // reading input values
    int noPoints = std::atoi(argv[1]);
    int minVal = std::atoi(argv[2]);
    int maxVal = std::atoi(argv[3]);
    int seed = std::atoi(argv[4]);
    std::string filename = "input_knn.txt";
    if(argc > 5){
        filename = argv[5];
    }

    std::ofstream outputFile;
    outputFile.open(filename);

    //uniform number generation
    std::mt19937 gen(seed); 
    std::uniform_real_distribution<> dis(minVal, maxVal);
    long double x, y;
    for(int i = 0; i < noPoints; i++){
        x = dis(gen);
        y = dis(gen);
        outputFile <<std::setprecision(10)<< x << ',' << y << std::endl;
    }
    outputFile.close();
  return 0;
}