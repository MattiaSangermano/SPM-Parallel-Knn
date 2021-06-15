import matplotlib.pyplot as plt
import sys
import json


def getDictFromFile(filename):
    with open(filename) as json_file:
        data = json.load(json_file)
    return data


def speeupApproximation(t_seq, t_read, t_write, nw):
    t_ov = 0.111
    t_knn = t_seq - (t_read + t_write)
    n = t_seq / (t_read + t_write + t_knn/nw + t_ov * nw)
    return n


files_name = ['seq-', 'par-', 'ff-', 'ff_for_']
size = sys.argv[1]
read_dict = getDictFromFile('read_time.json')
write_dict = getDictFromFile('write_time.json')
files_dicts = []

for name in files_name:
    data = getDictFromFile(name + size + '.json')
    files_dicts.append(data)

speedups = []
for dict in files_dicts[1:]:
    plots = {}
    for nw in dict.keys():
        for k in dict[nw]:
            if k != '10':
                val = float(files_dicts[0]['-1'][k]) / float(dict[nw][k])
                if k not in plots:
                    plots[k] = []
                plots[k].append(val)
    speedups.append(plots)

fig = plt.figure()

colours = {
    'par-': 'red',
    'ff-': 'blue',
    'ff_for_': 'green'
    }
linestyles = {
    '5': 'dotted',
    '50': 'solid',
    '100': 'dashdot'
}

for index_k, k in enumerate(speedups[0].keys()):
    ax = fig.add_subplot(1, 3, index_k + 1)
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
    expected = [speeupApproximation(t_seq, t_read, t_write, int(nw)) for nw in list(files_dicts[1].keys())]
    ax.plot(list(files_dicts[1].keys()), expected,
            label='expected', color='black', linestyle='solid')
    plt.legend()

'''
ax.plot([float(n) for n in x[0]], [float(n) for n in y[0]], color='tab:blue')
ax.plot([float(n) for n in x[1]], [float(n) for n in y[1]], color='tab:orange')
ax.plot([float(n) for n in x[2]], [float(n) for n in y[2]], color='tab:red')
'''
plt.show()
