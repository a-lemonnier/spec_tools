#ifndef _WAVERAGE_H_
#define _WAVERAGE_H_

#include <filesystem>
#include <fstream>
#include <memory>

#include <cmath>

#include <string>
#include <sstream>
#include <iomanip>

#include <algorithm> 
#include <vector>
#include <valarray>
#include <tuple>

#include <boost/algorithm/string/split.hpp>       
#include <boost/algorithm/string.hpp>      
#include <boost/any.hpp>

#include <CCfits/CCfits>
#include <CCfits/CCfits.h>
#include <CCfits/PHDU.h>



// the purpose of this class is only to provide methods to read or write file, and to convert vector to valarray
template<typename _T=double>
class _io {
public:
    
    // why do i need those atm?
    explicit _io();
    virtual ~_io();
    
    // seems better than std::pair
    typedef struct _vec {
        _T x;
        _T y;
    } vec;
    
    bool read(std::string sFilename); // read one file
    bool read_dir(std::string sExtension); // read the whole directory, only need the extension of files
    
    bool read_fits(std::string sFilename); // convert fits to vectors
    bool read_fits_dir(std::string sExtension); 
    
    bool write(std::string sFilename); // write the results
    
    void set_fileIn(std::string sFilename); // set the input file name
    void set_fileOut(std::string sFilename); // set the output name
    
    const std::vector<vec> get_vector() const; // get spectrum 
    const std::vector<std::vector<vec> > get_vectors() const; // get spectra
    const std::valarray<std::valarray<_T> > get_valarray(); // convert vector to an valarray
    const std::vector<std::valarray<std::valarray<_T> > > get_valarrays();
    
private:
    std::string sFnameIn, sFnameOut;
    std::vector<std::string> vsFileList;
    std::string sFilesExt;
    
    std::vector<vec> vvSpec; // a spectrum
    std::vector<std::vector<vec> > vvvSpec; // vector of spectra
    
};














#include <waverage.tpp>

#endif // waverage.h
