#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include "cpuinfo.h"

using namespace std;

void split(const string s, string sep, vector<string>& result) {
    result.clear();
    size_t start = 0;
    size_t pos = s.find(sep, start);
    while (pos != string::npos) {
        result.push_back(s.substr(start, pos - start));
        start = pos + 1;
        pos = s.find(sep, start);
    }
    result.push_back(s.substr(start));
}

string strip(const string& s) {
    size_t start = 0;
    for (auto it = s.cbegin(); it != s.cend(); ++it, ++start) {
        char c = *it;
        if (c != ' ' && c != '\t' && c != '\n')
            break; 
    }

    size_t end = s.length();
    for (auto rit = s.rbegin(); rit != s.rend(); ++rit, --end) {
        char c = *rit;
        if (c != ' ' && c != '\t' && c != '\n')
            break; 
    }
    return s.substr(start, end - start);
}

bool get_cpu_info(cpu_info_t& cpuinfo) {
    ifstream fin("/proc/cpuinfo", ifstream::in);
    if (!fin.good()) {
        cerr << "errors in reading /proc/cpuinfo" << endl;
        return false;
    }
    int np = 0;
    vector<int> core_phyical_map;
    for (string line; getline(fin, line, '\n'); ) {
        // cout << line << endl;
        vector<string> sv;
        split(line, ":", sv);
        if (sv.size() >= 2) {
            string k = strip(sv[0]);
            string v = strip(sv[1]);
            if (k == "processor"){
                np++;
            } else if (k == "siblings") cpuinfo.n_cores = stoi(v);
            else if (k == "model") cpuinfo.model = stoi(v);
            else if (k == "cpu family") cpuinfo.cpu_family = stoi(v);
            else if (k == "physical id") {
                core_phyical_map.push_back(stoi(v));
            }
            else if (k == "model name") cpuinfo.model_name = v;
            else if (k == "vendor_id") cpuinfo.vendor = v;
        }
    }
    
    set<int> physical_ids(core_phyical_map.begin(), core_phyical_map.end());
    cpuinfo.n_pkgs = physical_ids.size();
    if (core_phyical_map[0] == core_phyical_map[1])
        cpuinfo.core_mapping = CM_CONTIGUOUS;
    else
        cpuinfo.core_mapping = CM_INTERLEAVED;
    cpuinfo.n_processors = np;
    /*
    cout <<"vendor=" << cpuinfo.vendor << " cpu_family=" << cpuinfo.cpu_family << " model=" 
        << cpuinfo.model << " " << "pkgs=" << cpuinfo.n_pkgs << " cores=" 
        <<  cpuinfo.n_cores << " processor=" << cpuinfo.n_processors << " core_mapping=" << cpuinfo.core_mapping << endl;
    cout <<"model_name=" << cpuinfo.model_name << endl;
    */
    return true;
}

/*
int main() {
    cpu_info_t cpuinfo;

    get_cpu_info(cpuinfo);
    
    cout <<"vendor=" << cpuinfo.vendor << " cpu_family=" << cpuinfo.cpu_family << " model=" 
        << cpuinfo.model << " " << "pkgs=" << cpuinfo.n_pkgs << " cores=" 
        <<  cpuinfo.n_cores << " processor=" << cpuinfo.n_processors << " core_mapping=" << cpuinfo.core_mapping << endl;
    cout <<"model_name=" << cpuinfo.model_name << endl;
}
*/
