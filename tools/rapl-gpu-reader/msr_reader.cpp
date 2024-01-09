#include <cstdio>
#include <bitset>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include "msr_reader.h"
#include "cpuid.h"
#include "cpuinfo.h"

using namespace std;


extern int errno; //errno defined in <cerrno>

MSRReader::MSRReader(unsigned long core_id) : mCoreId{core_id}, 
                                    mFd{0}, mTmp{0}, 
                                    mPower{(MSRData){0,0,0,0}}, 
                                    mEnergy{(MSRData){0,0,0,0}}, 
                                    currEnergy{&mData[0]}, prevEnergy{&mData[1]}, 
                                    sPowerUnits{0.0}, sPkgEnergyUnits{0.0}, 
                                    sDramEnergyUnits{0.0}, sTimeUnits{0.0}
{
    mData[0] = (MSRData){0,0,0,0};
    mData[1] = (MSRData){0,0,0,0};
	char msr_filename[MAX_MSR_FILENAME_LEN];
	sprintf(msr_filename, "/dev/cpu/%ld/msr", mCoreId);
    mFd = open(msr_filename, O_RDONLY);
	if (mFd == 0) {
		fprintf(stderr, "could not open file - %s\n", msr_filename);
		throw MSRException(mCoreId, errno, __FILE__, __LINE__);
	}
    MSRReader::init();

}

MSRReader::MSRReader() : MSRReader::MSRReader{0} {}

MSRReader::~MSRReader(){
	if (mFd != 0) {
		close(mFd);
	}
}

MSR_DATA_T MSRReader::readMSRDate(int which){
	  if ( pread(mFd, &mTmp, sizeof(mTmp), which) != sizeof(mTmp)) {
		  fprintf(stderr, "pread error in %s:%d\n", __FILE__, __LINE__);
		  throw MSRException(mCoreId, errno, __FILE__, __LINE__);
	  }
	  return (MSR_DATA_T)mTmp;
}

void MSRReader::readEnergyData(){
	MSRData* tmp = prevEnergy;  prevEnergy=currEnergy;  currEnergy= tmp;

	currEnergy->time = MSRReaderSet::getCurrentTime();
	if ( pread(mFd, &mTmp, sizeof(mTmp), MSR_PKG_ENERGY_STATUS) != sizeof(mTmp)) {
		fprintf(stderr, "pread error in %s:%d\n", __FILE__, __LINE__);
		throw MSRException(mCoreId, errno, __FILE__, __LINE__);
	}
	currEnergy->pkg = MSRReader::sPkgEnergyUnits * (double)mTmp;

	if ( pread(mFd, &mTmp, sizeof(mTmp), MSR_PP0_ENERGY_STATUS)  != sizeof(mTmp)) {
		fprintf(stderr, "pread error in %s:%d\n", __FILE__, __LINE__);
		throw MSRException(mCoreId, errno, __FILE__, __LINE__);
	};
	currEnergy->pp0 = MSRReader::sPkgEnergyUnits * (double)mTmp;
/*
	if ( pread(mFd, &mTmp, sizeof(mTmp), MSR_PP1_ENERGY_STATUS)  != sizeof(mTmp)) {
		throw MSRException(mCoreId, errno, __FILE__, __LINE__);
	}
	currValue->pp1 = MSRReader::sEnergyUnits * (double)mTmp;
*/
	if ( pread(mFd, &mTmp, sizeof(mTmp), MSR_DRAM_ENERGY_STATUS)  != sizeof(mTmp)) {
		fprintf(stderr, "pread error in %s:%d\n", __FILE__, __LINE__);
		throw MSRException(mCoreId, errno, __FILE__, __LINE__);
	}
	currEnergy->dram = MSRReader::sDramEnergyUnits * (double)mTmp;
}

void MSRReader::print(){
	cout << "[" << mCoreId << "] " << prevEnergy->pkg << "\t" << prevEnergy->pp0 << "\t";
	/* cout << currValue->pp1_energy << "\t" */
	cout << prevEnergy->dram << "\n";
}

static void printMSRData(MSR_DATA_T d) {
    bitset<sizeof(MSR_DATA_T) << 3> bits;
    for (size_t i = 0; i < (sizeof(MSR_DATA_T) << 3); i++) {
        if ( (d >> i) & 0x1) {
            bits[i] = 1;
        }
    }    
    for (int i = (sizeof(MSR_DATA_T) << 3)  -1; i >= 0; i--) {
        cout << i%10;
    }
    cout << endl;
    cout << bits.to_string() << endl;
}

