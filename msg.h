#ifndef _MSG_H
#define _MSG_H

#include <iostream>
#include <string>
#include <sstream>

/**
 * @todo write docs
 */
class _msg
{
public:
    enum eMsg {
        START, 
        MID, 
        END, 
        ERROR, 
        THREADS };
    
    explicit _msg();

    explicit _msg(const _msg& other);

    virtual ~_msg();
        
    void msg(const std::string& sMsg) const;
    void msg(eMsg emType, const std::string& sMsg) const;
    void error(const std::string& sMsg) const;
    
    
    template<typename ...Args> void msg(const Args& ...args) const {
        std::stringstream sS;
        (void)(int[]){0, ( (void)(sS << " " << args), 0 ) ... };
        msg(sS.str());
    }

    template<typename ...Args> void msg(eMsg emType, const Args& ...args) const {
        std::stringstream sS;
        (void)(int[]){0, ( (void)(sS << args << " "), 0 ) ... };
        msg(emType, sS.str());
    }
    
    template<typename ...Args> void error(const Args& ...args) const {
        std::stringstream sS;
        (void)(int[]){0, ( (void)(sS << args << " "), 0 ) ... };
        error(sS.str());
    }
    
    void set_name(const std::string sName);
    void set_threadname(const std::string sName);
    

private:
    std::string sThreadname;
    std::string sName;
    std::string sSuf;
    std::string sSta_pre;
    std::string sMid_pre;
    std::string sEnd_pre;
    std::string sThd_pre;
    std::string sThd_suf;
    std::string sErr_pre;
};

#endif // _MSG_H
