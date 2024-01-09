#!/usr/bin/python3
import argparse
from experiment import *

import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt

font = {'family' : 'normal',                                                                                            
        'weight' : 'bold',                                                                                              
        'size'   : 22}                                                                                                  
                                                                                                                        
#matplotlib.rc('font', **font)                                                                                          
plt.rcParams.update({                                                                                                   
    "text.usetex": True,                                                                                                
    "font.family": "sans-serif",                                                                                        
    "font.sans-serif": ["Helvetica"],                                                                                   
    "font.size": 20})    

matplotlib.rcParams['agg.path.chunksize'] = 10000 

# Expect cpu, gpu input files in that order
def main():
    parser = argparse.ArgumentParser(description="plot one text")
    parser.add_argument('file',  type=str, help='input file path')
    parser.add_argument('-o',  type=str, default="./", help='outdir')

    args = parser.parse_args()
    f = args.file
    experiments = get_experiments(f)

    print([e.spatial_perc for e in experiments])

    experiments = list(filter(lambda e: e.spatial_perc == 100, experiments))
    pb150 = sorted([e for e in experiments if e.gpu_pcap == 150 and e.sm_offset == 0], key=lambda e: e.mem_offset)
    pb200 = sorted([e for e in experiments if e.gpu_pcap == 200 and e.sm_offset == 0], key=lambda e: e.mem_offset)
    pb250 = sorted([e for e in experiments if e.gpu_pcap == 250 and e.sm_offset == 0], key=lambda e: e.mem_offset)

    #pb150 = sorted([e for e in experiments if e.gpu_pcap == 150 and e.mem_offset == 0], key=lambda e: e.sm_offset)
    #pb200 = sorted([e for e in experiments if e.gpu_pcap == 200 and e.mem_offset == 0], key=lambda e: e.sm_offset)
    #pb250 = sorted([e for e in experiments if e.gpu_pcap == 250 and e.mem_offset == 0], key=lambda e: e.sm_offset)

    print(pb150)
    print(pb200)
    print(pb250)

    pb = pb150 + pb200 + pb250
    ys = [e.perf for e in pb]
    xs = [e.mem_offset for e in pb]
    plt.bar(xs, ys)
    plt.tight_layout()

    print("Saving fig3.png...")
    plt.savefig("fig3.png", dpi=300)
    print("Done")
        
        

        


if __name__ == "__main__":
    main()
