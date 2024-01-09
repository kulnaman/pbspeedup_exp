#!/usr/bin/python3
#SBATCH -N 1
#SBATCH -w n01
#SBATCH -J cublas-cpu-gpu
#SBATCH --exclusive
#SBATCH -t 12:00:00
import argparse
import os
import sys
sys.path.insert(0, '.') 


from experiment import *


def main():

    parser = argparse.ArgumentParser(description="HPCG experiments over power ranges and spatial for gpu and cpu")
    parser.add_argument('-o',  type=str, default="./", help='outdir')


    GPUEperimentConfig.set_env()
    gputype = get_gpu_type()
    default = GPUEperimentConfig.get_default_experiment_config()
    default.set_experiment_config(gputype)

    gpu = generate_gpu_experiments(gputype)
    
    

    try:
        #sh.module.load.cuda()
        
        os.chdir("../hpcg")
        print ("Starting GPU experiments")
        gpu_exp = Experiment("HPCG", "default", "gpu", "./xhpcg", [])
        for gpu_ec in gpu:
            gpu_exp.run(gpu_ec)
  


    except:
        raise
    finally:
        default.set_experiment_config(gputype)

    print ("done")


if __name__ == "__main__":
    main()
