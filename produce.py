
from subprocess import PIPE
import subprocess
# import matplotlib.pyplot as plt
import sys
import json


def get_float(target):
    out = target.replace('\n', '')
    return float(out)


filename = sys.argv[1]
outFilename = sys.argv[4]
n_ex = 5
nws = [2, 4, 8, 16, 32, 64, 128, 256]
ks = [5, 10]
times = {}
time = 0.0
if filename == 'sequential':
    k_dict = {}
    for k in ks:
        time = 0.0
        for ex in range(n_ex):
            result = subprocess.check_output(["./sequential", str(k), sys.argv[2], sys.argv[3]], universal_newlines=True)
            curr_time = get_float(result)
            time += curr_time
        k_dict[k] = time / n_ex
    times[-1] = k_dict
else:
    for nw in nws:
        k_dict = {}
        for k in ks:
            time = 0.0
            for ex in range(n_ex):
                result = subprocess.check_output(["./"+filename, str(k), str(nw), sys.argv[2],
                             sys.argv[3]], universal_newlines=True)
                c_time = get_float(result)
                time += c_time
            k_dict[k] = time / n_ex
        times[nw] = k_dict

with open(outFilename, 'w+') as fp:
    json.dump(times, fp, indent=4)


'''
plt.plot(seq, label="Sequential")
plt.plot(pthread, label="pthread")
plt.plot(openmp, label="OpenMP")
plt.ylabel("Time")
plt.xlabel("#Workers")
plt.legend()
plt.show()
'''

'''
import subprocess
import matplotlib.pyplot as plt

n = 10000
m = 10000
iter = 100
seed = 123
nws = [1, 4, 8, 16, 32, 64, 128]

subprocess.call(["g++ utils.h utils.cpp par_omp.cpp -fopenmp -O3 -o par_omp"])
subprocess.call(["g++ utils.h utils.cpp par_thread.cpp -pthread -O3 -o par_thread"])
subprocess.call(["g++ utils.h utils.cpp seq.cpp -O3 -o seq"])

for nw in nws:
    subprocess.call(["./seq %d %d %d %d" %(n, m, iter, seed)])
    subprocess.call(["./par_thread %d %d %d %d %d" %(n, m, iter, seed, nw)])
    subprocess.call(["./seq %d %d %d %d %d" %(n, m, iter, seed, nw)])
'''
