#include <msg.h>

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
    
    std::lock_guard<std::mutex> lgG(this->mLock);
    
    if (sfFlux.is_open()) {
        sfFlux << "\n";
        sfFlux.close();
    }
}

bool _msg::msg(const std::string& sMsg) {
    
    std::stringstream ssS, ssSw;
    
    time_t ttT=time(nullptr);
    std::string sS(asctime(gmtime(&ttT)));
    
    sS.erase(std::remove(sS.begin(), sS.end(), '\n'), sS.end());
    
    ssSw << "- " << sS << " | ";
    
     ssS << sMid_pre << sName << sSuf
               << ":"
               << sMsg
               << "\n";
    ssSw << sName << ": " << sMsg << "\n";
    
    std::cout << ssS.str();
    if (bLog)
        return write(ssSw.str());
    return true;
}

bool _msg::msg(eMsg emType, const std::string& sMsg) {
    
    std::stringstream ssS, ssSw;
    
    time_t ttT=time(nullptr);
    std::string sS(asctime(gmtime(&ttT)));
    
    sS.erase(std::remove(sS.begin(), sS.end(), '\n'), sS.end());
    
    ssSw << "- " << sS << " | ";
    
    switch(emType) {
        case eMsg::START:
           ssS << sSta_pre << sName << sSuf
                      << " "
                      << sMsg
                      << "\n";
            ssSw << sName << " " << sMsg << "\n";
            break;
        case eMsg::MID:
            ssS << sMid_pre << sName << sSuf
                      << " "
                      << sMsg
                      << "\n";
            ssSw << sName << ": " << sMsg << "\n";
            break;
        case eMsg::END:
            ssS << sEnd_pre << sName << sSuf
                      << " "
                      << sMsg
                      << "\n";
            ssSw << sName << ": " << sMsg << "\n";
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
            ssSw << sThreadname << "("<< syscall(__NR_gettid) << "): " << sMsg << "\n";
            break;
        case eMsg::ERROR:
            ssS << sErr_pre << sName << sSuf
                      << " "
                      << sMsg
                      << "\n";
            ssSw << "Error: " << sName << ": " << sMsg << "\n";
            break;
    }
    std::cout << ssS.str();
    if (bLog)
        return write(ssSw.str());
    return true;
}

bool _msg::error(const std::string& sMsg) {
    std::stringstream ssS, ssSw;
    
    time_t ttT=time(nullptr);
    std::string sS(asctime(gmtime(&ttT)));
    
    sS.erase(std::remove(sS.begin(), sS.end(), '\n'), sS.end());
    
    ssSw << "- " << sS << " | ";
    
    std::cout << sErr_pre << sName << sSuf
               << " "
               << sMsg
               << "\n";
               
    std::cout << ssS.str();
    if (bLog)
        return write(ssSw.str());
    return true;
}

bool _msg::set_name(const std::string sName) {
    if (!sName.empty()) {
        this->sName=sName;
        return true;
    }
    std::cerr << "- _msg::set_name(): empty string\n";
    if (sfFlux)
        sfFlux << "- _msg::set_name(): empty string\n";
    return false;
}


bool _msg::set_threadname(const std::string sName) {
    if (!sName.empty()) {
        this->sThreadname=sName;
        return true;
    }
    std::cerr << "- _msg::set_threadname(): empty string\n";
    if (sfFlux)
        sfFlux << "- _msg::set_threadname(): empty string\n";
    return false;
}

bool _msg::set_log(const std::string sLog) {
    if (sLog.empty()) {
        std::cerr << "- _msg::set_log(): empty string\n";
        if (sfFlux)
            sfFlux << "- _msg::set_log(): empty string\n";
        return false;
    }
    this->sLog=sLog;
    this->bLog=true;
    return true;
}

bool _msg::write(const std::string& sS) {
    std::lock_guard<std::mutex> lgG(this->mLock);
    sfFlux=std::fstream(sLog, std::ios::app);
    if (sfFlux) {
        sfFlux << sS;
        sfFlux.close();
        return true;
    }
    std::cerr << "- _msg::write(): flux not available\n";
    return false;
}

void _msg::enable_log(bool bLog) {
    this->bLog=bLog;
}
