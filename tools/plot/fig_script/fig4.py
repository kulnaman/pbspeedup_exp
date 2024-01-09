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
    "font.size": 18})    

matplotlib.rcParams['agg.path.chunksize'] = 10000 

# Expect cpu, gpu input files in that order
def main():
    is_cpu=True
    parser = argparse.ArgumentParser(description="plot one text")
    parser.add_argument('file',  type=str, help='input file path')
    parser.add_argument('-o',  type=str, default="./", help='outdir')

    args = parser.parse_args()
    f = args.file
    experiments = get_experiments(f)
    if "gpu" in f.lower():
        is_cpu=False
    
    print([e.spatial_perc for e in experiments])
    if is_cpu:
        pbMem20 = sorted([e for e in experiments if e.dram_pcap == 20 and e.spatial_perc == 40], key=lambda e: e.pkg_pcap)
        pbMem30 = sorted([e for e in experiments if e.dram_pcap == 30 and e.spatial_perc == 40], key=lambda e: e.pkg_pcap)
    else:
        #For GPY spatial_perc
        pbMem20 = sorted([e for e in experiments if e.dram_pcap == 20 and e.spatial_perc == 100], key=lambda e: e.pkg_pcap)
        pbMem30 = sorted([e for e in experiments if e.dram_pcap == 30 and e.spatial_perc == 100], key=lambda e: e.pkg_pcap)
    #pb200 = sorted([e for e in experiments if e.gpu_pcap == 200 and e.sm_offset == 0 and e.mem_offset == 0], key=lambda e: e.spatial_perc)
    #pb250 = sorted([e for e in experiments if e.gpu_pcap == 250 and e.sm_offset == 0 and e.mem_offset == 0], key=lambda e: e.spatial_perc)

    #print(pbMem20)
    #print(pb200)
    #print(pb250)
    ys20 = [e.perf for e in pbMem20]
    xs20 = [e.dram_pcap+e.pkg_pcap for e in pbMem20]
    ys30 = [e.perf for e in pbMem30]
    xs30 = [e.dram_pcap+e.pkg_pcap for e in pbMem30]
    plt.xlabel("Total Power Consumption for "+f[:f.rfind(".")] +" in W")
    plt.ylabel("performance")
    plt.xticks(rotation=90)
    plt.grid()
    plt.plot(xs20, ys20,label="Memory Power Consumption 20W")
    plt.plot(xs30, ys30,label="Memory Power Consumption 30W")
    plt.legend(prop={'size':6})
    plt.tight_layout()
    
    #print(len(ys20),xs20)
    #print(len(ys30),xs30)
    print("Saving fig4.png...")
    plt.savefig("fig4.png", dpi=300)
    print("Done")
        
        

        


if __name__ == "__main__":
    main()
