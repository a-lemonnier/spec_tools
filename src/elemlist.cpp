/**
 * \file elemlist.cpp
 * \brief Add a line to the elemlist.
 * \author Audric Lemonnier
 * \version 0.1
 * \date 30/03/2020
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <fstream>
#include <string>
#include <sstream>

#include <boost/program_options.hpp>

#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_numeric.hpp>

#if __has_include (<boost/timer/timer.hpp>)
#include <boost/timer/timer.hpp> 
#define HAS_BOOST_TIMER /**< boost::timer availability */
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

#define LOGFILE ".elemlist.log" /**< Define the default logfile  */
#define HISTFILE ".history" /**< Define the default histfile (shared)  */


// Prototype
// ----------------------------------------------------
/**
 * \fn template<typename _T=std::string> bool add_elem(const std::string& sElem, _T TWl, const std::string& sFilename)
 * \brief Add a line to a file.
 */
template<typename _T=std::string>
bool add_elem(const std::string& sElem, _T TWl, const std::string& sFilename);

/**
 * \fn template<typename _T=std::string> bool add_elem(const std::string& sSymbol, const std::string& sElem, _T TWl, const std::string& sFilename)
 * \brief Add a line to a file, with the indicator symbol
 */
template<typename _T=std::string>
bool add_elem(const std::string& sSymbol, const std::string& sElem, _T TWl, const std::string& sFilename);

/**
 * \fn bool is_float(const std::string &sVal)
 * \brief Determine if a string is a number.
 */
bool is_float(const std::string &sVal);

// ----------------------------------------------------

int main(int argc, char** argv) {

#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif

    _msg msgM;
    msgM.set_name("elemlist");
    msgM.set_log(LOGFILE);
        
// Parse cmd line
// ----------------------------------------------------  
    namespace po = boost::program_options;
    po::options_description description("Usage");
    
    description.add_options()
    ("help,h", "Display this help message")
    ("list,l",  po::value<std::string>()->default_value("line.list"),"Filename of elemlist")
    ("elem,e",  po::value<std::string>(),"The element.")
    ("wavelength,w",  po::value<std::string>(),"The wavelength.")
    ("Mask,M",  "Comment the line: #")
    ("Bold,B",  "Highlight the line: !")
    ("Temp,T",  "Temporary comment: % or @!");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
    if (vm.count("help") || vm.size()<1) {
        msgM.enable_log(false);
        std::cout << description;
        std::cout << "\nCtrl + C to exit.\n";
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
    
    msgM.msg(_msg::eMsg::MID, "check command line");
    
    if (!(vm.count("elem") && vm.count("wavelength"))) {
        bool bEnd=true;
        
        while(bEnd) {
            std::string sElem, sIon, sWl, sSymbol; 
            
            std::cout << "Type an element to add: ";
            std::cin >> sElem;
            sElem[0]=std::toupper(sElem[0]);
            
            std::cin.sync();
            std::cin.clear();
            
            std::cout << "Ionization ? ";
            std::cin >> sIon;
            for(auto &cC: sIon) cC=std::toupper(cC);
            
            std::cin.sync();
            std::cin.clear();
            
            sElem+=" "+sIon;            
            sElem.erase(std::remove(sElem.begin(), sElem.end(), '\n'), sElem.end());
            
            std::cout << "Wavelength = ";
            std::cin >> sWl;

            if (!is_float(sWl)) {
                msgM.msg(_msg::eMsg::ERROR, "wavelength is NaN:", sWl);
                sWl="-1";
            }
            
            std::cout << "Indicator: "; 
            std::cout << "(bold: ! - temp: % - temp bold: @! - mask: # - q or s: skip) ?" << std::endl;
            
            std::string sS;
            std::cin >> sS;
                
            if (sS!="#" && sS!="!" && sS!="@" && sS!="%" && sS!="#!"
                        && sS!="%!"&& sS!="@!"&& sS!="%" && sS!="q"
                        && sS!="Q" && sS!="s" && sS!="S") {
                    msgM.msg(_msg::eMsg::ERROR, "bad char: '", sS, "'");
                    sS=" ";
            }
        
            sS.erase(std::remove(sS.begin(), sS.end(), 'q'), sS.end());
            sS.erase(std::remove(sS.begin(), sS.end(), 'Q'), sS.end());
            sS.erase(std::remove(sS.begin(), sS.end(), 's'), sS.end());
            sS.erase(std::remove(sS.begin(), sS.end(), 'S'), sS.end());
            
            sSymbol+=sS;
            add_elem<>(sSymbol, sElem, sWl, vm["list"].as<std::string>());
        }
    }
    else {
        std::stringstream ssS;
        
        if (vm.count("Mask"))  ssS << "#";
        if (vm.count("Bold"))  ssS << "!";
        if (vm.count("Temp") && !vm.count("Bold")) ssS << "%";
        if (vm.count("Temp") && vm.count("Bold"))  ssS << "@!";
            
        add_elem<>(ssS.str(), 
                   vm["elem"].as<std::string>(), 
                   vm["wavelength"].as<std::string>(), 
                   vm["list"].as<std::string>());
    }
    
#ifdef HAS_BOOST_TIMER
    msgM.msg(_msg::eMsg::END, btTimer.format());
#endif
    return EXIT_SUCCESS;
}

template<typename _T>
bool add_elem(const std::string& sElem, _T TWl, const std::string& sFilename) {
    _msg msgM;
    msgM.set_name("elemlist");
    msgM.set_threadname("add_elem");
    msgM.set_log(LOGFILE);
    
    std::fstream sfFlux(sFilename, std::ios::out | std::ios::app);
    if (sfFlux) {
        msgM.msg(_msg::eMsg::MID, "add:", sElem, TWl);
        sfFlux << "\"" << sElem << "\", " << TWl << "\n";
        sfFlux.close();
        return true;
    }
    else
        msgM.msg(_msg::eMsg::ERROR, "cannot open file");
    
    return false;
}

template<typename _T>
bool add_elem(const std::string& sSymbol, const std::string& sElem, _T TWl, const std::string& sFilename) {
    _msg msgM;
    msgM.set_name("elemlist");
    msgM.set_threadname("add_elem");
    msgM.set_log(LOGFILE);
    
    std::fstream sfFlux(sFilename, std::ios::out | std::ios::app);
    
    if (sfFlux) {
        msgM.msg(_msg::eMsg::MID, "add:", sElem, TWl);
        sfFlux << sSymbol << "\"" << sElem << "\", " << TWl << "\n";
        sfFlux.close();
        return true;
    }
    else
        msgM.msg(_msg::eMsg::ERROR, "cannot open file");
    return false;
}

bool is_float(const std::string &sVal) {
                    std::string::const_iterator first(sVal.begin()), last(sVal.end());
                    return boost::spirit::qi::parse(first, last, boost::spirit::double_) && 
                           first == last;
}
