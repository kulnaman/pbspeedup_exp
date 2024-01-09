// GPU info, runs until signal caught

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/sysinfo.h>
#include "Timer.h"
#include "msr_reader.h"

#define likely(x)      __builtin_expect(!!(x), 1) 
#define unlikely(x)    __builtin_expect(!!(x), 0) 


volatile unsigned int usr_interrupt = 0;

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        usr_interrupt = 1;
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
    //unsigned int num_devices;
    //unsigned int power_usage;
    // .1 second
    const struct timespec sleep = {0, 100000000};
    //unsigned int mem_clock;
    RAPLSetting setting;
    Timer t;
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sig_handler;
    act.sa_flags = 0;

    //TODO remove cruft when sockets implemented
    //int sockets = get_num_cores_sockets();
    //printf("cores, sockets: %d, %d\n", cores, sockets);
    //printf("cores: %d\n", get_nprocs());
    // first cpu in each socket
    char sockets[64];
    //TODO this assumes 2 sockets more or less; need to fix to be better if use other systems
    sprintf(sockets, "0,%d", get_nprocs()/2);
    //TODO make this work i guess dynamically
    const char* fakeargs[] = {"fake", "-c", sockets};
    //const char* fakeargs[] = {"fake", "-c", "0,"};
    setting.initFromArguments(3, (char**) fakeargs);
    MSRReaderSet cpu(&setting);
    int cores = cpu.getNumCoresSampled();
    float pkg[cores];
    float dram[cores];
    
    printf("END_HEADER\n");

    sigaction(SIGINT, &act, NULL);

    t.start();
    while (!usr_interrupt)
    {
        for (unsigned int i = 0; i < 2; i++)
        {
            if (i == 0)
            {
                printf("%f,%f,%d,%d", t.elapsedSeconds(), 0.0, 0, 0);
            }
            else
            {
                printf(",%f,%d,%d", 0.0, 0, 0);
            }
        } 

        cpu.read();
		for(int i=0; i < cores; i++) {
			MSRData& power = cpu.getEnergyData(i);
            pkg[i] = power.pkg;
            dram[i] = power.dram;
        }
        
        for (int i = 0; i < cores; i++)
        {
            printf(",%f,%f", pkg[i], dram[i]);
        }
        printf("\n");
        nanosleep(&sleep, NULL);
    }
    return 0;
}
