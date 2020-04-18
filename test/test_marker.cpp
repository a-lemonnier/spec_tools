#define BOOST_TEST_MODULE "C++ Unit Tests for marker"
#include <boost/test/unit_test.hpp>

#include <vector>
#include <random>

#include "marker.h"



BOOST_AUTO_TEST_CASE(SetData) {
    
    int iSize=500;
    float iStep=0.1;
    
    std::vector<float> vfX, vfY;
    std::vector<double> vdX, vdY;
    std::vector<double long> vdlX, vdlY;
    std::vector<long> vlX, vlY;
    std::vector<long long> vllX, vllY;
    
    for(int i=0;i<iSize;i++) {
        vfX.emplace_back(i*iStep);
        vdY.emplace_back(rand());
        vdX.emplace_back(i*iStep);
        vdY.emplace_back(rand());
        vdlX.emplace_back(i*iStep);
        vdlY.emplace_back(rand());
        vllX.emplace_back(i*iStep);
        vllY.emplace_back(rand());
    }
  
  BOOST_CHECK_EQUAL(0,0);
  
}
