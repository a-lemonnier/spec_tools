#ifndef _MARKER_H
#define _MARKER_H

#include <iostream>
#include <cstdlib>

#include <fstream>

#include <string>
#include <sstream>
#include <iomanip>

#include <iterator>
#include <vector>
#include <utility>
#include <algorithm> 

#include <boost/program_options.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_numeric.hpp>

#include <csv.h>
#include <msg.h>


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
    
//     exlicit _marker(const _marker<_T>& other);

    virtual ~_marker();

    void set_verbose(const bool bVerbose);
    
    bool set_data(const std::vector<_T>& vTX, const std::vector<_T>& vTY);
    
    bool set_title(const std::string& sTitle);
    bool set_label(const std::string& sLabel);
    bool set_xlabel(const std::string& sXlabel);
    bool set_ylabel(const std::string& sYlabel);
    bool set_xunit(const std::string& sXunit);
    bool set_yunit(const std::string& sYunit);
    
    bool set_output(const std::string& sFilename);
    
    /**
     * \fn bool set_output(const std::string& sFilename, const int iDpi)
     * \brief Set the picture filename with the extension (png, pdf, jpeg...) and the density (iDpi>50)
     * \param sFilename Picture name
     * \param iDpi Density
     */
    bool set_output(const std::string& sFilename, const int iDpi);
    
    /**
     * \fn bool set_continuum(const _T TContinuum)
     * \brief Set the continuum position and therefore ymax
     */
    bool set_continuum(const _T TContinuum);
    
    /**
     * \fn bool set_supp(const _T TXmin, const _T TXmax)
     * \brief Set the support of the first spectrum
     * \param TXmin \f$x_{min}\f$
     * \param TXmax \f$x_{max}\f$
     */
    bool set_supp(const _T TXmin, const _T TXmax);
    bool set_xmin(const _T TXmin);
    bool set_xmax(const _T TXmax);
    
    bool set_ymin(const _T TYmin);
    bool set_ymax(const _T TYmax);

    void set_figsize(int iHeight, int iWidth);
    
    /**
     * \fn void set_colorline(const std::string &sColor)
     * \brief Set the color of the first curve.
     * \param sColor A string like "red", "green", "blue" or and a rgba hex string like "#rrggbbaa"
     */
    void set_colorline(const std::string &sColor);
    void set_linewidth(float fWidth);
    void set_titlesize(int iSize);
    void set_labelsize(int iSize);
    void set_ticklabelsize(int iSize);
    void set_annotatesize(int iSize);
    void set_legendsize(int iSize);
    void set_continnumsize(float fWidth);
    void set_showgrid(bool bShowgrid);
    
    /**
     * \fn void set_scriptname(const std::string &sScriptname)
     * \brief Set the name of the py script. Default: .plot.py
     */
    void set_scriptname(const std::string &sScriptname);
    
    /**
     * \fn void set_log(const std::string& sLog)
     * \brief Enable or disable log file
     */
    void set_log(const std::string& sLog);
    
    /**
     * \fn void add_line(_T TWl, const std::string &sName)
     * \brief Add a marker with a name on the figure
     */
    void add_line(_T TWl, const std::string &sName);
    
    /**
     * \fn add_data(const std::vector<_T>& vTX, const std::vector<_T>& vTY)
     * \brief Add an additionnal spectrum which has to be plot
     */
    void add_data(const std::vector<_T>& vTX, 
                  const std::vector<_T>& vTY);
    
    /**
     * \fn add_data(const std::vector<_T>& vTX, const std::vector<_T>& vTY, const std::string &sTitle)
     * \brief Add an additionnal spectrum which has to be plot
     */
    void add_data(const std::vector<_T>& vTX, 
                  const std::vector<_T>& vTY, 
                  const std::string &sLabel);
    
    _T get_continuum() const;
    /**
     * \fn const std::pair<_T,_T> get_supp()
     * \brief Get the support of the first spectrum
     * \return std::pair of 2 _T: [\f$x_{min}\,x_{max}\f$]
     */
    const std::pair<_T,_T> get_supp();
    
    const std::string& get_scriptname();
    const std::string& get_output();
    
    const std::string& get_title() const;
    const std::string& get_label() const;
    const std::string& get_xlabel() const;
    const std::string& get_xunit() const;
    const std::string& get_ylabel() const;
    const std::string& get_yunit() const;
    
    /**
     * \fn const std::pair<int, int> get_figsize() const
     * \brief Get the defined figsize, if defined. First: Height and Second: Width.
     * \return std::pair of 2 int
     */
    const std::pair<int, int> get_figsize() const;
    
    int get_dpi() const;

    /**
     * \fn bool make()
     * \brief Write spectra, write script with markers
     */
    bool make();
    
    /**
     * \fn int plot()
     * \brief Run the py script
     */
    int plot();
    
protected:
    _msg msgM; /**< Interface to print message to std output */

private:
    std::string get_ext(const std::string& sS) const;
    std::string get_cmd() const;
    void add_cmd(const std::string &sCmd);
    
    /**
     * \fn bool is_float(const std::string &val) const
     * \brief Check if a string is a float and does not contain non digit characters. It uses boost::qi.
     * \param TVal The string  
     * \return true if float
     */
    bool is_float(const std::string &sVal) const;
    
    bool bVerbose;
    
    std::vector<_T> X, Y;
    std::vector<std::vector<std::vector<_T> > > vvvAdddata; /**< Additionnal spectrum */
    
    std::vector<std::string> vsTitle;
    
    vlList vllSet; /**< Set of lines with name of lines */
    
    std::string sExec;
    
    std::string sFilename;
    std::string sExt;
    std::string sScriptname;
    
    std::string sTitle;
    std::string sLabel;
    
    std::string sXlabel, sYlabel;
    std::string sXunit, sYunit;
    
    _T TYcontinuum;
    _T TXmin, TXmax;
    _T TYmin, TYmax;

    int iDpi;
    
    int iHeight, iWidth;
    
    std::string sColorline;
    float fLinewidth;
    int iTitlesize;
    int iLabelsize;
    int iTicklabelsize;
    int iAnnotatesize;
    int iLegendsize;
    float fContinnumsize;
    bool bShowgrid;
    
    bool bIsset_fig_size; /**< Indicate whether the figsize is defined or not */
    
    std::string sLog;
    bool bLog;
    
};


#include "marker.tpp"

#endif // _MARKER_H