void MSRReader::init() {
	//if (!sInitialized) {
                cpu_info_t cpuinfo;
                get_cpu_info(cpuinfo);

                cout << "cpu_info: vendor=" << cpuinfo.vendor << " model_name=" << cpuinfo.model_name << endl;
                cout << "\tcpu_family=" << cpuinfo.cpu_family << " model=" << cpuinfo.model << endl;
                cout << "\tnpkgs=" << cpuinfo.n_pkgs 
                     << "\tcores_per_pkg=" << cpuinfo.n_cores << " core_mapping=" << cpuinfo.core_mapping << endl;
                uint32_t ps;
                ps = get_processor_signature();
                uint32_t model_id = ((ps >> 4) & 0x0f) | ((ps >> 12) & 0xf0);
                uint32_t cpu_family = (ps >> 8) & 0x0f;
                cout << "\tprocessor signature=0x" << hex << setfill('0') << setw(6) << ps << " model_id=0x" << model_id << endl;
                cout << setfill(' ');
                cout << endl;

		MSR_DATA_T result = readMSRDate(MSR_RAPL_POWER_UNIT);
        printMSRData(result);
		sPowerUnits = pow(0.5, (double) (result & 0xf));
		sPkgEnergyUnits = pow(0.5, (double) ((result >> 8) & 0x1f));
		sDramEnergyUnits = pow(0.5, (double) ((result >> 8) & 0x1f));

                if (cpu_family == 0x06) {
                    if (model_id == 0x2d  /* Xeon E5 v1,  Sandy Bridge, table 2-23*/
                        || model_id == 0x3e /* Xeon E5 v2, Ivy Bridge-E, table 2-26 */
                        || model_id == 0x57 || model_id == 0x85 /* Xeon Phi, table 2-46 */ 
                        || model_id == 0x3c || model_id == 0x45 || model_id == 0x46 /* Xeon E3 v3, Table 2-29, Haswell-E` */ ){
                        sDramEnergyUnits = pow(0.5, (double) ((result >> 8) & 0x1f));
                    } else if (model_id == 0x3f /* Xeon E5 v3, Haswell-E, table 2-32 */
                            || model_id == 0x56 || model_id == 0x4f /* Xeon E5 v4 and Xeon D based on Broadwell, table 2-36 */
                            || model_id == 0x55 /* Xeon Scalable, table 2-45 */) {
                        sDramEnergyUnits = pow(0.5, (double) (0x10));
                    } else {
                        cerr << "CPU model" << model_id << "is not supported by this program." << endl;
                        throw MSRException(0, 0, __FILE__, __LINE__);
                    }
                } else {
                        cerr << "CPU model" << model_id << "is not supported by this program." << endl;
                        throw MSRException(0, 0, __FILE__, __LINE__);
                }
		sTimeUnits = pow(0.5, (double) ((result >> 16) & 0xf));
	//}
}

MSRReaderSet::MSRReaderSet(RAPLSetting* setting) : readers{new MSRReader*[setting->num_cores_sampled]}, config{setting}
{
	//config = setting;
	//readers = new MSRReader*[config->num_cores_sampled];
	for (int i=0; i < setting->num_cores_sampled; i++){
		readers[i] = new MSRReader(setting->cores_sampled[i]);
	}

	log.open(config->log_filename.c_str());
	if ( log.bad() ){
		cerr << "Create log file " << config->log_filename << " failed at " << __FILE__ << __LINE__ <<endl;
		exit(-1);
	}
}

MSRReaderSet::~MSRReaderSet(){
	if (readers) {
		for (int i=0; i<config->num_cores_sampled; i++){
			delete readers[i];
		}
		delete[] readers;
	}
	if (log.is_open())
		log.close();
}

int MSRReaderSet::getNumCoresSampled()
{
    return config->num_cores_sampled;
}

MSRData& MSRReaderSet::getEnergyData(int index)
{
    return readers[index]->getPower();
}

void MSRReaderSet::read()
{
    for(int i=0; i<config->num_cores_sampled; i++) {
        readers[i]->readEnergyData();
    }
}

void MSRReaderSet::doWork(){
	//initial read
	currTime = getCurrentTime();
	for(int i=0; i<config->num_cores_sampled; i++) {
		readers[i]->readEnergyData();
	}

	//output the header
	log << setw(14) << "#Time";
	for(int i=0; i<config->num_cores_sampled; i++) {
		log << setw(6) << "Core" << setw(10) << "P_PKG" << setw(10) <<"P_PP0" << setw(10) << "P_DRAM";
	}
	log << endl;

	if (config->log_to_console) {
		cout << setw(14) << "#Time";
		for(int i=0; i<config->num_cores_sampled; i++) {
			cout << setw(6) << "Core" << setw(10) << "P_PKG" << setw(10) <<"P_PP0" << setw(10) << "P_DRAM";
		}
		cout << endl;
	}
	

	for(;;) {
		//sleep for an interval
		usleep(config->sample_interval_in_useconds);

		//read current data
		prevTime = currTime; currTime = getCurrentTime();
		for(int i=0; i<config->num_cores_sampled; i++) {
			readers[i]->readEnergyData();
		}

		//output data
		log << fixed << showpoint << setprecision(2) << setw(14) << currTime;
		for(int i=0; i<config->num_cores_sampled; i++) {
			MSRData& power = readers[i]->getPower();
			log << setw(6) << readers[i]->getCoreId() << setw(10) << power.pkg << setw(10) << power.pp0 << setw(10) << power.dram;
		}
		log << endl;
		
		if (config->log_to_console) {
			cout << fixed << showpoint << setprecision(2) << setw(14) << currTime;
			for(int i=0; i<config->num_cores_sampled; i++) {
				MSRData& power = readers[i]->getPower();
				cout << setw(6) << readers[i]->getCoreId() << setw(10) << power.pkg << setw(10) << power.pp0 << setw(10) << power.dram;
			}
			cout << endl;
		}
	}
}

