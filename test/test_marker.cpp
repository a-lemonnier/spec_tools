#define BOOST_TEST_MODULE Tests

#include <iostream>
#include <cstdio>
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
// String Setters and Getters

BOOST_AUTO_TEST_CASE(SetTitle) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_title(gen_rand_string()));
    BOOST_CHECK(!rMarker.get_title().empty());
}

BOOST_AUTO_TEST_CASE(SetGetLabel) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_label(gen_rand_string()));
    BOOST_CHECK(!rMarker.get_label().empty());
}

BOOST_AUTO_TEST_CASE(SetGetXlabel) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_xlabel(gen_rand_string()));
    BOOST_CHECK(!rMarker.get_xlabel().empty());
}

BOOST_AUTO_TEST_CASE(SetGetYlabel) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_ylabel(gen_rand_string()));
    BOOST_CHECK(!rMarker.get_ylabel().empty());
}

BOOST_AUTO_TEST_CASE(SetXunit) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_xunit(gen_rand_string()));
    BOOST_CHECK(!rMarker.get_xunit().empty());
}

BOOST_AUTO_TEST_CASE(SetGetYunit) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_yunit(gen_rand_string()));
    BOOST_CHECK(!rMarker.get_yunit().empty());
}

BOOST_AUTO_TEST_CASE(SetGetOutput) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_output(gen_rand_string()+".png"));
    BOOST_CHECK(rMarker.set_output(gen_rand_string()+".jpg"));
    BOOST_CHECK(rMarker.set_output(gen_rand_string()+".jpeg"));
    BOOST_CHECK(rMarker.set_output(gen_rand_string()+".pdf"));
    BOOST_CHECK(!rMarker.get_output().empty());
}

BOOST_AUTO_TEST_CASE(SetOutputDpi) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    int iDpi=(rand()%(300-150+1)+150);
    BOOST_CHECK(rMarker.set_output(gen_rand_string()+".png", iDpi));
    BOOST_CHECK(rMarker.set_output(gen_rand_string()+".jpg", iDpi));
    BOOST_CHECK(rMarker.set_output(gen_rand_string()+".jpeg", iDpi));
    BOOST_CHECK(rMarker.set_output(gen_rand_string()+".pdf", iDpi));
    BOOST_CHECK(!rMarker.get_output().empty());
    BOOST_CHECK(rMarker.get_dpi()>0);
}

BOOST_AUTO_TEST_CASE(SetGetScriptName) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_scriptname(gen_rand_string()));
    BOOST_CHECK(!rMarker.get_scriptname().empty());
    
}

BOOST_AUTO_TEST_CASE(SetLog) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_log(gen_rand_string()));    
}

// ----------------------------------
// Real Setters and Getters
BOOST_AUTO_TEST_CASE(SetGetContinuum_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_continuum(norm_rand()));
    BOOST_CHECK(rMarker.get_continuum()>0);
}

BOOST_AUTO_TEST_CASE(SetGetContinuum_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_continuum(static_cast<real>(norm_rand())));
    BOOST_CHECK(rMarker.get_continuum()>0);
}

BOOST_AUTO_TEST_CASE(SetGetContinuum_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_continuum(static_cast<real>(norm_rand())));
    BOOST_CHECK(rMarker.get_continuum()>0);
}

BOOST_AUTO_TEST_CASE(SetGetSupp_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    real RMin=static_cast<real>(norm_rand());
    real RMax=static_cast<real>(norm_rand());
    while(RMin>RMax) {
        RMin=static_cast<real>(norm_rand());
        RMax=static_cast<real>(norm_rand());
    }
    BOOST_CHECK(rMarker.set_supp(RMin, RMax));
    auto [iA, iB]=rMarker.get_supp();
    BOOST_CHECK(iA<iB);
}

BOOST_AUTO_TEST_CASE(SetGetSupp_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    real RMin=static_cast<real>(norm_rand());
    real RMax=static_cast<real>(norm_rand());
    while(RMin>RMax) {
        RMin=static_cast<real>(norm_rand());
        RMax=static_cast<real>(norm_rand());
    }
    BOOST_CHECK(rMarker.set_supp(RMin, RMax));
    auto [iA, iB]=rMarker.get_supp();
    BOOST_CHECK(iA<iB);
}

