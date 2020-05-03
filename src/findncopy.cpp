/**
 * \file findncopy.cpp
 * \brief Copy files from a list in a new folder.
 * \author Audric Lemonnier
 * \version 0.1
 * \date 09/03/2020
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

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

#include <msg.h>
#include <log.h>
#include <findncopy.h>

#define LOGFILE ".findncopy.log" /**< Define the default logfile  */
#define HISTFILE ".history" /**< Define the default histfile (shared)  */

int main(int argc, char** argv) {

#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif

    _msg msgM;
    msgM.set_name("findncopy");
    msgM.set_log(LOGFILE);
        
// Parse cmd line
// ----------------------------------------------------  
    
    namespace po = boost::program_options;
    po::options_description description("Usage");
    
    description.add_options()
    ("help,h", "Display this help message")
    ("namelist,n",  po::value<std::string>(),"Filename of the list of name to find")
    ("i_folder,i",  po::value<std::string>(),"Name of the folder where original data are")
    ("o_folder,o",  po::value<std::string>()->default_value("data_out"),"Name of the folder where files will be copied")
    ("exclude,e",   po::value<std::string>(),"Exclude files matching the string");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
    if (vm.count("help") || !vm.count("i_folder") 
                         || !vm.count("namelist")  
                         || vm.size()<2) {
        msgM.enable_log(false);
        std::cout << description;
        std::cout << "\nExample:\n";
        std::cout << "# ls\n";
        std::cout << "data findncopy star_list.txt\n";
        std::cout << "# cat tar_list.txt\n";
        std::cout << "HD87205\nHD87205\nHD87309\nHD87309\n...\n";
        std::cout << "# ./findncopy -n star_list.txt -i data -o spectra -e sky\n";
        msgM.msg(_msg::eMsg::START);
        msgM.msg(_msg::eMsg::MID, "write history");
        msgM.msg(_msg::eMsg::MID, "remove duplicates in history");
        msgM.msg(_msg::eMsg::MID, "parsing file: 36 lines");
        msgM.msg(_msg::eMsg::MID, "searching file: 240 files found");
        msgM.msg(_msg::eMsg::MID, "erasing string: 'data'");
        msgM.msg(_msg::eMsg::MID, "creating directory list: done");
        msgM.msg(_msg::eMsg::MID, "creating folders: done");
        msgM.msg(_msg::eMsg::MID, "copying files: 174 files copied");
        msgM.msg(_msg::eMsg::END, " 0.176172s wall, 0.130000s user + 0.040000s system = 0.170000s CPU (96.5%)\n");
        return EXIT_SUCCESS;
    }

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
    
    std::string sListname = vm["namelist"].as<std::string>();
    std::vector<std::string> vsFilelist;
    
    std::fstream flux0(sListname, std::ios::in);
    if (flux0) {
        std::vector<std::string> vsFullrpath;
        std::vector<std::string> vsBaserpath;
        
        std::string sExclude(vm["exclude"].as<std::string>());
        std::string sIfolder(vm["i_folder"].as<std::string>());
        std::string sOfolder(vm["o_folder"].as<std::string>());
        
        fs::path bfPidir(sIfolder);
        fs::path bfPodir(sOfolder);

        // parse list
        vsFilelist=parse_filelist(flux0);
        
        // get relative file paths
        if (vm.count("exclude"))
            vsFullrpath=get_fullrpath(vsFilelist, bfPidir, sExclude);
        else
            vsFullrpath=get_fullrpath(vsFilelist, bfPidir);
        
        // erase the name of the input base name folder
        erase_string(vsFullrpath, vm["i_folder"].as<std::string>());
        
        // get list of input sub directory
        vsBaserpath=make_dir_list(bfPidir, sIfolder);
        
        // mkdir -p 
        make_dir(vsBaserpath, sOfolder);

        // copy file...
        copy_file(vsFullrpath, sOfolder, sIfolder);
    }
    else {
        msgM.msg(_msg::eMsg::ERROR, "error while opening file");
        return EXIT_FAILURE;
    }

#ifdef HAS_BOOST_TIMER
    msgM.msg(_msg::eMsg::END, btTimer.format());
#endif
    return EXIT_SUCCESS;
}
