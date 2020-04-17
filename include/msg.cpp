#include "msg.h"

_msg::_msg(): sSuf("\033[0m"),
              sSta_pre("\033[3;32m\u25B6\033[0m \033[1;34m"),
              sMid_pre("\033[3;32m\u2690\033[0m \033[1;30m"),
              sEnd_pre("\033[3;96m\u2690\033[0m \033[1;30m"),
              sThd_pre("\033[3;95m\u26A1\033[0m \033[1;30m"),
              sThd_suf("\033[1;95m"),
              
              sErr_pre("\033[5;31m\u2639\033[0m \033[1;30m"),
              bLog(false)
{

}

_msg::_msg(const _msg& other) {
    this->sName=other.sName;
    this->sThreadname=other.sThreadname;
    
}

_msg::~_msg() {
    sName.clear();
    sThreadname.clear();
    
    sSuf.clear();
    sSta_pre.clear();
    sMid_pre.clear();
    sEnd_pre.clear();
    sErr_pre.clear();
    sThd_pre.clear();
    sThd_suf.clear();
    
    if (sfFlux.is_open())
        sfFlux.close();
}

void _msg::msg(const std::string& sMsg) {
    
    std::stringstream ssS;
    
     ssS << sMid_pre << sName << sSuf
               << ":"
               << sMsg
               << "\n";
    
    std::cout << ssS.str();
    write(ssS.str());
}

void _msg::msg(eMsg emType, const std::string& sMsg) {
    
    std::stringstream ssS;
    
    switch(emType) {
        case eMsg::START:
           ssS << sSta_pre << sName << sSuf
                      << " "
                      << sMsg
                      << "\n";  
            break;
        case eMsg::MID:
            ssS << sMid_pre << sName << sSuf
                      << " "
                      << sMsg
                      << "\n";  
            break;
        case eMsg::END:
            ssS << sEnd_pre << sName << sSuf
                      << " "
                      << sMsg
                      << "\n";  
            break;
        case eMsg::THREADS:
            ssS << sThd_pre << sThreadname 
                      << "("
                      << sSuf
                      << sThd_suf
                      << syscall(__NR_gettid)
                      << sSuf
                      << "\033[1;30m"
                      << "): "
                      << sSuf
                      << sMsg
                      << "\n";  
            break;
        case eMsg::ERROR:
            ssS << sErr_pre << sName << sSuf
                      << " "
                      << sMsg
                      << "\n";
            break;
        
        std::cout << ssS.str();
        write(ssS.str());
    }
}

void _msg::error(const std::string& sMsg) {
    
    std::stringstream ssS;
    
     std::cout << sErr_pre << sName << sSuf
               << " "
               << sMsg
               << "\n";
               
    std::cout << ssS.str();
    write(ssS.str());
}

void _msg::set_name(const std::string sName) {
    if (!sName.empty())
        this->sName=sName;
    else {
        std::cerr << "set_name(): empty string\n";
        if (sfFlux)
            sfFlux << "set_name(): empty string\n";
    }
}

void _msg::set_threadname(const std::string sName) {
    if (!sName.empty())
        this->sThreadname=sName;
    else {
        std::cerr << "set_threadname(): empty string\n";
        if (sfFlux)
            sfFlux << "set_threadname(): empty string\n";
    }
}

void _msg::set_log(const std::string sLog) {
    if (sLog.empty()) {
        std::cerr << "set_log(): empty string\n";
        if (sfFlux)
            sfFlux << "set_log(): empty string\n";
    }
    else {
        this->sLog=sLog;
        this->bLog=true;
    
        sfFlux=std::fstream(sLog, std::ios::app);

        if(!sfFlux)
            std::cerr << "set_log(): cannot open "<< sLog << "\n";
        
    }
}

void _msg::write(const std::string& sS) {
    if (sfFlux)
        sfFlux << sS;
    else
        std::cerr << "write(): flux not available\n";
}

