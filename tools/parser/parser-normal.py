#!/usr/bin/python3
import argparse
import os
import sys
from glob import glob
import errno
from statistics import mean

# Reads its OWN CSV
def get_experiments(fname):
    with open(fname, 'r') as of:
        # split each in line in file into components and unpack them for constructor
        return [Experiment.parse_csv_line(line.split(',')) for line in of.readlines()[1:]]

class Experiment:
    def __init__(self, appstr, time,time_app, cpu_freq, pkg_pcap, dram_pcap, avg_pkg_pow, avg_dram_pow, gpu_pcap, 
                       avg_gpu_pow, avg_sm_freq, avg_mem_freq, sm_clock, gpu_mem_clock, sm_offset, mem_offset, 
                       spatial_perc, inf, perf=-1,):
        self.appstr = appstr
        self.time = float(time)
        self.time_app=float(time_app)
        self.cpu_freq = int(cpu_freq)
        self.pkg_pcap = int(pkg_pcap)
        self.dram_pcap = int(dram_pcap)
        self.avg_pkg_pow = float(avg_pkg_pow)
        self.avg_dram_pow = float(avg_dram_pow)
        self.gpu_pcap = int(gpu_pcap)
        self.avg_gpu_pow = float(avg_gpu_pow)
        self.avg_sm_freq = float(avg_sm_freq)
        self.avg_mem_freq = float(avg_mem_freq)
        self.sm_clock = int(sm_clock)
        self.gpu_mem_clock = int(gpu_mem_clock)
        self.sm_offset = int(sm_offset)
        self.mem_offset = int(mem_offset)
        self.spatial_perc = int(spatial_perc)
        self.file = inf
        self.perf = float(perf)

    @staticmethod
    def parse_csv_line(csv_str):
        return Experiment(*csv_str)

    @staticmethod
    def csv_header():
        return "appstr,time,time_app,cpu_freq,pkg_pcap,dram_pcap,avg_pkg_pow,avg_dram_pow,gpu_pcap,avg_gpu_pow,avg_sm_freq,avg_mem_freq,sm_clock,gpu_mem_clock,sm_offset,mem_offset,spatial_perc,inf,perf"

    def __str__(self):
        return f"{self.appstr},{self.time},{self.time_app},{self.cpu_freq},{self.pkg_pcap},{self.dram_pcap},{self.avg_pkg_pow},{self.avg_dram_pow},{self.gpu_pcap},{self.avg_gpu_pow},{self.avg_sm_freq},{self.avg_mem_freq},{self.sm_clock},{self.gpu_mem_clock},{self.sm_offset},{self.mem_offset},{self.spatial_perc},{self.file},{self.perf}"


def avg(li):
    return sum(li)/len(li)


# threshold of power consumption at which we consider the gpu to be "active"
# should be at least 15 to account for fan and idle power
POWER_THRESH = 100 #110


cublas_table = {16384: 8796093022208 * 50}
fft_table = {80000000: 232179628032 * 150}
stream_table = {"3.75": 3 * 4 * 1006632960 * 1000}
cloverleaf_table = {"tp5" : 3840 * 3840} # should this be +?
hpcg_table = {"128x128x128" : 93189454650}
minife_table = {"512x512x96" : 1.70716e+12 }

yaxis_label = {"cublas": "TFLOPS",
               "stream": "GB/s",
               "CUFFT" : "TFLOPS",
               "HPCG" : "GFLOPs",
               "miniFE" : "TFLOPS",
               "Cloverleaf" : "Cells/Second",
               "dgemm":"TFLOPS"}


def get_perf(appstr, lines,time):
    for line in lines:
        if "dgemm" in appstr:
            if "GPU Performance" in line:
                # SYS Performance =     135.00 GFLOPS/s
                print(line.split())
                return float(line.split()[2])/ 1000
        elif "miniFE" in appstr:
            if "Total_CG_Mflops" in line:
                return float(line.split()[1]) / 1e6
        elif "clover_leaf" in appstr:
            cells=0
            steps=1000
            cells=cloverleaf_table["tp5"]
            return steps*cells / time
        elif "cufft" in appstr:
            flops=fft_table[80000000]
            return flops / time / 1e12
        elif "stream" in appstr:
            eles=stream_table["3.75"]
            return eles / time / 1e9
        elif "HPCG" in appstr:
            if "final" in line:
                return float(line.split()[2]) / 1000
        elif "cublas" in appstr:
            flops=cublas_table[16384]
            print(line.split())
            if "Performance" in line:
                return float(line.split()[1]) / 1e-3
        else:
            eprint("unknown app")

def time_report_bench(appstr,lines,iteration=1):
    app_not_total_time = ['cufft','stream'] 
    # HPCG Total Time
    # cublas Time * iter
    # DGEMM Total_time
    # MINIFE looking into time reported by Total_CG_Mflops
    for line in lines:
        if "dgemm" in appstr:
            if "GPU Performance" in line:
                # SYS Performance =     135.00 GFLOPS/s
                print("*****")
                print(float(line.split()[5]))
                return int(iteration) * float(line.split()[5]) * 1e-3
        if any(x in appstr for x in app_not_total_time):
            return 0
        elif "miniFE" in appstr:
            if "making matrix indices" in line:
                print(line.split()[-1])
                return float(line.split()[-1]) 
        elif "HPCG" in appstr:
            if "Total Time" in line:
                print(line.split()[-2])
                return float(line.split()[-2])
        elif "cublas" in appstr:
            if "Performance" in line:
                return int(iteration) * float( line.split()[4] ) * 1e-3


