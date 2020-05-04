#ifndef SHIFT_H
#define SHIFT_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include <string>
#include <tuple>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/range/iterator_range.hpp>

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

#define CLIGHT 299792.458 // /**< Speed of light in km/s  */

#define LOGFILE ".shift.log" /**< Define the default logfile  */
#define HISTFILE ".history" /**< Define the default histfile (shared)  */

// Prototype
// ----------------------------------------------------
/**
 * \fn void add(const std::vector<std::string> &vsList, float fWavelength)
 * \brief Add the defined wavelength to the first column of spectra. Default sep is '\\t'.
 */
void add(const std::vector<std::string> &vsList, float fWavelength);

/**
 * \fn void add_sep(const std::vector<std::string> &vsList, char cSep , float fWavelength)
 * \brief Add the defined wavelength to the first column of spectra.
 */
void add_sep(const std::vector<std::string> &vsList, char cSep , float fWavelength);

/**
 * \fn void transform_sep(const std::vector<std::string> &vsList, char cSep , float fVr)
 * \brief Correct the radial velocity effect on spectra. Perform a linear transformation.
 * \param fVr Radial Velocity
 */
void transform_sep(const std::vector<std::string> &vsList, char cSep , float fVr);

/**
 * \fn double long CPU_utilization()
 * \brief Get the CPU usage (%)
 */
double long CPU_utilization();

std::tuple<double long, double long> get_stat();

// ----------------------------------------------------
// ----------------------------------------------------

void add(const std::vector<std::string> &vsList, float fWavelength) {    
    _msg msgM;
    msgM.set_threadname("add");
    msgM.set_name("add");
    msgM.set_log(LOGFILE);
    
     for(auto sFile: vsList) {
        
        _csv<float> csv(sFile, '\t');
        
        if(csv.read()) {
            
            csv.set_verbose(_csv<float>::eVerbose::QUIET);
            
            csv.shift(fWavelength);
            
            csv.write();
            
        }
     }
    
#ifdef HAS_SYSCALL   
    msgM.msg(_msg::eMsg::THREADS, vsList.size(), "files parsed.");
#else
    msgM.msg(_msg::eMsg::MID, vsList.size(), " files parsed.");
#endif
}

void add_sep(const std::vector<std::string> &vsList, char cSep , float fWavelength) {    
    _msg msgM;
    msgM.set_threadname("add");
    msgM.set_name("add");
    msgM.set_log(LOGFILE);
    
     for(auto sFile: vsList) {
        
        _csv<float> csv(sFile, cSep);
        
        if(csv.read()) {
            
            csv.set_verbose(_csv<float>::eVerbose::QUIET);
            
            csv.shift(fWavelength);
            
            csv.write();
            
        }    
    }
    
#ifdef HAS_SYSCALL   
    msgM.msg(_msg::eMsg::THREADS, vsList.size(), "files parsed.");
#else
    msgM.msg(_msg::eMsg::MID, vsList.size(), " files parsed.");
#endif
}

void transform_sep(const std::vector<std::string> &vsList, char cSep , float fVr) {
    _msg msgM;
    msgM.set_threadname("transform");
    msgM.set_name("transform");
    msgM.set_log(LOGFILE);
    
     for(auto sFile: vsList) {
        
        _csv<float> csv(sFile, cSep);
        
        if(csv.read()) {
            csv.set_verbose(_csv<float>::eVerbose::QUIET);
            float fBeta=1/(1+fVr/CLIGHT);
            csv.transform_lin(fBeta, 0, 0);
            csv.write();
        }    
    }
    
#ifdef HAS_SYSCALL   
    msgM.msg(_msg::eMsg::THREADS, vsList.size(), "files parsed.");
#else
    msgM.msg(_msg::eMsg::MID, vsList.size(), " files parsed.");
#endif
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





#endif // shift.cpp
