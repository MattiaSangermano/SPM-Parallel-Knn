#include<iostream>
#include<thread>
#include<vector>
#include <chrono>
int main(int argc,char* argv[]){
    int n = std::atoi(argv[1]);
    int nw = std::atoi(argv[2]);
    std::vector<std::thread*> vec(nw);
    auto t_start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < n; i++){
        for(int j = 0; j < nw; j++){
            vec[j] = new std::thread(
                        [](int i) {return;},
                        j);
        }
        for(int j = 0; j < nw; j++){
            vec[j]->join();
        }
    }
    auto t_end = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout << (elapsed_time_ms/((float)n))/((float)nw) << std::endl;
    return 0;
}