import matplotlib.pyplot as plt
import sys
import json
import math

def getDictFromFile(filename):
    with open(filename) as json_file:
        data = json.load(json_file)
    return data


def speeupApproximation(t_seq=0, t_read=0, t_write=0, nw=0, metric=None):
    t_ov = 0.111
    t_knn = t_seq - (t_read + t_write)
    if metric == 'ct' or metric == 's':
        n = t_seq / (t_read + t_write + t_knn/nw + t_ov * nw)
    elif metric == 'e':
        n = t_seq / (nw * (t_read + t_write + t_knn/nw + t_ov * nw))
    return n


def compute_metric(metric, nw=0, k=0, seq_dict=None, parallel_dict=None):
    if metric == 'ct':  # Completion Time
        val = float(seq_dict['-1'][k]) / float(parallel_dict[nw][k])
    elif metric == 's':  # Scalability
        val = float(parallel_dict['1'][k])/float(parallel_dict[nw][k])
    elif metric == 'e':  # Efficency
        val = float(seq_dict['-1'][k]) / (int(nw) * float(parallel_dict[nw][k]))
    return val

directory = 'experiments/'
# files_name = ['seq-', 'par-', 'ff-', 'ff_for_']
files_name = ['seq-', 'ff-', 'ff_pin_']
size = sys.argv[1]
metric = sys.argv[2]
read_dict = getDictFromFile(directory + 'read_time.json')
write_dict = getDictFromFile(directory + 'write_time.json')
files_dicts = []

for name in files_name:
    data = getDictFromFile(directory + name + size + '.json')
    files_dicts.append(data)

speedups = []
for dict in files_dicts[1:]:
    plots = {}
    for nw in dict.keys():
        for k in dict[nw]:
            if k != '10':
                val = compute_metric(metric, nw, k, files_dicts[0], dict)
                if k not in plots:
                    plots[k] = []
                plots[k].append(val)
    speedups.append(plots)

fig = plt.figure()

colours = {
    'par-': 'red',
    'ff-': 'blue',
    'ff_pin_': 'red',
    'ff_for_': 'green'
    }
linestyles = {
    '5': 'dotted',
    '50': 'solid',
    '100': 'dashdot'
}

for index_k, k in enumerate(speedups[0].keys()):
    ax = fig.add_subplot(3, 1, index_k + 1)
    plt.grid(linestyle='dotted')
    for index, dict in enumerate(speedups):
        label = files_name[index+1][:-1]
        color = colours[files_name[index+1]]
        linestyle = linestyles[k]
        ax.plot(list(files_dicts[index + 1].keys()), dict[k],
                label=label, color=color, linestyle='solid')
        ax.title.set_text('k= ' + k)
    t_write = float(write_dict[size][k])
    t_read = float(read_dict[size])
    t_seq = float(files_dicts[0]['-1'][k])
    if metric == 'ct' or metric == 'e' or metric == 's':
        expected = [speeupApproximation(t_seq, t_read, t_write, int(nw), metric) for nw in list(files_dicts[1].keys())]
        ax.plot(list(files_dicts[1].keys()), expected,
                label='expected', color='black', linestyle='dashed')
    plt.tight_layout()
    plt.legend(loc='upper right')
plt.show()
