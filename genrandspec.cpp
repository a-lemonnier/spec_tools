/**
 * \file genrandspec.cpp
 * \brief Generate a set of randomized-flux spectra between two wavelengths for test purposes
 * \author Audric Lemonnier
 * \version 0.3
 * \date 17/03/2020
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <cmath>
#include <random>
#include <thread>
#include <future>
#include <ctime>
#include <boost/program_options.hpp>

#if __has_include (<boost/timer/timer.hpp>)
#include <boost/timer/timer.hpp>
#define HAS_BOOST_TIMER
#endif

#if __has_include (<filesystem>)
#include <filesystem>
#define FS_STD
namespace fs = std::filesystem;
#elif __has_include (<experimental/filesystem>) && !__has_include (<filesystem>)
#include <experimental/filesystem>
#define FS_STDEXP
namespace fs = std::experimental::filesystem;
#elif __has_include(<boost/filesystem.hpp>) && !__has_include (<filesystem>) && !__has_include (<experimental/filesystem>)
#include <boost/filesystem.hpp>
#define FS_BOOST
namespace fs = boost::filesystem;
#else
#error "No filesystem header found"
#endif

#include "csv.h"
#include "msg.h"

/**
 * \define MaxFilepDir
 * \brief Set the maximum number of files to create in a folder
 */
#define MaxFilepDir 10

// Prototype
// ----------------------------------------------------
/**
 * \fn void run(const std::string& sOutput, char cSep, float fMinw, float fMaxw, float fStep)
 * \brief Write random spectra on disk
 */
void run(const std::string& sOutput, char cSep, float fMinw, float fMaxw, float fStep);
// ----------------------------------------------------

int main(int argc, char** argv) {
    
#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif
    
    _msg msgM;
    msgM.set_name("genrandspec");
    
// Parse cmd line
// ----------------------------------------------------  
    
    namespace po = boost::program_options;
    po::options_description description("Usage");
    
    description.add_options()
    ("help,h", "Display this help message")
    ("minw,l",  po::value<float>()->default_value(4000),"Lower wavelength bound")
    ("maxw,u",  po::value<float>()->default_value(8000),"Upper wavelength bound")
    ("step,s",  po::value<float>()->default_value(0.05),"Difference between two neighbored wavelengths")
    ("output,o",  po::value<std::string>()->default_value("rand_spectra"),"Filename of folder results")
    ("separator,s",  po::value<char>()->default_value('\t'),"The column separator. Do not set this option for \\tab.");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
     if (vm.count("help")) {
        std::cout << description;
        return EXIT_SUCCESS;
    }
   
    std::string sOutput=vm["output"].as<std::string>();
    
    char cSep=vm["separator"].as<char>();

    fs::path pOutput(vm["output"].as<std::string>());
    
    // ----------------------------------------------------
    
    msgM.msg(_msg::eMsg::START);
    msgM.msg(_msg::eMsg::MID, "check command line");
    
    float fMax, fMin;
    float fStep=vm["step"].as<float>();
    
    if (!(fMax<fMin) && !(fMax>fMin) ) {
        msgM.msg(_msg::eMsg::ERROR, "invalid boundaries");
        return EXIT_FAILURE;
    }
    
    if (vm["maxw"].as<float>() < vm["minw"].as<float>()) {
        fMax=vm["minw"].as<float>();
        fMin=vm["maxw"].as<float>();
    }
    else {
        fMax=vm["maxw"].as<float>();
        fMin=vm["minw"].as<float>();
    }
    
    if (fStep<0) {
        msgM.msg(_msg::eMsg::ERROR, "bad step:",fStep);
        return EXIT_FAILURE;
    }
    
    if (fs::exists(pOutput)) {
        msgM.msg(_msg::eMsg::ERROR, "directory",sOutput,"exists");
        return EXIT_FAILURE;
    }
    
    int iMax_thread=std::thread::hardware_concurrency();   
    
    msgM.msg(_msg::eMsg::MID, "create", iMax_thread, "folders");
    
    if (iMax_thread>1) {
        msgM.msg(_msg::eMsg::MID, "start", iMax_thread, "async threads");
        
        msgM.msg(_msg::eMsg::MID, "create directories");
        if (fs::create_directory(pOutput)) {
            std::vector<std::string> vsList;
            for(int i=0; i<iMax_thread; i++)
                vsList.emplace_back(sOutput+"/"+std::to_string(i));
            
            std::launch flag=std::launch::async | std::launch::deferred;
            
            std::vector<std::future<void> > vfThread;
            
            for(auto sFile: vsList) 
                if (fs::create_directory(fs::path(sFile)))
                    vfThread.emplace_back(std::async(flag, run, sFile, cSep, fMin, fMax, fStep));
            
            std::for_each(vfThread.begin(), vfThread.end(), [](std::future<void> &th) { th.get(); });
            
        }
        else {
            msgM.msg(_msg::eMsg::ERROR, "cannot mkdir",sOutput);
            return EXIT_FAILURE;
        }
    }
    else {
        msgM.msg(_msg::eMsg::MID, "multi-threading disabled");
        msgM.msg(_msg::eMsg::MID, "generate spectrum");
        
        _csv<float> csv;
        
        csv.set_verbose(_csv<float>::eVerbose::QUIET);
        
        csv.set_filename_out(sOutput);
        csv.set_separator(cSep);
        
        csv.genrandspec(fMin,fMax,fStep);
        
        csv.write();
        
        msgM.msg(_msg::eMsg::MID, "output:", sOutput);   
        
    }
    
#ifdef HAS_BOOST_TIMER
    msgM.msg(_msg::eMsg::END, btTimer.format());
#endif
     
    return EXIT_SUCCESS;
}

// ----------------------------------------------------
// ----------------------------------------------------

void run(const std::string& sOutput, char cSep, float fMinw, float fMaxw, float fStep) {
    
    _msg msgM;
    msgM.set_name("run()");
    msgM.set_threadname("run");
    
#ifdef HAS_SYSCALL
    msgM.msg(_msg::eMsg::THREADS, "create spectra in", sOutput);
#else
    msgM.msg(_msg::eMsg::MID, "create spectra in", sOutput);
#endif
    
    for(int i=0; i< MaxFilepDir; i++) {
        std::string sFname=sOutput+"/"+std::to_string(i)+".dat";
        
        _csv<float> csv;
        
        csv.set_verbose(_csv<float>::eVerbose::QUIET);
        
        csv.set_separator(cSep);
        csv.set_filename_out(sFname);
        
        csv.genrandspec(fMinw,fMaxw,fStep);
        
        csv.write();
        
    }
}
