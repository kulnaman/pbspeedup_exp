#!/usr/bin/python3
import errno
import os
import sys
from io import StringIO
import time
import shutil
import nvgpu
import psutil
from enum import Enum, auto
from subprocess import Popen, PIPE
import shlex

from pathlib import Path
import sh


from functools import wraps

toolpath = ""

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def shprint(f):
    @wraps(f)
    def wrapped(*args, **kwargs):
        print(f)
        print(f(*args, **kwargs))
    return wrapped




min_gpu_power = 125
max_gpu_power = 300

cpu_power_step = 5
mem_power_step = 5


def default_SM_freq(gpu):
    if gpu is GPUType.titanv:
        return 1912
    elif gpu is GPUType.titanxp:
        return 1911
    elif gpu is GPUType.v10032:
        return 1530
    elif gpu is GPUType.rtx3060:
        return 1852
    else:
        raise ValueError

def default_memory_freq(gpu):
    if gpu is GPUType.titanv:
        return 850
    elif gpu is GPUType.titanxp:
        return 5705
    elif gpu is GPUType.v10032:
        return 877
    elif gpu is GPUType.rtx3060:
        return 7500
    else:
        raise ValueError


def mem_offset_range(gpu):
    if gpu is GPUType.titanv:
        return range(0,400+1, 50)
    elif gpu is GPUType.titanxp:
        return range(-2000, 1600, 100)
    elif gpu is GPUType.v10032:
        #FIXME i have no idea if it actually mimics titanv
        return range(0,500+1, 50)
    elif gpu is GPUType.rtx3060:
        return range(-1000,3000,200)
    else:
        raise ValueError

def generate_cpu_experiments(benchmark_used=None):
    ret = []
    cpufreq = CPUExperimentConfig.get_max_cpu_freq()
    
    buf = StringIO()
    sh.sudo("../../tools/power_tools/powercap-info", "intel-rapl", "-c", "1", "-z", "0", "-U", _out=buf)
    min_pkg_power = 40
    #max_pkg_power = 150
    #max_pkg_power = 240
    max_pkg_power = int(buf.getvalue())// int(1e6)

    buf = StringIO()
    sh.sudo("../../tools/power_tools/powercap-info", "intel-rapl", "-c", "0", "-z", "0", "-U", _out=buf)
    max_pkg_power_long = int(buf.getvalue()) // int(1e6)
    #max_pkg_power = 270

    #min_dram_power = 30
    #max_dram_power = 120
    buf = StringIO()
    min_dram_power = 5
    sh.sudo("../../tools/power_tools/powercap-info", "intel-rapl", "-c", "0", "-z", "0:0", "-U", _out=buf)
    max_dram_power = int(buf.getvalue()) // int(1e6)
#max_dram_power = 30
    #cpu_ranges = list(range(min_pkg_power, max_pkg_power//2, cpu_power_step)) + list(range(max_pkg_power//2, max_pkg_power, cpu_power_step * 2))
    if benchmark_used==None:
        thread_list=[10,20,30,40]
    elif "hpcc" in benchmark_used:
        thread_list=[40]
    for threads in thread_list:
        for pkg_power in range(95, min_pkg_power - cpu_power_step, -cpu_power_step):
        #for pkg_power in range(135, min_pkg_power - cpu_power_step, -cpu_power_step):
    #for pkg_power in range(max_pkg_power, min_pkg_power - cpu_power_step, -cpu_power_step):
    #for pkg_power in cpu_ranges:
            for dram_power in range(max_dram_power, min_dram_power - mem_power_step, -mem_power_step):
                ret.append(CPUExperimentConfig(cpufreq, pkg_power, dram_power, threads))
    return ret


MIN_GPU_POWER=125
MAX_GPU_POWER=300

spatial_step = 10
def generate_spatial_gpu_experiments(gpu):
    # Default to max CPU frequency
    ret = []
    cpufreq = GPUExperimentConfig.get_max_cpu_freq()
    for perc in range(spatial_step, 101, spatial_step):
        ret.append(GPUExperimentConfig(cpufreq,  default_SM_freq(gpu), default_memory_freq(gpu), 0, 0, MAX_GPU_POWER, perc))
    print (ret)
    return ret

