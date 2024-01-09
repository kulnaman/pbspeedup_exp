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
    
    experiments = sorted(experiments, key=lambda e: e.mem_offset)

    ys = [e.avg_mem_freq for e in experiments]
    ys2 = [e.avg_sm_freq for e in experiments]
    xs = [str(e.mem_offset) for e in experiments]
    plt.plot(xs, ys, label="avg mem freq")
    plt.plot(xs, ys2, label="avg sm freq")
    plt.legend()
    plt.tight_layout()

    print("Saving freq.png...")
    plt.savefig("freq.png", dpi=300)
    print("Done")
        
        

        


if __name__ == "__main__":
    main()
