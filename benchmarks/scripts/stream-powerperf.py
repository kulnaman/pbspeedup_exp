#!/usr/bin/python3
import argparse
import os
import sys
sys.path.insert(0, '.') 


from experiment import *


def main():

    parser = argparse.ArgumentParser(description="dgemm experiments over power ranges and spatial for gpu and cpu")
    parser.add_argument('-o',  type=str, default="./", help='outdir')
    parser.add_argument('-p',  type=int, default="./", help='problem size')
    parser.add_argument('-et', type=str,default="gpu",help="define what type of experiment. Options : gpu and cpu")
    args = parser.parse_args()
    exp_type= args.et
    psize=args.p
    if exp_type == "gpu":
        GPUExperimentConfig.set_env()
        gputype = get_gpu_type()
        print(gputype)
        default = GPUExperimentConfig.get_default_experiment_config()
        default.set_experiment_config(gputype)
        os.chdir("../stream")
        experiment_configs = generate_gpu_experiments(gputype)
    
    try:
        sh.make()
        print ("Starting GPU experiments")
        gpu_exp = Experiment("stream", psize,exp_type, "./cuda-stream",[f'-s', f'{psize}', f"--float", f"--triad-only",f'-n','1000'])
        for gpu_ec in experiment_configs:
            gpu_exp.run(gpu_ec)
  


    except:
        raise
    finally:
        default.set_experiment_config(gputype)

    print ("done")


if __name__ == "__main__":
    main()
