#define BOOST_TEST_MODULE Tests

#include <iostream>
#include <ctime>
#include <tuple>

#include "marker.h"
#include "msg.h"

#include <boost/test/unit_test.hpp>

// ----------------------------------
// Test parameters
#define SIZE 500
#define STEP 0.1
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
std::tuple<std::vector<_T>, std::vector<_T> > gen_rand_spec(int iSize);
// ----------------------------------

// ----------------------------------
// Test _marker::set_data()

BOOST_AUTO_TEST_CASE(SetData_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>(SIZE);
    BOOST_CHECK(rMarker.set_data(vrX, vrY));      
}

BOOST_AUTO_TEST_CASE(SetData_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>(SIZE);
    BOOST_CHECK(rMarker.set_data(vrX, vrY));      
}

BOOST_AUTO_TEST_CASE(SetData_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>(SIZE);
    BOOST_CHECK(rMarker.set_data(vrX, vrY));      
}

// ----------------------------------




// ----------------------------------
// Misc. functions

float norm_rand() {
    return static_cast<float>(rand())/RAND_MAX;
}

template<typename _T>
std::tuple<std::vector<_T>, std::vector<_T> > gen_rand_spec(int iSize) {
    std::vector<_T> TX, TY;
    srand(time(nullptr));
    for(int i=0;i<iSize;i++) {
        TX.emplace_back(i*STEP);
        TY.emplace_back(norm_rand());
     }
    return {TX, TY};
}
// ----------------------------------
