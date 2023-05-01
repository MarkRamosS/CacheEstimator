import os
import csv
path = 'raw_data/output1024/'
for file in os.listdir(path):
    o_llc = []
    o_l1d = []
    o_l2c = []
    prev_allc, prev_hllc = 0, 0
    prev_al1d, prev_hl1d = 0, 0
    prev_al2c, prev_hl2c = 0, 0
    with open(path+file) as f:
        lines = f.readlines()
        ctr = 0
        ctr2 = 0
        ctr3 = 0
        for line in lines:
            if line[:9] == "LLC TOTAL":
                ctr += 1
                r = line.split()
                allc, hllc = int(r[3]), int(r[5])
                o_llc.append([allc - prev_allc, hllc - prev_hllc])
                prev_allc = allc
                prev_hllc = hllc
            if line[:14] == "cpu0_L2C TOTAL":
                ctr2 += 1
                r = line.split()
                al2c, hl2c = int(r[3]), int(r[5])
                o_l2c.append([al2c - prev_al2c, hl2c - prev_hl2c])
                prev_al2c = al2c
                prev_hl2c = hl2c
            if line[:14] == "cpu0_L1D TOTAL":
                ctr3 += 1
                r = line.split()
                al1d, hl1d = int(r[3]), int(r[5])
                o_l1d.append([al1d - prev_al1d, hl1d - prev_hl1d])
                prev_al1d = al1d
                prev_hl1d = hl1d
    x = [[0, 0]] * (751 - len(o_llc))
    y = [[0, 0]] * (751 - len(o_l2c))
    z = [[0, 0]] * (751 - len(o_l1d))
    fields = ['filename', 'accesses', 'hits']
    print(f'Printing: {file}, {len(x)} {len(y)} {len(z)} {len(o_llc)+ len(x)} caches {ctr} {ctr2} {ctr3}')
    with open('dataset/output/'+file[:-3]+'_llc.csv', 'w') as g:
        writer = csv.writer(g)
        writer.writerow(fields)
        writer.writerows([[prev_allc, prev_hllc]])
        writer.writerows(x)
        writer.writerows(o_llc[:-2])
    with open('dataset/output/'+file[:-3]+'_l2c.csv', 'w') as g:
        writer = csv.writer(g)
        writer.writerow(fields)
        writer.writerows([[prev_al2c, prev_hl2c]])
        writer.writerows(y)
        writer.writerows(o_l2c[:-2])
    with open('dataset/output/'+file[:-3]+'_l1d.csv', 'w') as g:
        writer = csv.writer(g)
        writer.writerow(fields)
        writer.writerows([[prev_al1d, prev_hl1d]])
        writer.writerows(z)
        writer.writerows(o_l1d[:-2])
