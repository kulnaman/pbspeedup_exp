#ifndef _CPUINFO_H
#define _CPUINFO_H
#include <string>
#include <vector>

enum core_map_type {
    CM_CONTIGUOUS = 1,
    CM_INTERLEAVED = 2
};

typedef struct cpu_info {
    int n_pkgs;
    int n_cores;
    int n_processors;
    int cpu_family;
    int model;
    int core_mapping;
    std::string vendor;
    std::string model_name;
} cpu_info_t;

void split(const std::string& s, std::string& sep, std::vector<std::string>& result);
std::string strip(const std::string& s);
bool get_cpu_info(cpu_info_t& cpuinfo);

#endif