def generate_gpu_experiments(gpu):
    global MIN_GPU_POWER
    global MAX_GPU_POWER

    ret = []
    # Step by 10 except min
    gpu_pow_range = [] # [MIN_GPU_POWER] + list(range(130, MAX_GPU_POWER+1, 10))

    if gpu is GPUType.titanv or  gpu is GPUType.titanxp:
        MIN_GPU_POWER=125
        MAX_GPU_POWER=300
        gpu_pow_range = [MIN_GPU_POWER] + list(range(130, MAX_GPU_POWER+1, 10))
    elif gpu is GPUType.v10032:
        MIN_GPU_POWER=150
        MAX_GPU_POWER=300
        gpu_pow_range = list(range(MIN_GPU_POWER, MAX_GPU_POWER+1, 10))
    elif gpu is GPUType.rtx3060:
        print("GPU TYPE IS RTX")
        MIN_GPU_POWER = 125
        MAX_GPU_POWER = 187
        gpu_pow_range = list(range(MIN_GPU_POWER,MAX_GPU_POWER+1,5))

    cpufreq = GPUExperimentConfig.get_max_cpu_freq()
    # Default to max CPU frequency
    for perc in range(100, 101, 20):
        for gpup in gpu_pow_range:
            for moff in mem_offset_range(gpu):
                # FIXME these numbers are for titanv
                #print(perc)
                ret.append(GPUExperimentConfig(cpufreq, -1, -1, default_SM_freq(gpu), default_memory_freq(gpu), 0, moff, gpup, perc))
    #sys.exit()
    return ret


class GPUType(Enum):
    titanv = auto(),
    titanxp = auto(),
    v10032 = auto(),
    rtx3060= auto()

class Experiment:
    monitor = None
    def __init__(self, appstr, sizestr, classtr, path, args):

        self.appstr = appstr
        self.sizestr = sizestr
        self.classtr = classtr
        if"hpcc" not in self.appstr:
            self.command = sh.Command(path)
        self.path = path
        self.args = args

    def __str__(self):
        return f"{self.appstr}\n{self.path}\n{self.args}\n"


    def run_spatial(self, exp_config, gputype):
        outdir = f"{self.appstr}-{self.sizestr}-{self.classtr}-spatial"
        try:
            os.makedirs(outdir)
        except OSError as e:
            if e.errno != errno.EEXIST:
                raise

        outfile= f"{outdir}/results-{exp_config.pkg_pcap}_{exp_config.dram_pcap}_{exp_config.gpu_pcap}_{exp_config.sm_offset}_{exp_config.gpu_memory_offset}_{exp_config.spatial_perc}.csv"
        if os.path.exists(outfile):
            eprint(f"Skipping {outfile}; already exists")
        else:
            with open(outfile, "w+") as of:
                sh.nvidia_cuda_mps_control("-d")
                time.sleep(2)
                os.environ["CUDA_MPS_ACTIVE_THREAD_PERCENTAGE"] = str(exp_config.spatial_perc)

                of.write(str(self))
                of.write(str(exp_config))
                of.write(nvgpu.gpu_info()[0]['type'] + "\n")
                of.flush()
                exp_config.set_experiment_config(gputype)
                monitor_process = Experiment.monitor(_bg=True, _out=of)
                outp = self.command(*self.args)
                #kill hack because process was started as root
                sh.sudo("kill", "-s", "SIGINT", str(monitor_process.pid))
                sleep(2)
                of.write(f"FOOTER\n{str(outp)}")

                os.system("echo quit | nvidia-cuda-mps-control")
                time.sleep(2)


    def run(self, exp_config):
        outdir = f"{self.appstr}-{self.sizestr}-{self.classtr}"
        try:
            os.makedirs(outdir)
        except OSError as e:
            if e.errno != errno.EEXIST:
                raise
        
        #outfile= f"{outdir}/results-{exp_config.pkg_pcap}_{exp_config.dram_pcap}_{exp_config.gpu_pcap}_{exp_config.sm_offset}_{exp_config.gpu_memory_offset}.csv"
        if "gpu" in self.classtr:
            gputype=get_gpu_type()
            outfile= f"{outdir}/results-{exp_config.pkg_pcap}_{exp_config.dram_pcap}_{exp_config.gpu_pcap}_{exp_config.sm_offset}_{exp_config.gpu_memory_offset}_{exp_config.spatial_perc}.csv"
        else:
            outfile= f"{outdir}/results-{exp_config.pkg_pcap}_{exp_config.dram_pcap}_{exp_config.spatial_perc}.csv"
        print ("Starting", outfile)
        if os.path.exists(outfile):
            eprint(f"Skipping {outfile}; already exists")
        else:
            with open(outfile, "w+") as of:
                if "gpu" in self.classtr:
                    if exp_config.spatial_perc < 100:
                        sh.nvidia_cuda_mps_control("-d")
                        time.sleep(2)
                        os.environ["CUDA_MPS_ACTIVE_THREAD_PERCENTAGE"] = str(exp_config.spatial_perc)
                        of.write(nvgpu.gpu_info()[0]['type'] + "\n")

                else:
                    os.environ["OPENBLAS_NUM_THREADS"] = str(exp_config.spatial_perc)
                    os.environ["OMP_NUM_THREADS"] = str(exp_config.spatial_perc)
                of.write(str(self))
                of.write(str(exp_config))
                of.flush()
                if "gpu" in self.classtr:
                  exp_config.set_experiment_config(gputype)
                else:
                  exp_config.set_experiment_config()
                monitor_process = Experiment.monitor(_bg=True, _out=of)
                if "hpcc" in self.appstr:
                
                    #command=f"mpirun -np {exp_config.spatial_perc} {self.path}"
                    #print(command)
                    #outp=sh.Command(command)
                    process = Popen( shlex.split(f"mpirun -np {exp_config.spatial_perc} {self.path}"), shell=False, universal_newlines=True,
                    stdin=PIPE, stdout=PIPE, stderr=PIPE )
                    output, error = process.communicate();
                    print (output);
                    outp=output
                else:
                    print("test")
                    print(*self.args)
                    outp = self.command(*self.args)
                #kill hack because process was started as root
                
                while psutil.pid_exists(monitor_process.pid):
                    sh.sudo("kill", "-s", "SIGINT", str(monitor_process.pid))
                    time.sleep(1)
                sh.sudo.sync()
                of.write(f"FOOTER\n")
                if "hpcc" not in self.appstr:
                    of.write(f"{str(outp)}")
            if "hpcc" in self.appstr:
                with open(outfile,"a") as of:
                    resultfile="hpccoutf.txt"
                    with open(resultfile,"r") as rf:
                        of.write(rf.read())
                        output_file=Path("~/latest_pbgpu/benchmarks/hpcc-1.5.0/hpccoutf.txt")
                        print(output_file)
                        output_file.unlink(missing_ok=True)
                        
            if "gpu" in self.classtr:
                if exp_config.spatial_perc < 100:
                    os.system("echo quit | nvidia-cuda-mps-control")
                    time.sleep(1)
                    del os.environ["CUDA_MPS_ACTIVE_THREAD_PERCENTAGE"]
            else:
                del os.environ["OPENBLAS_NUM_THREADS"]
                del os.environ["OMP_NUM_THREADS"]
                    
        print ("Finished", outfile)

