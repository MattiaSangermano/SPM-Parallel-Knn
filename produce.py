
from subprocess import PIPE
import subprocess
import sys
import json


def get_float(target):
    out = target.replace('\n', '')
    return float(out)


if(len(sys.argv) != 6):
    print('wrong usage: python ', sys.argv[0], ' executableName ', 'inputFileName', 'outputFileName', 'fileResultName', 'numExecution')
    exit(-1)
executableName = sys.argv[1]
exInputFilename = sys.argv[2]
exOnputFilename = sys.argv[3]
outFilename = sys.argv[4]
n_ex = int(sys.argv[5])

nws = [2, 4, 8, 16, 32, 64, 128, 256]
ks = [5, 50, 100]
times = {}
time = 0.0
if executableName == 'sequential':
    k_dict = {}
    for k in ks:
        time = 0.0
        for ex in range(n_ex):
            result = subprocess.check_output(["./sequential", str(k), exInputFilename,exOnputFilename], universal_newlines=True)
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
                result = subprocess.check_output(["./"+executableName, str(k), str(nw), exInputFilename,
                             exOnputFilename], universal_newlines=True)
                c_time = get_float(result)
                time += c_time
            k_dict[k] = time / n_ex
        times[nw] = k_dict

with open(outFilename, 'w+') as fp:
    json.dump(times, fp, indent=4)
