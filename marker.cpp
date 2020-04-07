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
    ("input,i", po::value<std::vector<std::string> >()->multitoken(),"Set input files.")
    ("title,t", po::value<std::string>(), "Set the title.")
    ("label,l", po::value<std::vector<std::string> >()->multitoken(),"Set labels. If more than one label is defined, the number of labels must be equal to the numbers of files.")
    ("xmin", po::value<float>(), "Set the min range.")
    ("xmax", po::value<float>(), "Set the max range.")
    ("xlabel", po::value<std::string>(), "Set xlabel.")
    ("ylabel", po::value<std::string>(), "Set ylabel.")
    ("xunit", po::value<std::string>(), "Set xunit.")
    ("yunit", po::value<std::string>(), "Set yunit.")
    ("dpi", po::value<unsigned int>()->default_value(300), "Set the dpi.")
    ("width", po::value<float>()->default_value(0.25), "Set the width of curves.")
    ("contsize", po::value<float>()->default_value(0.6), "Set the continnum width.")
    ("sep,s", po::value<std::vector<std::string> >()->multitoken(), "Set separators. If more than one sep is defined, the number of sep must be equal to the numbers of files.")
    ("element,e",  po::value<std::vector<std::string> >()->multitoken(),"Set the name of an element. Ex: \\$H\\\\\\\\beta\\$.")
    ("wavelength,w",po::value<std::vector<float> >()->multitoken(),"Set the wavelength of the line.")
    ("verbose,v","Toggle verbosity.");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
    if (vm.count("help") || !vm.count("input")  || vm.size()<1) {
        std::cout << description;
        std::cout << "\nExample:\n";
        std::cout << "-i rand_spectra/0/0.dat -i rand_spectra/0/1.dat -l 0.dat -l 1.dat  -t Spectra -l 'Spectrum 1' -l 'Spectrum 2' -w 4861 -e \\$H\\\\beta\\$\n";
        msgM.msg(_msg::eMsg::START);
        msgM.msg(_msg::eMsg::MID, "check command line and fill class");
        msgM.msg(_msg::eMsg::MID, "set input: rand_spectra/0/0.dat with sep:'\t'");
        msgM.msg(_msg::eMsg::MID, "set input: rand_spectra/0/1.dat with sep:'\t'");
        msgM.msg(_msg::eMsg::MID, "data read and stored");
        msgM.msg(_msg::eMsg::MID, "set data to plot");
        msgM.msg(_msg::eMsg::MID, "set plot options");
        msgM.msg(_msg::eMsg::MID, "write and run script");
        msgM.msg(_msg::eMsg::END, " 3.052816s wall, 2.760000s user + 0.070000s system = 2.830000s CPU (92.7%)\n");
        return EXIT_SUCCESS;
    }
    
    // ---------------------------------------------------- 
        
    msgM.msg(_msg::eMsg::START);
    msgM.msg(_msg::eMsg::MID, "check command line and fill class");  
    
    std::string sTitle;
    std::string sXlabel, sYlabel;
    std::string sXunit, sYunit;
    std::vector<std::string> vsLabels;
    std::vector<std::tuple<float, std::string> > vstLines;
    float fXmin, fXmax;
    
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
            
    if (vm.count("xlabel"))
        sXlabel=vm["xlabel"].as<std::string>();
    if (vm.count("ylabel"))
        sYlabel=vm["ylabel"].as<std::string>();
    
    if (vm.count("xunit"))
        sXunit=vm["xunit"].as<std::string>();
    if (vm.count("yunit"))
        sYunit=vm["yunit"].as<std::string>();
    
    if (vm.count("xmin"))
        fXmin=vm["xmin"].as<float>();
    if (vm.count("xmax"))
        fXmax=vm["xmax"].as<float>();
    
    if (vm.count("title"))
        sTitle=vm["title"].as<std::string>();
    
    std::vector<std::string> vsFlist(vm["input"].as<std::vector<std::string> >());
    std::vector<_csv<float> > vCsv;
    
    if (vm.count("label"))
        vsLabels=vm["label"].as<std::vector<std::string> >();
    
     if (vm.count("sep")) {
         if (vm["input"].as<std::vector<std::string> >().size()!=vm["sep"].as<std::vector<std::string> >().size() && vm["sep"].as<std::vector<std::string> >().size()<2) {
             msgM.msg(_msg::eMsg::ERROR, "the number of files do not match the number of separators");
             return EXIT_FAILURE;
         }
     }
        
    if (!vm.count("sep")) {
        for(auto sFile: vsFlist) {
            msgM.msg(_msg::eMsg::MID, "set input:", sFile, "with sep:'",'\t',"'");
            vCsv.push_back(_csv<float>(sFile,'\t'));
        }
    }
    else {
        if (vm["sep"].as<std::vector<std::string> >().size()==1) {
            for(auto sFile: vsFlist) {
                msgM.msg(_msg::eMsg::MID, 
                         "set input:", sFile, 
                         "with sep:'",vm["sep"].as<std::vector<std::string> >()[0],"'");
                vCsv.push_back(_csv<float>(sFile, vm["sep"].as<std::vector<std::string> >()[0]));
            }
        }
        else {
            for(auto sFile: vsFlist) 
                vCsv.push_back(_csv<float>());

            int i=0;
            for(auto &csv: vCsv) {
                msgM.msg(_msg::eMsg::MID,
                         "set input:", 
                         vm["input"].as<std::vector<std::string> >()[i], 
                         "with sep:'",vm["sep"].as<std::vector<std::string> >()[i],"'");
                csv.set_filename(vm["input"].as<std::vector<std::string> >()[i]);
                csv.set_separator(vm["sep"].as<std::vector<std::string> >()[i]);
                i++;
            }
        }
    }
     
    if (vm.count("verbose")) 
        for(auto &csv: vCsv)
            csv.set_verbose(_csv<float>::DEBUG);
    
    // Read all files
    bool bRead=true;
    for(auto &csv: vCsv)
        bRead&=csv.read();
        
    if (bRead) {
    
        msgM.msg(_msg::eMsg::MID, "data read and stored");
        
        _marker Marker;
        
        if (vm.count("verbose"))
            Marker.set_verbose(true);
        else
            Marker.set_verbose(false);
        
        msgM.msg(_msg::eMsg::MID, "set data to plot");
        if (!vm.count("label"))
            for(auto &csv: vCsv) 
                Marker.add_data(csv.select_column(0), 
                                csv.select_column(1));
        else if (vsLabels.size()==1) {
            for(auto &csv: vCsv) 
                Marker.add_data(csv.select_column(0), 
                                csv.select_column(1));
            Marker.set_label(vsLabels[0]);
        }
        else {
            int i=0;
            for(auto &csv: vCsv) {
                if (i==0)
                    Marker.set_data(csv.select_column(0), 
                                    csv.select_column(1));
                else 
                    Marker.add_data(csv.select_column(0), 
                                    csv.select_column(1), 
                                    vsLabels[i-1]);
                i++;
            }
        }
        
        msgM.msg(_msg::eMsg::MID, "set plot options");
        
        Marker.set_output("plot.pdf", static_cast<int>(vm["dpi"].as<unsigned int>()));

        if (vm.count("title"))
            Marker.set_title(sTitle);
        else
            Marker.set_title(" ");
        
        if (vm.count("xlabel"))
            Marker.set_xlabel(sXlabel);
        if (vm.count("ylabel"))
            Marker.set_ylabel(sYlabel);
        
        if (vm.count("xunit"))
            Marker.set_xunit(sXunit);
        if (vm.count("yunit"))
            Marker.set_yunit(sYunit);
        
        if (vm.count("xmin"))
            Marker.set_xmin(fXmin);
        if (vm.count("xmax"))
            Marker.set_xmax(fXmax);
        
        Marker.set_colorline("red");
        
        Marker.set_linewidth(abs(vm["width"].as<float>()));
        Marker.set_continnumsize(abs(vm["contsize"].as<float>()));
        Marker.set_annotatesize(5);
        
        for(auto tLine: vstLines) {
            if (std::get<0>(tLine)>Marker.get_supp()[0] && 
                std::get<0>(tLine)<Marker.get_supp()[1]) {
            msgM.msg(_msg::eMsg::MID, "add '", std::get<1>(tLine), "' at", std::get<0>(tLine));
            Marker.add_line(std::get<0>(tLine), std::get<1>(tLine));
            }
        }
        
        msgM.msg(_msg::eMsg::MID, "write and run script");
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