ONE_HOUR_US = int(60 * 60 * 1E6)
ONE_MIN_US = int(60 * 1E6)
THREE_SEC_US = int(3 * 1E6)



class CPUExperimentConfig:
    def __init__(self, cpu_freq, pkg_pcap, dram_pcap, spatial_perc=100):
        self.cpu_freq = cpu_freq
        self.pkg_pcap = pkg_pcap
        self.dram_pcap = dram_pcap
        self.spatial_perc = spatial_perc

    def __str__(self):
        return f"{self.cpu_freq}\n{self.pkg_pcap}\n{self.dram_pcap}\n{self.spatial_perc}\n"

    def set_experiment_config(self):
        self.set_cpu_freq()
        self.set_cpu_freq()
        self.set_caps()

    def set_cpu_freq(self):
        num_cpus = sh.grep("-c", "processor", "/proc/cpuinfo")
        for i in range(num_cpus):
            with open(f"/sys/devices/system/cpu/cpu{i}/cpufreq/scaling_setspeed", "w+") as of:
                of.write(self.cpu_freq)

    @shprint
    def set_caps(self):
        buf = StringIO()
        sh.sudo(f"{toolpath}/powercap-info", "intel-rapl", "-n", _out=buf)

        for zone in range(0, int(buf.getvalue())):
            sh.sudo(f"{toolpath}/powercap-set", "intel-rapl", "-z", str(zone), "-e", "0")
            if self.pkg_pcap != -1:
                sh.sudo(f"{toolpath}/powercap-set", "intel-rapl", "-z", str(zone), "-c", "0", "-l", str(int(max_pkg_power_long * 1e6)), "-s", str(ONE_HOUR_US))
                sh.sudo(f"{toolpath}/powercap-set", "intel-rapl", "-z", str(zone), "-c", "1", "-l", str(int(self.pkg_pcap * 1e6)), "-s", str(THREE_SEC_US))
                sh.sudo(f"{toolpath}/powercap-set", "intel-rapl", "-z", str(zone), "-e", "1")
            # {zone}:0 should be dram plane generally....
            sh.sudo(f"{toolpath}/powercap-set", "intel-rapl", "-z", f"{zone}:0", "-e", "0")
            if self.dram_pcap != -1:
                sh.sudo(f"{toolpath}/powercap-set", "intel-rapl", "-z", f"{zone}:0", "-c", "0", "-l", str(int(self.dram_pcap * 1e6)), "-s", str(THREE_SEC_US))
                sh.sudo(f"{toolpath}/powercap-set", "intel-rapl", "-z", f"{zone}:0", "-e", "1")

    @staticmethod
    def get_max_cpu_freq():
        return sh.sudo(f"{toolpath}/cpufreq-avail.pl").__str__().strip().split()[0]

    @staticmethod
    def get_default_experiment_config():
        cpu = CPUExperimentConfig.get_max_cpu_freq()
        return CPUExperimentConfig(cpu, -1, -1)

    @staticmethod
    def set_env():
        #mklroot = "/home/share/intel/mkl"
        #os.environ["LD_LIBRARY_PATH"] = os.environ.get("LD_LIBRARY_PATH", "") + f":{mklroot}/lib/intel64"
        #os.environ["LD_RUN_PATH"] = os.environ.get("LD_RUN_PATH", "") + f":{mklroot}/lib/intel64"
        #os.environ["RUN_PATH"] = os.environ.get("RUN_PATH", "") + f":{mklroot}/lib/intel64"
        os.environ["PATH"] = os.environ.get("PATH", "") + f":{os.getcwd()}/../../tools/power_tools/"
        global toolpath
        toolpath = f"{os.getcwd()}/../../tools/power_tools"
        print(f"toolPath is {toolpath}")
        Experiment.monitor = sh.sudo.bake(f"{toolpath}/../rapl-gpu-reader/monitor_cpu")



