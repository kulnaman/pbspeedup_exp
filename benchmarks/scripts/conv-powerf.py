#!/usr/bin/python3
#SBATCH -N 1
#SBATCH -w deepgreen
#SBATCH -J conv-cpu-gpu
#SBATCH --exclusive
#SBATCH -t 24:00:00
import argparse
import os
import sys
sys.path.insert(0, '.') 


from experiment import *


def main():

    parser = argparse.ArgumentParser(description="dgemm experiments over power ranges and spatial for gpu and cpu")
    parser.add_argument('-o',  type=str, default="./", help='outdir')
    parser.add_argument('-iter',  type=int, default="./", help='outdir')
    parser.add_argument('-et', type=str,default="gpu",help="define what type of experiment. Options : gpu and cpu")
    args = parser.parse_args()
    exp_type= args.et
    iter=args.iter
    if exp_type == "gpu":
        GPUExperimentConfig.set_env()
        gputype = get_gpu_type()
        default = GPUExperimentConfig.get_default_experiment_config()
        default.set_experiment_config(gputype)
        os.chdir("../cudnn-benchmark")
        experiment_configs = generate_gpu_experiments(gputype)
    
        try:
            sh.make()
            print ("Starting GPU experiments")
            exp = Experiment("conv", "18593750",exp_type , "./run.sh", [f"{iter}"])
            for ec in experiment_configs:

                exp.run(ec)

        finally:
            raise


if __name__ == "__main__":
    main()
