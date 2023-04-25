import os
import csv
o_llc = []
o_l1d = []
o_l2c = []
for file in os.listdir('raw_data'):
    with open('raw_data/'+file) as f:
        lines = f.readlines()
        tmp = False
        d = []
        ctr = 0
        tlt = False
        for line in lines:
            if tlt:
                tlt = False
                print(line.split('(')[1], end="")
            if line[:len("*** Reached end of trace:")] == "*** Reached end of trace:":
                ctr += 1
                tlt = True
            if line[:9] == "LLC TOTAL":
                o_llc.append([file[:-6], line.split()[3], line.split()[5]])
            if line[:14] == "cpu0_L2C TOTAL":
                o_l2c.append([file[:-6], line.split()[3], line.split()[5]])
            if line[:14] == "cpu0_L1D TOTAL":
                o_l1d.append([file[:-6], line.split()[3], line.split()[5]])
            if line == "~~~~~~~~~~Reuse Distance Profiler output~~~~~~~~~~\n":
                tmp = True
                continue
            if line == "~~~~~~~~~~Reuse Distance Profiler output end~~~~~~~~~~\n":
                tmp = False
            if not tmp:
                continue
            s = line.split()
            d.append(s)
        if ctr != 1:
            print(f"Reached EOF: {ctr} times in file {file[:-6]}")
        if len(o_l1d) != len(o_llc) and len(o_l2c) != len(o_llc):
            print(file+" , ")
        if len(d) != 896:
            print(file+" , "+str(len(d)))
        fields = ['reuse distance', 'histogram value', 'read histogram value']
        with open('dataset/input/'+file[:-6]+'.csv', 'w') as g:
            writer = csv.writer(g)
            writer.writerow(fields)
            writer.writerows(d)
fields = ['filename', 'accesses', 'hits']
with open('dataset/output/LLC.csv', 'w') as g:
    writer = csv.writer(g)
    writer.writerow(fields)
    writer.writerows(sorted(o_llc))
with open('dataset/output/L2C.csv', 'w') as g:
    writer = csv.writer(g)
    writer.writerow(fields)
    writer.writerows(sorted(o_l2c))
with open('dataset/output/L1D.csv', 'w') as g:
    writer = csv.writer(g)
    writer.writerow(fields)
    writer.writerows(sorted(o_l1d))

