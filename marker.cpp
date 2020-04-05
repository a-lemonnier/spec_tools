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
    
    std::vector<float> X,Y;
    
    _csv<float> csv;
    
    csv.set_verbose(_csv<float>::DEBUG);
    
    csv.set_filename("HD87240_p1.sub.obs.norm");
    csv.set_separator('\t');
    
    if (csv.read()) {
    
        X=csv.select_column(0);
        Y=csv.select_column(1);
        
        _marker Marker;
        
        Marker.set_verbose(true);
        
        Marker.set_output("plot.png", 300);
        
        Marker.set_data(X,Y);
        
        Marker.add_line(4861,"$H\\\\beta$");
        
        if (Marker.make()) {
            Marker.plot();
        }
    }
    else
        return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}
