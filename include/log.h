/**
 * \file log.h
 * \brief A class to write log file.
 * \author Audric Lemonnier
 * \version 0.1
 * \date 03/05/2020
 */

#ifndef _LOG_H
#define _LOG_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <algorithm>

#include <mutex>

#include <boost/program_options.hpp>

class _log {
public:
    
    explicit _log();
    /**
     * \fn explicit _log(char** argv, const std::string &sFilename)
     * \brief Initialize with the first argument of the command line, and the log name.
     */
    explicit _log(char** argv, const std::string &sFilename);
    virtual ~_log();
    
    const std::string get_execname() const;
    const std::string get_logname() const;
    const std::string get_historyname() const;
    
    bool set_execname(char **argv);
    bool set_logname(const std::string &sFilename);
    bool set_historyname(const std::string &sFilename);
    
    /**
     * \fn bool write(const std::string &sS)
     * \brief Write a string in the log file.
     */
    bool write(const std::string &sS);
    bool write(const std::stringstream &ssS);
    /**
     * \fn bool write_history(const boost::program_options::variables_map &vm)
     *  \brief Write history file with information from boost::program_options
     */
    bool write_history(const boost::program_options::variables_map &vm);
    
    /**
     * \fn bool remove_duplicate()
     * \brief Remove duplicates in history file.
     */
    bool remove_duplicate();
    
private:
    std::string sExec;
    std::string sLogname;
    std::string sHistname;
    
    std::fstream sfFlux;
    
    std::mutex mLock; /**< Be sure access to file is not shared. */
  
    void err_msg(const std::string &sMsg) const;
    void err_msg(const std::string& sMsg0, 
                 const std::string& sMsg1) const;
    void err_msg(const std::string& sMsg0, 
                 const std::string& sMsg1,
                 const std::string& sMsg2) const;
};



#endif // log.h
