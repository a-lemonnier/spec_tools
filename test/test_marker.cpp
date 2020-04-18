#define BOOST_TEST_MODULE Tests

#include <iostream>
#include <string>
#include <ctime>
#include <tuple>
#include <random>

#include "marker.h"
#include "msg.h"

#include <boost/test/unit_test.hpp>

// ----------------------------------
// Test parameters
#define MAXLSTR 20 // length-5
#define STEP 1./(rand()+1.)
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

std::string gen_rand_string();

// ----------------------------------

// ----------------------------------
// Test _marker::set_data()

BOOST_AUTO_TEST_CASE(SetData_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    BOOST_CHECK(rMarker.set_data(vrX, vrY));      
}

BOOST_AUTO_TEST_CASE(SetData_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    BOOST_CHECK(rMarker.set_data(vrX, vrY));      
}

BOOST_AUTO_TEST_CASE(SetData_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    BOOST_CHECK(rMarker.set_data(vrX, vrY));      
}

// ----------------------------------
// String Setters

BOOST_AUTO_TEST_CASE(SetTitle) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_title(gen_rand_string()));
}

BOOST_AUTO_TEST_CASE(SetLabel) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_label(gen_rand_string()));
}

BOOST_AUTO_TEST_CASE(SetXlabel) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_xlabel(gen_rand_string()));
}

BOOST_AUTO_TEST_CASE(SetYlabel) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_ylabel(gen_rand_string()));
}

BOOST_AUTO_TEST_CASE(SetXunit) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_xunit(gen_rand_string()));
}

BOOST_AUTO_TEST_CASE(SetYunit) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_yunit(gen_rand_string()));
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
    int iSize=rand()%500+500;
    for(int i=0;i<iSize;i++) {
        TX.emplace_back(i*STEP);
        TY.emplace_back(norm_rand());
     }
    return {TX, TY};
}

std::string gen_rand_string() {
    srand(time(nullptr));
    std::string sRes;
    int iSize=rand()%MAXLSTR+5;
    for(int i=0; i<iSize/2;i++) {
        char cC;
        if (i%2==0)
            cC=(rand()%(90-65+1))+65;
        else
            cC=(rand()%(122-97+1))+97;
        sRes+=cC;
    }   
    return sRes;
}

// ----------------------------------
