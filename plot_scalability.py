import matplotlib.pyplot as plt
import sys
import json

def getDictFromFile(filename):
    with open(filename) as json_file:
        data = json.load(json_file)
    return data


sizes = ['small', 'medium', 'large']
files_dicts = []
directory = 'experiments/'
read_dict = getDictFromFile(directory + 'read_time.json')
write_dict = getDictFromFile(directory + 'write_time.json')
for name in sizes:
    data = getDictFromFile(directory + 'par-' + name + '.json')
    files_dicts.append(data)

speedups = []
for i, size in enumerate(sizes):
    plots = []
    for nw in files_dicts[0].keys():
        val = float(files_dicts[i]['1']['50'])/float(files_dicts[i][nw]['50'])
        plots.append(val)
    speedups.append(plots)

fig = plt.figure()

colours = {
    'small': 'red',
    'medium': 'blue',
    'large': 'green'
    }

for index_s, s in enumerate(sizes):
    ax = fig.add_subplot(1, 1, 1)
    plt.grid(linestyle='dotted')
    color = colours[s]
    ax.plot(list(files_dicts[index_s].keys()), speedups[index_s],
            label=s, color=color, linestyle='solid')
    ax.title.set_text('scalability')
    plt.legend()
plt.show()
