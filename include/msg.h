#ifndef _MSG_H
#define _MSG_H

#include <iostream>
#include <string>
#include <sstream>

#if __has_include (<sys/syscall.h>) && __has_include (<unistd.h>)
#include <unistd.h>
#include <sys/syscall.h>
#define HAS_SYSCALL
#endif

/**
 * \class _msg
 * \brief A class that sends string to std output...
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
     * \fn void msg(const std::string& sMsg) const
     * \brief Send a message with eMsg::MID as default
     */
    void msg(const std::string& sMsg) const;
    
    /**
     * \fn void msg(eMsg emType, const std::string& sMsg) const
     * \brief Send a message...
     * \param emType See enum eMsg::
     */
    void msg(eMsg emType, const std::string& sMsg) const;
    
    /**
     * \fn void error(const std::string& sMsg) const
     * \brief Send an error message...
     */
    void error(const std::string& sMsg) const;
    
    /**
     * \fn template<typename ...Args> void msg(const Args& ...args) const
     * \brief A variadic formatter method that indeed sends arbitratry number of variable to the std output... with eMsg::MID as default
     */
    template<typename ...Args> void msg(const Args& ...args) const {
        std::stringstream sS;
        (void)(int[]){0, ( (void)(sS << " " << args), 0 ) ... };
        msg(sS.str());
    }

    /**
     * \fn template<typename ...Args> void msg(eMsg emType, const Args& ...args) const
     * \brief A variadic formatter method that indeed sends arbitratry number of variable to the std output... The first parameter is always the enum eMsg.
     */
    template<typename ...Args> void msg(eMsg emType, const Args& ...args) const {
        std::stringstream sS;
        (void)(int[]){0, ( (void)(sS << args << " "), 0 ) ... };
        msg(emType, sS.str());
    }
    
    /**
     * \fn template<typename ...Args> void error(const Args& ...args) const
     * \brief A variadic formatter method that indeed sends arbitratry number of variable to the std error output... with eMsg::ERROR as default
     */
    template<typename ...Args> void error(const Args& ...args) const {
        std::stringstream sS;
        (void)(int[]){0, ( (void)(sS << args << " "), 0 ) ... };
        error(sS.str());
    }
    
    /**
     * \fn void set_name(const std::string sName)
     * \brief Set the name of the main instance
     */
    void set_name(const std::string sName);
    
    /**
     * \fn void set_threadname(const std::string sName)
     * \brief Set the name of threads
     */
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
