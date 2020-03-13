/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2020  A. Lemonnier <lemonnier.audric@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "msg.h"

_msg::_msg(): sSuf("\033[0m"),
              sSta_pre("\033[3;32m\u25B6\033[0m \033[1;34m"),
              sMid_pre("\033[3;32m\u2690\033[0m \033[1;30m"),
              sEnd_pre("\033[3;32m\u2690\033[0m \033[1;30m"),
              sThd_pre("\033[3;32m\u2690\033[0m \033[1;30m"),
              sThd_suf("\033[0m"),
              sErr_pre("\033[5;31m\u2639\033[0m \033[1;30m")
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
}

void _msg::msg(const std::string& sMsg) const {
     std::cout << sMid_pre << sName << sSuf
               << ":"
               << sMsg
               << "\n";    
}

void _msg::msg(eMsg emType, const std::string& sMsg) const {
    switch(emType) {
        case eMsg::START:
            std::cout << sSta_pre << sName << sSuf
                      << " "
                      << sMsg
                      << "\n";  
            break;
        case eMsg::MID:
            std::cout << sMid_pre << sName << sSuf
                      << ": "
                      << sMsg
                      << "\n";  
            break;
        case eMsg::END:
            std::cout << sEnd_pre << sName << sSuf
                      << ": "
                      << sMsg
                      << "\n";  
            break;
        case eMsg::THREADS:
            std::cout << sThd_pre << sThreadname << sSuf
                      << ": "
                      << sMsg
                      << "\n";  
            break;
        case eMsg::ERROR:
            std::cout << sErr_pre << sName << sSuf
                      << " "
                      << sMsg
                      << "\n";
            break;
    }
}

void _msg::error(const std::string& sMsg) const {
     std::cout << sErr_pre << sName << sSuf
               << ": "
               << sMsg
               << "\n";
}

void _msg::set_name(const std::string sName) {
    if (!sName.empty())
        this->sName=sName;
    else
        std::cerr << "set_name(): empty string\n";
}

void _msg::set_threadname(const std::string sName) {
    if (!sName.empty())
        this->sThreadname=sName;
    else
        std::cerr << "set_threadname(): empty string\n";
}

    
    