BOOST_AUTO_TEST_CASE(SetGetSupp_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    real RMin=static_cast<real>(norm_rand());
    real RMax=static_cast<real>(norm_rand());
    while(RMin>RMax) {
        RMin=static_cast<real>(norm_rand());
        RMax=static_cast<real>(norm_rand());
    }
    BOOST_CHECK(rMarker.set_supp(RMin, RMax));
    auto [iA, iB]=rMarker.get_supp();
    BOOST_CHECK(iA<iB);
}

BOOST_AUTO_TEST_CASE(SetXmin_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_xmin(static_cast<real>(0.)));
}

BOOST_AUTO_TEST_CASE(SetXmin_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_xmin(static_cast<real>(0.)));
}

BOOST_AUTO_TEST_CASE(SetXmin_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_xmin(static_cast<real>(0. )));
}

BOOST_AUTO_TEST_CASE(SetXmax_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_xmax(static_cast<real>(norm_rand()*100.+1.)));
}

BOOST_AUTO_TEST_CASE(SetXmax_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_xmax(static_cast<real>(norm_rand()*100.+1.)));
}

BOOST_AUTO_TEST_CASE(SetXmax_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_xmax(static_cast<real>(norm_rand()*100.+1.)));
}

BOOST_AUTO_TEST_CASE(SetYmin_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_ymax(static_cast<real>(0)));
}

BOOST_AUTO_TEST_CASE(SetYmin_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_ymax(static_cast<real>(0)));
}

BOOST_AUTO_TEST_CASE(SetYmin_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_ymax(static_cast<real>(0)));
}

BOOST_AUTO_TEST_CASE(SetYmax_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_ymax(static_cast<real>(norm_rand()+.1)));
}

BOOST_AUTO_TEST_CASE(SetYmax_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_ymax(static_cast<real>(norm_rand()+.1)));
}

BOOST_AUTO_TEST_CASE(SetYmax_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_ymax(static_cast<real>(norm_rand()+.1)));
}

BOOST_AUTO_TEST_CASE(SetFigsize_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_figsize(rand()%1024+1, rand()%1024+1));
}

BOOST_AUTO_TEST_CASE(SetFigsize_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_figsize(rand()%1024+1, rand()%1024+1));
}

BOOST_AUTO_TEST_CASE(SetFigsize_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_figsize(rand()%1024+1, rand()%1024+1));
}

BOOST_AUTO_TEST_CASE(SetLineWidth_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_linewidth(norm_rand()+0.01));
}

BOOST_AUTO_TEST_CASE(SetLineWidth_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_linewidth(norm_rand()+0.01));
}

BOOST_AUTO_TEST_CASE(SetLineWidth_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_linewidth(norm_rand()+0.01));
}

BOOST_AUTO_TEST_CASE(SetTitleSize) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_titlesize(rand()%10+1));
}

BOOST_AUTO_TEST_CASE(SetLabelSize) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_labelsize(rand()%10+1));
}

BOOST_AUTO_TEST_CASE(SetTickLabelSize) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_ticklabelsize(rand()%10+1));
}

BOOST_AUTO_TEST_CASE(SetAnnotateSize) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_annotatesize(rand()%10+1));
}

BOOST_AUTO_TEST_CASE(SetLegendSize) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_legendsize(rand()%10+1));
}

BOOST_AUTO_TEST_CASE(SetContinuumSize_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_continuum(norm_rand()+1.));
}

BOOST_AUTO_TEST_CASE(SetContinuumSize_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_continuum(norm_rand()+1.));
}

BOOST_AUTO_TEST_CASE(SetContinuumSize_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.set_continuum(norm_rand()+1.));
}


// ----------------------------------
// Misc. Tests

BOOST_AUTO_TEST_CASE(AddLine_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.add_line(static_cast<real>(rand()%(8000-4000+1)+4000),
                                gen_rand_string()));
}

BOOST_AUTO_TEST_CASE(AddLine_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.add_line(static_cast<real>(rand()%(8000-4000+1)+4000),
                                gen_rand_string()));
}

