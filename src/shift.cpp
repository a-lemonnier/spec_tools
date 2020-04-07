/**
 * \file shift.cpp
 * \brief Shift whole spectrum by a given wavelength. This code is multi-threaded or not if not available.
 * \author Audric Lemonnier
 * \version 0.2
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

#include <csv.h>
#include <msg.h>

#define CLIGHT 299792.458 // km/s


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
// ----------------------------------------------------

int main(int argc, char** argv) {
        
#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif
    
    _msg msgM;
    msgM.set_name("shift");

// Parse cmd line
// ----------------------------------------------------  
    
    namespace po = boost::program_options;
    po::options_description description("Usage");
    
    description.add_options()
    ("help,h", "Display this help message")
    ("wavelength,w",  po::value<float>(),"Wavelength")
    ("velocity,v",  po::value<float>(),"Radial velocity of the source (km/s)")
    ("filename,f",  po::value<std::string>(),"Shift a single file")
    ("input_folder,i",  po::value<std::string>(),"Name of the folder where original data are")
    ("output,o",  po::value<std::string>()->default_value("data_out"),"Set the directory or the file where store new data.")
    ("separator,s",  po::value<char>()->default_value('\t'),"The column separator. Do not set this option for \\tab.");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
    if (vm.count("help") || !(vm.count("wavelength") ^ vm.count("velocity")) || !(vm.count("input_folder") ^ vm.count("filename"))  || vm.size()<2) {
        std::cout << description;
        std::cout << "\nExample:\n";
        std::cout << "./shift -w -1.0 -f CD-592728.obs\n";
        msgM.msg(_msg::eMsg::START);
        msgM.msg(_msg::eMsg::MID, "check command line");
        msgM.msg(_msg::eMsg::MID, "shift the spectrum by -1");
        msgM.msg(_msg::eMsg::MID, "output: data_out");
        msgM.msg(_msg::eMsg::END," 0.043272s wall, 0.040000s user + 0.000000s system = 0.040000s CPU (92.4%)\n");

        return EXIT_SUCCESS;
    }

    bool bDefVr=false;
    
    float fVr;
    float fWavelength;
    
    if (vm.count("velocity")) {
        fVr=vm["velocity"].as<float>();
        bDefVr=true;
    }
    else 
        fWavelength=vm["wavelength"].as<float>();
    
    
    std::string sFilename;
    std::string sOutput=vm["output"].as<std::string>();
    char cSep=vm["separator"].as<char>();
    
    fs::path pFilename;
    
    // one copies the original folder and then works on the original
    fs::path path(vm["output"].as<std::string>());
    fs::path path_out;
 
// ----------------------------------------------------  
        
    msgM.msg(_msg::eMsg::START);
    msgM.msg(_msg::eMsg::MID, "check command line");
        
    if (vm.count("filename")) {
        sFilename=vm["filename"].as<std::string>();
        path=fs::path(sFilename);
        if (!fs::exists(path)) {
            msgM.msg(_msg::eMsg::ERROR, "file", sFilename, "does not exist");
            return EXIT_FAILURE;
        }
    }
   
    if (vm.count("input_folder")) {
        sFilename=vm["input_folder"].as<std::string>();
        path_out=fs::path(vm["input_folder"].as<std::string>());
        if (!fs::exists(path_out)) {
             msgM.msg(_msg::eMsg::ERROR, "directory", sFilename, "does not exist\n");
            return EXIT_FAILURE;
        }
        
        // copy the original data with the available lib
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
            msgM.msg(_msg::eMsg::ERROR, "error directory", path.string(), "exists");
            return EXIT_FAILURE;
        }
    } 
    
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
            
            msgM.msg(_msg::eMsg::MID, "starting", max_thread, "async threads");
            
            // let's dont decide the flag
            std::launch flag=std::launch::async | std::launch::deferred;
            
            std::vector<std::future<void> > thread;
            
            if (!bDefVr)
                for(auto t_list: list_divided)
                    thread.emplace_back(std::async(flag, add_sep, t_list, cSep, fWavelength));
            else
                for(auto t_list: list_divided)
                    thread.emplace_back(std::async(flag, transform_sep, t_list, cSep, fVr));                
            
            std::for_each(thread.begin(), thread.end(), [](std::future<void> &th) { th.get(); });
        }  
        else {
            msgM.msg(_msg::eMsg::MID, "multi-threading disabled");
            if (!bDefVr)
                add_sep(list, cSep, fWavelength);
            else
                transform_sep(list, cSep, fVr);
        }
    }
    else {
        if (!bDefVr)
            msgM.msg(_msg::eMsg::MID, "shift the spectrum by", fWavelength);
        else
            msgM.msg(_msg::eMsg::MID, "correct radial velocity vr =", fVr, "and 1/(\u03B2+1) =", 1/(1+fVr/CLIGHT));
        
        _csv<float> csv(sFilename, cSep);
        
        csv.set_filename_out(sOutput);
        
        if (csv.read()) {
            
            csv.set_verbose(_csv<float>::eVerbose::QUIET);
            
            if (!bDefVr)
                csv.shift(fWavelength);
            else {
                float fBeta=1/(1+fVr/CLIGHT);
                csv.transform_lin(fBeta, 0., 0);
            }
            
            csv.write();
            
        }
    }
    
    msgM.msg(_msg::eMsg::MID, "output:", sOutput);
    
#ifdef HAS_BOOST_TIMER
    msgM.msg(_msg::eMsg::END, btTimer.format());
#endif
    
    return EXIT_SUCCESS;
}

// ----------------------------------------------------
// ----------------------------------------------------


void add(const std::vector<std::string> &vsList, float fWavelength) {    
    _msg msgM;
    msgM.set_threadname("add");
    msgM.set_name("add");
    
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
