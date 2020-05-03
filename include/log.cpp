#include <log.h>

_log::_log(): sHistname(".history") { }

_log::_log(char** argv, const std::string &sFilename) {
    this->set_logname(sFilename);
    set_execname(argv);
}

_log::~_log() { if (this->sfFlux)  sfFlux.close(); }

bool _log::set_execname(char** argv) {
    if (argv[0]!=nullptr) {
        this->sExec=std::string(argv[0]);
        return true;
    }
    err_msg("_log::_log()", "empty *argv");
    return false;
}

bool _log::set_logname(const std::string& sFilename) {
     if (sFilename.empty()) {
         err_msg("_log::get_logname()", "empty filename");
         return false;
     }
     this->sLogname=sFilename;
     return true;
}

bool _log::set_historyname(const std::string& sFilename) {
     if (sFilename.empty()) {
         err_msg("_log::get_historyname()", "empty filename");
         return false;
     }
     this->sHistname=sFilename;
     return true;
}

const std::string _log::get_execname() const{
    if (this->sExec.empty()) {
        err_msg("_log::get_execname()", "empty filename");
        return "NONAME";
    }
    return this->sLogname;
}

const std::string _log::get_logname() const{
    if (this->sLogname.empty()) {
        err_msg("_log::get_logname()", "empty filename");
        return "NONAME";
    }
    return this->sLogname;
}

const std::string _log::get_historyname() const{
    if (this->sLogname.empty()) {
        std::cerr << "- _log::get_historyname(): empty filename" << std::endl;
        err_msg("_log::get_historyname()", "empty filename");
        return "NONAME";
    }
    return this->sHistname;
}

bool _log::write(const std::string& sS) {
    std::lock_guard<std::mutex> lgG(this->mLock);
    sfFlux=std::fstream(this->get_logname(), std::ios::app);
    if (sfFlux) {
        sfFlux << sS;
        sfFlux.close();
        return true;
    }
    err_msg("_log::write()", "flux not available");
    return false;
}

bool _log::write_history(const boost::program_options::variables_map& vm) {
    sfFlux=std::fstream(get_historyname(), std::ios::app);
    if (sfFlux) {
        
        std::stringstream ssS;
        ssS << this->get_execname();
        for(const auto &arg: vm) {
            if (arg.second.value().type()==typeid(std::string))
                ssS << " --" << arg.first.c_str() << " \""<< arg.second.as<std::string>() << "\"";
            if (arg.second.value().type()==typeid(int))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<int>();
            if (arg.second.value().type()==typeid(unsigned int))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<unsigned int>();
            if (arg.second.value().type()==typeid(float))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<float>();
            if (arg.second.value().type()==typeid(char))
                ssS << " --" << arg.first.c_str() << " "<< arg.second.as<char>();
            if (arg.second.value().type()==typeid(std::vector<std::string>)) {
                for(auto sS: arg.second.as<std::vector<std::string>>())
                    ssS << " --" << arg.first.c_str() << " \""<< sS << "\"";
            }
        }
        ssS << "\n";
        sfFlux << ssS.str();
        sfFlux.close();

        return true;
    }
    err_msg("_log::write_history()", "cannot open ", get_historyname());
    return false;
}

bool _log::remove_duplicate() {

    sfFlux=std::fstream(this->get_historyname(), std::ios::in);
    if (sfFlux) {
        std::vector<size_t> vIndex;
        std::vector<std::string> vsLine;
        
        std::string sLine;
                
        while(std::getline(sfFlux,sLine)) 
            vsLine.emplace_back(sLine);
    
        std::vector<std::string>::iterator vsiTmp(std::unique(vsLine.begin(), vsLine.end()));
        vsLine.resize(std::distance(vsLine.begin(), vsiTmp));

        sfFlux.close(); // close and reopen
        
        sfFlux=std::fstream(this->get_historyname(), std::ios::out | std::ios::trunc);
        for(auto sS: vsLine)
            sfFlux << sS << "\n";
        sfFlux.close();
        return true;
    }
    err_msg("_log::write_history()", "cannot open ", get_historyname() );
    return false;
}

void _log::err_msg(const std::string& sMsg0) const {
    std::cerr << "\033[5;31m\u2639\033[0m \033[1;30m" << sMsg0 << "\033[1;95m:" << std::endl;
}

void _log::err_msg(const std::string& sMsg0, 
                   const std::string& sMsg1) const {
    std::cerr << "\033[5;31m\u2639\033[0m \033[1;30m" << sMsg0 << "\033[1;95m:" << sMsg1 << std::endl;
}

void _log::err_msg(const std::string& sMsg0, 
                   const std::string& sMsg1,
                   const std::string& sMsg2) const {
    std::cerr << "\033[5;31m\u2639\033[0m \033[1;30m" << sMsg0 << "\033[1;95m:" << sMsg1 << sMsg2 << std::endl;
}
