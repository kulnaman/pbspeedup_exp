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
    #pb150 = sorted([e for e in experiments if e.gpu_pcap == 150 and e.sm_offset == 0 and e.mem_offset == 0], key=lambda e: e.spatial_perc)
    #pb200 = sorted([e for e in experiments if e.gpu_pcap == 200 and e.sm_offset == 0 and e.mem_offset == 0], key=lambda e: e.spatial_perc)
    #pb250 = sorted([e for e in experiments if e.gpu_pcap == 250 and e.sm_offset == 0 and e.mem_offset == 0], key=lambda e: e.spatial_perc)

    max_dram_pcap = max(experiments, key=lambda e: e.dram_pcap).dram_pcap
    pb75 = [e for e in experiments if e.pkg_pcap + e.dram_pcap == 75 and e.dram_pcap == max_dram_pcap]
    pb85 = [e for e in experiments if e.pkg_pcap + e.dram_pcap == 85 and e.dram_pcap == max_dram_pcap]
    pb100 = [e for e in experiments if e.pkg_pcap + e.dram_pcap == 100 and e.dram_pcap == max_dram_pcap]
    pb120 = [e for e in experiments if e.pkg_pcap + e.dram_pcap == 120 and e.dram_pcap == max_dram_pcap]
    pb150 = [e for e in experiments if e.pkg_pcap + e.dram_pcap == 150 and e.dram_pcap == max_dram_pcap]


    print("\n\n\n")
    print(pb75)
    #print(pb100)
    #print(pb120)
    #print(pb150)

    ys = [e.perf for e in pb75]
    xs = [str(e.spatial_perc) + "t" for e in pb75]
    plt.bar(xs, ys)
    plt.tight_layout()

    print("Saving fig1-cpu.png...")
    plt.savefig("fig1-cpu.png", dpi=300)
    print("Done")
        
        

        


if __name__ == "__main__":
    main()
