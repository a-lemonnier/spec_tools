#define BOOST_TEST_MODULE Tests

#include <iostream>
#include <ctime>

#include "marker.h"
#include "msg.h"

#include <boost/test/unit_test.hpp>

float norm_rand() {
    return static_cast<float>(rand())/RAND_MAX;
}

BOOST_AUTO_TEST_CASE(SetData_float) {
    int iSize=500;
    float iStep=0.1;
    std::vector<float> vfX, vfY;
    srand(time(nullptr));
    for(int i=0;i<iSize;i++) {
        vfX.emplace_back(i*iStep);
        vfY.emplace_back(norm_rand());
    }
    _marker<float> fMarker;
    fMarker.set_verbose(false);
    BOOST_CHECK(fMarker.set_data(vfX, vfY ));      
}

BOOST_AUTO_TEST_CASE(SetData_double) {
    int iSize=500;
    float iStep=0.1;
    std::vector<double> vdX, vdY;
    srand(time(nullptr));
    for(int i=0;i<iSize;i++) {
        vdX.emplace_back(i*iStep);
        vdY.emplace_back(norm_rand());
     }
    _marker<double> dMarker;
    dMarker.set_verbose(false);
    BOOST_CHECK(dMarker.set_data(vdX, vdY ));
}

BOOST_AUTO_TEST_CASE(SetData_double_long) {
    int iSize=500;
    float iStep=0.1;
    std::vector<double long> vdlX, vdlY;
    srand(time(nullptr));
    for(int i=0;i<iSize;i++) {
        vdlX.emplace_back(i*iStep);
        vdlY.emplace_back(norm_rand());
     }
    _marker<double long> dlMarker;
    dlMarker.set_verbose(false);
    BOOST_CHECK(dlMarker.set_data(vdlX, vdlY ));
}

// BOOST_AUTO_TEST_CASE(SetData_float128) {
//     int iSize=500;
//     float iStep=0.1;
//     std::vector<__float128> vqX, vqY;
//     srand(time(nullptr));
//     for(int i=0;i<iSize;i++) {
//         vqX.emplace_back(i*iStep);
//         vqY.emplace_back(norm_rand());
//      }
//     _marker<__float128> qMarker;
//     qMarker.set_verbose(false);
//     BOOST_CHECK(qMarker.set_data(vqX, vqY ));
// }


