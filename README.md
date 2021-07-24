# Parallel Knn

The repo contains a `C++` implementation of the KNN algorithm. There are four different implementations: 
- `parallel.cpp`: is a parallel implementation using only `pthread`. Input format: the value of `K`, the number of threads to use, the input file name, the output file name.  
- `ff_parallel`: is a parallel implementation consisting of a Master/Worker using the `FastFlow` library. Input format: the value of `K`, the number of threads to use, the input file name, the output file name, `y` if you want to use the custom thread pinning (is an adhoc implementation for the machine used during the experiments). 
- `ff_parallel_for`: is a parallel implementation consisting of a parallel for reduce using the `FastFlow` library.
- `sequential`: is the sequential version of the KNN. Input format: the value of `K`, the input file name, the output file name.

In the `experiments` directory there are all the execution times computed during the experiments.\
In the `plot` directory there are plots of the speedup, efficiency and scalability.