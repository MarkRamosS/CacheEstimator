import os
import csv
path = 'raw_data/output1024/'
hist = {}
second_hist = {}
for file in os.listdir(path):
    hist_list = []
    with open(path+file) as f:
        lines = f.readlines()
        ctr = 0
        conf_values = []
        while lines[ctr] != "~~~~~~~~~~Reuse Distance Profiler output~~~~~~~~~~\n":
            ctr += 1
        ctr += 1
        while lines[ctr] != "~~~~~~~~~~Reuse Distance Profiler output end~~~~~~~~~~\n":
            x, y, z = lines[ctr].split()
            conf_values.append(x)
            hist[x] = int(y)
            second_hist[x] = int(y)
            ctr += 1
        # hist_list.append(hist.copy()) # First one is the warmup reuse distances
        while len(lines) - 20 > ctr:
            while lines[ctr] != "~~~~~~~~~~Reuse Distance Profiler output~~~~~~~~~~\n":
                ctr += 1
            ctr += 1
            while lines[ctr] != "~~~~~~~~~~Reuse Distance Profiler output end~~~~~~~~~~\n":
                x, y, z = lines[ctr].split()
                hist[x] = int(y) - second_hist[x]
                second_hist[x] = int(y)
                ctr += 1
            hist_list.append(hist.copy())

    print(f'Printing: {file}, {len(hist_list)} rds')
    with open('dataset/input/'+file[:-3]+'csv', 'w') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=conf_values)
        writer.writeheader()
        writer.writerow(second_hist)    # Final/full histogram
        for hst in hist_list[:-1]:     # (last one's all 0 and first one has warmup instructions)
            writer.writerow(hst)        # In between histograms
