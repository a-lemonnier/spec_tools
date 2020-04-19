#define BOOST_TEST_MODULE Tests

#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <ctime>
#include <tuple>
#include <random>


#include "csv.h"

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

BOOST_AUTO_TEST_CASE(csv_str_char_float) {
    typedef float real;
    std::string sFile(gen_rand_string());
    gen_rand_csv<real>(sFile);
    _csv<real> csv(sFile, '\t');
    BOOST_CHECK(csv.get_filename()==sFile);
    BOOST_CHECK(csv.get_separator()=='\t');
    BOOST_CHECK(csv.read());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_str_char_double) {
    typedef double real;
    std::string sFile(gen_rand_string());
    gen_rand_csv<real>(sFile);
    _csv<real> csv(sFile, '\t');
    BOOST_CHECK(csv.get_filename()==sFile);
    BOOST_CHECK(csv.get_separator()=='\t');
    BOOST_CHECK(csv.read());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_str_char_double_long) {
    typedef double long real;
    std::string sFile(gen_rand_string());
    gen_rand_csv<real>(sFile);
    _csv<real> csv(sFile, '\t');
    BOOST_CHECK(csv.get_filename()==sFile);
    BOOST_CHECK(csv.get_separator()=='\t');
    BOOST_CHECK(csv.read());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_str_str_float) {
    typedef float real;
    std::string sFile(gen_rand_string());
    gen_rand_csv<real>(sFile);
    _csv<real> csv(sFile, "\t");
    BOOST_CHECK(csv.get_filename()==sFile);
    BOOST_CHECK(csv.get_separator()=='\t');
    BOOST_CHECK(csv.read());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_str_str_double) {
    typedef double real;
    std::string sFile(gen_rand_string());
    _csv<real> csv(sFile, "\t");
    gen_rand_csv<real>(sFile);
    BOOST_CHECK(csv.get_filename()==sFile);
    BOOST_CHECK(csv.get_separator()=='\t');
    BOOST_CHECK(csv.read());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_str_str_double_long) {
    typedef double long real;
    std::string sFile(gen_rand_string());
    gen_rand_csv<real>(sFile);
    _csv<real> csv(sFile, "\t");
    BOOST_CHECK(csv.get_filename()==sFile);
    BOOST_CHECK(csv.get_separator()=='\t');
    BOOST_CHECK(csv.read());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_vv_float) {
    typedef float real;
    std::string sFile(gen_rand_string());
    _csv<real> csv(gen_rand_vv<real>());
    csv.set_filename_out(sFile);
    BOOST_CHECK(!csv.empty());
    BOOST_CHECK(csv.write());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_vv_double) {
    typedef double real;
    std::string sFile(gen_rand_string());
    _csv<real> csv(gen_rand_vv<real>());
    csv.set_filename_out(sFile);
    BOOST_CHECK(!csv.empty());
    BOOST_CHECK(csv.write());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_vv_double_long) {
    typedef double long real;
    std::string sFile(gen_rand_string());
    _csv<real> csv(gen_rand_vv<real>());
    csv.set_filename_out(sFile);
    BOOST_CHECK(!csv.empty());
    BOOST_CHECK(csv.write());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_vs_vv_float) {
    typedef float real;
    std::string sFile(gen_rand_string());
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(gen_rand_vs<real>(vvR.size()),vvR);
    csv.set_filename_out(sFile);
    BOOST_CHECK(!csv.empty());
    BOOST_CHECK(csv.write());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_vs_vv_double) {
    typedef double real;
    std::string sFile(gen_rand_string());
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(gen_rand_vs<real>(vvR.size()),vvR);
    csv.set_filename_out(sFile);
    BOOST_CHECK(!csv.empty());
    BOOST_CHECK(csv.write());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_vs_vv_double_long) {
    typedef double long real;
    std::string sFile(gen_rand_string());
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(gen_rand_vs<real>(vvR.size()),vvR);
    csv.set_filename_out(sFile);
    BOOST_CHECK(!csv.empty());
    BOOST_CHECK(csv.write());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_vs_vv_char_float) {
    typedef float real;
    std::string sFile(gen_rand_string());
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(gen_rand_vs<real>(vvR.size()),vvR, '\t');
    csv.set_filename_out(sFile);
    BOOST_CHECK(!csv.empty());
    BOOST_CHECK(csv.write());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_vs_vv_char_double) {
    typedef double real;
    std::string sFile(gen_rand_string());
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(gen_rand_vs<real>(vvR.size()),vvR, '\t');
    csv.set_filename_out(sFile);
    BOOST_CHECK(!csv.empty());
    BOOST_CHECK(csv.write());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(csv_vs_vv_char_double_long) {
    typedef double long real;
    std::string sFile(gen_rand_string());
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(gen_rand_vs<real>(vvR.size()),vvR, '\t');
    csv.set_filename_out(sFile);
    BOOST_CHECK(!csv.empty());
    BOOST_CHECK(csv.write());
    remove(sFile.c_str());
}

BOOST_AUTO_TEST_CASE(SelectLine_float) {
    typedef float real;
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0; i< vvR.size(); i++)
        BOOST_CHECK(vvR[i]==csv.select_line(i));
}

BOOST_AUTO_TEST_CASE(SelectLine_double) {
    typedef double real;
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0; i< vvR.size(); i++)
        BOOST_CHECK(vvR[i]==csv.select_line(i));
}

