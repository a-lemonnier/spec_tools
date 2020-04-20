#define BOOST_TEST_MODULE Tests

#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <ctime>
#include <tuple>
#include <random>


#include "msg.h"

#include <boost/test/unit_test.hpp>

// ----------------------------------
// Test parameters
#define MAXLSTR 30 // length-5
#define STEP .1
// ----------------------------------

// ----------------------------------
// Misc. functions

/**
 * \fn float norm_rand()
 * generate a random float normalize to 1
 */
float norm_rand();

/**
 * \fn template<typename _T> std::tuple<std::vector<_T>, std::vector<_T> > gen_rand_spec(int iSize)
 * \brief Generate a random spectrum normalize to 1
 * \return std::tuple (decomposition declarations c++17 used)
 */
template<typename _T>
std::tuple<std::vector<_T>, std::vector<_T> > gen_rand_spec();

template<typename _T>
std::vector<std::vector<_T> > gen_rand_vv();

template<typename _T>
std::vector<std::string > gen_rand_vs(int iSize);

std::string gen_rand_string();

template<typename _T>
void gen_rand_csv(const std::string &sFilename);

// ----------------------------------

// ----------------------------------

BOOST_AUTO_TEST_CASE(Msg_str) {
    _msg msg;
    std::string sLog(gen_rand_string());
    msg.set_log(sLog);
    msg.set_name(gen_rand_string());
    msg.set_threadname(gen_rand_string());
    BOOST_CHECK(msg.msg(gen_rand_string()));
    BOOST_CHECK(msg.msg(gen_rand_string(), gen_rand_string()));
    remove(sLog.c_str());
}

BOOST_AUTO_TEST_CASE(Msg_enum_str) {
    _msg msg;
    std::string sLog(gen_rand_string());
    msg.set_log(sLog);
    msg.set_name(gen_rand_string());
    msg.set_threadname(gen_rand_string());
    BOOST_CHECK(msg.msg(_msg::eMsg::START, gen_rand_string()));
    BOOST_CHECK(msg.msg(_msg::eMsg::MID, gen_rand_string()));
    BOOST_CHECK(msg.msg(_msg::eMsg::THREADS, gen_rand_string()));
    BOOST_CHECK(msg.msg(_msg::eMsg::ERROR, gen_rand_string()));
    BOOST_CHECK(msg.msg(_msg::eMsg::END, gen_rand_string()));
    BOOST_CHECK(msg.msg(_msg::eMsg::START, gen_rand_string(), gen_rand_string()));
    BOOST_CHECK(msg.msg(_msg::eMsg::MID, gen_rand_string(), gen_rand_string()));
    BOOST_CHECK(msg.msg(_msg::eMsg::THREADS, gen_rand_string(), gen_rand_string()));
    BOOST_CHECK(msg.msg(_msg::eMsg::ERROR, gen_rand_string(), gen_rand_string()));
    BOOST_CHECK(msg.msg(_msg::eMsg::END, gen_rand_string(), gen_rand_string()));
    remove(sLog.c_str());
}


BOOST_AUTO_TEST_CASE(Err_str) {
    _msg msg;
    std::string sLog(gen_rand_string());
    msg.set_log(sLog);
    msg.set_name(gen_rand_string());
    msg.set_threadname(gen_rand_string());
    BOOST_CHECK(msg.error(gen_rand_string()));
    BOOST_CHECK(msg.error(gen_rand_string(), gen_rand_string()));
    remove(sLog.c_str());
}

BOOST_AUTO_TEST_CASE(SetName) {
    _msg msg;
    BOOST_CHECK(msg.set_name(gen_rand_string()));
}

BOOST_AUTO_TEST_CASE(SetThName) {
    _msg msg;
    BOOST_CHECK(msg.set_threadname(gen_rand_string()));
}

BOOST_AUTO_TEST_CASE(SetLog) {
    _msg msg;
    BOOST_CHECK(msg.set_log(gen_rand_string()));
}

// ----------------------------------
// Misc. functions

float norm_rand() {
    return static_cast<float>(rand())/RAND_MAX;
}

template<typename _T>
std::tuple<std::vector<_T>, std::vector<_T> > gen_rand_spec() {
    srand(time(nullptr));
    std::vector<_T> TX, TY;
    int iSize=rand()%4000+4000;
    for(int i=0;i<iSize;i++) {
        TX.emplace_back(i*STEP+4000);
        TY.emplace_back(norm_rand());
     }
    return {TX, TY};
}

template<typename _T>
std::vector<std::vector<_T> > gen_rand_vv() {
    srand(time(nullptr));
    int iSize=rand()%4000+4000;
    std::vector<std::vector<_T> > vvT;
    for(int i=0;i<iSize;i++) 
        vvT.emplace_back(std::vector<_T>({static_cast<_T>(i*STEP+4000.), static_cast<_T>(norm_rand())}));
    return vvT;
}

template<typename _T>
std::vector<std::string > gen_rand_vs(int iSize) {
    srand(time(nullptr));
    std::vector<std::string> vsS;
    for(int i=0;i<iSize;i++)
        vsS.emplace_back(gen_rand_string());
    return vsS;
}

std::string gen_rand_string() {
    srand(time(nullptr));
    std::string sRes;
    int iSize=rand()%MAXLSTR+5;
    for(int i=0; i<iSize;i++) {
        char cC;
        if (i%2==0)
            cC=(rand()%(90-65+1))+65;
        else
            cC=(rand()%(122-97+1))+97;
        sRes+=cC;
    }   
    return sRes;
}

template<typename _T>
void gen_rand_csv(const std::string &sFilename) {
    std::fstream sfFlux(sFilename, std::ios::out);
    if (sfFlux) {
        auto vtT=gen_rand_spec<_T>();
        for(int i=0; i < std::get<0>(vtT).size(); i++) 
            sfFlux << std::get<0>(vtT)[i] << '\t' << std::get<1>(vtT)[i] << "\n";
        sfFlux.close();
    }
}




// ----------------------------------
