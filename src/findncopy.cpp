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

#define LOGFILE ".findncopy.log" /**< Define the default logfile  */
#define HISTFILE ".history" /**< Define the default histfile (shared)  */

// Prototype
// ----------------------------------------------------
/**
 * \fn std::vector<std::string> parse_filelist(std::fstream &flux)
 * \brief Create a vector of strings from the filelist
 */
std::vector<std::string> parse_filelist(std::fstream &flux);

/**
 * \fn std::vector<std::string> get_fullrpath(std::vector<std::string>& vsFilelist, const fs::path &fspPidir)
 * \brief Get the full relative path of all file
 */
std::vector<std::string> get_fullrpath(std::vector<std::string>& vsFilelist, 
                                        const fs::path &fspPidir);

/**
 * \fn std::vector<std::string> get_fullrpath(std::vector<std::string>& vsFilelist, const fs::path &fspPidir, const std::string &sExclude)
 * \brief Get the full relative path of all file and exclude a string in paths
 */
std::vector<std::string> get_fullrpath(std::vector<std::string>& vsFilelist, 
                                        const fs::path &fspPidir,
                                        const std::string &sExclude);

/**
 * \fn void erase_string(std::vector<std::string> &vsFullrpath, const std::string &sToerase)
 * \brief Erase a string pattern in the path list
 */
void erase_string(std::vector<std::string> &vsFullrpath, 
                  const std::string &sToerase);

/**
 * \fn std::vector<std::string> make_dir_list(const fs::path &fspPath, const std::string &sDirbase)
 * \brief Make a list of the folder structure
 */
std::vector<std::string> make_dir_list(const fs::path &fspPath, 
                                        const std::string &sDirbase);
/**
 * \fn void make_dir(const std::vector<std::string> &vsBaserpath, const std::string &sOfolder)
 * \brief Recreate the folder structure
 */
void make_dir(const std::vector<std::string> &vsBaserpath,  
              const std::string &sOfolder);

/**
 * \fn void copy_file(std::vector<std::string> &vsFullrpath, const std::string &sOfolder, const std::string &sIfolder)
 * \brief Copy the found files
 */
void copy_file(std::vector<std::string> &vsFullrpath,
               const std::string &sOfolder,
               const std::string &sIfolder);
// ----------------------------------------------------

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
    
    // Write history
    // ----------------------------------------------------  
    
    msgM.msg(_msg::eMsg::MID, "write history");
    
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
        ssS << "\n";
    
        sfFlux << ssS.str();
        
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
            sfFlux << sS << "\n";
        sfFlux.close();
    }
    else
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


// ----------------------------------------------------
// ----------------------------------------------------

std::vector<std::string> parse_filelist(std::fstream &flux) {
    
    _msg msgM;
    msgM.set_name("findncopy");
    msgM.set_log(LOGFILE);
    
    std::vector<std::string> vsFilelist;
    std::string sS;
    while(std::getline(flux,sS)) {
        try { vsFilelist.emplace_back(sS); } 
        catch(std::bad_alloc &ba) { msgM.msg(_msg::eMsg::ERROR, "bad alloc"); }
    }
    msgM.msg(_msg::eMsg::MID, "parsing files:", vsFilelist.size(), "lines");
    return vsFilelist;
}

std::vector<std::string> get_fullrpath(std::vector<std::string>& vsFilelist, 
                                        const fs::path &fspPidir) {
     _msg msgM;
    msgM.set_name("findncopy");
    msgM.set_log(LOGFILE);
    
    std::vector<std::string> vsFullrpath;
    for(auto isF: vsFilelist) 
            for(auto ibsd_itr: fs::recursive_directory_iterator(fspPidir)) {
                std::string sName(ibsd_itr.path().filename().string());
                if (sName.find(isF)!=std::string::npos) 
                    vsFullrpath.emplace_back(ibsd_itr.path().string());
            }
        
    msgM.msg(_msg::eMsg::MID, "searching files:", vsFullrpath.size(), "files found");
    return vsFullrpath;
}

std::vector<std::string> get_fullrpath(std::vector<std::string>& vsFilelist, 
                                        const fs::path &fspPidir,
                                        const std::string &sExclude) {
    _msg msgM;
    msgM.set_name("findncopy");
    msgM.set_log(LOGFILE);
    
    std::vector<std::string> vsFullrpath;
    for(auto isF: vsFilelist) 
            for(auto ibsd_itr: fs::recursive_directory_iterator(fspPidir)) {
                std::string sName(ibsd_itr.path().filename().string());
                if (sName.find(isF)!=std::string::npos && sName.find(sExclude)==std::string::npos) 
                    vsFullrpath.emplace_back(ibsd_itr.path().string());
            }
        
    msgM.msg(_msg::eMsg::MID, "searching files:", vsFullrpath.size(), "files found");
    return vsFullrpath;
}

void erase_string(std::vector<std::string> &vsFullrpath, 
                  const std::string &sToerase) {
    _msg msgM;
    msgM.set_name("findncopy");
    msgM.set_log(LOGFILE);
    
    msgM.msg(_msg::eMsg::MID, "erasing string:", sToerase);
 
    std::for_each(vsFullrpath.begin(), vsFullrpath.end(), [&](std::string &sS) {
            size_t stPos = sS.find(sToerase);
            if (stPos!=std::string::npos)
                sS.erase(stPos, sToerase.length());
        });
}

std::vector<std::string> make_dir_list(const fs::path &fspPath, 
                                        const std::string &sDirbase) {
    _msg msgM;
    msgM.set_name("findncopy");
    msgM.set_log(LOGFILE);
    
    msgM.msg(_msg::eMsg::MID, "creating directory lists");
    
    std::vector<std::string> vsDirlist;//=new std::vector<std::string>;
    
    for(auto bsd_itr: fs::recursive_directory_iterator(fspPath)) {
        if (bsd_itr.path()!=fspPath) {
            std::string sPath(bsd_itr.path().string());
            std::string sFilename(bsd_itr.path().filename().string());
            
            sPath.erase(sPath.find(sDirbase), sDirbase.length());
            sPath.erase(sPath.find(sFilename), sFilename.length());
            
            vsDirlist.emplace_back(sPath);
        }
    }
    return vsDirlist;
}

void make_dir(const std::vector<std::string> &vsBaserpath, 
              const std::string &sOfolder) {
    _msg msgM;
    msgM.set_name("findncopy");
    msgM.set_log(LOGFILE);
    
    msgM.msg(_msg::eMsg::MID, "creating folders");

    for(auto sP: vsBaserpath) 
        if (!fs::exists((fs::path(sOfolder+sP))))
            fs::create_directories(fs::path(sOfolder+sP));
}

void copy_file(std::vector<std::string> &vsFullrpath,
               const std::string &sOfolder,
               const std::string &sIfolder) {
    _msg msgM;
    msgM.set_name("findncopy");
    msgM.set_log(LOGFILE);
    
    int iCount=0;
    for(auto sF: vsFullrpath) 
        if (!fs::exists(fs::path(sOfolder+"/"+sF))) {
            fs::copy(fs::path(sIfolder+"/"+sF),fs::path(sOfolder+"/"+sF));
            iCount++;
        }
        
    msgM.msg(_msg::eMsg::MID, "copying files:", iCount , "files copied");
}
               
// ----------------------------------------------------