BOOST_AUTO_TEST_CASE(SelectLine_double_long) {
    typedef double long real;
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0; i< vvR.size(); i++)
        BOOST_CHECK(vvR[i]==csv.select_line(i));
}


BOOST_AUTO_TEST_CASE(SelectCol_float) {
    typedef float real;
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int j=0; j< vvR[0].size(); j++)
        for(int i=0; i< vvR.size(); i++)
            BOOST_CHECK( vvR[i][j]==csv.select_column(j)[i] );
}

BOOST_AUTO_TEST_CASE(SelectCol_double) {
    typedef double real;
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int j=0; j< vvR[0].size(); j++)
        for(int i=0; i< vvR.size(); i++)
            BOOST_CHECK( vvR[i][j]==csv.select_column(j)[i] );
}

BOOST_AUTO_TEST_CASE(SelectCol_double_long) {
    typedef double long real;
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int j=0; j< vvR[0].size(); j++)
        for(int i=0; i< vvR.size(); i++)
            BOOST_CHECK( vvR[i][j]==csv.select_column(j)[i] );
}

BOOST_AUTO_TEST_CASE(GenRandSpec_float) {
    typedef float real;
    _csv<real> csv;
    real rMin=rand()%4000+4000;
    BOOST_CHECK(csv.genrandspec(rMin, rMin+4000, STEP));
}

BOOST_AUTO_TEST_CASE(GenRandSpec_double) {
    typedef double real;
    _csv<real> csv;
    real rMin=rand()%4000+4000;
    BOOST_CHECK(csv.genrandspec(rMin, rMin+4000, STEP));
}

BOOST_AUTO_TEST_CASE(GenRandSpec_double_long) {
    typedef double long real;
    _csv<real> csv;
    real rMin=rand()%4000+4000;
    BOOST_CHECK(csv.genrandspec(rMin, rMin+4000, STEP));
}

BOOST_AUTO_TEST_CASE(TransformLin_float) {
    typedef float real;
    real rA=rand()%4000;
    real rB=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0;i<vvR[0].size();i++)
        BOOST_CHECK(csv.transform_lin(rA, rB, i));
}

BOOST_AUTO_TEST_CASE(TransformLin_double) {
    typedef double real;
    real rA=rand()%4000;
    real rB=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0;i<vvR[0].size();i++)
        BOOST_CHECK(csv.transform_lin(rA, rB, i));
}

BOOST_AUTO_TEST_CASE(TransformLin_double_long) {
    typedef double long real;
    real rA=rand()%4000;
    real rB=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0;i<vvR[0].size();i++)
        BOOST_CHECK(csv.transform_lin(rA, rB, i));
}

BOOST_AUTO_TEST_CASE(Shift_float) {
    typedef float real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    BOOST_CHECK(csv.shift(rR));
}

BOOST_AUTO_TEST_CASE(Shift_double) {
    typedef double real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    BOOST_CHECK(csv.shift(rR));
}

BOOST_AUTO_TEST_CASE(Shift_double_long) {
    typedef double long real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    BOOST_CHECK(csv.shift(rR));
}

BOOST_AUTO_TEST_CASE(MaxThres_float) {
    typedef float real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    BOOST_CHECK(csv.apply_max_threshold(rR));
}

BOOST_AUTO_TEST_CASE(MaxThres_double) {
    typedef double real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    BOOST_CHECK(csv.apply_max_threshold(rR));
}

BOOST_AUTO_TEST_CASE(MaxThres_double_long) {
    typedef double long real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    BOOST_CHECK(csv.apply_max_threshold(rR));
}

BOOST_AUTO_TEST_CASE(MinThres_float) {
    typedef float real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    BOOST_CHECK(csv.apply_min_threshold(rR));
}

BOOST_AUTO_TEST_CASE(MinThres_double) {
    typedef double real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    BOOST_CHECK(csv.apply_min_threshold(rR));
}

BOOST_AUTO_TEST_CASE(MinThres_double_long) {
    typedef double long real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    BOOST_CHECK(csv.apply_min_threshold(rR));
}

BOOST_AUTO_TEST_CASE(MaxThres_int_float) {
    typedef float real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0; i<vvR[0].size(); i++)
        BOOST_CHECK(csv.apply_max_threshold(rR,i));
}

BOOST_AUTO_TEST_CASE(MaxThres_int_double) {
    typedef double real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0; i<vvR[0].size(); i++)
        BOOST_CHECK(csv.apply_max_threshold(rR,i));
}

BOOST_AUTO_TEST_CASE(MaxThres_int_double_long) {
    typedef double long real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0; i<vvR[0].size(); i++)
        BOOST_CHECK(csv.apply_max_threshold(rR,i));
}

BOOST_AUTO_TEST_CASE(MinThres_int_float) {
    typedef float real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0; i<vvR[0].size(); i++)
        BOOST_CHECK(csv.apply_min_threshold(rR,i));
}

BOOST_AUTO_TEST_CASE(MinThres_int_double) {
    typedef double real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0; i<vvR[0].size(); i++)
        BOOST_CHECK(csv.apply_min_threshold(rR,i));
}

BOOST_AUTO_TEST_CASE(MinThres_int_double_long) {
    typedef double long real;
    real rR=1./(rand()+1.0);
    auto vvR=gen_rand_vv<real>();
    _csv<real> csv(vvR);
    for(int i=0; i<vvR[0].size(); i++)
        BOOST_CHECK(csv.apply_min_threshold(rR,i));
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
