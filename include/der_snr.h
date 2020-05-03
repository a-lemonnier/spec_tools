#ifndef DER_SNR_H
#define DER_SNR_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <functional> 
#include <thread>
#include <future>
#include <tuple>
#include <chrono>

#include <boost/program_options.hpp>

#if __has_include (<boost/timer/timer.hpp>)
#include <boost/timer/timer.hpp> 
#define HAS_BOOST_TIMER /**< boost::timer availability */
#endif

#if __has_include (<sys/syscall.h>)
#include <sys/syscall.h>
#define HAS_SYSCALL /**< linux syscall availability */
#endif

#if __has_include (<filesystem>)
#include <filesystem>
#define FS_STD /**< std::filesystem availability (C++17) */
namespace fs = std::filesystem;
#elif __has_include (<experimental/filesystem>) && !__has_include (<filesystem>)
#include <experimental/filesystem>
#define FS_STDEXP /**< std::experimental::filesystem availability */
namespace fs = std::experimental::filesystem;
#elif __has_include(<boost/filesystem.hpp>) && !__has_include (<filesystem>) && !__has_include (<experimental/filesystem>)
#include <boost/filesystem.hpp>
#define FS_BOOST /**< boost::filesystem availability */
namespace fs = boost::filesystem;
#else
#error "No filesystem header found"
#endif

#include <csv.h>
#include <msg.h>
#include <log.h>
#include <der_snr.h>

#define LOGFILE ".der_snr.log" /**< Define the default logfile  */
#define HISTFILE ".history" /**< Define the default histfile (shared)  */

// Reference
// ----------------------------------------------------
// F. Stoehr et al: DER_SNR: A Simple & General Spectroscopic Signal-to-Noise Measurement Algorithm
// 394, Astronomical Data Analysis Software and Systems (ADASS) XVII
// 2008ASPC..394..505S
// ----------------------------------------------------

// Prototypes
// ----------------------------------------------------
/**
 * \fn void compute(const std::vector<std::string>& list, const std::string& sOutput)
 * \brief Compute S/N for all the string in the vector of strings. Default sep is tab. Used in the multithreaded mode. 
 * \param list list of files
 * \param sOutput output filename
 */
void compute(const std::vector<std::string>& list, const std::string& sOutput);

/**
 * \fn void compute_sep(const std::vector<std::string>& list, const std::string& sOutput, const char& cSep)
 * \brief Compute S/N for all the string in the vector of strings. Used in the multithreaded mode.
 * \param list list of files
 * \param sOutput output filename
 * \param cSep char separator
 */
void compute_sep(const std::vector<std::string>& list, const std::string& sOutput, const char& cSep);

/**
 * \fn bool merge(const std::string &sPattern)
 * \brief Merge files from threads following a filename pattern, i.e. the given output name.
 * \param sPattern basename without ext
 */
bool merge(const std::string &sPattern);

/**
 * \fn bool write(std::vector<std::string> vsResults, const std::string& sOutput)
 * \brief Write on disk results with the default sep.
 * \param vsResults data to write
 * \param sOutput output filename
 */
bool write(std::vector<std::string> vsResults, const std::string& sOutput);

/**
 * \fn bool write(std::vector<std::string> vsResults, const std::string& sOutput, const char& cSep)
 * \brief Write on disk results
 * \param vsResults data to write
 * \param sOutput output filename
 * \param cSep char separator
 */
bool write(std::vector<std::string> vsResults, const std::string& sOutput, const char& cSep);

/**
 * \fn float der_snr(const std::vector<float> &vFlux)
 * \brief Compute the S/N with der_snr method.
 * \param vFlux flux vector
 * \return -1 if error happens
 */
float der_snr(const std::vector<float> &vFlux);
double der_snr(const std::vector<double> &vFlux);

/**
 * \fn float median(const std::vector<float> &vFlux)
 * \brief Simple computation of the median
 * \param vFlux flux vector
 * \return 0 if error happens
 */
float median(const std::vector<float> &vFlux);
double median(const std::vector<double> &vFlux);

/**
 * \fn double long CPU_utilization()
 * \brief Get the CPU usage (%)
 */
double long CPU_utilization();

std::tuple<double long, double long> get_stat();

// ----------------------------------------------------
// ----------------------------------------------------

