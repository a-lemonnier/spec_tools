/**
 * \file der_snr.cpp
 * \brief Remove value under a threshold in a folder or in a file. This code is multi-threaded or not if not available.
 * \author Audric Lemonnier
 * \version 0.1
 * \date 16/03/2020
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include <string>
#include <boost/program_options.hpp>
#include <boost/range/iterator_range.hpp>

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

#include "include/csv.h"
#include "include/msg.h"

// Prototype
// ----------------------------------------------------
/**
 * \fn void trim(const std::vector<std::string> &list, double threshold)
 * \brief Remove zero and negative values of the csv files in the std string vector.
 * \param list String list of files 
 * \param threshold The threshold
 */
void trim(const std::vector<std::string> &list, double threshold);

// ----------------------------------------------------

/**
 * \fn int main(int argc, char **argv)
 * \brief This code removes zeros and negative values in csv located in "./data". The maximum of thread has been used to accelerate code.
 * \todo Parsing command line to get folder name and csv separator, for example.
 */

int main(int argc, char **argv) {
    
#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif
    
    _msg msgM;
    msgM.set_name("threshold");
    
    // Parse cmd line
    // ----------------------------------------------------  
    
    namespace po = boost::program_options;
    po::options_description description("Usage");
    
    description.add_options()
    ("help,h", "Display this help message")
    ("input_folder,i",  po::value<std::string>(),"Set the input directory.")
    ("output_folder,o",  po::value<std::string>()->default_value("data_out"),"Set the directory where set the threshold.")
    ("threshold,t",  po::value<double>(),"Apply a threshold in all 2D spectrum data.\nf<=threshold will be deleted.");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
    if (vm.count("help") || !vm.count("threshold") || !vm.count("input_folder")  || vm.size()<2) {
        msgM.set_threadname("trim");
        std::cout << description;
        std::cout << "\nExample:\n";
        std::cout << "./threshold -i data -o spectra -t 0\n";
        msgM.msg(_msg::eMsg::START);
        msgM.msg(_msg::eMsg::MID, "check command line");
        msgM.msg(_msg::eMsg::MID, "starting 8 async threads");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "524 files parsed");
        msgM.msg(_msg::eMsg::END, " 57.269018s wall, 331.160000s user + 1.350000s system = 332.510000s CPU (580.6%)\n");

        return EXIT_SUCCESS;
    }
    
    fs::path path(vm["output_folder"].as<std::string>());
    fs::path path_out(vm["input_folder"].as<std::string>());
    
    // ---------------------------------------------------- 
        
    msgM.msg(_msg::eMsg::START);
    msgM.msg(_msg::eMsg::MID, "check command line");    
    
    if (!fs::exists(path_out)) {
        msgM.msg(_msg::eMsg::ERROR, "error directory", path_out.string(), "does not exist");
        return EXIT_FAILURE;
    }
    
    if (!fs::exists(path)) {
#ifdef FS_STD 
        msgM.msg(_msg::eMsg::MID, "copying folder using std::"); 
        fs::copy(path_out,path, fs::copy_options::recursive);
#endif
#ifdef FS_STDEXP
        msgM.msg(_msg::eMsg::MID, "copying folder using std::experimental"); 
        fs::copy(path_out,path, fs::copy_options::recursive);
#endif
#ifdef FS_BOOST
        msgM.msg(_msg::eMsg::MID, "copying folder using boost:: : not implemented !");
        return EXIT_FAILURE;
#endif
    }
    else { 
        msgM.msg(_msg::eMsg::ERROR, "error directory", path.string(), " exists");
        return EXIT_FAILURE;
    }
    
    if (fs::is_directory(path)) {
        
        fs::recursive_directory_iterator step0(path);
        std::vector<std::string> list;
        
        for(auto &file: boost::make_iterator_range(step0,{}) ) {
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
        
        int max_thread=std::thread::hardware_concurrency();
        
        if (max_thread>1) {
            const std::size_t size_divided=list.size()/max_thread;
            
            std::vector<std::vector<std::string> > list_divided;
            
            for(std::size_t i=0; i< max_thread-1; i++) 
                list_divided.emplace_back(std::vector<std::string>(list.begin()+i*size_divided, list.begin() + size_divided*(i+1)));
            
            // 1 extra thread for extra files : rest of division
            list_divided.emplace_back(std::vector<std::string>(list.begin()+(max_thread-1)*size_divided, list.end()));
            
            msgM.msg(_msg::eMsg::MID, "starting", max_thread, "async threads");
            
            // let's dont decide the flag
            std::launch flag=std::launch::async | std::launch::deferred;
            
            std::vector<std::future<void> > thread;
            
            for(auto t_list: list_divided)
                thread.emplace_back(std::async(flag,trim,t_list,vm["threshold"].as<double>()));
            
            std::for_each(thread.begin(), thread.end(), [](std::future<void> &th) { th.get(); });
        }  
        else {
            msgM.msg(_msg::eMsg::MID, "multi-threading disabled");
            trim(list, vm["threshold"].as<double>());
        }
    }
    
#ifdef HAS_BOOST_TIMER
     msgM.msg(_msg::eMsg::END, btTimer.format());
#endif
     
    return EXIT_SUCCESS;
}


void trim(const std::vector<std::string> &list, double threshold) {
    
    _msg msgM;
    msgM.set_name("compute()");
    msgM.set_threadname("compute");
    
    for(auto file: list ) {
        
        _csv<> csv; 
        
        csv.set_filename(file);
        csv.set_separator('\t');
        
        if (csv.read()) {
            
            csv.set_verbose(_csv<>::eVerbose::QUIET);
            
            csv.apply_min_threshold(threshold,1);
            
            csv.write();
        }
    } 
    
#ifdef HAS_SYSCALL
    msgM.msg(_msg::eMsg::THREADS, list.size(), "files parsed");
#else
    msgM.msg(_msg::eMsg::MID, list.size(), " files parsed");
#endif
    
}

