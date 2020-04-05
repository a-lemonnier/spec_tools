#ifndef _MARKER_H
#define _MARKER_H

#include <iostream>
#include <cstdlib>

#include <fstream>

#include <string>
#include <sstream>
#include <iomanip>

#include <vector>
#include <algorithm> 

#include "msg.h"

/**
 * \class _marker
 * \brief A class to plot spectra with line markers using py matplotlib
 */
template<typename _T=float>
class _marker {
public:
    /**
     * \struct Line
     * \brief Define a line
     * \var TWl The wavelength
     * \var sElem The name of the element 
     */
    typedef struct {
        _T TWl;
        std::string sElem;
    } Line;
    
    typedef std::vector<Line> vlList;
    
    explicit _marker();
    explicit _marker(const _marker<_T>& other);

    virtual ~_marker();

    void set_verbose(const bool bVerbose);
    
    void set_data(const std::vector<_T>& vTX, const std::vector<_T>& vTY);
    
    void set_title(const std::string& sTitle);
    void set_xlabel(const std::string& sXlabel);
    void set_ylabel(const std::string& sYlabel);
    void set_xunit(const std::string& sXunit);
    void set_yunit(const std::string& sYunit);
    
    
    void set_output(const std::string& sFilename);
    /**
     * \fn void set_output(const std::string& sFilename, const int iDpi)
     * \brief Set the picture filename with the extension (png, pdf, jpeg...) and the density (iDpi>50)
     * \param sFilename Picture name
     * \param iDpi Density
     */
    void set_output(const std::string& sFilename, const int iDpi);
    
    /**
     * \fn void set_continuum(const _T TContinuum)
     * \brief Set the continuum position and therefore ymax
     */
    void set_continuum(const _T TContinuum);
    
    /**
     * \fn void set_supp(const _T TXmin, const _T TXmax)
     * \brief Set the support of the first spectrum
     * \param TXmin \f$x_{min}\f$
     * \param TXmax \f$x_{max}\f$
     */
    void set_supp(const _T TXmin, const _T TXmax);
    void set_xmin(const _T TXmin);
    void set_xmax(const _T TXmax);
    
    void set_ymin(const _T TYmin);
    void set_ymax(const _T TYmax);

    void set_figsize(int iHeight, int iWidth);
    
    /**
     * \fn void set_scriptname(const std::string &sScriptname)
     * \brief Set the name of the py script. Default: .plot.py
     */
    void set_scriptname(const std::string &sScriptname);
    
    /**
     * \fn void add_line(_T TWl, const std::string &sName)
     * \brief Add a marker with a name on the figure
     */
    void add_line(_T TWl, const std::string &sName);
    
    /**
     * \fn add_data(const std::vector<_T>& vTX, const std::vector<_T>& vTY)
     * \brief Add an additionnal spectrum which has to be plot
     */
    void add_data(const std::vector<_T>& vTX, const std::vector<_T>& vTY);
    
    _T get_continuum() const;
    /**
     * \fn _T* get_supp() const
     * \brief Get the support of the first spectrum
     * \return Array of 2 _T: [\f$x_{min}\,x_{max}\f$]
     */
    _T* get_supp() const;
    
    const std::string& get_scriptname() const;
    const std::string& get_output() const;
    
    const std::string& get_title() const;
    const std::string& get_xlabel() const;
    const std::string& get_xunit() const;
    const std::string& get_ylabel() const;
    const std::string& get_yunit() const;
    
    /**
     * \fn int* get_figsize() const
     * \brief Get the defined figsize, if defined
     * \return Array of 2 int
     */
    int* get_figsize() const;
    
    int get_dpi() const;

    /**
     * \fn bool make()
     * \brief Write spectra, write script with markers
     */
    bool make();
    
    /**
     * \fn void plot()
     * \brief Run the py script
     */
    void plot();
    
protected:
    _msg msgM; /**< Interface to print message to std output */

private:
    std::string get_ext(const std::string& sS) const;
    std::string get_cmd() const;
    void add_cmd(const std::string &sCmd);
    
    bool bVerbose;
    
    std::vector<_T> X, Y;
    std::vector<std::vector<std::vector<_T> > > vvvAdddata; /**< Additionnal spectrum */
    
    vlList vllSet; /**< Set of lines with name of lines */
    
    std::string sExec;
    
    std::string sFilename;
    std::string sExt;
    std::string sScriptname;
    
    std::string sTitle;
    
    std::string sXlabel, sYlabel;
    std::string sXunit, sYunit;
    
    _T TYcontinuum;
    _T TXmin, TXmax;
    _T TYmin, TYmax;

    int iDpi;
    
    int iHeight, iWidth;
    
    bool bIsset_fig_size; /**< Indicate whether the figsize is defined or not */
    
};


#include "marker.tpp"

#endif // _MARKER_H
