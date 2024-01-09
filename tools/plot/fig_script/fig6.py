#!/usr/bin/python3
import argparse
from experiment import *
import pandas as pd
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
    df=pd.read_csv(args.file)
    if "gpu" in f.lower():
        is_cpu=False
    #print([e.spatial_perc for e in experiments])
    if is_cpu:
        df=df.loc[df['spatial_perc']==100]
        df['total_pkg_pcap']=df['pkg_pcap']+df['dram_pcap']
        data_pb160=df.loc[df['total_pkg_pcap'] == 160].sort_values('dram_pcap')
        data_pb200=df.loc[df['total_pkg_pcap'] == 200].sort_values('dram_pcap')
        data_pb160=data_pb160.iloc[[0,len(data_pb160)//2,-1]]
        data_pb200=data_pb200.iloc[[0,len(data_pb200)//2,-1]]
        print(data_pb160)
        print(data_pb200)
    else:
        #For GPU spatial_perc is 100% 
        df=df.loc[df['spatial_perc']==100]
        data_pb160=df.loc[df['gpu_pcap'] == 160].sort_values('dram_pcap')
        data_pb200=df.loc[df['gpu_pcap'] == 200].sort_values('dram_pcap')
        data_pb160=data_pb160.iloc[[0,len(data_pb160)//2,-1]]
        data_pb200=data_pb200.iloc[[0,len(data_pb200)//2,-1]]
    plt.xlabel("Total Power Consumption for "+f[:f.rfind(".")] +" in W")
    plt.ylabel("performance")
    if is_cpu:
        plt.bar(data_pb160['dram_pcap'],data_pb160['perf'],label="Performance at 160W")
        #plt.bar(data_pb200['dram_pcap'],data_pb200['perf'],label="Performance at 200W")
    else:
        plt.plot(data_pb160['gpu_pcap'],data_pb160['perf'],label="Performance at 160W")
        plt.plot(data_pb200['gpu_pcap'],data_pb200['perf'],label="Performance at 200W")
    plt.xticks(rotation=90)
    plt.grid()
    plt.legend(prop={'size':6})
    plt.tight_layout()
    print("Saving fig6.svg...")
    plt.savefig("fig6.svg", dpi=300)
    print("Done")
        
        

        


if __name__ == "__main__":
    main()
