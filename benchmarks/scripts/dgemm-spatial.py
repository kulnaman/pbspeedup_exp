#!/usr/bin/python3
#SBATCH -N 1
#SBATCH -w n01
#SBATCH -J cublas-cpu-gpu
#SBATCH --exclusive
#SBATCH -t 12:00:00
import os
import sys
sys.path.insert(0, '.') 


from experiment import *


def main():
    set_env()
    gputype = get_gpu_type()
    default = ExperimentConfig.get_default_experiment_config()
    default.set_experiment_config(gputype)

    gpu = generate_spatial_gpu_experiments(gputype)
    print([e.spatial_perc for e in gpu])

    psize = 4096 * 4
    iters = 5

    os.chdir("../dgemm")
    try:
        #sh.module.load.cuda()
        sh.make()

        default.set_experiment_config(gputype)
        print ("Starting GPU experiments")
        gpu_exp = Experiment("dgemm", psize, "gpu", "./matrixMulCUBLAS2", [f"-Wa={psize}", f"-Wb={psize}", f"-Ha={psize}", f"-Hb={psize}", f"GPU=100", f"--repeats={iters}"])
        for gpu_ec in gpu:
            gpu_exp.run_spatial(gpu_ec, gputype)

    except:
        raise
    finally:
        default.set_experiment_config(gputype)

    print ("done")


if __name__ == "__main__":
    main()
