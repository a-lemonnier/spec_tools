/**
 * \file shift.cpp
 * \brief Shift whole spectrum by a given wavelength. This code is multi-threaded or not if not available.
 * \author Audric Lemonnier
 * \version 0.1
 * \date 09/03/2020
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
void shift(const std::vector<std::string> &list, float fWavelength);

// ----------------------------------------------------

int main(int argc, char** argv) {
    
#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif

// Parse cmd line
// ----------------------------------------------------  
    
    namespace po = boost::program_options;
    po::options_description description("Usage");
    
    description.add_options()
    ("help,h", "Display this help message")
    ("wavelength,w",  po::value<float>(),"Wavelength")
    ("filename,f",  po::value<std::string>(),"Shift a single file")
    ("input_folder,i",  po::value<std::string>(),"Name of the folder where original data are")
    ("output_folder,o",  po::value<std::string>()->default_value("data_out"),"Set the directory where store new data.");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
    if (vm.count("help") || !vm.count("wavelength") || !(vm.count("input_folder") ^ vm.count("filename"))  || vm.size()<2) {
        std::cout << !vm.count("wavelength") || !(vm.count("input_folder") ^ vm.count("filename"));
        std::cout << description;
        std::cout << "\nExample:\n";
        std::cout << "./shift -w 1.0 -i data -o spectra_shifted\n";
        std::cout << "\033[3;32m\u25B6\033[0m \033[1;34mshift\033[0m\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift\033[0m: starting 8 async threads\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift(2416052)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift(2416051)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift(2416054)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift(2416053)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift(2416055)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift(2416050)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift(2416056)\033[0m: 521 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift(2416057)\033[0m: 524 files parsed.\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift\033[0m:  52.909230s wall, 296.410000s user + 0.710000s system = 297.120000s CPU (561.6%)\n";

        return EXIT_SUCCESS;
    }

// ----------------------------------------------------  
    
    float fWavelength=vm["wavelength"].as<float>();
    
    fs::path pFilename;
    fs::path path(vm["output_folder"].as<std::string>());
    fs::path path_out;
    
    if (fWavelength<=0) {
        std::cerr << "\033[5;31m\u2639\033[0m \033[1;30mshift\033[0m: bad wavelength: "+std::to_string(fWavelength)+"\n";
        return EXIT_FAILURE;
    }
   
    if (vm.count("filename")) {
        path=fs::path(vm["filename"].as<std::string>());
        if (fs::exists(pFilename)) {
            std::cerr << "\033[3;32m\u2690\033[0m \033[1;34mshift\033[0m: error file " << pFilename.string() << " exist\n";
            return EXIT_FAILURE;
        }
    }
   
    if (vm.count("input_folder")) {
        
        path_out=fs::path(vm["input_folder"].as<std::string>());
        
        if (!fs::exists(path_out)) {
            std::cerr << "\033[3;32m\u2690\033[0m \033[1;34mshift\033[0m: error directory " << path_out.string() << " does not exist\n";
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
            std::cerr << "\033[5;31m\u2639\033[0m \033[1;30mshift\033[0m: error directory " << path.string() << " exists\n";
            return EXIT_FAILURE;
        }
    } 
    
    std::cout << "\033[3;32m\u25B6\033[0m \033[1;34mshift\033[0m\n";
    
     if (vm.count("input_folder") && fs::is_directory(path)) {
        
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
            
            std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift\033[0m: starting " << max_thread <<" async threads\n";
            
            // let's dont decide the flag
            std::launch flag=std::launch::async | std::launch::deferred;
            
            std::vector<std::future<void> > thread;
            
            for(auto t_list: list_divided)
                thread.emplace_back(std::async(flag,shift,t_list,fWavelength));
            
            std::for_each(thread.begin(), thread.end(), [](std::future<void> &th) { th.get(); });
        }  
        else {
            std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift\033[0m: multi-threading disabled\n";
            shift(list, fWavelength);
        }
    }
    else {
        //TODO: do it for one file            
    }
#ifdef HAS_BOOST_TIMER
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift\033[0m: " << btTimer.format();
#endif
    
    return EXIT_SUCCESS;
}

// ----------------------------------------------------
// ----------------------------------------------------

void shift(const std::vector<std::string> &list, float fWavelength) {
    
    // TODO: add a shift method in _csv
   
    
#ifdef HAS_SYSCALL
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift("<< syscall(__NR_gettid) << ")\033[0m: " << list.size() << " files parsed.\n";
#else
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30mshift()\033[0m: " << list.size() << " files parsed.\n";
#endif
}
