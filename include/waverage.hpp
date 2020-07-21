#ifndef _WAVERAGE_H_
#define _WAVERAGE_H_

#include <fstream>
#include <memory>

#include <cmath>
#include <numeric>

#include <string>
#include <sstream>
#include <iomanip>

#include <algorithm> 
#include <vector>
#include <valarray>
#include <tuple>

#include <thread>
#include <future>
#include <chrono>

#include <boost/algorithm/string/split.hpp>       
#include <boost/algorithm/string.hpp>      
#include <boost/any.hpp>

#include <CCfits/CCfits>
#include <CCfits/CCfits.h>
#include <CCfits/PHDU.h>

#include <Eigen/Dense>

#if __has_include (<boost/timer/timer.hpp>)
#include <boost/timer/timer.hpp> 
#define HAS_BOOST_TIMER /**< boost::timer availability */
#endif

#if __has_include (<sys/syscall.h>)
#include <sys/syscall.h>
#define HAS_SYSCALL /**< linux syscall availability */
#endif

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

#if __has_include(<libnotify/notify.h>)
#include <libnotify/notify.h> 
#define LNOTIFY  /**< check libnotify library availability. */
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
    
    typedef std::vector<std::valarray<std::valarray<_T> > > Vvv;
    typedef std::valarray<std::valarray<_T> > vv;
    
    /**
     * \struct vec
     * \brief Define a (x,y) vector (seems better than std::pair)
     */
    typedef struct {
        _T x;
        _T y;
        _T SNR=-1;
    } vec;
    
    explicit _io();
    explicit _io(Vvv& VvvSpectr);

    virtual ~_io();
    
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
    
    void set_data(Vvv& VvvSpectr);
    
    void set_WaveScale(_T Scale); /**< multiply wavelength by Scale */
    
    int get_FileIndex(std::string sName) const; /**< return the position of sName in vsFileList */
    
    const std::vector<vec> get_vector() const; /**< get spectrum  */
    const std::vector<std::vector<vec> > get_vectors() const; /**< get spectra */
    const vv get_valarray(); /**< convert vector to an valarray */
    const Vvv get_valarrays();
    
    static void notify(std::string sTitle, std::string sMsg, int iTime); /**< send desktop notification using libnotify.*/
    
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
    typedef std::vector<std::valarray<std::valarray<_T> > > Vvv;
    typedef std::valarray<std::valarray<_T> > vv;
    typedef std::vector<std::vector<_T> > VV;
    
    explicit _op();
    explicit _op(Vvv &VvvSpectr);
    
    virtual ~_op();

    bool resize_spectr(); /**< resize all spectra to the same size (maximal). */
    bool rebuild_wlStep(); /**< rebuild wavelength axis. */

    void remove_zero(); /**< trim spectra where flux is 0 (assuming zeros are at the beginning or the end). */
    
    inline bool filter_SG(int n); /**< Savitzky-Golay on spectrum n.*/
    inline void filter_SG_th(int n); /**< Savitzky-Golay on a given spectrum (for threads). */
    bool filter_SG(); /**< Savitzky-Golay on all spectra n.*/
    
    void test();
    
    const vv compute_mean() const; /**< compute arithmetic mean*/
    void compute_wmean(); /**< compute weighted arithmetic mean */

    std::pair<_T, _T> get_wlRange(int n) const; /**< get the minimum and maximum wavelength */
    std::pair<_T, _T> get_wlRangeMin() const; /**< return the smallest support */
    
    void set_data(Vvv &VvvSpectr); /**< set external spectra. **/
    
    bool write(_io<_T>& ioInterface); /**< write the data using a _io class. */
    bool write_mean(std::string sFilename) const; /**< write only the w mean. */
        
private:
    Vvv VvvSpectr;
    vv vvWMean;
    
    _T Step;
    
    bool bHaveSNR;
    
    inline std::vector<_T> SG_conv(std::valarray<_T> &X,int i1, int i2, int PolyDeg) const; /**< Savitzky-Golay coefficients. */
    inline _T mean_step(const std::valarray<_T> &vArray) const; /**< mean step for a given support. */
    _T der_snr(const std::valarray<_T> &vFlux) const;
    _T median(const std::valarray<_T> &vFlux) const;
    
    /**
    * \fn double long CPU_utilization()
    * \brief Get the CPU usage (%)
    */
    double long CPU_utilization();
    std::tuple<double long, double long> get_stat();
};












#include <waverage.tpp>

#endif // waverage.h