void compute(const std::vector<std::string>& vsList, const std::string& sOutput) {

    _msg msgM;
    msgM.set_name("compute()");
    msgM.set_threadname("compute");
    msgM.set_log(LOGFILE);
    
#ifdef HAS_SYSCALL
    msgM.msg(_msg::eMsg::THREADS, "compute S/N for", vsList.size(), "files");
#else
    msgM.msg(_msg::eMsg::MID, "compute S/N for", vsList.size(), "files\n";
#endif
    
    std::vector<std::string> vsResults;
    //vsResults.emplace_back("File\tSNR\n");
    
    for(auto sFile: vsList) {
        
        _csv<float> csv(sFile, '\t');
        
        if(csv.read()) {
            
            csv.set_verbose(_csv<float>::eVerbose::QUIET);
            
            vsResults.emplace_back(sFile+"\t"+std::to_string(der_snr(csv.select_column(1))) + "\n");
        }
    }
    write(vsResults, sOutput);
}

void compute_sep(const std::vector<std::string>& vsList, const std::string& sOutput, const char& cSep) {
    std::vector<std::string> vsResults;
    
    _msg msgM;
    msgM.set_name("compute()");
    msgM.set_threadname("compute");
    msgM.set_log(LOGFILE);
    
#ifdef HAS_SYSCALL
    msgM.msg(_msg::eMsg::THREADS, "compute S/N for", vsList.size(), "files");
#else
    msgM.msg(_msg::eMsg::MID, "compute S/N for", vsList.size(), "files\n";
#endif
    
//     vsResults.emplace_back("File\tSNR\n");    
    
    for(auto sFile: vsList) {
        
        _csv<float> csv(sFile, cSep);
        
        if(csv.read()) {
            
            csv.set_verbose(_csv<float>::eVerbose::QUIET);
            
            vsResults.emplace_back(sFile+"\t"+std::to_string(der_snr(csv.select_column(1))) + "\n");
        }
    }
    write(vsResults, sOutput);
}

bool write(std::vector<std::string> vsResults, const std::string& sOutput) {
    bool sStatus=true;
    
    std::fstream fFlux(sOutput, std::ios::out);
    
    if (fFlux) {
        for(auto sS: vsResults)
            fFlux << sS;
        
        fFlux.close();
    }
    else {
        _msg msgM;
        msgM.set_name("write()");
        msgM.set_log(LOGFILE);
        
        msgM.msg(_msg::eMsg::ERROR, "cannot open", sOutput);
        sStatus=false;
    }
    return sStatus;
}

bool merge(const std::string &sPattern) {
    _msg msgM;
    msgM.set_name("merge()");
    msgM.set_log(LOGFILE);
        
    std::fstream fFlux0(sPattern, std::ios::out);
    
    if (fFlux0) {
        int iCount=1;
        fs::path pTmp("part"+std::to_string(iCount)+"_"+sPattern);
        while(fs::exists(pTmp)) {
            msgM.msg(_msg::eMsg::MID, "merge file", pTmp.string());
            
            std::fstream fFlux1(pTmp.string(), std::ios::in);
            std::string sLine;
            
            while(std::getline(fFlux1, sLine))
                fFlux0 << sLine << std::endl;
            fFlux1.close();
            
            fs::remove(pTmp);
            iCount++;
            pTmp=fs::path("part"+std::to_string(iCount)+"_"+sPattern);
        }
        fFlux0.close();
    }
    else {
        msgM.msg(_msg::eMsg::ERROR, "cannot open", sPattern);
        return false;
    }
    return true;    
}

float median(const std::vector<float> &vFlux) {    
    int iSize=vFlux.size();
    
    if (iSize==0) {
        _msg msgM;
        msgM.set_name("median()");
        msgM.set_log(LOGFILE);
        msgM.msg(_msg::eMsg::MID, "error: flux is empty");
        return 0;
    }
    
    std::vector<float> vVec(vFlux);
    std::sort(vVec.begin(), vVec.end());
    
    if (iSize%2==0)     
        return (vVec[iSize/2-1]+vVec[iSize/2])/2;
    else
        return vVec[iSize/2];
}

double median(const std::vector<double> &vFlux) {
    int iSize=vFlux.size();
    
    if (iSize==0) {
        _msg msgM;
        msgM.set_name("median()");
        msgM.set_log(LOGFILE);
        msgM.msg(_msg::eMsg::MID, "error: flux is empty");
        return 0;
    }
    
    std::vector<double> vVec(vFlux);
    std::sort(vVec.begin(), vVec.end());
    
    if (iSize%2==0)
        return (vVec[iSize/2-1]+vVec[iSize/2])/2;
    else
        return vVec[iSize/2];
    
    return 0;
}


float der_snr(const std::vector<float> &vFlux) {
    if (vFlux.empty()) {
        _msg msgM;
        msgM.set_name("der_snr()");
        msgM.set_log(LOGFILE);
        msgM.msg(_msg::eMsg::MID, "error: flux is empty");
        return 0;
    }
    
    if (vFlux.size()>4) {
        std::vector<float> vNo0(vFlux);
        
        vNo0.erase(std::remove_if(vNo0.begin(), vNo0.end(), 
                                   [&](float fF) { return fF<0; }),vNo0.end());
        
        int iSize=vNo0.size();
        
        std::vector<float> vSum; vSum.resize(iSize);
        std::vector<float> vSum0=std::vector<float>(vNo0.begin()+2, vNo0.end()-2);
        std::vector<float> vSum1=std::vector<float>(vNo0.begin()  , vNo0.end()-4);
        std::vector<float> vSum2=std::vector<float>(vNo0.begin()+4, vNo0.end()  );
        
        // Sum1 = Sum1 + Sum2 
        std::transform(vSum1.begin(), vSum1.end(), 
                       vSum2.begin(), vSum1.begin(), 
                       std::plus<float>());
        
        // Sum0 = 2*Sum0 
        std::transform(vSum0.begin(), vSum0.end(), 
                       vSum0.begin(),
                       std::bind(std::multiplies<float>(), std::placeholders::_1, 2.0));
        
        // Sum0 = Sum0 - Sum1
        std::transform(vSum0.begin(), vSum0.end(), 
                       vSum1.begin(), vSum0.begin(), 
                       std::minus<float>());
        
        // Sum0 = abs(Sum0)
        std::for_each(vSum0.begin(), vSum0.end(), 
                      [](float &fF) { fF=abs(fF);});
        
        float fNoise=1.482602/sqrt(6)*median(vSum0);
        float fSignal=median(vNo0);
        
        return fSignal/fNoise;
    }
    return -1;
}

double der_snr(const std::vector<double> &vFlux) {
    if (vFlux.empty()) {
        _msg msgM;
        msgM.set_name("der_snr()");
        msgM.set_log(LOGFILE);
        
        msgM.msg(_msg::eMsg::MID, "error: flux is empty");
        return 0;
    }
    
    if (vFlux.size()>4) {
        std::vector<double> vNo0(vFlux);
        
        vNo0.erase(std::remove_if(vNo0.begin(), vNo0.end(), 
                                  [](float fF) { return fF<0.0; }), vNo0.end());
        
        std::vector<double> vSum0=std::vector<double>(vNo0.begin()+2, vNo0.end()-2);
        std::vector<double> vSum1=std::vector<double>(vNo0.begin()  , vNo0.end()-4);
        std::vector<double> vSum2=std::vector<double>(vNo0.begin()+4, vNo0.end()  );
        
        // Sum1 = Sum1 + Sum2 
        std::transform(vSum1.begin(), vSum1.end(), 
                       vSum2.begin(), vSum1.begin(), 
                       std::plus<double>());
        
        // Sum0 = 2*Sum0 
        std::transform(vSum0.begin(), vSum0.end(), 
                       vSum0.begin(),
                       std::bind(std::multiplies<double>(), std::placeholders::_1, 2));
        
        // Sum0 = Sum0 - Sum1
        std::transform(vSum0.begin(), vSum0.end(), 
                       vSum1.begin(), vSum0.begin(), 
                       std::minus<double>());
        
        // Sum0 = abs(Sum0)
        std::for_each(vSum0.begin(), vSum0.end(), 
                      [](double &fF) { fF=abs(fF);});
        
        double fNoise=1.482602/sqrt(6)*median(vSum0);
        double fSignal=median(vNo0);
        
        return fSignal/fNoise;
    }
    return -1;
}

double long CPU_utilization() {
     _msg msgM;
    msgM.set_name("genrandspec");
    msgM.set_threadname("CPU_utilization");
    msgM.set_log(LOGFILE);
        
    auto [fTime_A, fIdle_A]=get_stat();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto [fTime_B, fIdle_B]=get_stat();
    
    return 100.*(1.-((fIdle_B-fIdle_A)/(fTime_B-fTime_A)));
}

std::tuple<double long, double long>  get_stat() {
    _msg msgM;
    msgM.set_name("genrandspec");
    msgM.set_threadname("get_stat");
    msgM.set_log(LOGFILE);
    
    std::fstream sfCpu("/proc/stat", std::ios::in);
    
    if (sfCpu) {

        std::string sLine;
        std::getline(sfCpu, sLine);
        
        // erase "cpu"
        sLine.erase(sLine.begin(), sLine.begin()+sLine.find_first_of("0123456789")); 
        
        // locate " " and push position into vec
        std::vector<int> vPos;
        std::vector<double long> vCol;
        
        vPos.push_back(0); 
        
        int iCount=0;
        for(auto cC: sLine) {
            if (cC==' ')
                vPos.push_back(iCount);
            iCount++;  
        }
        
        // slice
        for(int i=0; i<4; i++) {
            std::string sVal=sLine.substr(vPos[i], vPos[i+1]-vPos[i]);
            sVal.erase(std::remove(sVal.begin(), sVal.end(), ' '), sVal.end()); 
            vCol.push_back(std::stod(sVal));
        }
        
        sfCpu.close();
                
        return {static_cast<double long>(vCol[0]+vCol[1]+vCol[2]+vCol[3]), static_cast<double long>(vCol[3])};
    }
    else 
        msgM.msg(_msg::eMsg::ERROR, "cannot open /proc/stat");
    
    return {-1,1};
}

#endif // der_snr.h
