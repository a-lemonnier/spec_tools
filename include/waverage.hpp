#ifndef _WAVERAGE_H_
#define _WAVERAGE_H_

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

#define PRECISION 10


void hello();

/**
 * \class _io
 * \brief The purpose of this class is only to provide methods to read ascii or FITS spectrum, and to convert vector to valarray.
 */
template<typename _T=double> // double ?
class _io {
public:
    
    explicit _io();
    virtual ~_io();

    /**
     * \struct vec
     * \brief Define a (x,y) vector (seems better than std::pair)
     */
    typedef struct {
        _T x;
        _T y;
        _T SNR=-1;
    } vec;
    
    typedef std::vector<std::valarray<std::valarray<_T> > > Vvv;
    typedef std::valarray<std::valarray<_T> > vv;
    
    bool read(std::string sFilename); /**<  read one file */
    bool read_dir(std::string sExtension); /**< read the whole directory, only need the extension of files */
    
    bool read_fits(std::string sFilename); /**< convert fits to vectors */
    bool read_fits_dir(std::string sExtension); 
    bool read_fits_dir(std::string sDirectory,std::string sExtension); 
    
    void show_data() const; /**< show a spectrum */ 
    void show_data(int n) const; /**< show spectrum n */ 
    void show_data(std::string sName) const; /**< show spectrum sName */
    
    bool write() const; /**< write the results */
    bool write(std::string sFilename) const; /**< write the results */
    
    void set_fileIn(std::string sFilename); /**< set the input file name */
    void set_fileOut(std::string sFilename); /**< set the output name */
    
    void set_WaveScale(_T Scale); /**< multiply wavelength by Scale */
    
    int get_FileIndex(std::string sName) const; /**< return the position of sName in vsFileList */
    
    const std::vector<vec> get_vector() const; /**< get spectrum  */
    const std::vector<std::vector<vec> > get_vectors() const; /**< get spectra */
    const vv get_valarray(); /**< convert vector to an valarray */
    const Vvv get_valarrays();
    
private:
    std::string sFnameIn, sFnameOut;
    std::vector<std::string> vsFileList;
    std::string sFilesExt;
    
    _T Scale;
    
    std::vector<vec> vvSpec; /**< a spectrum */
    std::vector<std::vector<vec> > VvvSpec; //  /**<vector of spectra */
        
    bool exists_SNR;

    int Precision;
};


/**
 * \class _op
 * \brief This class is a set of spectrum manipulation methods. It is work with std::valarray.
 * \TODO everything.
 */
template<typename _T=double> 
class _op {
public:
    
    explicit _op();
    virtual ~_op();
    
    typedef std::vector<std::valarray<std::valarray<_T> > > Vvv;
    typedef std::valarray<std::valarray<_T> > vv;
    
    const Vvv& resize_spectr(Vvv VvvSpectr); /**< resize all spectra to the same size (maximal). */
    bool rebuild_wlStep(Vvv VvvSpectr, _T Step); /**< rebuild wavelength axis. */
    
    const vv& compute_mean(Vvv VvvSpectr) const; /**< compute arithmetic mean*/
    const vv& compute_wmean(Vvv VvvSpectr) const; /**< compute weighted arithmetic mean */
    
};












#include <waverage.tpp>

#endif // waverage.h
