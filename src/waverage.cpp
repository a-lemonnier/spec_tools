/**
 * \file waverage.cpp
 * \brief Compute average of spectra from FITS weighted by the SNR or the exposition time.
 * \author Audric Lemonnier
 * \version 0.0
 * \date 16/07/2020
 */

#include <iostream>
#include <fstream>

#include <cmath>

#include <string>
#include <sstream>
#include <iomanip>

#include <algorithm> 
#include <vector>
#include <valarray>
#include <tuple>


#if __has_include (<filesystem>)
#include <filesystem>
#define FS_STD /**< std::filesystem availability (C++17) */
namespace fs = std::filesystem;
#elif __has_include (<experimental/filesystem>) && !__has_include (<filesystem>)
#include <experimental/filesystem>
#define FS_STDEXP /**< std::experimental::filesystem availability */
namespace fs = std::experimental::filesystem;
#elif __has_include(<boost/filesystem.hpp>) && !__has_include (<filesystem>) && !__has_include (<experimental/filesystem>)
#include <boost/filesystem.hpp>
#define FS_BOOST /**< boost::filesystem availability */
namespace fs = boost::filesystem;
#else
#error "No filesystem header found"
#endif

#include <waverage.hpp>


int main(int argc, char **argv) {

#ifdef HAS_BOOST_TIMER    
    boost::timer::cpu_timer btTimer;
#endif
        
    hello();
    
    _io<> io;

    _io<>::notify("reading fits...", "", 3);
    io.read_fits_dir("./data/GIRAFFE_fits",".fits");
    io.set_WaveScale(10.);
    
    
    std::vector< std::valarray< std::valarray< double long > > > Spectra(io.get_valarrays());    
    _op<> op(Spectra);
    
    _io<>::notify("removing zeros...", "", 3);
    op.remove_zero();
    
    _io<>::notify("resizing spectra...", "", 3);
    op.resize_spectr();
    
    _io<>::notify("rebuilding supports...", "", 3);
    op.rebuild_wlStep();

    _io<>::notify("filtering spectra...", "", 10);
    op.filter_SG();
    
    _io<>::notify("computing weighted mean...", "", 5);
    op.compute_wmean();
    
    _io<>::notify("writing data...", "", 3);
    op.write(io);
    op.write_mean("wmean.dat");
    
#ifdef HAS_BOOST_TIMER
    std::cout << "*" << btTimer.format()<< "\n";
#endif
    
    return EXIT_SUCCESS;
}

