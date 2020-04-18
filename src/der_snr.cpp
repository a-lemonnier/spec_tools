/**
 * \file der_snr.cpp
 * \brief An C++ implementation of the der_snr fortran code from: 
 * F. Stoehr et al: DER_SNR: A Simple & General Spectroscopic Signal-to-Noise Measurement Algorithm,\n
    394, Astronomical Data Analysis Software and Systems (ADASS) XVII\n
    2008ASPC..394..505S
 * This code is multi-threaded or not if not available.
 * \author Audric Lemonnier
 * \version 0.2
 * \date 18/04/2020
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <functional> 
#include <thread>
#include <future>
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

#include <csv.h>
#include <msg.h>

#define LOGFILE ".der_snr.log"
#define HISTFILE ".history"

// Reference
// ----------------------------------------------------
// F. Stoehr et al: DER_SNR: A Simple & General Spectroscopic Signal-to-Noise Measurement Algorithm
// 394, Astronomical Data Analysis Software and Systems (ADASS) XVII
// 2008ASPC..394..505S
// ----------------------------------------------------

// Prototype
// ----------------------------------------------------

void compute(const std::vector<std::string>& list, const std::string& sOutput);

/**
 * \fn void compute_sep(const std::vector<std::string>& list, const std::string& sOutput, const char& cSep)
 * \brief Compute S/N for all the string in the vector of strings. Used in the multithreaded mode.
 */
void compute_sep(const std::vector<std::string>& list, const std::string& sOutput, const char& cSep);

/**
 * \fn bool merge(const std::string &sPattern)
 * \brief Merge files from threads following a filename pattern, i.e. the given output name.
 */
bool merge(const std::string &sPattern);


bool write(std::vector<std::string> vsResults, const std::string& sOutput);

/**
 * \fn bool write(std::vector<std::string> vsResults, const std::string& sOutput, const char& cSep)
 * \brief Write on disk results
 */
bool write(std::vector<std::string> vsResults, const std::string& sOutput, const char& cSep);

/**
 * \fn float der_snr(const std::vector<float> &vFlux)
 * \brief Compute the S/N with der_snr method.
 */
float der_snr(const std::vector<float> &vFlux);
double der_snr(const std::vector<double> &vFlux);

/**
 * \fn float median(const std::vector<float> &vFlux)
 * \brief Simple computation of the median
 */
float median(const std::vector<float> &vFlux);
double median(const std::vector<double> &vFlux);

// ----------------------------------------------------

