/**
 * \file marker.cpp
 * \brief Highlight lines on spectrum 
 * \author Audric Lemonnier
 * \version 0.1
 * \date 03/04/2020
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>

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

#include "marker.h"
#include "msg.h"
#include "csv.h"

int main(int argc, char** argv) {
    
#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif
    
    _msg msgM;
    msgM.set_name("marker");
    
    // Parse cmd line
    // ----------------------------------------------------  
    
    namespace po = boost::program_options;
    po::options_description description("Usage");
    
    description.add_options()
    ("help,h", "Display this help message")
    ("input,i",  po::value<std::vector<std::string> >()->multitoken(),"Set input files.")
    ("title,t",  po::value<std::string>(), "Set title.")
    ("label,l",  po::value<std::vector<std::string> >()->multitoken(),"Set labels. If more than one label is defined, the number of labels must be equal to the numbers of files")
    ("sep,s", po::value<std::vector<char> >()->multitoken(), "Set separators. If more than one sep is defined, the number of sep must be equal to the numbers of files.")
    ("element,e",  po::value<std::vector<std::string> >()->multitoken(),"Set the name of an element. Ex: \\$H\\\\\\\\beta\\$")
    ("wavelength,w",po::value<std::vector<float> >()->multitoken(),"Set the wavelength of the line.")
    ("verbose,v","Toggle verbosity.");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
    if (vm.count("help") || !vm.count("input")  || vm.size()<1) {
        std::cout << description;
        return EXIT_SUCCESS;
    }
    
    // ---------------------------------------------------- 
        
    msgM.msg(_msg::eMsg::START);
    msgM.msg(_msg::eMsg::MID, "check command line and fill class");  
    
    std::string sTitle;
    std::vector<std::string> vsLabels;
    std::vector<std::tuple<float, std::string> > vstLines;
    
    if (vm.count("wavelength") && vm.count("element")) {
        if (vm["wavelength"].as<std::vector<float> >().size()==vm["element"].as<std::vector<std::string> >().size()) {
            for(int i=0; i<vm["wavelength"].as<std::vector<float> >().size(); i++)
                vstLines.emplace_back(
                    std::make_tuple(vm["wavelength"].as<std::vector<float> >()[i],
                                    vm["element"].as<std::vector<std::string> >()[i]));
        }
        else {
            msgM.msg(_msg::eMsg::ERROR, "the number of elements do not match the number of wavelengths");
        }        
    }
    
    if (vm.count("title"))
        sTitle=vm["title"].as<std::string>();
    
    std::vector<std::string> vsFlist(vm["input"].as<std::vector<std::string> >());
    std::vector<_csv<float> > vCsv;
    
    if (vm.count("label"))
        vsLabels=vm["label"].as<std::vector<std::string> >();
    
     if (vm.count("sep")) {
         if (vm["input"].as<std::vector<std::string> >().size()!=vm["sep"].as<std::vector<char> >().size() && vm["sep"].as<std::vector<char> >().size()<2) {
             msgM.msg(_msg::eMsg::ERROR, "the number of files do not match the number of separators");
             return EXIT_FAILURE;
         }
     }
        
    if (!vm.count("sep"))
        for(auto sFile: vsFlist) {
            _csv<float> tmp(sFile,'\t');
            vCsv.push_back(tmp);
        }
    else {
        if (vm["sep"].as<std::vector<char> >().size()==1) 
            for(auto sFile: vsFlist) {
                _csv<float> tmp(sFile,vm["sep"].as<std::vector<char> >()[0]);
                vCsv.push_back(tmp);
        }
        else {
            for(auto sFile: vsFlist) {
                _csv<float> tmp;
                vCsv.push_back(tmp);
            }
            int i=0;
            for(auto &csv: vCsv) {
                csv.set_filename(vm["input"].as<std::vector<std::string> >()[i]);
                csv.set_separator(vm["sep"].as<std::vector<char> >()[i]);
                i++;
            }
        }
    }
     
    if (vm.count("verbose")) 
        for(auto &csv: vCsv)
            csv.set_verbose(_csv<float>::DEBUG);
    
    bool bRead=true;
    for(auto &csv: vCsv)
        bRead&=csv.read();
        
    if (bRead) {
    
        _marker Marker;
        
        if (vm.count("verbose"))
            Marker.set_verbose(true);
        else
            Marker.set_verbose(false);
        
        msgM.msg(_msg::eMsg::MID, "set output","plot.pdf","with dpi:", 300 );
        Marker.set_output("plot.pdf", 300);

        if (vm.count("title"))
            Marker.set_title(sTitle);
        else
            Marker.set_title(" ");
        
        msgM.msg(_msg::eMsg::MID, "set title '", Marker.get_title(),"'" );
        
        msgM.msg(_msg::eMsg::MID, "set data and labels");
        if (!vm.count("label"))
            for(auto &csv: vCsv) 
                Marker.add_data(csv.select_column(0), csv.select_column(1));
        else if (vsLabels.size()==1) {
            for(auto &csv: vCsv) 
                Marker.add_data(csv.select_column(0), csv.select_column(1));
            Marker.set_label(vsLabels[0]);
        }
        else {
            int i=0;
            for(auto &csv: vCsv) {
                Marker.add_data(csv.select_column(0), csv.select_column(1), vsLabels[i]);
                i++;
            }
        }
              
        msgM.msg(_msg::eMsg::MID, "set plot options");
        Marker.set_supp(4820,4880);
        
        Marker.set_xlabel("$\\\\lambda$");
        Marker.set_ylabel("Normalized flux");
        Marker.set_xunit("$\\\\AA$");
        
        Marker.set_linewidth(0.4);
        Marker.set_continnumsize(0.6);
        
        msgM.msg(_msg::eMsg::MID, "add markers");
        for(auto tLine: vstLines)
            Marker.add_line(std::get<0>(tLine), std::get<1>(tLine));
        
        msgM.msg(_msg::eMsg::MID, "plot");
        if (Marker.make()) 
            Marker.plot();

    }
    else
        return EXIT_FAILURE;
    
#ifdef HAS_BOOST_TIMER
     msgM.msg(_msg::eMsg::END, btTimer.format());
#endif
    
    return EXIT_SUCCESS;
}
