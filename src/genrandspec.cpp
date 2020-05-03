/**
 * \file genrandspec.cpp
 * \brief Generate a set of randomized-flux spectra between two wavelengths for test purposes
 * \author Audric Lemonnier
 * \version 0.4
 * \date 18/04/2020
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

#define LOGFILE ".genrandspec.log" /**< Define the default logfile  */
#define HISTFILE ".history" /**< Define the default histfile (shared)  */

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

/**
 * \fn double long CPU_utilization()
 * \brief Get the CPU usage (%)
 */
double long CPU_utilization();

std::tuple<double long, double long> get_stat();
// ----------------------------------------------------

int main(int argc, char** argv) {
    
#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif
    
    _msg msgM;
    msgM.set_name("genrandspec");
    msgM.set_log(LOGFILE);
    
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
        msgM.set_threadname("run");
        msgM.set_log(LOGFILE);
        msgM.enable_log(false);
        
        std::cout << description;
        
        std::cout << "\nExample:\n";
        std::cout << "./shift -w -1.0 -f CD-592728.obs\n";
        msgM.msg(_msg::eMsg::START);
        msgM.msg(_msg::eMsg::MID, "write history");
        msgM.msg(_msg::eMsg::MID, "remove duplicates in history");
        msgM.msg(_msg::eMsg::MID, "check command line");
        msgM.msg(_msg::eMsg::MID, "CPU utilization: 3 %"); 
        msgM.msg(_msg::eMsg::MID, "create 8 async threads");
        msgM.msg(_msg::eMsg::MID, "start 8 async threads");
        msgM.msg(_msg::eMsg::THREADS, "create spectra in rand_spectra/0 ");
        msgM.msg(_msg::eMsg::THREADS, "create spectra in rand_spectra/1 ");
        msgM.msg(_msg::eMsg::THREADS, "create spectra in rand_spectra/2 ");
        msgM.msg(_msg::eMsg::THREADS, "create spectra in rand_spectra/3 ");
        msgM.msg(_msg::eMsg::THREADS, "create spectra in rand_spectra/4 ");
        msgM.msg(_msg::eMsg::THREADS, "create spectra in rand_spectra/5 ");
        msgM.msg(_msg::eMsg::THREADS, "create spectra in rand_spectra/6 ");
        msgM.msg(_msg::eMsg::THREADS, "create spectra in rand_spectra/7 ");
        msgM.msg(_msg::eMsg::END," 10.694788s wall, 77.990000s user + 3.050000s system = 81.040000s CPU (757.8%)");
        return EXIT_SUCCESS;
    }
   
    std::string sOutput=vm["output"].as<std::string>();
    
    char cSep=vm["separator"].as<char>();
    
    fs::path pOutput(vm["output"].as<std::string>());

    // ----------------------------------------------------    
    msgM.msg(_msg::eMsg::START);
        
        
    std::fstream sfFlux;
    
    _log log;
    log.set_execname(argv);
    log.set_historyname(HISTFILE);
    log.set_logname(LOGFILE);
    
    // Write history
    // ----------------------------------------------------  
    msgM.msg(_msg::eMsg::MID, "write history");
    if (!log.write_history(vm))
        msgM.msg(_msg::eMsg::ERROR, "cannot open history");
    // ----------------------------------------------------    
    
    // Remove duplicates
    // ----------------------------------------------------
    msgM.msg(_msg::eMsg::MID, "remove duplicates in history");
    if (!log.remove_duplicate())
        msgM.msg(_msg::eMsg::ERROR, "cannot open history");
    // ----------------------------------------------------
    
    msgM.msg(_msg::eMsg::MID, "check command line");
    
    float fMax, fMin;
    float fStep=vm["step"].as<float>();
    
    if (abs(vm["maxw"].as<float>()-vm["minw"].as<float>())<1e-35) {
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

// Limit CPU usage -------------------------------------
#if defined (__linux__)
    msgM.msg(_msg::eMsg::MID, "CPU utilization:", static_cast<int>(CPU_utilization()), "%");
    if (CPU_utilization()>50) {
        iMax_thread=1;
        msgM.msg(_msg::eMsg::MID, "Reduce max thread");
    }
#endif
// -----------------------------------------------------
    
    msgM.msg(_msg::eMsg::MID, "create", iMax_thread, "folders");
    
    if (iMax_thread>1) {
        msgM.msg(_msg::eMsg::MID, "start", iMax_thread, "async threads");
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
    msgM.set_log(LOGFILE);
    
#ifdef HAS_SYSCALL
    msgM.msg(_msg::eMsg::THREADS, "create spectra in", sOutput);
#else
    msgM.msg(_msg::eMsg::MID, "create spectra in", sOutput);
#endif
    
    _csv<float> csv;
    csv.set_verbose(_csv<float>::eVerbose::QUIET);
    csv.set_separator(cSep);
    
    for(int i=0; i< MaxFilepDir; i++) {
        std::string sFname=sOutput+"/"+std::to_string(i)+".dat";

        csv.set_filename_out(sFname);
        
        csv.genrandspec(fMinw,fMaxw,fStep);
        
        csv.write();
        
    }
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
