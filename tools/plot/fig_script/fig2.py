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
    parser.add_argument('files',  type=str, nargs="+", help='input file path')
    parser.add_argument('-o',  type=str, default="./", help='outdir')

    args = parser.parse_args()
    
    ys_150 = []
    ys_250 = []

    for i,f in enumerate(args.files):
        experiments = get_experiments(f)
        pb150 = []
        pb250 = []

        for e in experiments:
            print("e.pkg_pcap, e.dram_pcap, e.pkg_pcap + e.dram_pcap, spatial_perc: ", e.pkg_pcap, e.dram_pcap, e.pkg_pcap + e.dram_pcap, e.spatial_perc)
        if i % 2 == 1:
            pb150 = [e for e in experiments if e.gpu_pcap == 150 and e.sm_offset == 0 and e.mem_offset == 0 and e.spatial_perc == 100]
            pb250 = [e for e in experiments if e.gpu_pcap == 250 and e.sm_offset == 0 and e.mem_offset == 0 and e.spatial_perc == 100]
        else:
            max_dram_pcap = max(experiments, key=lambda e: e.dram_pcap).dram_pcap
            print("max_dram_pcap: ", max_dram_pcap)
            # TODO ignoring spatial_perc for now
            pb150 = [e for e in experiments if e.pkg_pcap + e.dram_pcap == 150 and e.dram_pcap == max_dram_pcap]
            pb250 = [e for e in experiments if e.pkg_pcap + e.dram_pcap == 250 and e.dram_pcap == max_dram_pcap]
        print(i)
        print(pb150)
        print(pb250)
        assert len(pb150) == 1
        assert len(pb250) == 1

        pb150_perf = pb150[0].perf
        pb250_perf = pb250[0].perf

        ys_150.append(pb150_perf)
        ys_250.append(pb250_perf)

    ys = ys_150 + ys_250
    xs = ["CPU-150", "GPU-150", "CPU-250", "GPU-250"] #* (len(ys)//2)

    print("len(ys), len(xs):", len(ys), ",", len(xs))

    plt.bar(xs, ys)
    plt.tight_layout()

    plt.savefig("fig2.png", dpi=300)
        
        

        


if __name__ == "__main__":
    main()
