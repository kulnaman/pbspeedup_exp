#!/usr/bin/python3
#SBATCH -N 1
#SBATCH -w n01
#SBATCH -J cublas-cpu-gpu
#SBATCH --exclusive
#SBATCH -t 24:00:00
import argparse
import os
import sys
sys.path.insert(0, '.') 


from experiment import *


def main():

    parser = argparse.ArgumentParser(description="dgemm experiments over power ranges and spatial for gpu and cpu")
    parser.add_argument('-iter',  type=int, help='Number of iterations',default=300)
    parser.add_argument('-o',  type=str, default="./", help='outdir')
    parser.add_argument('-et', type=str,default="gpu",help="define what type of experiment. Options : gpu and cpu")
    args = parser.parse_args()
    exp_type= args.et
    iter = args.iter
    if exp_type == "gpu":
        GPUExperimentConfig.set_env()
        gputype = get_gpu_type()
        default = GPUExperimentConfig.get_default_experiment_config()
        default.set_experiment_config(gputype)
        os.chdir("../gpu-bfs")
        experiment_configs = generate_gpu_experiments(gputype)
    
        try:
            sh.make()
            print ("Starting GPU experiments")
            exp = Experiment("bfs", "",exp_type , "./run.sh", ['-E',f'-n {iter}'])
            for ec in experiment_configs:

                exp.run(ec)

        finally:
            raise
    elif exp_type == "cpu":
    
        CPUExperimentConfig.set_env()
        default = CPUExperimentConfig.get_default_experiment_config()
        default.set_experiment_config()
        experiment_configs= generate_cpu_experiments()

        os.chdir("../gpu-bfs")
        try:
            sh.make()
            print ("Starting CPU experiments")
            exp = Experiment("bfs", "",exp_type , "./run.sh", ['-c',f'-n {iter}'])
            for ec in experiment_configs:

                exp.run(ec)
        finally:
            raise
    print ("done")


if __name__ == "__main__":
    main()
