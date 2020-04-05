/**
 * \file marker.cpp
 * \brief Highlight lines on spectrum 
 * \author Audric Lemonnier
 * \version 0.1
 * \date 03/04/2020
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include <boost/program_options.hpp>

#if __has_include (<boost/timer/timer.hpp>)
#include <boost/timer/timer.hpp>
#define HAS_BOOST_TIMER
#endif

#if __has_include (<filesystem>)
#include <filesystem>
#define FS_STD
namespace fs = std::filesystem;
#elif __has_include (<experimental/filesystem>) && !__has_include (<filesystem>)
#include <experimental/filesystem>
#define FS_STDEXP
namespace fs = std::experimental::filesystem;
#elif __has_include(<boost/filesystem.hpp>) && !__has_include (<filesystem>) && !__has_include (<experimental/filesystem>)
#include <boost/filesystem.hpp>
#define FS_BOOST
namespace fs = boost::filesystem;
#else
#error "No filesystem header found"
#endif

#include "marker.h"
#include "msg.h"
#include "csv.h"

int main(int argc, char** argv) {
       
    _csv<float> csv, csv2;
    
    csv.set_verbose(_csv<float>::DEBUG);
    csv2.set_verbose(_csv<float>::DEBUG);
    
    csv.set_filename("rand_spectra/0/0.dat");
    csv2.set_filename("rand_spectra/0/1.dat");
    
    csv.set_separator('\t');
    csv2.set_separator('\t');
    
    if (csv.read() && csv2.read()) {
    
        _marker Marker;
        
        Marker.set_verbose(true);
        
        Marker.set_output("plot.pdf", 300);
        
        Marker.set_data(csv.select_column(0), csv.select_column(1));
        Marker.add_data(csv2.select_column(0), csv2.select_column(1));
        
        Marker.set_xlabel("$\\\\lambda$");
        Marker.set_ylabel("Normalized flux");
        
        Marker.set_xunit("$\\\\AA$");
        Marker.set_yunit("cgs");

        Marker.add_line(4750,"Elem 1");
        Marker.add_line(4800,"Elem 2");
        Marker.add_line(4825,"Elem 3");
        Marker.add_line(4861,"$H\\\\beta$");
        
        if (Marker.make()) 
            Marker.plot();

    }
    else
        return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}
