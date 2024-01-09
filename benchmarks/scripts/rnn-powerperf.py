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
    parser.add_argument('-s',default=40,type=int,help="Sequence Length")
    parser.add_argument('-l',default=40,type=int,help="Number of layers")
    parser.add_argument('-hs',default=512,type=int,help="Hidden Step Size")
    parser.add_argument('-b',default=512,type=int,help="Batch Size")
    parser.add_argument('-m',default=3,type=int,help="Modes: =  RNN_RELU,1 = RNN_TANH, 2 = LSTM, 3 = GRU\n")
    parser.add_argument('-i', type=int, default="./", help='iteration')
    parser.add_argument('-et', type=str,default="gpu",help="define what type of experiment. Options : gpu and cpu")
    args = parser.parse_args()

    exp_type= args.et
    
    batch_size=args.b
    seq_len=args.s
    layers=args.l
    hidden_step_size=args.hs
    mode=args.m

    iteration=args.i
    if exp_type == "gpu":
        GPUExperimentConfig.set_env()
        gputype = get_gpu_type()
        default = GPUExperimentConfig.get_default_experiment_config()
        default.set_experiment_config(gputype)
        os.chdir("../RNN_CUDNN")
        experiment_configs = generate_gpu_experiments(gputype)
        try:
            sh.make()
            print ("Starting GPU experiments")
            exp = Experiment("RNN", "",exp_type , "./RNN", [f"{seq_len}",f"{layers}",f"{hidden_step_size}",f"{batch_size}",f"{mode}",f"{iteration}"])
            for ec in experiment_configs:

                exp.run(ec)

        finally:
            raise


if __name__ == "__main__":
    main()
