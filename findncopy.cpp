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
#include <algorithm>
#include <string>
#include <boost/program_options.hpp>

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

// Prototype
// ----------------------------------------------------
std::vector<std::string> parse_filelist(std::fstream &flux);

std::vector<std::string> get_fullrpath(std::vector<std::string>& vsFilelist, 
                                        const fs::path &fspPidir);

std::vector<std::string> get_fullrpath(std::vector<std::string>& vsFilelist, 
                                        const fs::path &fspPidir,
                                        const std::string &sExclude);

void erase_string(std::vector<std::string> &vsFullrpath, 
                  const std::string &sToerase);

std::vector<std::string> make_dir_list(const fs::path &fspPath, 
                                        const std::string &sDirbase);

void make_dir(const std::vector<std::string> &vsBaserpath,  
              const std::string &sOfolder);

void copy_file(std::vector<std::string> &vsFullrpath,
               const std::string &sOfolder,
               const std::string &sIfolder);
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
    ("namelist,n",  po::value<std::string>(),"Filename of the list of name to find")
    ("i_folder,i",  po::value<std::string>(),"Name of the folder where original data are")
    ("o_folder,o",  po::value<std::string>()->default_value("data_out"),"Name of the folder where files will be copied")
    ("exclude,e",   po::value<std::string>(),"Exclude files matching the string");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
    if (vm.count("help") || !vm.count("i_folder") || !vm.count("namelist")  || vm.size()<2) {
        std::cout << description;
        std::cout << "\nExample:\n";
        std::cout << "# ls\n";
        std::cout << "data findncopy star_list.txt\n";
        std::cout << "# cat tar_list.txt\n";
        std::cout << "HD87205\nHD87205\nHD87309\nHD87309\n...\n";
        std::cout << "# ./findncopy -n star_list.txt -i data -o spectra -e sky\n";
        std::cout << "\033[3;32m\u25B6\033[0m \033[1;34mfindncopy\033[0m\n";
        std::cout << "\033[3;32m\u2690\033[0m parsing file: 36 lines\n";
        std::cout << "\033[3;32m\u2690\033[0m searching file: 240 files found\n";
        std::cout << "\033[3;32m\u2690\033[0m erasing string: 'data'\n";
        std::cout << "\033[3;32m\u2690\033[0m creating directory list: done\n";
        std::cout << "\033[3;32m\u2690\033[0m creating folders: done\n";
        std::cout << "\033[3;32m\u2690\033[0m copying files: 174 files copied\n";
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30mfindncopy\033[0m:  0.176172s wall, 0.130000s user + 0.040000s system = 0.170000s CPU (96.5%)\n";

        return EXIT_SUCCESS;
    }

// ----------------------------------------------------  
    
    std::cout << "\033[3;32m\u25B6\033[0m \033[1;34mfindncopy\033[0m\n";

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
        std::cerr << "\033[5;31m\u2639\033[0m \033[1;30merror while opening file\033[0m\n";
        return EXIT_FAILURE;
    }

#ifdef HAS_BOOST_TIMER
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30mfindncopy\033[0m: " << btTimer.format();
#endif
    return EXIT_SUCCESS;
}


// ----------------------------------------------------
// ----------------------------------------------------

std::vector<std::string> parse_filelist(std::fstream &flux) {
    
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30mparsing file\033[0m: ";
    
    std::vector<std::string> vsFilelist;
    std::string sS;
    while(std::getline(flux,sS)) {
        try { vsFilelist.emplace_back(sS); } 
        catch(std::bad_alloc &ba) { std::cerr << "\033[5;31m\u2639\033[0m \033[1;30mbad alloc\033[0m\n"; }
    }
    std::cout << vsFilelist.size() << " lines\n";
    return vsFilelist;
}

std::vector<std::string> get_fullrpath(std::vector<std::string>& vsFilelist, 
                                        const fs::path &fspPidir) {
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30msearching file\033[0m: ";
    
    std::vector<std::string> vsFullrpath;
    for(auto isF: vsFilelist) 
            for(auto ibsd_itr: fs::recursive_directory_iterator(fspPidir)) {
                std::string sName(ibsd_itr.path().filename().string());
                if (sName.find(isF)!=std::string::npos) 
                    vsFullrpath.emplace_back(ibsd_itr.path().string());
            }
        
    std::cout << vsFullrpath.size() << " files found\n";
    return vsFullrpath;
}

std::vector<std::string> get_fullrpath(std::vector<std::string>& vsFilelist, 
                                        const fs::path &fspPidir,
                                        const std::string &sExclude) {
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30msearching file\033[0m: ";
    
    std::vector<std::string> vsFullrpath;
    for(auto isF: vsFilelist) 
            for(auto ibsd_itr: fs::recursive_directory_iterator(fspPidir)) {
                std::string sName(ibsd_itr.path().filename().string());
                if (sName.find(isF)!=std::string::npos && sName.find(sExclude)==std::string::npos) 
                    vsFullrpath.emplace_back(ibsd_itr.path().string());
            }
        
    std::cout << vsFullrpath.size() << " files found\n";
    return vsFullrpath;
}

void erase_string(std::vector<std::string> &vsFullrpath, 
                  const std::string &sToerase) {
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30merasing string\033[0m: '" << sToerase << "'\n";
    std::for_each(vsFullrpath.begin(), vsFullrpath.end(), [&](std::string &sS) {
            size_t stPos = sS.find(sToerase);
            if (stPos!=std::string::npos)
                sS.erase(stPos, sToerase.length());
        });
}

std::vector<std::string> make_dir_list(const fs::path &fspPath, 
                                        const std::string &sDirbase) {
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30mcreating directory list\033[0m: ";
    
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
    std::cout << "done\n";
    return vsDirlist;
}

void make_dir(const std::vector<std::string> &vsBaserpath, 
              const std::string &sOfolder) {
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30mcreating folders\033[0m: ";
    for(auto sP: vsBaserpath) 
        if (!fs::exists((fs::path(sOfolder+sP))))
            fs::create_directories(fs::path(sOfolder+sP));
    std::cout << "done\n";
}

void copy_file(std::vector<std::string> &vsFullrpath,
               const std::string &sOfolder,
               const std::string &sIfolder) {
    std::cout << "\033[3;32m\u2690\033[0m \033[1;30mcopying files\033[0m: ";
    int iCount=0;
    for(auto sF: vsFullrpath) 
        if (!fs::exists(fs::path(sOfolder+"/"+sF))) {
            fs::copy(fs::path(sIfolder+"/"+sF),fs::path(sOfolder+"/"+sF));
            iCount++;
        }
        
    std::cout << iCount << " files copied\n";
}
               
// ----------------------------------------------------
