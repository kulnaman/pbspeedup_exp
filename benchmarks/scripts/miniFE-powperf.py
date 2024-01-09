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

    parser = argparse.ArgumentParser(description="miniFE experiments over power ranges and spatial for gpu and cpu")
    parser.add_argument('-nx',  type=int, help='problem size')
    parser.add_argument('-ny',  type=int, help='problem size')
    parser.add_argument('-nz',  type=int, help='problem size')
    parser.add_argument('-o',  type=str, default="./", help='outdir')
    parser.add_argument('-et', type=str,default="gpu",help="define what type of experiment. Options : gpu,cpu")
    args = parser.parse_args()
    exp_type= args.et
    if exp_type == "gpu":
        GPUExperimentConfig.set_env()
        gputype = get_gpu_type()
        default = GPUExperimentConfig.get_default_experiment_config()
        default.set_experiment_config(gputype)
        os.chdir("../miniFE-cuda/src")
        experiment_configs = generate_gpu_experiments(gputype)
    
    elif exp_type == "cpu":
    
        CPUExperimentConfig.set_env()
        default = CPUExperimentConfig.get_default_experiment_config()
        default.set_experiment_config()
        experiment_configs= generate_cpu_experiments()

        os.chdir("../miniFE-omp/src")
    nx = args.nx
    ny = args.ny
    nz = args.nz
    
    if nx is None or ny is None or nz is None:
        eprint("No problem size specified")
        sys.exit(1)

    try:
        #sh.module.load.cuda()
        sh.make()

        exp = Experiment("miniFE", f"{nx}-{ny}-{nz}",exp_type , "./miniFE.x", [f"-nx={nx}", f"-ny={ny}", f"-nz={nz}"])
        for ec in experiment_configs:
            exp.run(ec)

        #os.chdir("../../miniFE-omp/src")
        #print("changing directory to:", os.getcwd())    
        #sh.make()
        
        #print ("Starting cpu experiments")
        #cpu_exp = Experiment("miniFE", f"{nx}-{ny}-{nz}", "cpu", "./miniFE.x", [f"-nx={nx}", f"-ny={ny}", f"-nz={nz}"])
        #for cpu_ec in cpu:
        #   cpu_exp.run(cpu_ec, gputype)


    except:
        raise
    finally:
        default.set_experiment_config(gputype)

    print ("done")


if __name__ == "__main__":
    main()
