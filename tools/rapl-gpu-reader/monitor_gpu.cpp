
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <nvml.h>
#include <time.h>
#include "Timer.h"

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

void print_dev_info(unsigned int* num_devices, nvmlDevice_t* dev)
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

    status = nvmlSystemGetDriverVersion(version, 80);
    printf("Driver Version Status: %d\n", status);
    status = nvmlDeviceGetCount(num_devices);
    printf("Device Count Status: %d\n", status);
    status = nvmlDeviceGetHandleByIndex(0, dev);
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
    printf("Device count: %u\n", *num_devices);
    printf("Graphics Clock: %uMHz\n", graphics_clock);
    printf("SM Clock: %uMHz\n", sm_clock);
    printf("Mem Clock: %uMHz\n", mem_clock);
    printf("Max Graphics Clock: %uMHz\n", max_graphics_clock);
    printf("Max SM Clock: %uMHz\n", max_sm_clock);
    printf("Max Mem Clock: %uMHz\n", max_mem_clock);
    printf("Power Limit: %umW\n", power_limit);
    printf("Power Usage: %umW\n", power_usage);
    printf("END_HEADER\n");
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

int main(void)
{
    sigset_t mask;
    sigset_t oldmask;
    nvmlDevice_t dev;
    nvmlReturn_t status;
    unsigned int num_devices;
    unsigned int power_usage;
    // .1 second
    const struct timespec sleep = {0, 100000000};
    unsigned int sm_clock;
    unsigned int mem_clock;
    Timer t;

    nvmlInit();

    print_dev_info(&num_devices, &dev);

    signal(SIGINT, sig_handler);
    sigprocmask (SIG_BLOCK, &mask, &oldmask);
    t.start();
    while (!usr_interrupt)
    {
        status = nvmlDeviceGetPowerUsage(dev, &power_usage);
        checkNvmlErr(status);
        status = nvmlDeviceGetClock (dev, NVML_CLOCK_SM, NVML_CLOCK_ID_CURRENT, &sm_clock);
        checkNvmlErr(status);
        status = nvmlDeviceGetClock (dev, NVML_CLOCK_MEM, NVML_CLOCK_ID_CURRENT, &mem_clock);
        checkNvmlErr(status);

        printf("%f,%f,%d,%d\n", t.elapsedSeconds(), power_usage/1000.f, sm_clock, mem_clock);
        nanosleep(&sleep, NULL);
    }
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    nvmlShutdown();

    return 0;
}

