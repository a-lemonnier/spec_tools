#ifndef _MARKER_H
#define _MARKER_H

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm> 

#include "msg.h"

template<typename _T=float>
class _marker {
public:
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
    
    void set_output(const std::string& sFilename);
    void set_output(const std::string& sFilename, const int iDpi);
    
    void set_continuum(const _T TContinuum);
    
    void set_supp(const _T TXmin, const _T TXmax);
    void set_xmin(const _T TXmin);
    void set_xmax(const _T TXmax);
    
    void set_ymin(const _T TYmin);
    void set_ymax(const _T TYmax);

    void set_figsize(int iHeight, int iWidth);
    
    void set_scriptname(const std::string &sScriptname);
    
    const std::string& get_scriptname() const;
    const std::string& get_output() const;
    
    int* get_figsize() const;
    int get_dpi() const;
    
    
    void make();
    void plot();
    void save();
    
protected:
    _msg msgM;

private:
    std::string get_ext(const std::string& sS) const;
    void add_cmd(const std::string &sCmd);
    
    bool bVerbose;
    
    std::vector<_T> X, Y;
    
    vlList vllSet;
    
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
    
    bool bIsset_fig_size;
    
    
    
};


#include "marker.tpp"

#endif // _MARKER_H
