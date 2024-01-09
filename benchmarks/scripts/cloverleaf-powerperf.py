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

    parser = argparse.ArgumentParser(description="cloverleaf experiments over power ranges and spatial for gpu and cpu")
    parser.add_argument('-o',  type=str, default="./", help='outdir')

    parser.add_argument('-et', type=str,default="gpu",help="define what type of experiment. Options : gpu and cpu")
    args = parser.parse_args()

    exp_type= args.et
    if exp_type == "gpu":
        GPUExperimentConfig.set_env()
        gputype = get_gpu_type()
        default = GPUExperimentConfig.get_default_experiment_config()
        default.set_experiment_config(gputype)
        experiment_configs = generate_gpu_experiments(gputype)

    try:
        #sh.module.load.cuda()
        
        os.chdir("../CloverLeaf_CUDA")
        sh.make()
        print ("Starting GPU experiments")
        gpu_exp = Experiment("clover_leaf", f"test_problem 5", "gpu", "./clover_leaf", [])
        for gpu_ec in experiment_configs:
            gpu_exp.run(gpu_ec)

    except:
        raise

    print ("done")


if __name__ == "__main__":
    main()
