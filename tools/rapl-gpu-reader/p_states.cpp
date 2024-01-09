// GPU info, runs until signal caught

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <nvml.h>
#include <time.h>
#include <sys/sysinfo.h>
#include "Timer.h"
#include "msr_reader.h"

#define likely(x)      __builtin_expect(!!(x), 1) 
#define unlikely(x)    __builtin_expect(!!(x), 0) 

void get_num_devices(unsigned int* num_devices)
{
    nvmlReturn_t status;
    nvmlInit();
    status = nvmlDeviceGetCount(num_devices);
    printf("Device Count Status: %d\n", status);
    printf("Device count: %u\n", *num_devices);
}

void print_dev_info(nvmlDevice_t* devs, unsigned int num_devices)
{
    char version[80];
    nvmlReturn_t status;
    unsigned int graphics_clock;
    unsigned int sm_clock;
    unsigned int mem_clock;
    unsigned int max_graphics_clock;
    unsigned int max_sm_clock;
    unsigned int max_mem_clock;
    unsigned int power_limit;
    unsigned int power_usage;

    for (unsigned int i = 0; i < num_devices; i++)
    {
        printf("gpu %d\n", i);
        nvmlDevice_t* dev = &devs[i];
        status = nvmlSystemGetDriverVersion(version, 80);
        printf("Driver Version Status: %d\n", status);
        status = nvmlDeviceGetHandleByIndex_v2(1, dev);
        printf("Device Get Status: %d\n", status);
        status = nvmlDeviceGetApplicationsClock(*dev, (nvmlClockType_t) 0, &graphics_clock); 
        printf("Graphics Clock Status: %d\n", status);
        status = nvmlDeviceGetApplicationsClock(*dev, (nvmlClockType_t) 1, &sm_clock); 
        printf("SM Clock Status: %d\n", status);
        status = nvmlDeviceGetDefaultApplicationsClock(*dev, (nvmlClockType_t) 2, &mem_clock); 
        printf("Mem Clock Status: %d\n", status);
        status = nvmlDeviceGetMaxClockInfo(*dev, (nvmlClockType_t) 0, &max_graphics_clock); 
        printf("Max Graphics Clock Status: %d\n", status);
        status = nvmlDeviceGetMaxClockInfo(*dev, (nvmlClockType_t) 1, &max_sm_clock); 
        printf("Max SM Clock Status: %d\n", status);
        status = nvmlDeviceGetMaxClockInfo(*dev, (nvmlClockType_t) 2, &max_mem_clock); 
        printf("Max Mem Clock Status: %d\n", status);
        status = nvmlDeviceGetPowerManagementLimit(*dev, &power_limit);
        printf("Power Limit Status: %d\n", status);
        status = nvmlDeviceGetPowerUsage(*dev, &power_usage);
        printf("Power Usage Status%d\n", status);

        printf("\n");

        printf("Device Driver Version: %s\n", version);
        printf("Graphics Clock: %uMHz\n", graphics_clock);
        printf("SM Clock: %uMHz\n", sm_clock);
        printf("Mem Clock: %uMHz\n", mem_clock);
        printf("Max Graphics Clock: %uMHz\n", max_graphics_clock);
        printf("Max SM Clock: %uMHz\n", max_sm_clock);
        printf("Max Mem Clock: %uMHz\n", max_mem_clock);
        printf("Power Limit: %umW\n", power_limit);
        printf("Power Usage: %umW\n", power_usage);
    }
}

volatile unsigned int usr_interrupt = 0;

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        usr_interrupt = 1;
    }
}

__inline__ void checkNvmlErr(nvmlReturn_t result)
{
    if (unlikely(result != NVML_SUCCESS))
    {
        printf("%s\n", nvmlErrorString(result));
    }
}

// TODO
/*
int get_num_sockets()
{
    
}
*/

int main(void)
{
    unsigned int num_devices;
    nvmlReturn_t status;
    unsigned int power_usage;
    // .1 second
    const struct timespec sleep = {0, 100000000};
    unsigned int sm_clock;
    unsigned int mem_clock;
    RAPLSetting setting;
    Timer t;
    get_num_devices(&num_devices);
    nvmlDevice_t devs[num_devices];
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sig_handler;
    act.sa_flags = 0;

    //TODO remove cruft when sockets implemented
    //int sockets = get_num_cores_sockets();
    //printf("cores, sockets: %d, %d\n", cores, sockets);
    //printf("cores: %d\n", get_nprocs());

    print_dev_info(devs, num_devices);
    // first cpu in each socket
    char sockets[64];
    //TODO this assumes 2 sockets more or less; need to fix to be better if use other systems
    sprintf(sockets, "0,%d", get_nprocs()/2);
    //TODO make this work i guess dynamically
    const char* fakeargs[] = {"fake", "-c", sockets};
    //const char* fakeargs[] = {"fake", "-c", "0,"};
    setting.initFromArguments(3, (char**) fakeargs);
    //MSRReaderSet cpu(&setting);
    //int cores = cpu.getNumCoresSampled();
    //float pkg[cores];
    //float dram[cores];
    float pkg[2];
    float dram[2];
    printf("END_HEADER\n");

    sigaction(SIGINT, &act, NULL);

    t.start();
    while (!usr_interrupt)
    {

        for (unsigned int i = 0; i < num_devices; i++)
        {
            status = nvmlDeviceGetPowerUsage(devs[i], &power_usage);
            checkNvmlErr(status);
            status = nvmlDeviceGetClock (devs[i], NVML_CLOCK_SM, NVML_CLOCK_ID_CURRENT, &sm_clock);
            checkNvmlErr(status);
            status = nvmlDeviceGetClock (devs[i], NVML_CLOCK_MEM, NVML_CLOCK_ID_CURRENT, &mem_clock);
            checkNvmlErr(status);
           if (i == 0)
            {
                printf("%f,%f,%d,%d", t.elapsedSeconds(), power_usage/1000.f, sm_clock, mem_clock);
            }
            else

           {
               printf(",%f,%d,%d", power_usage/1000.f, sm_clock, mem_clock);
          }
        } 

        //cpu.read();
		//for(int i=0; i < cores; i++) {
   
		for(int i=0; i < 2; i++) {
			//MSRData& power = cpu.getEnergyData(i);
            pkg[i] = 0;//power.pkg;
            dram[i] = 0;//power.dram;
        }
        
        //for (int i = 0; i < cores; i++)
        for (int i = 0; i < 2; i++)
        {
            printf(",%f,%f", pkg[i], dram[i]);
        }
        printf("\n");
        nanosleep(&sleep, NULL);
    }

    nvmlShutdown();

    return 0;
}
