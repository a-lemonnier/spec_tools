/**
 * \file marker.cpp
 * \brief Highlight lines on spectrum 
 * \author Audric Lemonnier
 * \version 0.6
 * \date 29/04/2020
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
#include <iterator>
#include <limits>

#include <boost/program_options.hpp>

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

#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_numeric.hpp>

#include <marker.h>
#include <msg.h>
#include <log.h>
#include <csv.h>

#define LOGFILE ".marker.log" /**< Define the default logfile  */
#define HISTFILE ".history" /**< Define the default histfile (shared)  */

int main(int argc, char** argv) {

#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif
    
    _msg msgM;
    msgM.set_name("marker");
    msgM.set_log(LOGFILE);
    
    // Parse cmd line
    // ----------------------------------------------------
    namespace po = boost::program_options;
    po::options_description description("Usage");
    
    description.add_options()
    ("help,h", "Display this help message")
    ("input,i", po::value<std::vector<std::string> >()->multitoken(),"Set input files.")
    ("sep,s", po::value<std::vector<std::string> >()->multitoken(), "Set separators. If more than one sep is defined, the number of sep must be equal to the numbers of files.")
    ("output,o", po::value<std::string>(),"Set the output.")
    ("title,t", po::value<std::string>(), "Set the title.")
    ("label,l", po::value<std::vector<std::string> >()->multitoken(),"Set labels. If more than one label is defined, the number of labels must be equal to the numbers of files.")
    ("xmin", po::value<float>(), "Set the min range.")
    ("xmax", po::value<float>(), "Set the max range.")
    ("ymin", po::value<float>(), "Set ymin.")
    ("ymax", po::value<float>(), "Set ymax...")
    ("xlabel", po::value<std::string>(), "Set xlabel.")
    ("ylabel", po::value<std::string>(), "Set ylabel.")
    ("xunit", po::value<std::string>(), "Set xunit.")
    ("yunit", po::value<std::string>(), "Set yunit.")
    ("width", po::value<float>()->default_value(0.25), "Set the width of curves.")
    ("rgb", po::value<std::string>(), "Set the color of the first spectrum as RGBalpha: '#rrggbbaa'.")
    ("contsize", po::value<float>()->default_value(0.6), "Set the continuum width.")
    ("nolegend", "Disable the legend.")
    ("halfbox", "Show only left and bottom axis.")
    ("dotted", "Dotted curve.")
    ("dotdashed", "Dot-dashed curve.")
    ("fontsize", po::value<int>(), "Set the font size.")
    ("wide", "Indicate if markers have to be adapted to the spectrum size.")
    ("element,e",  po::value<std::vector<std::string> >()->multitoken(),"Set the name of an element. Ex: \\$H\\\\\\\\beta\\$.")
    ("wavelength,w",po::value<std::vector<float> >()->multitoken(),"Set the wavelength of the line.")
    ("elemlist",po::value<std::string>(),
            "Set the line list: \n\
            'Elem 1',       wavelength_1\n\
            'Elem 2',       wavelength_2\n\
        !   'Elem 3 bold',  wavelength_3\n\
        @!  'Elem 3 unbold',wavelength_3\n\
        #   blabla          (comment)\n\
        %   blablabla       (comment again)\n...")
    ("shiftfirst", po::value<float>(), "Shift the first spectrum.")
    ("shift", po::value<float>(), "Shift spectra (except the first).")
    ("grid,g","Show the grid.")
    ("dpi", po::value<unsigned int>()->default_value(300), "Set the dpi.")
    ("nolog","Toggle off log.")
    ("verbose,v","Toggle verbosity.");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("input")  || vm.size()<1) {
        msgM.enable_log(false);
        std::cout << description;
        std::cout << "\nExample:\n";
        std::cout << "./marker -i rand_spectra/0/0.dat -i rand_spectra/0/1.dat -l 0.dat -l 1.dat  -t Spectra -l 'Spectrum 1' -l 'Spectrum 2' -w 4861 -e \\$H\\\\beta\\$ --nolog\n";
        msgM.msg(_msg::eMsg::START);
        msgM.msg(_msg::eMsg::MID, "write history");
        msgM.msg(_msg::eMsg::MID, "remove duplicates in history");
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

    msgM.msg(_msg::eMsg::MID, "check command line and fill class");  
    
    std::string sTitle;
    std::string sXlabel, sYlabel;
    std::string sXunit, sYunit;
    std::vector<std::string> vsLabels;
    std::vector<std::tuple<float, std::string, bool> > vstLines;
    float fXmin, fXmax;
    
    // Add lines from cmd line
    if (vm.count("wavelength") && vm.count("element")) {
        if (vm["wavelength"].as<std::vector<float> >().size()==vm["element"].as<std::vector<std::string> >().size()) {
            for(int i=0; i<vm["wavelength"].as<std::vector<float> >().size(); i++)
                vstLines.emplace_back(
                    std::make_tuple(vm["wavelength"].as<std::vector<float> >()[i],
                                    vm["element"].as<std::vector<std::string> >()[i], false));
            std::sort(vstLines.begin(), vstLines.end());
        }
        else 
            msgM.msg(_msg::eMsg::ERROR, "the number of elements do not match the number of wavelengths");
    }
    
    // Sort elemlist
    // ----------------------------------------------------
    if (vm.count("elemlist")) {
        msgM.msg(_msg::eMsg::MID, "sort elemlist");
        if (!_marker<>::sort_elemlist(vm["elemlist"].as<std::string>()))
            msgM.msg(_msg::eMsg::ERROR, "error while sorting elemlist");
    }
    
    // Add lines from file
    // ----------------------------------------------------
    if (vm.count("elemlist")) {
        fs::path pElemlist(vm["elemlist"].as<std::string>());
        if (fs::exists(pElemlist)) {
            std::fstream fFlux(vm["elemlist"].as<std::string>(), std::ios::in);
            
            if (fFlux) {
                std::string sLine;
                
                auto is_float=[](const std::string &sVal) {
                    std::string::const_iterator first(sVal.begin()), last(sVal.end());
                    return boost::spirit::qi::parse(first, last, boost::spirit::double_) && 
                           first == last;
                };
                
                while(std::getline(fFlux, sLine)) {
                    std::string sName;
                    std::string sWl;
                    
                    // check highlight symbol
                    bool bBold=false;
                    if (sLine.find("!")!=std::string::npos && sLine.find("@")==std::string::npos) {
                        sLine.erase(std::remove(sLine.begin(), sLine.end(), '!'), sLine.end());
                        if (vm.count("verbose")) 
                            msgM.msg(_msg::eMsg::MID, "highlight ", sLine);
                        bBold=true;                        
                    }
                    
                    if (!vm.count("wide")) { // @! or !, !@ unbold the line
                        if (sLine.find("!")!=std::string::npos && sLine.find("@")!=std::string::npos) {
                            sLine.erase(std::remove(sLine.begin(), sLine.end(), '!'), sLine.end());
                            sLine.erase(std::remove(sLine.begin(), sLine.end(), '@'), sLine.end());
                        }
                    }
                    else 
                        if (sLine.find("!")!=std::string::npos && sLine.find("@")!=std::string::npos) {
                            sLine.erase(std::remove(sLine.begin(), sLine.end(), '!'), sLine.end());
                            sLine.erase(std::remove(sLine.begin(), sLine.end(), '@'), sLine.end());
                            bBold=true;
                        }
                    
                    if (sLine.find("#")==std::string::npos && sLine.find("%")==std::string::npos)  {
                        sName=sLine.substr(0, sLine.find_first_of(","));
                        sWl=sLine.substr(sLine.find_first_of(",")+1, sLine.size());
                        
                        // remove useless char
                        sName.erase(std::remove(sName.begin(), sName.end(), '"'), sName.end());
                        sName.erase(std::remove(sName.begin(), sName.end(), '\''), sName.end());
                        sWl.erase(std::remove(sWl.begin(), sWl.end(), ' '), sWl.end());
                        sWl.erase(std::remove(sWl.begin(), sWl.end(), '\n'), sWl.end());
                        sWl.erase(std::remove(sWl.begin(), sWl.end(), '\0'), sWl.end());
                        
                        // check if wl is NaN
                        if (!is_float(sWl)) {
                            msgM.msg(_msg::eMsg::ERROR, "NaN in line list");
                            break;
                        }
                        
                        // check duplicates
                        bool bExists=false;
                        for(auto stLine: vstLines) {
                            std::stringstream ssS;
                            ssS << std::fixed << std::setw(2) << std::setprecision(2) << std::get<0>(stLine);
                            if (ssS.str()==sWl)
                                bExists=true;
                        }
                        if (!bExists) 
                            vstLines.emplace_back(std::make_tuple(std::stod(sWl), sName, bBold));
                    }
                    else
                        if (vm.count("verbose")) msgM.msg(_msg::eMsg::MID, "ignore ", sLine);
                }
                std::sort(vstLines.begin(), vstLines.end());
                fFlux.close();
            }
            else
                msgM.msg(_msg::eMsg::ERROR, "error while opening", vm["elemlist"].as<std::string>()); 
        }
        else 
            msgM.msg(_msg::eMsg::ERROR, vm["elemlist"].as<std::string>(), "does not exist");
    }
    // ----------------------------------------------------
    if (vm.count("xlabel")) sXlabel=vm["xlabel"].as<std::string>();
    if (vm.count("ylabel")) sYlabel=vm["ylabel"].as<std::string>();
    if (vm.count("xunit"))  sXunit=vm["xunit"].as<std::string>();
    if (vm.count("yunit"))  sYunit=vm["yunit"].as<std::string>();
    if (vm.count("xmin"))   fXmin=vm["xmin"].as<float>();
    if (vm.count("xmax"))   fXmax=vm["xmax"].as<float>();
    if (vm.count("title"))  sTitle=vm["title"].as<std::string>();
    
    std::vector<std::string> vsFlist(vm["input"].as<std::vector<std::string> >());
    std::vector<_csv<float> > vCsv;
    
    if (vm.count("label")) vsLabels=vm["label"].as<std::vector<std::string> >();
    
    if (vm.count("sep")) 
         if (vm["input"].as<std::vector<std::string> >().size()!=vm["sep"].as<std::vector<std::string> >().size() && vm["sep"].as<std::vector<std::string> >().size()<2) {
             msgM.msg(_msg::eMsg::ERROR, "the number of files do not match the number of separators");
             return EXIT_FAILURE;
         }
        
    if (!vm.count("sep")) 
        for(auto sFile: vsFlist) {
            msgM.msg(_msg::eMsg::MID, "set input:", sFile, "with sep: '",'\t',"'");
            vCsv.push_back(_csv<float>(sFile,'\t'));
        }
    else {
        if (vm["sep"].as<std::vector<std::string> >().size()==1) 
            for(auto sFile: vsFlist) {
                msgM.msg(_msg::eMsg::MID, 
                         "set input:", sFile, 
                         "with sep: '",vm["sep"].as<std::vector<std::string> >()[0],"'");
                vCsv.push_back(_csv<float>(sFile, vm["sep"].as<std::vector<std::string> >()[0]));
            }
        else {
            for(auto sFile: vsFlist) vCsv.push_back(_csv<float>());
 
            int i=0;
            for(auto &csv: vCsv) {
                msgM.msg(_msg::eMsg::MID,
                         "set input:", 
                         vm["input"].as<std::vector<std::string> >()[i], 
                         "with sep: '",vm["sep"].as<std::vector<std::string> >()[i],"'");
                csv.set_filename(vm["input"].as<std::vector<std::string> >()[i]);
                csv.set_separator(vm["sep"].as<std::vector<std::string> >()[i]);
                i++;
            }
        }
    }
     
    if (vm.count("verbose")) 
        for(auto &csv: vCsv) csv.set_verbose(_csv<float>::DEBUG);
    
    // Read all files
    bool bRead=true;
    for(auto &csv: vCsv) bRead&=csv.read();
        
    if (bRead) {
        msgM.msg(_msg::eMsg::MID, "data read and stored");
        
        if (vm.count("shiftfirst")) {
            msgM.msg(_msg::eMsg::MID, "shift first spectrum by:", vm["shift"].as<float>());
            vCsv[0].shift(vm["shift"].as<float>());
        }
        if (vm.count("shift")) {
            msgM.msg(_msg::eMsg::MID, "shift spectra by:", vm["shift"].as<float>());
            for(int i=1; i<vCsv.size(); i++)
                vCsv[i].shift(vm["shift"].as<float>());
        }
            
        _marker<> Marker;
        
        if (vm.count("verbose")) Marker.set_verbose(true);
        else Marker.set_verbose(false);
        
        if (vm.count("nolog")) Marker.set_log(LOGFILE);
        else Marker.set_log(LOGFILE);
        
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
        
        if (vm.count("output"))
            Marker.set_output(vm["output"].as<std::string>(), 
                              static_cast<int>(vm["dpi"].as<unsigned int>()));
        else
            Marker.set_output("plot.pdf", static_cast<int>(vm["dpi"].as<unsigned int>()));
        
        msgM.msg(_msg::eMsg::MID, "output:", Marker.get_output());

        Marker.set_colorline("red");
        
        if (vm.count("title"))  Marker.set_title(sTitle);
        if (vm.count("xlabel")) Marker.set_xlabel(sXlabel);
        if (vm.count("ylabel")) Marker.set_ylabel(sYlabel);
        if (vm.count("xunit"))  Marker.set_xunit(sXunit);
        if (vm.count("yunit"))  Marker.set_yunit(sYunit);
        if (vm.count("xmin"))   Marker.set_xmin(fXmin);
        if (vm.count("xmax"))   Marker.set_xmax(fXmax);
        if (vm.count("ymin"))   Marker.set_ymin(vm["ymin"].as<float>());
        if (vm.count("ymax"))   Marker.set_ymax(vm["ymax"].as<float>());
        if (vm.count("grid"))   Marker.set_showgrid(true);
        if (vm.count("color"))  Marker.set_colorline(vm["color"].as<std::string>());
        if (vm.count("dotted")) Marker.set_dotted(true);
        if (vm.count("dotdashed")) Marker.set_dotdashed(true);
        if (vm.count("wide"))   Marker.set_wide(true);
        if (vm.count("nolegend"))  Marker.set_legend(false);
        if (vm.count("halfbox"))   Marker.set_halfbox(true);
        if (vm.count("width"))  Marker.set_linewidth(abs(vm["width"].as<float>()));
        if (vm.count("contsize"))  Marker.set_continnumsize(abs(vm["contsize"].as<float>()));
        else if (vm.count("width")) Marker.set_continnumsize(abs(vm["width"].as<float>()));
        
        if (vm.count("fontsize")) {
            Marker.set_titlesize(vm["fontsize"].as<int>());
            Marker.set_labelsize(vm["fontsize"].as<int>());
            Marker.set_ticklabelsize(abs(vm["fontsize"].as<int>()-2));
            Marker.set_legendsize(abs(vm["fontsize"].as<int>()-2));
            Marker.set_annotatesize(vm["fontsize"].as<int>());
        }
        
        for(auto tLine: vstLines) {
            if (std::get<0>(tLine) > Marker.get_supp().first && 
                std::get<0>(tLine) < Marker.get_supp().second) {
            msgM.msg(_msg::eMsg::MID, "add '", std::get<1>(tLine), 
                                      "' at", std::get<0>(tLine));
            Marker.add_line(std::get<0>(tLine), 
                            std::get<1>(tLine),
                            std::get<2>(tLine));
            }
        }
        msgM.msg(_msg::eMsg::MID, "write and run script");
        if (Marker.make()) Marker.plot();
    }
    else
        return EXIT_FAILURE;
    
#ifdef HAS_BOOST_TIMER
     msgM.msg(_msg::eMsg::END, btTimer.format());
#endif
    
    return EXIT_SUCCESS;
}
