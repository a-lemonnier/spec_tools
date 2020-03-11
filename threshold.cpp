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

#if __has_include (<sys/syscall.h>)
#include <sys/syscall.h>
#define HAS_SYSCALL
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
        std::cout << description;
        std::cout << "\nExample:\n";
        std::cout << "./threshold -i data -o spectra -t 0\n";
        std::cout << "\033[3;32m\u25B6\033[0m \033[1;34mthreshold\033[0m\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mthreshold\033[0m: starting 8 async threads\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mtrim(2470785)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mtrim(2470786)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mtrim(2470787)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mtrim(2470784)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mtrim(2470783)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mtrim(2470788)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mtrim(2470789)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mtrim(2470790)\033[0m: 524 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mthreshold\033[0m:  57.269018s wall, 331.160000s user + 1.350000s system = 332.510000s CPU (580.6%)\n";

        return EXIT_SUCCESS;
    }
    
    std::cout << "\033[3;32m\u25B6\033[0m \033[1;34mthreshold\033[0m\n";
    
    fs::path path(vm["output_folder"].as<std::string>());
    fs::path path_out(vm["input_folder"].as<std::string>());
    
    if (!fs::exists(path_out)) {
        std::cerr << "\033[5;31m\u2639\033[0m \033[1;30mthreshold\033[0m: error directory " << path_out.string() << " does not exist\n";
        return EXIT_FAILURE;
    }
    
    if (!fs::exists(path)) {
#ifdef FS_STD 
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mthreshold\033[0m: copying folder using std::\n"; 
        fs::copy(path_out,path, fs::copy_options::recursive);
#endif
#ifdef FS_STDEXP
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mthreshold\033[0m: copying folder using std::experimental\n"; 
        fs::copy(path_out,path, fs::copy_options::recursive);
#endif
#ifdef FS_BOOST
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mthreshold\033[0m: copying folder using boost:: : not implemented !\n";
        return EXIT_FAILURE;
#endif
    }
    else { 
        std::cerr << "\033[5;31m\u2639\033[0m \033[1;30mthreshold\033[0m: error directory " << path.string() << " exists\n";
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
            
            std::cout << "\033[3;32m\u2690\033[0m \033[1;30mthreshold\033[0m: starting " << max_thread <<" async threads\n";
            
            // let's dont decide the flag
            std::launch flag=std::launch::async | std::launch::deferred;
            
            std::vector<std::future<void> > thread;
            
            for(auto t_list: list_divided)
                thread.emplace_back(std::async(flag,trim,t_list,vm["threshold"].as<double>()));
            
            std::for_each(thread.begin(), thread.end(), [](std::future<void> &th) { th.get(); });
        }  
        else {
            std::cout << "\033[3;32m\u2690\033[0m \033[1;30mthreshold\033[0m: multi-threading disabled\n";
            trim(list, vm["threshold"].as<double>());
        }
    }
    
#ifdef HAS_BOOST_TIMER
     std::cout << "\033[3;32m\u2690\033[0m \033[1;30mthreshold\033[0m: " << btTimer.format();
#endif
     
    return EXIT_SUCCESS;
}


void trim(const std::vector<std::string> &list, double threshold) {
    
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
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30mtrim("<< syscall(__NR_gettid) << ")\033[0m: " << list.size() << " files parsed.\n";
#else
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30mtrim()\033[0m: " << list.size() << " files parsed.\n";
#endif
    
}

