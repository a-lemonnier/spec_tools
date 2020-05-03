#ifndef FINDNCOPY_H
#define FINDNCOPY_H

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


#endif // findncopy.h
