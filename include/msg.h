/**
 * \file msg.h
 * \brief A class to print and write message
 * \author Audric Lemonnier
 * \version 0.2
 * \date 18/04/2020
 */

#ifndef _MSG_H
#define _MSG_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <algorithm>

#include <mutex>

#if __has_include (<sys/syscall.h>) && __has_include (<unistd.h>)
#include <unistd.h>
#include <sys/syscall.h>
#define HAS_SYSCALL
#endif

/**
 * \class _msg
 * \brief A class that sends string to std output and in a file...
 */
class _msg
{
public:
    /**
     * \enum eMsg
     * \brief enum for method in order to define whether the message is at the begin, at the end or an error,
     */
    enum eMsg {
        START, 
        MID, 
        END, 
        ERROR, 
        THREADS };
    
    explicit _msg();

    explicit _msg(const _msg& other);

    virtual ~_msg();
        
    /**
     * \fn bool msg(const std::string& sMsg)
     * \brief Send a message with eMsg::MID as default
     */
    bool msg(const std::string& sMsg);
    
    /**
     * \fn bool msg(eMsg emType, const std::string& sMsg)
     * \brief Send a message...
     * \param emType See enum eMsg::
     */
    bool msg(eMsg emType, const std::string& sMsg);
    
    /**
     * \fn bool error(const std::string& sMsg) 
     * \brief Send an error message...
     */
    bool error(const std::string& sMsg);
    
    /**
     * \fn template<typename ...Args> bool msg(const Args& ...args) 
     * \brief A variadic formatter method that indeed sends arbitratry number of variable to the std output... with eMsg::MID as default
     */
    template<typename ...Args> bool msg(const Args& ...args) {
        std::stringstream sS;
        (void)(int[]){0, ( (void)(sS << " " << args), 0 ) ... };
        return msg(sS.str());
    }

    /**
     * \fn template<typename ...Args> bool msg(eMsg emType, const Args& ...args) 
     * \brief A variadic formatter method that indeed sends arbitratry number of variable to the std output... The first parameter is always the enum eMsg.
     */
    template<typename ...Args> bool msg(eMsg emType, const Args& ...args) {
        std::stringstream sS;
        (void)(int[]){0, ( (void)(sS << args << " "), 0 ) ... };
        return msg(emType, sS.str());
    }
    
    /**
     * \fn template<typename ...Args> bool error(const Args& ...args)
     * \brief A variadic formatter method that indeed sends arbitratry number of variable to the std error output... with eMsg::ERROR as default
     */
    template<typename ...Args> bool error(const Args& ...args) {
        std::stringstream sS;
        (void)(int[]){0, ( (void)(sS << args << " "), 0 ) ... };
        return error(sS.str());
    }
    
    /**
     * \fn void set_name(const std::string sName)
     * \brief Set the name of the main instance
     */
    bool set_name(const std::string sName);
    
    /**
     * \fn void set_threadname(const std::string sName)
     * \brief Set the name of threads
     */
    bool set_threadname(const std::string sName);
    
    /**
     * \fn void set_log(const std::string& sLog)
     * \brief Enable or disable log file
     */
    bool set_log(const std::string sLog);
    
    /**
     * \fn void enable_log(bool bLog)
     * \brief Enable or disable the log file
     */
    void enable_log(bool bLog);
    

private:
    /**
     * \fn bool write(const std::string& sS)
     * \brief Write a string in a defined file
     */
    bool write(const std::string& sS);
    
    std::string sThreadname;
    std::string sName;
    std::string sSuf;
    std::string sSta_pre;
    std::string sMid_pre;
    std::string sEnd_pre;
    std::string sThd_pre;
    std::string sThd_suf;
    std::string sErr_pre;
    
    std::fstream sfFlux;
    
    std::string sLog; /**< If sLog is defined, msg write a log  */  
    bool bLog;
    
    std::mutex mLock; /**< Exclusive file opening  */  

};

#endif // _MSG_H
