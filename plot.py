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
    if metric == 'sp' or metric == 's':
        n = t_seq / (t_read + t_write + t_knn/nw + t_ov * nw)
    elif metric == 'e':
        n = t_seq / (nw * (t_read + t_write + t_knn/nw + t_ov * nw))
    return n


def compute_metric(metric, nw=0, k=0, seq_dict=None, parallel_dict=None):
    if metric == 'sp':  # Completion Time
        val = float(seq_dict['-1'][k]) / float(parallel_dict[nw][k])
    elif metric == 's':  # Scalability
        val = float(parallel_dict['1'][k])/float(parallel_dict[nw][k])
    elif metric == 'e':  # Efficency
        val = float(seq_dict['-1'][k]) / (int(nw) * float(parallel_dict[nw][k]))
    return val


sizes = ['small', 'medium', 'large']

for size in sizes:
    directory = 'experiments/'
    files_name = ['seq-', 'par-', 'ff-pin-', 'ff_for_']
    #files_name = ['seq-', 'ff-', 'ff-pin-']
    metric = sys.argv[1]
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
        'ff-pin-': 'blue',
        'ff_for_': 'green'
        }
    linestyles = {
        '5': 'dotted',
        '50': 'solid',
        '100': 'dashdot'
    }
    label = {
        'ff-pin-': 'ff-pin',
        'ff-': 'ff',
        'par-': 'par',
        'ff_for_': 'ff_for'
    }

    for index_k, k in enumerate(speedups[0].keys()):
        ax = fig.add_subplot(3, 1, index_k + 1)
        plt.grid(linestyle='dotted')
        for index, dict in enumerate(speedups):
            lbl = label[files_name[index+1]]
            color = colours[files_name[index+1]]
            linestyle = linestyles[k]
            ax.plot(list(files_dicts[index + 1].keys()), dict[k],
                    label=lbl, color=color, linestyle='solid')
            ax.title.set_text('k= ' + k)
        t_write = float(write_dict[size][k])
        t_read = float(read_dict[size])
        t_seq = float(files_dicts[0]['-1'][k])
        if metric == 'sp' or metric == 'e' or metric == 's':
            expected = [speeupApproximation(t_seq, t_read, t_write, int(nw), metric) for nw in list(files_dicts[1].keys())]
            ax.plot(list(files_dicts[1].keys()), expected,
                    label='expected', color='black', linestyle='dashed')
        plt.xlabel('num workers')
        plt.ylabel('speedup')
        plt.legend(loc='upper left')
        plt.tight_layout()
    plt.show()
