/**
 * \file der_snr.cpp
 * \brief An C++ implementation of the der_snr fortran code from: 
 * F. Stoehr et al: DER_SNR: A Simple & General Spectroscopic Signal-to-Noise Measurement Algorithm,\n
    394, Astronomical Data Analysis Software and Systems (ADASS) XVII\n
    2008ASPC..394..505S
 * \author Audric Lemonnier
 * \version 0.2
 * \date 18/04/2020
 */

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

// Reference
// ----------------------------------------------------
// F. Stoehr et al: DER_SNR: A Simple & General Spectroscopic Signal-to-Noise Measurement Algorithm
// 394, Astronomical Data Analysis Software and Systems (ADASS) XVII
// 2008ASPC..394..505S
// ----------------------------------------------------

int main(int argc, char** argv) {
    
#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif
    
    _msg msgM;
    msgM.set_name("der_snr");
    msgM.set_log(LOGFILE);
    
// Parse cmd line
// ----------------------------------------------------  
    namespace po = boost::program_options;
    po::options_description description("Usage");
    
    description.add_options()
    ("help,h", "Display this help message")
    ("filename,f",  po::value<std::string>(),"Filename of the spectrum")
    ("directory,d",  po::value<std::string>(),"Directory where compute the S/N")
    ("output,o",  po::value<std::string>()->default_value("output.csv"),"Filename of results")
    ("separator,s",  po::value<char>()->default_value('\t'),"The column separator. Do not set this option for \\tab.")
    ("exclude,e",  po::value<std::string>(),"Exclude a string in filenames");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
     if (vm.count("help") || vm.size()<1 || !(vm.count("filename") ^ vm.count("directory"))) {
        msgM.enable_log(false);
        std::cout << description;
        std::cout << std::endl;
        std::cout << "Examples:\n";
        std::cout << "./der_snr -f CPD-591792.obs\n";
        msgM.msg(_msg::eMsg::START);
        msgM.msg(_msg::eMsg::MID, "check command line");
        msgM.msg(_msg::eMsg::MID, "compute S/N for 1 file");
        msgM.msg(_msg::eMsg::MID, "CPD-591792.obs: S/N = 95.68");
        msgM.msg(_msg::eMsg::MID, "output: output.csv");
        msgM.msg(_msg::eMsg::END, " 0.039347s wall, 0.040000s user + 0.000000s system = 0.040000s CPU (101.7%)\n");
        std::cout << std::endl;
        
        std::cout << "./der_snr -d data\n";
        msgM.msg(_msg::eMsg::START);
        msgM.msg(_msg::eMsg::MID, "check command line");
        msgM.msg(_msg::eMsg::MID, "CPU utilization: 0 %");
        msgM.msg(_msg::eMsg::MID, "starting 8 async threads");
        msgM.set_threadname("compute");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 524 files");
        msgM.set_name("merge()");
        msgM.msg(_msg::eMsg::MID, "merge file part1_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part2_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part3_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part4_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part5_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part6_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part7_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part8_snr.csv");
        msgM.set_name("der_snr");
        msgM.msg(_msg::eMsg::MID, "output: output.csv");        
        msgM.msg(_msg::eMsg::END, " 56.395825s wall, 323.050000s user + 0.450000s system = 323.500000s CPU (573.6%)\n");
        
        std::cout << "\nRef:\n";
        std::cout << "F. Stoehr et al: DER_SNR: A Simple & General Spectroscopic Signal-to-Noise Measurement Algorithm\n";
        std::cout << "394, Astronomical Data Analysis Software and Systems (ADASS) XVII\n";
        std::cout << "2008ASPC..394..505S\n";
        return EXIT_SUCCESS;
    }
    
    bool bDefExclude=false;
    bool bDefSep=false;
    
    std::string sOutput;
    std::string sExclude;
    
    char cSep;
    
    fs::path pDirectory;
    fs::path pFilename;
    fs::path pOutput;
    
    // ----------------------------------------------------
    msgM.msg(_msg::eMsg::START);
    
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
    
    if (vm.count("directory") && vm.count("filename")) {
        msgM.msg(_msg::eMsg::ERROR, "ambiguous entries");
        return EXIT_FAILURE;
    }
    
    if (vm.count("directory")) {
        pDirectory=fs::path(vm["directory"].as<std::string>());
        if (!fs::is_directory(pDirectory)) {
            msgM.msg(_msg::eMsg::ERROR, pDirectory.string(),"does not exist");
            return EXIT_FAILURE;
        }
    }
    
    if (vm.count("filename")) {
        pFilename=fs::path(vm["filename"].as<std::string>());
        if (!fs::exists(pFilename)) {
            msgM.msg(_msg::eMsg::ERROR, "error:", pFilename.string(),"does not exist");
            return EXIT_FAILURE;
        }
    }
    
    if (vm.count("output")) {
        pOutput=fs::path(vm["output"].as<std::string>());
        if (fs::exists(pOutput)) {
            msgM.msg(_msg::eMsg::MID, pOutput.string(), " exists: deleting");
            fs::remove(pOutput);
        }
        sOutput=vm["output"].as<std::string>();
    }
    
    if (vm.count("separator")) {
        cSep=vm["separator"].as<char>();
        bDefSep=true;
    }
    
    if (vm.count("exclude")) {
        sExclude=vm["exclude"].as<std::string>();
        bDefExclude=true;
    }
    
    if (vm.count("filename")) {
        msgM.msg(_msg::eMsg::MID, "compute S/N for 1 file");
        
        std::string sFilename=vm["filename"].as<std::string>();
        
        _csv<float> csv(sFilename, '\t');
        
        if(csv.read()) {
            csv.set_verbose(_csv<float>::eVerbose::QUIET);
            msgM.msg(_msg::eMsg::MID,sFilename,": S/N =", der_snr(csv.select_column(1)));
        }
    }
    
    if (vm.count("directory")) {
        
        fs::recursive_directory_iterator step0(pDirectory);
        std::vector<std::string> list;
        
        if (bDefExclude) {
            for(auto &file: boost::make_iterator_range(step0,{}) ) 
                // csv<> unable to parse non csv file
                if (!fs::is_directory(file) && 
                    file.path().has_extension() && 
                    file.path().filename().string().find(std::string(".tar"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".tar.gz"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".tgz"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".zip"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".txt"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".directory"))==std::string::npos &&
                    file.path().filename().string().find(sExclude)==std::string::npos )
                    list.emplace_back(file.path().relative_path().string());       
        }
        else {
            for(auto &file: boost::make_iterator_range(step0,{}) ) 
                // csv<> unable to parse non csv file
                if (!fs::is_directory(file) && 
                    file.path().has_extension() && 
                    file.path().filename().string().find(std::string(".tar"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".tar.gz"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".tgz"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".zip"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".txt"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".directory"))==std::string::npos )
                    list.emplace_back(file.path().relative_path().string());
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
        
        if (iMax_thread>1) {
            const std::size_t stSize_divided=list.size()/iMax_thread;
            
            std::vector<std::vector<std::string> > vvsList_divided;
            
            for(std::size_t i=0; i< iMax_thread-1; i++) 
                vvsList_divided.emplace_back(std::vector<std::string>(list.begin()+i*stSize_divided, list.begin() + stSize_divided*(i+1)));
            
            // 1 extra thread for extra files : rest of division
            vvsList_divided.emplace_back(std::vector<std::string>(list.begin()+(iMax_thread-1)*stSize_divided, list.end()));
            
            msgM.msg(_msg::eMsg::MID, "starting", iMax_thread, " async threads");
            
            // let's don't decide the flag
            std::launch lFlag=std::launch::async | std::launch::deferred;
            
            std::vector<std::future<void> > vfThread;
            std::vector<std::vector<std::string> > vvsRes;
            
            if (bDefSep) {
                int iCount=0;
                for(auto t_list: vvsList_divided) {
                    iCount++;
                    std::string sNewoutput="part"+std::to_string(iCount)+"_"+sOutput; 
                    vfThread.emplace_back(std::async(lFlag, compute_sep, t_list, sNewoutput, cSep));
                }
            }
            else {
                int iCount=0;
                for(auto t_list: vvsList_divided) {
                    iCount++;
                    std::string sNewoutput="part"+std::to_string(iCount)+"_"+sOutput; 
                    vfThread.emplace_back(std::async(lFlag, compute, t_list, sNewoutput));
                }
            }
            std::for_each(vfThread.begin(), vfThread.end(), [](std::future<void> &th) { th.get(); });
            merge(sOutput);            
        }  
        else {
            msgM.msg(_msg::eMsg::MID, "multi-threading disabled");
            compute(list, sOutput);
        }
    }
    
    msgM.msg(_msg::eMsg::MID, "output:", sOutput);   
    
#ifdef HAS_BOOST_TIMER
    msgM.msg(_msg::eMsg::END, btTimer.format());
#endif
     
    return EXIT_SUCCESS;
}