# FIXME: FIRST GPU ONLY FOR NOW
def get_gpu_type():
    gpu_info = nvgpu.gpu_info()[0]
    gputype = None
    if "TITAN Xp" in gpu_info['type']:
        gputype = GPUType.titanxp
    elif "TITAN V" in gpu_info['type']:
        gputype = GPUType.titanv
    elif "Tesla V100-SXM2-32GB" in gpu_info['type']:
        gputype = GPUType.v10032
    elif "RTX 3060" in gpu_info['type']:
        gputype = GPUType.rtx3060
    else:
        print("Unknown gpu type:", gpu_info)
    print(gputype)
    return gputype

class GPUExperimentConfig:
    def __init__(self, cpu_freq, pkg_pcap, dram_pcap,sm_clock, gpu_mem_clock, sm_offset, gpu_memory_offset, gpu_pcap, spatial_perc=100,gpuid=0):
        self.sm_clock = sm_clock
        self.cpu_freq = cpu_freq
        self.pkg_pcap = pkg_pcap
        self.dram_pcap = dram_pcap
        self.gpuId = gpuid
        self.gpu_mem_clock = gpu_mem_clock
        self.sm_offset = sm_offset
        self.gpu_memory_offset = gpu_memory_offset
        self.gpu_pcap = gpu_pcap
        self.spatial_perc = spatial_perc
    
    def __str__(self):
        return f"{self.cpu_freq}\n{self.pkg_pcap}\n{self.dram_pcap}\n{self.sm_clock}\n{self.gpu_mem_clock}\n{self.sm_offset}\n{self.gpu_memory_offset}\n{self.gpu_pcap}\n{self.spatial_perc}\n"


    def set_experiment_config(self, gputype):
        self.set_sm_clock(gputype)
        self.set_gpu_memclock()
        self.set_sm_offset(gputype)
        self.set_gpu_memclock_offset(gputype)
        self.set_gpu_pcap()


    @shprint
    def set_sm_clock(self, gputype):
        if gputype is GPUType.titanv:
            if self.sm_clock == -1:
                #FIXME one gpu
                return sh.sudo("nvidia-smi", "--cuda-clocks=0")
            else:
                print("Overriding clock")
                return sh.sudo("nvidia-smi", "--cuda-clocks=1")
        pass

    def set_gpu_memclock(self):
        pass

    # TODO: these functions do not support multi-gpu; i.e. they only affect gpu 1
    # also should probably be sudo?
    @shprint
    def set_sm_offset(self, gputype):
        if gputype == GPUType.titanv:
            return sh.nvidia_settings("-a", f"[gpu:{self.gpuId}]/GPUGraphicsClockOffset[2]={self.sm_offset}")
        elif gputype == GPUType.titanxp:
            return sh.nvidia_settings("-a", f"[gpu:{self.gpuId}]/GPUGraphicsClockOffset[3]={self.sm_offset}")
        elif gputype == GPUType.v10032:
            # FIXME
            return sh.nvidia_settings("-a", f"[gpu:{self.gpuId}]/GPUGraphicsClockOffset[2]={self.sm_offset}")
        elif gputype == GPUType.rtx3060:
            return sh.nvidia_settings("-a", f"[gpu:{self.gpuId}]/GPUGraphicsClockOffset[2]={self.sm_offset}")
        else:
            raise ValueError

    #FIXME should probably be sudo
    @shprint
    def set_gpu_memclock_offset(self, gputype):
        print(f"sm_offset {self.gpu_memory_offset}")
        if gputype == GPUType.titanv:
            return sh.nvidia_settings("-a", f"[gpu:{self.gpuId}]/GPUMemoryTransferRateOffset[2]={self.gpu_memory_offset}")
        elif gputype == GPUType.titanxp:
            return sh.nvidia_settings("-a", f"[gpu:{self.gpuId}]/GPUMemoryTransferRateOffset[3]={self.gpu_memory_offset}")
            #raise NotImplementedError
        elif gputype == GPUType.v10032:
            #FIXME
            return sh.nvidia_settings("-a", f"[gpu:{self.gpuId}]/GPUGraphicsClockOffset[2]={self.gpu_memory_offset}")
        elif gputype == GPUType.rtx3060:
            return sh.nvidia_settings("-a", f"[gpu:{self.gpuId}]/GPUMemoryTransferRateOffsetAllPerformanceLevels={self.gpu_memory_offset}")
        else:
            raise ValueError
        pass

    # GPU0 only
    @shprint
    def set_gpu_pcap(self):
        print(f"{self.gpuId}")
        result = sh.sudo("nvidia-smi", "-i", f"{self.gpuId}", "-pl", f"{self.gpu_pcap}")
        return result

    def set_cpu_freq(self):
        num_cpus = sh.grep("-c", "processor", "/proc/cpuinfo")
        for i in range(num_cpus):
            with open(f"/sys/devices/system/cpu/cpu{i}/cpufreq/scaling_setspeed", "w+") as of:
                of.write(self.cpu_freq)

    @staticmethod
    def get_default_experiment_config():
        cpu = GPUExperimentConfig.get_max_cpu_freq()
        return GPUExperimentConfig(cpu, -1, -1, -1, -1, 0, 0, 170)
    @staticmethod
    def get_max_cpu_freq():
        return 4100
        #return sh.sudo(f"{toolpath}/cpufreq-avail").__str__().strip().split()[0]

    def set_env():
        """
        if os.path.exists("/run/user/125/gdm"):
            os.environ["XAUTHORITY"] = "/run/user/125/gdm/Xauthority"
        elif os.path.exists("/run/user/1000/gdm"):
            os.environ["XAUTHORITY"] = "/run/user/1000/gdm/Xauthority"
        else:
            print("Couldn't find XAUTHORITY auth key - quitting")
            sys.exit()
        """
        if os.path.exists("/var/run/lightdm"):
            os.environ["XAUTHORITY"] = "/var/run/lightdm/root/:0"
        elif os.path.exists("/var/run/slim.auth"):
            os.environ["XAUTHORITY"] = "/var/run/slim.auth"
        else:
            print("Couldn't find XAUTHORITY auth key - quitting")
            sys.exit()
        os.environ["DISPLAY"] = ":0"
        os.environ["PATH"] = os.environ.get("PATH", "") + f":{os.getcwd()}/../../tools/power_tools/"
        sh.sudo.xhost("+")
        global toolpath
        toolpath = f"{os.getcwd()}/../../tools/power_tools"
        Experiment.monitor = sh.sudo.bake(f"node-stats",["-g","-c","-p","1"])
