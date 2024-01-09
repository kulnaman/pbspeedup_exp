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
    parser.add_argument('files',  type=str,nargs="+", help='input file path')
    parser.add_argument('-o',  type=str, default="./", help='outdir')
    
    args = parser.parse_args()
    f1 = args.files[0]
    f2 = args.files[1]
    gpu_files=0
    cpu_files=0
    expirement_name=args.files[0][:args.files[0].rfind(".")]
    for i,f in enumerate(args.files):
        if "gpu" in f.lower():
            gpu_files=pd.read_csv(f)
        elif "cpu" in f.lower():
            cpu_files=pd.read_csv(f)
        else:
            print("Error")
                
    #print([e.spatial_perc for e in experiments])

    cpu_files=cpu_files.loc[cpu_files['spatial_perc']==100]
    gpu_files=gpu_files.loc[gpu_files['spatial_perc']==100]
    cpu_files['total_pkg_pcap']=(cpu_files['pkg_pcap']+cpu_files['dram_pcap'])*2
    gpu_files['total_pkg_pcap']=gpu_files['gpu_pcap']+110
    data_cpu_a=cpu_files.loc[cpu_files['total_pkg_pcap']==260]
    data_cpu_a=data_cpu_a.loc[data_cpu_a.groupby('total_pkg_pcap')['perf'].idxmax()]
    data_cpu_b=cpu_files.loc[cpu_files['total_pkg_pcap']==310]
    data_cpu_b=data_cpu_b.loc[data_cpu_b.groupby('total_pkg_pcap')['perf'].idxmax()]
    data_gpu_a=gpu_files.loc[gpu_files['total_pkg_pcap']==260]
    data_gpu_a=data_gpu_a.loc[data_gpu_a.groupby('total_pkg_pcap')['perf'].idxmax()]
    data_gpu_b=gpu_files.loc[gpu_files['total_pkg_pcap']==310]
    data_gpu_b=data_gpu_b.loc[data_gpu_b.groupby('total_pkg_pcap')['perf'].idxmax()]
    print(data_cpu_a)
    print(data_cpu_b)
    print(data_gpu_a)
    print(data_gpu_b)
    x=[0,1,2,3]
    y=[]
    y.append(data_cpu_a['perf'].to_numpy()[0])
    y.append(data_gpu_a['perf'].to_numpy()[0])
    y.append(data_cpu_b['perf'].to_numpy()[0])
    y.append(data_gpu_b['perf'].to_numpy()[0])

    print(x)
    print(y)
    plt.xlabel("Total Power Consumption for MiniFE in W")
    plt.ylabel("performance")
    plt.bar(x,y)
    my_ticks=['cpu 260','gpu 260','cpu 310','gpu 310']
    plt.xticks(x,my_ticks)
    plt.grid()
    plt.ylim(ymin=0)
    plt.tight_layout()
   # print("Saving fig6.png...")
    plt.savefig("fig7.svg", dpi=300)
   # print("Done")
        
        

        


if __name__ == "__main__":
    main()