#TODO gonna assume two packages for now because of pain
# accepts BULK-GENERATED CSVs, not its own output
def parse_input(exps, files, outdir):
    figname = None
    for inf in files:
        print (f"Reading {inf}")
        i = 0
        pl = 0
        lines = None
        # time (s), power (w), sm clock (mhz), mem_clock (mhz)
        with open(inf, 'r') as foo:
            lines = foo.readlines()
        #2201000 / 1000 = 2201; 2.201 ghz i guess so mhz
        appstr = lines[0].strip()
        figname = lines[0].strip()
        title = lines[0].strip()
        iteration = 1
        if appstr == "dgemm":
            lines[2]=lines[2][1:-2]
            # print(lines[2][-2])
            print(lines[2].strip().split(","))
            print(lines[2][-3:-1]) 
            iteration = lines[2][-3:-1]
        elif appstr == "cublas":
            iteration = 50
        cpu_freq = int(lines[3].strip()) / 1000.0
        pkg_pcap = int(lines[4].strip())
        dram_pcap = int(lines[5].strip())
        sm_clock = int(lines[6].strip())
        gpu_mem_clock = int(lines[7].strip())
        sm_offset = int(lines[8].strip())
        mem_offset = int(lines[9].strip())
        gpu_pcap = int(lines[10].strip())
        # FIXME i think this is right
        spatial_perc = int(lines[11].strip())
        gpus = int(lines[14].split()[2].strip())


        while "END_HEADER" not in lines[i]:
            #if "Power Limit:" in lines[i]:
                # split by colon, take second half, remove whitespace, remove "mW" postfix
            #    pl = int(int(lines[i].split(":")[1].strip()[:-2]) / 1000.0)
            i += 1
        i += 1

        times = []
        cpu_powers = []
        gpu_powers = []
        dram_powers = []
        sms = []
        mems = []
        time_offset = -1
        perf = -1
        for k, line in enumerate(lines[i:]):
            if "FOOTER" not in line:
                lsplit = line.split(",")
                #time = float(lsplit[0]) - time_offset
                gpu_power = []
                m_sm_clock = []
                m_mem_clock = []
                pkg_pows = []
                dram_pows = []
                sockets = 2
                #TODO we only support one gpu after this
                for dev in range(gpus):
                    gpu_power.append(float(lsplit[1 + dev * 3]))
                    m_sm_clock.append(float(lsplit[2 + dev * 3]) / 1000.0)
                    
                    m_mem_clock.append(float(lsplit[3 + dev * 3]) / (1e+3*1e+2))
                base = gpus * 3
                for cpu in range(sockets):
                    pkg_pows.append(float(0))
                    dram_pows.append(float(0))
                time=lsplit[0]
                program_fin=False
                if float(avg(gpu_power)) > POWER_THRESH and not program_fin:
                    if time_offset == -1:
                        time_offset = float(time)
                    times.append(float(time) - time_offset)
                    gpu_powers.append(gpu_power)
                    sms.append(m_sm_clock)
                    mems.append(m_mem_clock)
                    cpu_powers.append(pkg_pows)
                    dram_powers.append(dram_pows)
                elif time_offset != -1 and float(avg(gpu_power))<POWER_THRESH and not program_fin:   
                    avg_list=[]  
                    for j in range(0,4):
                        if"FOOTER" in lines[i:][k + j]:
                            break 
                        else:
                            avg_list.append(float(lines[i:][k + j].split(",")[4]))
                    if(avg(avg_list)>POWER_THRESH):
                        times.append(float(time) - time_offset)
                        gpu_powers.append(gpu_power)
                        sms.append(m_sm_clock)
                        mems.append(m_mem_clock)
                        cpu_powers.append(pkg_pows)
                        dram_powers.append(dram_pows)
                    else:
                        program_fin=True
            elif "FOOTER" in line:
                perf = get_perf(appstr, lines[i+k:],times[-1])
                print(f"iteration {iteration}")
                time_app = time_report_bench(appstr,lines[i+k:],iteration)
                print(time_app)
                break
        print(avg([m[0] for m in mems]))
        exps.append(Experiment(appstr, times[-1],
                                       time_app,
                                       cpu_freq,
                                       pkg_pcap,
                                       dram_pcap,
                                       avg([sum(a) for a in cpu_powers]),
                                       avg([sum(a) for a in dram_powers]),
                                       gpu_pcap, 
                                       avg([g[0] for g in gpu_powers]), 
                                       avg([s[0] for s in sms]), 
                                       avg([m[0] for m in mems]), 
                                       sm_clock, gpu_mem_clock, sm_offset, mem_offset, spatial_perc, inf, perf))
        if perf == -1:
            print("no perf?")
            sys.exit(1)

    csvf = os.path.join(outdir, figname + ".csv")
    print(f"Writing CSV file: {csvf}")
    with open(csvf, 'w+') as of:
        of.write(Experiment.csv_header() + "\n")
        for e in exps:
            of.write(str(e) + "\n")


def emain():
    parser = argparse.ArgumentParser(description="plot one text")
    parser.add_argument('files',  type=str, nargs="+", help='input file path')
    parser.add_argument('-o',  type=str, default="./", help='outdir')

    args = parser.parse_args()
    # will capture all files from directory IF regex
    if len(args.files) == 1:
        args.files = glob(args.files[0])

    try:
        os.makedirs(args.o)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise

    print(os.getcwd())
    exps = []
    files = args.files

    assert(len(files) > 0)
    parse_input(exps, files, args.o)


if __name__ == "__main__":
    emain()