BOOST_AUTO_TEST_CASE(AddLine_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    BOOST_CHECK(rMarker.add_line(static_cast<real>(rand()%(8000-4000+1)+4000),
                                gen_rand_string()));
}

BOOST_AUTO_TEST_CASE(AddData_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    BOOST_CHECK(rMarker.add_data(vrX, vrY));      
}

BOOST_AUTO_TEST_CASE(AddData_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    BOOST_CHECK(rMarker.add_data(vrX, vrY));      
}

BOOST_AUTO_TEST_CASE(AddData_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    BOOST_CHECK(rMarker.add_data(vrX, vrY));      
}

BOOST_AUTO_TEST_CASE(AddDataLabel_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    BOOST_CHECK(rMarker.add_data(vrX, vrY, gen_rand_string()));      
}

BOOST_AUTO_TEST_CASE(AddDataLabel_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    BOOST_CHECK(rMarker.add_data(vrX, vrY, gen_rand_string()));      
}

BOOST_AUTO_TEST_CASE(AddDataLabel_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    BOOST_CHECK(rMarker.add_data(vrX, vrY, gen_rand_string()));      
}

BOOST_AUTO_TEST_CASE(Make_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    rMarker.set_data(vrX, vrY);
    rMarker.add_data(vrX, vrY, gen_rand_string());
    std::string sOutput(gen_rand_string()+".png");
    rMarker.set_output(sOutput);    
    BOOST_CHECK(rMarker.make());
    remove(sOutput.c_str());
    remove(".plot.py");
    remove(".data.csv");
    remove(".data_0.csv");
}

BOOST_AUTO_TEST_CASE(Make_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    rMarker.set_data(vrX, vrY);
    rMarker.add_data(vrX, vrY, gen_rand_string());
    std::string sOutput(gen_rand_string()+".png");
    rMarker.set_output(sOutput);    
    BOOST_CHECK(rMarker.make());
    remove(sOutput.c_str());
    remove(".plot.py");
    remove(".data.csv");
    remove(".data_0.csv");
}

BOOST_AUTO_TEST_CASE(Make_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    rMarker.set_data(vrX, vrY);
    rMarker.add_data(vrX, vrY, gen_rand_string());
    std::string sOutput(gen_rand_string()+".png");
    rMarker.set_output(sOutput);    
    BOOST_CHECK(rMarker.make());
    remove(sOutput.c_str());
    remove(".plot.py");
    remove(".data.csv");
    remove(".data_0.csv");
}

BOOST_AUTO_TEST_CASE(Plot_float) {
    typedef float real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    rMarker.set_data(vrX, vrY);
    rMarker.add_data(vrX, vrY, gen_rand_string());
    std::string sOutput(gen_rand_string()+".png");
    rMarker.set_output(sOutput);    
    rMarker.make();
    BOOST_CHECK(rMarker.plot()==0);
    remove(sOutput.c_str());
    remove(".plot.py");
    remove(".data.csv");
    remove(".data_0.csv");
}

BOOST_AUTO_TEST_CASE(Plot_double) {
    typedef double real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    rMarker.set_data(vrX, vrY);
    rMarker.add_data(vrX, vrY, gen_rand_string());
    std::string sOutput(gen_rand_string()+".png");
    rMarker.set_output(sOutput);    
    rMarker.make();
    BOOST_CHECK(rMarker.plot()==0);
    remove(sOutput.c_str());
    remove(".plot.py");
    remove(".data.csv");
    remove(".data_0.csv");
}

BOOST_AUTO_TEST_CASE(Plot_double_long) {
    typedef double long real;
    _marker<real> rMarker;
    rMarker.set_verbose(false);
    auto [vrX, vrY]=gen_rand_spec<real>();
    rMarker.set_data(vrX, vrY);
    rMarker.add_data(vrX, vrY, gen_rand_string());
    std::string sOutput(gen_rand_string()+".png");
    rMarker.set_output(sOutput);    
    rMarker.make();
    BOOST_CHECK(rMarker.plot()==0);
    remove(sOutput.c_str());
    remove(".plot.py");
    remove(".data.csv");
    remove(".data_0.csv");
}

// ----------------------------------
// Misc. functions

float norm_rand() {
    return static_cast<float>(rand())/static_cast<float>(RAND_MAX);
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

// ----------------------------------
