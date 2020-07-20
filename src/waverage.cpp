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

    hello();
    
    _io<> io;

    io.read_fits_dir("./data/GIRAFFE_fits",".fits");
    io.set_WaveScale(10.);
    
    std::vector< std::valarray< std::valarray< double > > > Spectra(io.get_valarrays());
    
    
    _op<> op(Spectra);
 
    op.remove_zero();
    op.resize_spectr();
    op.rebuild_wlStep();

    op.filter_SG();

    
    op.write(io);
    
    return EXIT_SUCCESS;
}