int main(int argc, char** argv) {
    
#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif
    
    _msg msgM;
    msgM.set_name("der_snr");
    msgM.set_log(LOGFILE);
    
// Parse cmd line
// ----------------------------------------------------  
    
    namespace po = boost::program_options;
    po::options_description description("Usage");
    
    description.add_options()
    ("help,h", "Display this help message")
    ("filename,f",  po::value<std::string>(),"Filename of the spectrum")
    ("directory,d",  po::value<std::string>(),"Directory where compute the S/N")
    ("output,o",  po::value<std::string>()->default_value("output.csv"),"Filename of results")
    ("separator,s",  po::value<char>()->default_value('\t'),"The column separator. Do not set this option for \\tab.")
    ("exclude,e",  po::value<std::string>(),"Exclude a string in filenames");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
    
     if (vm.count("help") || vm.size()<1 || !(vm.count("filename") ^ vm.count("directory"))) {
        std::cout << description;
        std::cout << "\n";
        std::cout << "Examples:\n";
        std::cout << "./der_snr -f CPD-591792.obs\n";
        msgM.msg(_msg::eMsg::START);
        msgM.msg(_msg::eMsg::MID, "check command line");
        msgM.msg(_msg::eMsg::MID, "compute S/N for 1 file");
        msgM.msg(_msg::eMsg::MID, "CPD-591792.obs: S/N = 95.68");
        msgM.msg(_msg::eMsg::MID, "output: output.csv");
        msgM.msg(_msg::eMsg::END, " 0.039347s wall, 0.040000s user + 0.000000s system = 0.040000s CPU (101.7%)\n");
        std::cout << "\n";
        
        std::cout << "./der_snr -d data\n";
        msgM.msg(_msg::eMsg::START);
        msgM.msg(_msg::eMsg::MID, "check command line");
        msgM.msg(_msg::eMsg::MID, "starting 8 async threads");
        msgM.set_threadname("compute");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 521 files");
        msgM.msg(_msg::eMsg::THREADS, "S/N for 524 files");
        msgM.set_name("merge()");
        msgM.msg(_msg::eMsg::MID, "merge file part1_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part2_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part3_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part4_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part5_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part6_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part7_snr.csv");
        msgM.msg(_msg::eMsg::MID, "merge file part8_snr.csv");
        msgM.set_name("der_snr");
        msgM.msg(_msg::eMsg::MID, "output: output.csv");        
        msgM.msg(_msg::eMsg::END, " 56.395825s wall, 323.050000s user + 0.450000s system = 323.500000s CPU (573.6%)\n");

        
        std::cout << "\nRef:\n";
        std::cout << "F. Stoehr et al: DER_SNR: A Simple & General Spectroscopic Signal-to-Noise Measurement Algorithm\n";
        std::cout << "394, Astronomical Data Analysis Software and Systems (ADASS) XVII\n";
        std::cout << "2008ASPC..394..505S\n";
        return EXIT_SUCCESS;
    }
    
    bool bDefExclude=false;
    bool bDefSep=false;
    
    std::string sOutput;
    std::string sExclude;
    
    char cSep;
    
    fs::path pDirectory;
    fs::path pFilename;
    fs::path pOutput;
    
    // ----------------------------------------------------
    
        // Write history
    // ----------------------------------------------------  
    
    std::fstream sfFlux(HISTFILE, std::ios::app);
    if (sfFlux) {
        
        std::stringstream ssS;
        ssS << argv[0];
        for(const auto &arg: vm) {
            if (arg.second.value().type()==typeid(std::string))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<std::string>();
            if (arg.second.value().type()==typeid(int))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<int>();
            if (arg.second.value().type()==typeid(unsigned int))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<unsigned int>();
            if (arg.second.value().type()==typeid(float))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<float>();
            if (arg.second.value().type()==typeid(char))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<char>();
        }
        ssS << "\n";
    
        sfFlux.close();
    }
    else
        msgM.msg(_msg::eMsg::ERROR, "cannot open history");
    
    // ----------------------------------------------------
    
    msgM.msg(_msg::eMsg::START);
    msgM.msg(_msg::eMsg::MID, "check command line");
    
    if (vm.count("directory") && vm.count("filename")) {
        msgM.msg(_msg::eMsg::ERROR, "ambiguous entries");
        return EXIT_FAILURE;
    }
    
    if (vm.count("directory")) {
        pDirectory=fs::path(vm["directory"].as<std::string>());
        if (!fs::is_directory(pDirectory)) {
            msgM.msg(_msg::eMsg::ERROR, pDirectory.string(),"does not exist");
            return EXIT_FAILURE;
        }
    }
    
    if (vm.count("filename")) {
        pFilename=fs::path(vm["filename"].as<std::string>());
        if (!fs::exists(pFilename)) {
            msgM.msg(_msg::eMsg::ERROR, "error:", pFilename.string(),"does not exist");
            return EXIT_FAILURE;
        }
    }
    
    if (vm.count("output")) {
        pOutput=fs::path(vm["output"].as<std::string>());
        if (fs::exists(pOutput)) {
            msgM.msg(_msg::eMsg::MID, pOutput.string(), " exists: deleting");
            fs::remove(pOutput);
        }
        sOutput=vm["output"].as<std::string>();
    }
    
    if (vm.count("separator")) {
        cSep=vm["separator"].as<char>();
        bDefSep=true;
    }
    
    if (vm.count("exclude")) {
        sExclude=vm["exclude"].as<std::string>();
        bDefExclude=true;
    }
    
    if (vm.count("filename")) {
        
        msgM.msg(_msg::eMsg::MID, "compute S/N for 1 file");
        
        std::string sFilename=vm["filename"].as<std::string>();
        
        _csv<float> csv(sFilename, '\t');
        
        if(csv.read()) {
            
            csv.set_verbose(_csv<float>::eVerbose::QUIET);
            
            msgM.msg(_msg::eMsg::MID,sFilename,": S/N =", der_snr(csv.select_column(1)));
        }
    }
    
    if (vm.count("directory")) {
        
        fs::recursive_directory_iterator step0(pDirectory);
        std::vector<std::string> list;
        
        if (bDefExclude) {
            for(auto &file: boost::make_iterator_range(step0,{}) ) 
                // csv<> unable to parse non csv file
                if (!fs::is_directory(file) && 
                    file.path().has_extension() && 
                    file.path().filename().string().find(std::string(".tar"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".tar.gz"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".tgz"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".zip"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".txt"))==std::string::npos &&
                    file.path().filename().string().find(std::string(".directory"))==std::string::npos &&
                    file.path().filename().string().find(sExclude)==std::string::npos )
                    list.emplace_back(file.path().relative_path().string());       
        }
        else {
            for(auto &file: boost::make_iterator_range(step0,{}) ) 
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
        
        int iMax_thread=std::thread::hardware_concurrency();
        
        if (iMax_thread>1) {
            const std::size_t stSize_divided=list.size()/iMax_thread;
            
            std::vector<std::vector<std::string> > vvsList_divided;
            
            for(std::size_t i=0; i< iMax_thread-1; i++) 
                vvsList_divided.emplace_back(std::vector<std::string>(list.begin()+i*stSize_divided, list.begin() + stSize_divided*(i+1)));
            
            // 1 extra thread for extra files : rest of division
            vvsList_divided.emplace_back(std::vector<std::string>(list.begin()+(iMax_thread-1)*stSize_divided, list.end()));
            
            msgM.msg(_msg::eMsg::MID, "starting", iMax_thread, " async threads");
            
            // let's don't decide the flag
            std::launch lFlag=std::launch::async | std::launch::deferred;
            
            std::vector<std::future<void> > vfThread;
            
            std::vector<std::vector<std::string> > vvsRes;
            
            if (bDefSep) {
                int iCount=0;
                for(auto t_list: vvsList_divided) {
                    iCount++;
                    std::string sNewoutput="part"+std::to_string(iCount)+"_"+sOutput; 
                    vfThread.emplace_back(std::async(lFlag, compute_sep, t_list, sNewoutput, cSep));
                }
            }
            else {
                int iCount=0;
                for(auto t_list: vvsList_divided) {
                    iCount++;
                    std::string sNewoutput="part"+std::to_string(iCount)+"_"+sOutput; 
                    vfThread.emplace_back(std::async(lFlag, compute, t_list, sNewoutput));
                }
            }
            std::for_each(vfThread.begin(), vfThread.end(), [](std::future<void> &th) { th.get(); });
            
            merge(sOutput);            
        }  
        else {
            msgM.msg(_msg::eMsg::MID, "multi-threading disabled");
            compute(list, sOutput);
        }
    }
    
    msgM.msg(_msg::eMsg::MID, "output:", sOutput);   
    
#ifdef HAS_BOOST_TIMER
    msgM.msg(_msg::eMsg::END, btTimer.format());
#endif
     
    return EXIT_SUCCESS;
}

// ----------------------------------------------------
// ----------------------------------------------------

void compute(const std::vector<std::string>& vsList, const std::string& sOutput) {

    _msg msgM;
    msgM.set_name("compute()");
    msgM.set_threadname("compute");
    
#ifdef HAS_SYSCALL
    msgM.msg(_msg::eMsg::THREADS, "compute S/N for", vsList.size(), "files");
#else
    msgM.msg(_msg::eMsg::MID, "compute S/N for", vsList.size(), "files\n";
#endif
    
    std::vector<std::string> vsResults;
    //vsResults.emplace_back("File\tSNR\n");
    
    for(auto sFile: vsList) {
        
        _csv<float> csv(sFile, '\t');
        
        if(csv.read()) {
            
            csv.set_verbose(_csv<float>::eVerbose::QUIET);
            
            vsResults.emplace_back(sFile+"\t"+std::to_string(der_snr(csv.select_column(1))) + "\n");
        }
    }
    write(vsResults, sOutput);
}

void compute_sep(const std::vector<std::string>& vsList, const std::string& sOutput, const char& cSep) {
    std::vector<std::string> vsResults;
    
    _msg msgM;
    msgM.set_name("compute()");
    msgM.set_threadname("compute");
    
#ifdef HAS_SYSCALL
    msgM.msg(_msg::eMsg::THREADS, "compute S/N for", vsList.size(), "files");
#else
    msgM.msg(_msg::eMsg::MID, "compute S/N for", vsList.size(), "files\n";
#endif
    
//     vsResults.emplace_back("File\tSNR\n");    
    
    for(auto sFile: vsList) {
        
        _csv<float> csv(sFile, cSep);
        
        if(csv.read()) {
            
            csv.set_verbose(_csv<float>::eVerbose::QUIET);
            
            vsResults.emplace_back(sFile+"\t"+std::to_string(der_snr(csv.select_column(1))) + "\n");
        }
    }
    write(vsResults, sOutput);
}

bool write(std::vector<std::string> vsResults, const std::string& sOutput) {
    bool sStatus=true;
    
    std::fstream fFlux(sOutput, std::ios::out);
    
    if (fFlux) {
        for(auto sS: vsResults)
            fFlux << sS;
        
        fFlux.close();
    }
    else {
        _msg msgM;
        msgM.set_name("write()");
        
        msgM.msg(_msg::eMsg::ERROR, "cannot open", sOutput);
        sStatus=false;
    }
    return sStatus;
}

bool merge(const std::string &sPattern) {
    _msg msgM;
    msgM.set_name("merge()");
        
    std::fstream fFlux0(sPattern, std::ios::out);
    
    if (fFlux0) {
        int iCount=1;
        fs::path pTmp("part"+std::to_string(iCount)+"_"+sPattern);
        while(fs::exists(pTmp)) {
            msgM.msg(_msg::eMsg::MID, "merge file", pTmp.string());
            
            std::fstream fFlux1(pTmp.string(), std::ios::in);
            std::string sLine;
            
            while(std::getline(fFlux1, sLine))
                fFlux0 << sLine << "\n";
            fFlux1.close();
            
            fs::remove(pTmp);
            iCount++;
            pTmp=fs::path("part"+std::to_string(iCount)+"_"+sPattern);
        }
        fFlux0.close();
    }
    else {
        msgM.msg(_msg::eMsg::ERROR, "cannot open", sPattern);
        return false;
    }
    return true;    
}

float median(const std::vector<float> &vFlux) {    
    int iSize=vFlux.size();
    
    if (iSize==0) {
        _msg msgM;
        msgM.set_name("median()");
        msgM.msg(_msg::eMsg::MID, "error: flux is empty");
        return 0;
    }
    
    std::vector<float> vVec(vFlux);
    std::sort(vVec.begin(), vVec.end());
    
    if (iSize%2==0)     
        return (vVec[iSize/2-1]+vVec[iSize/2])/2;
    else
        return vVec[iSize/2];
}

double median(const std::vector<double> &vFlux) {
    int iSize=vFlux.size();
    
    if (iSize==0) {
        _msg msgM;
        msgM.set_name("median()");
        msgM.msg(_msg::eMsg::MID, "error: flux is empty");
        return 0;
    }
    
    std::vector<double> vVec(vFlux);
    std::sort(vVec.begin(), vVec.end());
    
    if (iSize%2==0)
        return (vVec[iSize/2-1]+vVec[iSize/2])/2;
    else
        return vVec[iSize/2];
    
    return 0;
}


float der_snr(const std::vector<float> &vFlux) {
    if (vFlux.empty()) {
        _msg msgM;
        msgM.set_name("der_snr()");
        msgM.msg(_msg::eMsg::MID, "error: flux is empty");
        return 0;
    }
    
    if (vFlux.size()>4) {
        std::vector<float> vNo0(vFlux);
        
        vNo0.erase(std::remove_if(vNo0.begin(), vNo0.end(), 
                                   [&](float fF) { return fF<0; }),vNo0.end());
        
        int iSize=vNo0.size();
        
        std::vector<float> vSum; vSum.resize(iSize);
        std::vector<float> vSum0=std::vector<float>(vNo0.begin()+2, vNo0.end()-2);
        std::vector<float> vSum1=std::vector<float>(vNo0.begin()  , vNo0.end()-4);
        std::vector<float> vSum2=std::vector<float>(vNo0.begin()+4, vNo0.end()  );
        
        
        // Sum1 = Sum1 + Sum2 
        std::transform(vSum1.begin(), vSum1.end(), 
                       vSum2.begin(), vSum1.begin(), 
                       std::plus<float>());
        
        // Sum0 = 2*Sum0 
        std::transform(vSum0.begin(), vSum0.end(), 
                       vSum0.begin(),
                       std::bind(std::multiplies<float>(), std::placeholders::_1, 2.0));
        
        // Sum0 = Sum0 - Sum1
        std::transform(vSum0.begin(), vSum0.end(), 
                       vSum1.begin(), vSum0.begin(), 
                       std::minus<float>());
        
        // Sum0 = abs(Sum0)
        std::for_each(vSum0.begin(), vSum0.end(), 
                      [](float &fF) { fF=abs(fF);});
        
        
//         for(int i=0; i<iSize; i++) 
//             vSum[i]=abs(2*vSum0[i]-vSum1[i]-vSum2[i]);
            
        
        float fNoise=1.482602/sqrt(6)*median(vSum0);
        float fSignal=median(vNo0);
        
        return fSignal/fNoise;
    }
    return -1;
}

double der_snr(const std::vector<double> &vFlux) {
    if (vFlux.empty()) {
        _msg msgM;
        msgM.set_name("der_snr()");
        msgM.msg(_msg::eMsg::MID, "error: flux is empty");
        return 0;
    }
    
    if (vFlux.size()>4) {
        std::vector<double> vNo0(vFlux);
        
        vNo0.erase(std::remove_if(vNo0.begin(), vNo0.end(), 
                                  [](float fF) { return fF<0.0; }), vNo0.end());
        
        std::vector<double> vSum0=std::vector<double>(vNo0.begin()+2, vNo0.end()-2);
        std::vector<double> vSum1=std::vector<double>(vNo0.begin()  , vNo0.end()-4);
        std::vector<double> vSum2=std::vector<double>(vNo0.begin()+4, vNo0.end()  );
        
        // Sum1 = Sum1 + Sum2 
        std::transform(vSum1.begin(), vSum1.end(), 
                       vSum2.begin(), vSum1.begin(), 
                       std::plus<double>());
        
        // Sum0 = 2*Sum0 
        std::transform(vSum0.begin(), vSum0.end(), 
                       vSum0.begin(),
                       std::bind(std::multiplies<double>(), std::placeholders::_1, 2));
        
        // Sum0 = Sum0 - Sum1
        std::transform(vSum0.begin(), vSum0.end(), 
                       vSum1.begin(), vSum0.begin(), 
                       std::minus<double>());
        
        // Sum0 = abs(Sum0)
        std::for_each(vSum0.begin(), vSum0.end(), 
                      [](double &fF) { fF=abs(fF);});
        
        double fNoise=1.482602/sqrt(6)*median(vSum0);
        double fSignal=median(vNo0);
        
        return fSignal/fNoise;
    }
    return -1;
}
