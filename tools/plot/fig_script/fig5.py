#!/usr/bin/python3
import argparse
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
yaxis_label = {"cublas": "TFLOPS",
               "stream": "GB/s",
               "CUFFT" : "TFLOPS",
               "HPCG" : "TFLOPS",
               "miniFE" : "TFLOPS",
               "Cloverleaf" : "Cells/Second",
               "dgemm":"GFLOPS"}
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
    expirement_name=args.file[:args.file.rfind(".")]

    expirement_name=str(expirement_name).split('/')[-1].split("-")[0]

    #print([e.spatial_perc for e in experiments])
    if is_cpu:
        df=df.loc[df['spatial_perc']==40]
        df['total_pkg_pcap']=df['pkg_pcap']+df['dram_pcap']
        perf_min=df.groupby('total_pkg_pcap')['perf'].min().reset_index()
        perf_max=df.groupby('total_pkg_pcap')['perf'].max().reset_index()
    else:
        #For GPU spatial_perc is 100%
        df=df.loc[df['spatial_perc']==100]
        perf_min=df.groupby('gpu_pcap')['perf'].min().reset_index()
        perf_max=df.groupby('gpu_pcap')['perf'].max().reset_index()
    plt.xlabel("Total Power Consumption for "+expirement_name +" in W")
    plt.ylabel("performance ("+yaxis_label[expirement_name]+")")
    print(perf_max)
    print(perf_min)
    if is_cpu:
        plt.plot(perf_min['total_pkg_pcap'],perf_min['perf'],label="Minimum Performance")
        plt.plot(perf_max['total_pkg_pcap'],perf_max['perf'],label="Maximum Performance")
    else:
        plt.plot(perf_min['gpu_pcap'],perf_min['perf'],label="Minimum Performance")
        plt.plot(perf_max['gpu_pcap'],perf_max['perf'],label="Maximum Performance")
        plt.xticks(rotation=90)
    plt.grid()
    plt.legend(prop={'size':6})
    plt.tight_layout()
    plt.ylim(ymin=0)

    print("Saving"+expirement_name+"_fig5.png...")
    plt.savefig(expirement_name+"_fig5.png", dpi=300)
    print("Done")
        
        

        


if __name__ == "__main__":
    main()
