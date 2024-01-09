#!/usr/bin/python3
import argparse
import pandas as pd
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import numpy as np
import re

# font = {'family' : 'normal',                                                                                            
#         'weight' : 'bold',                                                                                              
#         'size'   : 22}                                                                                                  
                                                                                                                        
#matplotlib.rc('font', **font)                                                                                          
# plt.rcParams.update({                                                                                                   
#     "text.usetex": True,                                                                                                
#     "font.family": "sans-serif",                                                                                        
#     "font.sans-serif": ["Helvetica"],                                                                                   
#     "font.size": 18})    

# matplotlib.rcParams['agg.path.chunksize'] = 10000 

# Expect cpu, gpu input files in that order
yaxis_label = {"cublas": "TFLOPS",
               "stream": "GB/s",
               "CUFFT" : "TFLOPS",
               "HPCG" : "TFLOPS",
               "miniFE" : "TFLOPS",
               "Cloverleaf" : "Cells/Second",
               "dgemm":"TFLOPS  "}
def main():
    is_cpu=True
    parser = argparse.ArgumentParser(description="plot one text")
    parser.add_argument('files',  type=str,nargs="+", help='input file path')
    parser.add_argument('-o',  type=str, default="./", help='outdir')
    
    args = parser.parse_args()
    f1 = args.files[0]
    gpu_files=0
    cpu_files=0
    expirement_name=args.files[0][:args.files[0].rfind(".")]
    print(expirement_name)
    is_titan_xp="titanxp" in expirement_name    
    print(is_titan_xp)
    expirement_name=str(expirement_name).split('/')[-1].split("-")[0]
    print(expirement_name)
    for i,f in enumerate(args.files):
        if "gpu" in f.lower():
            gpu_files=pd.read_csv(f)
        elif "cpu" in f.lower():
            cpu_files=pd.read_csv(f)
        else:
            print("Error")
                
    #print([e.spatial_perc for e in experiments])
    if not is_titan_xp:
        gpu_files=gpu_files.loc[gpu_files['spatial_perc']==100]
    gpu_files['total_pkg_pcap']=gpu_files['gpu_pcap']
    data_150_0=gpu_files.loc[gpu_files['mem_offset']==0]
    
    data_150_best=gpu_files.loc[gpu_files.groupby('total_pkg_pcap')['perf'].idxmax()]
    # #data_200_0=gpu_files.loc[gpu_files['total_pkg_pcap']==200]
    # data_200_0=gpu_files.loc[gpu_files.groupby('total_pkg_pcap')['mem_offset']==0]
    # data_200_best=gpu_files.loc[gpu_files['total_pkg_pcap']==200]
    # data_200_best=data_200_best.loc[data_200_best.groupby('total_pkg_pcap')['perf'].idxmax()]
    print(data_150_0)
    print(data_150_best)
    y_best=[]
    y_default=[]
    # y_default.append(data_150_0['perf'].values[0])
    # y_default.append(data_200_0['perf'].values[0])
    # y_best.append(data_150_best['perf'].values[0])
    # y_best.append(data_200_best['perf'].values[0])
    labels=["150W","200W"]
    x = np.arange(len(labels))  # the label locations
    width = 0.35  # the width of the bars
    fig, ax = plt.subplots()
    print(data_150_0['total_pkg_pcap'])
    print(y_default)
    # rects1 = ax.plot(x - width/2, y_default, width, label='Default Memory Configuration')
    # rects2 = ax.plot(x + width/2, y_best, width, label='Best Memory Configuration')
    # legend=['Default Memory Configuration','Best Memory Configuration']
    plt.plot(data_150_0['total_pkg_pcap'],data_150_0['perf'],label="Default Performance")
    plt.plot(data_150_best['total_pkg_pcap'],data_150_best['perf'],label="Best Performance")

    ax.set_xlabel("Total Power Consumption for "+expirement_name+" in W")
    ax.set_ylabel("performance ("+yaxis_label[expirement_name]+")")
    # ax.set_xticks(x,labels)
    ax.grid()
    plt.ylim(ymin=0)

    # ax.bar_label(rects1, padding=3)
    # ax.bar_label(rects2, padding=3)
    # ax.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,
    #        ncol=2, mode="expand", borderaxespad=0.)
    fig.tight_layout()
    plt.legend(prop={'size':6})

    print("Saving "+expirement_name+"_figure_2.png...")
    plt.savefig(expirement_name+"_figure_2.png")
   # print("Done")
        
        

        


if __name__ == "__main__":
    main()
