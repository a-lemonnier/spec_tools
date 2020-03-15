#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <cmath>
#include <random>
#include <thread>
#include <future>
#include <ctime>
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

#include "csv.h"
#include "msg.h"

#define MaxFilepDir 50

// Prototype
// ----------------------------------------------------
void run(const std::string& sOutput, char cSep, float fMinw, float fMaxw, float fStep);

std::vector<std::vector<float> > randomize(float fMinw, float fMaxw, float fStep);
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
    ("minw,l",  po::value<float>()->default_value(4000),"Lower wavelength bound")
    ("maxw,u",  po::value<float>()->default_value(8000),"Upper wavelength bound")
    ("step,s",  po::value<float>()->default_value(0.05),"Difference between two neighbored wavelengths")
    ("output,o",  po::value<std::string>()->default_value("rand_spectra"),"Filename of folder results")
    ("separator,s",  po::value<char>()->default_value('\t'),"The column separator. Do not set this option for \\tab.");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
     if (vm.count("help")) {
        std::cout << description;
        return EXIT_SUCCESS;
    }
   
    std::string sOutput=vm["output"].as<std::string>();
    
    char cSep=vm["separator"].as<char>();

    fs::path pOutput(vm["output"].as<std::string>());
    
    // ----------------------------------------------------
    
    _msg msgM;
    msgM.set_name("genrandspec");
    
    msgM.msg(_msg::eMsg::START);
    msgM.msg(_msg::eMsg::MID, "check command line");
    
    float fMax, fMin;
    float fStep=vm["step"].as<float>();
    
    if (!(fMax<fMin) && !(fMax>fMin) ) {
        msgM.msg(_msg::eMsg::ERROR, "invalid boundaries");
        return EXIT_FAILURE;
        
    }
    
    if (vm["maxw"].as<float>() < vm["minw"].as<float>()) {
        fMax=vm["minw"].as<float>();
        fMin=vm["maxw"].as<float>();
    }
    else {
        fMax=vm["maxw"].as<float>();
        fMin=vm["minw"].as<float>();
    }
    
    if (fStep<0) {
        msgM.msg(_msg::eMsg::ERROR, "bad step:",fStep);
        return EXIT_FAILURE;
    }
    
    if (fs::exists(pOutput)) {
        msgM.msg(_msg::eMsg::ERROR, "directory",sOutput,"exists");
        return EXIT_FAILURE;
    }
    
    int iMax_thread=std::thread::hardware_concurrency();   
    
    msgM.msg(_msg::eMsg::MID, "create", iMax_thread, "folders");
    
    if (iMax_thread>1) {
        msgM.msg(_msg::eMsg::MID, "start", iMax_thread, "async threads");
        
        msgM.msg(_msg::eMsg::MID, "create directories");
        if (fs::create_directory(pOutput)) {
            std::vector<std::string> vsList;
            for(int i=0; i<iMax_thread; i++)
                vsList.emplace_back(sOutput+"/"+std::to_string(i));
            
            std::launch flag=std::launch::async | std::launch::deferred;
            
            std::vector<std::future<void> > vfThread;
            
            for(auto sFile: vsList) 
                if (fs::create_directory(fs::path(sFile)))
                    vfThread.emplace_back(std::async(flag, run, sFile, cSep, fMin, fMax, fStep));
            
            std::for_each(vfThread.begin(), vfThread.end(), [](std::future<void> &th) { th.get(); });
            
            
        }
        else {
            msgM.msg(_msg::eMsg::ERROR, "cannot mkdir",sOutput);
            return EXIT_FAILURE;
        }
    }
    else {
        msgM.msg(_msg::eMsg::MID, "multi-threading disabled");
        msgM.msg(_msg::eMsg::MID, "generate spectrum");
        
        _csv<float> csv;
        
        csv.set_verbose(_csv<float>::eVerbose::QUIET);
        
        csv.set_filename_out(sOutput);
        csv.set_separator(cSep);
        
        csv.set_data(randomize(fMin,fMax,fStep));
        
        csv.write();
        
        msgM.msg(_msg::eMsg::MID, "output:", sOutput);   
        
    }
        
    
#ifdef HAS_BOOST_TIMER
    msgM.msg(_msg::eMsg::END, btTimer.format());
#endif
     
    return EXIT_SUCCESS;
    
}

// ----------------------------------------------------
// ----------------------------------------------------

void run(const std::string& sOutput, char cSep, float fMinw, float fMaxw, float fStep) {
    
    _msg msgM;
    msgM.set_name("run()");
    msgM.set_threadname("run");
    
#ifdef HAS_SYSCALL
    msgM.msg(_msg::eMsg::THREADS, "create spectra in", sOutput);
#else
    msgM.msg(_msg::eMsg::MID, "create spectra in", sOutput);
#endif
    
    for(int i=0; i< MaxFilepDir; i++) {
        std::fstream fFlux(sOutput+"/"+std::to_string(i)+".dat", std::ios::out);
        std::vector<std::vector<float> > vvfData(randomize(fMinw,fMaxw,fStep));
        
        for(int k=0; k<vvfData.size(); k++)
            fFlux << vvfData[k][0] << cSep << vvfData[k][1] << "\n";
        
        fFlux.close();
    }
}

std::vector<std::vector<float> > randomize(float fMinw, float fMaxw, float fStep) {
    
    std::random_device rdDev;
    
    std::default_random_engine dreEngine(rdDev());
    std::normal_distribution<float> gauss_dist(0.95, 0.1);
    std::uniform_real_distribution<float> uniform_dist(0, 0.5);
    std::uniform_real_distribution<float> uniform_dist2(0, 0.005);
    std::cauchy_distribution<float> cauchy_distrib(0,0.1); 
    
    int iSize=abs(fMaxw-fMinw)/fStep;
    
    std::vector<std::vector<float> > vvfRand;
    std::vector<float> vX, vY;
    
    vX.resize(iSize);
    vY.resize(iSize);
        
    for(int i=0;i<iSize;i++)
        vX[i]=fMinw+fStep*i;
    
    float fX=1.0;
    std::generate(vY.begin(), vY.end(), [&]() { 
        float fV=cauchy_distrib(dreEngine);
        float fThres0=uniform_dist2(dreEngine);
        float fThres=uniform_dist(dreEngine);
        if (fV<-fThres0) fV=-fThres0;
        if (fV>fThres) fV=fThres;
        return (fX+0.01*gauss_dist(dreEngine)-fV);});     
   
    for(int i=0;i<iSize;i++) 
        vvfRand.emplace_back(std::vector<float>({vX[i], vY[i]}));
    
    return vvfRand;
}


