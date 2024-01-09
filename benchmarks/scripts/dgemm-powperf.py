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
    parser.add_argument('-p',  type=int, help='problem size')
    parser.add_argument('-o',  type=str, default="./", help='outdir')

    parser.add_argument('-iter',  type=int, default="50", help='outdir')
    parser.add_argument('-et', type=str,default="gpu",help="define what type of experiment. Options : gpu and cpu")
    args = parser.parse_args()
    exp_type= args.et
    psize=args.p
    iters= args.iter
    if exp_type == "gpu":
        GPUExperimentConfig.set_env()
        gputype = get_gpu_type()
        default = GPUExperimentConfig.get_default_experiment_config()
        default.set_experiment_config(gputype)
        os.chdir("../dgemm")
        experiment_configs = generate_gpu_experiments(gputype)
    
    try:
        #sh.module.load.cuda()
        sh.make()
        #default.set_experiment_config(gputype)
        print ("Starting GPU experiments")
        exp = Experiment("dgemm", psize, exp_type, "./matrixMulCUBLAS2", [f"-Wa={psize}", f"-Wb={psize}", f"-Ha={psize}", f"-Hb={psize}", f"GPU=100", f"--repeats={iters}"])
        for ec in experiment_configs:

            exp.run(ec)
    except:
        raise

    print ("done")


if __name__ == "__main__":
    main()
