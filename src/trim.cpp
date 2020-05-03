/**
 * \file der_snr
 * \brief Delete a wavelength outside a wavelength range for a file or a folder. This code is multi-threaded or not if not available.
 * \author Audric Lemonnier
 * \version 0.3
 * \date 18/04/2020
 */

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

#define LOGFILE ".trim.log" /**< Define the default logfile  */
#define HISTFILE ".history" /**< Define the default histfile (shared)  */

// Prototype
// ----------------------------------------------------
/**
 * \fn void trim(const std::vector<std::string> &list, float min, float max)
 * \brief Trim files in list between min and max.
 */
void trim(const std::vector<std::string> &list, float min, float max);

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
    msgM.set_name("trim");
    msgM.set_log(LOGFILE);
        
// Parse cmd line
// ----------------------------------------------------  
    
    namespace po = boost::program_options;
    po::options_description description("Usage");
    
    description.add_options()
    ("help,h", "Display this help message")
    ("min,l",  po::value<float>(),"Minimum wavelength")
    ("max,u",  po::value<float>(),"Maximumw avelength")
    ("input_folder,i",  po::value<std::string>(),"Name of the folder where original data are")
    ("output_folder,o",  po::value<std::string>()->default_value("data_out"),"Set the directory where store new data.");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
    if (vm.count("help") || !vm.count("min") || !vm.count("max") || !vm.count("input_folder")  || vm.size()<2) {
        msgM.enable_log(false);
        msgM.set_threadname("trim");
        std::cout << description;
        std::cout << "\nExample:\n";
        std::cout << "./trim -l 4700 -u 4800 -i data -o spectra_trimmed\n";
        msgM.msg(_msg::eMsg::START);
        msgM.msg(_msg::eMsg::MID, "write history");
        msgM.msg(_msg::eMsg::MID, "remove duplicates in history");
        msgM.msg(_msg::eMsg::MID, "check command line");
        msgM.msg(_msg::eMsg::MID, "CPU utilization: 3 %");
        msgM.msg(_msg::eMsg::MID, "starting 8 async threads");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "521 files parsed");
        msgM.msg(_msg::eMsg::THREADS, "524 files parsed");
        msgM.msg(_msg::eMsg::END, " 52.909230s wall, 296.410000s user + 0.710000s system = 297.120000s CPU (561.6%)\n");

        return EXIT_SUCCESS;
    }   
    
    float fMax, fMin;
    
    if (vm["max"].as<float>()<vm["min"].as<float>()) {
        fMax=vm["min"].as<float>();
        fMin=vm["max"].as<float>();
    }
    else {
        fMax=vm["max"].as<float>();
        fMin=vm["min"].as<float>();
    }
   
    fs::path path(vm["output_folder"].as<std::string>());
    fs::path path_out(vm["input_folder"].as<std::string>());
     
    // ----------------------------------------------------  

    msgM.msg(_msg::eMsg::START);
    
    // Write history
    // ----------------------------------------------------  
    
    std::fstream sfFlux(HISTFILE, std::ios::app);
    if (sfFlux) {
        
        std::stringstream ssS;
        ssS << argv[0];
        for(const auto &arg: vm) {
            if (arg.second.value().type()==typeid(std::string))
                ssS << " --" << arg.first.c_str() << " \""<< arg.second.as<std::string>() << "\"";
            if (arg.second.value().type()==typeid(int))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<int>();
            if (arg.second.value().type()==typeid(unsigned int))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<unsigned int>();
            if (arg.second.value().type()==typeid(float))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<float>();
            if (arg.second.value().type()==typeid(char))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<char>();
            if (arg.second.value().type()==typeid(std::vector<std::string>)) {
                for(auto sS: arg.second.as<std::vector<std::string>>())
                    ssS << " --" << arg.first.c_str() << " \""<< sS << "\"";
            }
        }
        ssS << std::endl;
    
        sfFlux.close();
    }
    else
        msgM.msg(_msg::eMsg::ERROR, "cannot open history");
    
    // ----------------------------------------------------
    
    // Remove duplicates
    // ----------------------------------------------------
    
    msgM.msg(_msg::eMsg::MID, "remove duplicates in history");
    
    sfFlux=std::fstream(HISTFILE, std::ios::in);
    if (sfFlux) {

        std::hash<std::string> hH;
        std::vector<size_t> vIndex;
        std::vector<std::string> vsLine;
        
        std::string sLine;
                
        while(std::getline(sfFlux,sLine)) 
            vsLine.emplace_back(sLine);
    
        std::vector<std::string>::iterator vsiTmp(std::unique(vsLine.begin(), vsLine.end()));
        vsLine.resize(std::distance(vsLine.begin(), vsiTmp));

        sfFlux.close();
        
        sfFlux=std::fstream(HISTFILE, std::ios::out | std::ios::trunc);
        for(auto sS: vsLine)
            sfFlux << sS << std::endl;
        sfFlux.close();
    }
    else
        msgM.msg(_msg::eMsg::ERROR, "cannot open history");
    
    // ----------------------------------------------------

    msgM.msg(_msg::eMsg::MID, "check command line");
    
    if (!fs::exists(path_out)) {
        msgM.msg(_msg::eMsg::ERROR, "error directory", path_out.string(), " does not exist");
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
        msgM.msg(_msg::eMsg::ERROR, "error directory", path.string(), "exists");
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
        
// Limit CPU usage -------------------------------------
#if defined (__linux__)
    msgM.msg(_msg::eMsg::MID, "CPU utilization:", static_cast<int>(CPU_utilization()), "%");
    if (CPU_utilization()>50) {
        max_thread=1;
        msgM.msg(_msg::eMsg::MID, "Reduce max thread");
    }
#endif
// -----------------------------------------------------

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
                thread.emplace_back(std::async(flag,trim,t_list,fMin,fMax));
            
            std::for_each(thread.begin(), thread.end(), [](std::future<void> &th) { th.get(); });
        }  
        else {
            msgM.msg(_msg::eMsg::MID, "multi-threading disabled");
            trim(list, fMin, fMax);
        }
    }
#ifdef HAS_BOOST_TIMER
    msgM.msg(_msg::eMsg::END, btTimer.format());
#endif
    
    return EXIT_SUCCESS;
}

// ----------------------------------------------------
// ----------------------------------------------------

void trim(const std::vector<std::string> &list, float min, float max) {
    
    _msg msgM;
    msgM.set_threadname("trim");
    msgM.set_name("trim()");
    msgM.set_log(LOGFILE);
    
    for(auto file: list ) {
        
        _csv<float> csv; 
        
        csv.set_filename(file);
        csv.set_separator('\t');
        
        if (csv.read()) {
            
            csv.set_verbose(_csv<float>::eVerbose::QUIET);
            
            csv.apply_min_threshold(min,0);
            csv.apply_max_threshold(max,0);
            
            csv.write();
        }
    }
#ifdef HAS_SYSCALL
   msgM.msg(_msg::eMsg::THREADS, list.size(), "files parsed");
#else
    msgM.msg(_msg::eMsg::MID, list.size(), " files parsed.");
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
