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
    parser.add_argument('-a',  type=str, default="./", help='app')

    args = parser.parse_args()

    set_env()
    gputype = get_gpu_type()
    default = ExperimentConfig.get_default_experiment_config()
    default.set_experiment_config(gputype)

    gpu = generate_experiments(gputype)
    
    if args.p is None:
        eprint("No problem size specified")
        sys.exit(1)
    psize = args.p
    apps=args.a
    iters = 5

    os.chdir("../npb_bin")
    os.mkdir(f"{apps}_result")   
    os.chdir(f"{apps}_result")
    try:
        #sh.module.load.cuda()
        sh.make()
        default.set_experiment_config(gputype)

        #default.set_experiment_config(gputype)
        print ("Starting GPU experiments")
        gpu_exp = Experiment(apps, psize, "gpu", f"./{apps}.{psize}.x", [])
        for gpu_ec in gpu:
            gpu_exp.run(gpu_ec, gputype)
        #Removing Cpu testing as not supported in deepgreen
        #print ("Starting cpu experiments")
        #cpu_exp = Experiment("dgemm", psize, "cpu", "./matrixMulCUBLAS2", [f"-Wa={psize}", f"-Wb={psize}", f"-Ha={psize}", f"-Hb={psize}", f"GPU=0", f"--repeats={iters}"])
        #for cpu_ec in cpu:
        #    cpu_exp.run(cpu_ec, gputype)


    except:
        raise
    finally:
        default.set_experiment_config(gputype)

    print ("done")


if __name__ == "__main__":
    main()
