template<typename _T>
_marker<_T>::_marker():
    bVerbose(true),
    X(0), Y(0),
    vvvAdddata(0),
    vllSet(vlList(0)),
    sExec("#!/usr/bin/env python3\n# -*- coding: utf-8 -*-\n#auto generated script\n\n"),
    sScriptname(".plot.py"),
    sTitle("Plot"),
    sXlabel("x"), sYlabel("y"),
    sXunit(" "), sYunit(" "),
    TYcontinuum(1),
    TYmin(0), TYmax(1),
    iDpi(150),
    bIsset_fig_size(false)
{
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"init marker");
    msgM.set_name("marker");    
}

template<typename _T>
_marker<_T>::_marker(const _marker<_T>& other) {
    
}

template<typename _T>
_marker<_T>::~_marker() {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"destroy");
    
    this->X.clear();
    this->Y.clear();
    this->vllSet.clear();
    this->vvvAdddata.clear();
}


template<typename _T>
void _marker<_T>::set_verbose(const bool bVerbose) {
    this->bVerbose=bVerbose;
    msgM.msg(_msg::eMsg::MID,"set verbosity");
}

template<typename _T>
void _marker<_T>::set_data(const std::vector<_T>& vTX, const std::vector<_T>& vTY) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set data");
    if (vTX.empty() || vTY.empty()) 
        msgM.msg(_msg::eMsg::ERROR,"empty vector");
    else if (vTX.size()!=vTY.size())
        msgM.msg(_msg::eMsg::ERROR,"dimension mismatches");
    else {
        this->X=vTX;
        this->Y=vTY;
    }
}

template<typename _T>
void _marker<_T>::set_title(const std::string& sTitle) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set title:", sTitle);
    if (sTitle.empty()) {
        msgM.msg(_msg::eMsg::MID,"reset title");
        this->sTitle="Plot";
    }
}

template<typename _T>
void _marker<_T>::set_xlabel(const std::string& sXlabel) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set xlabel:", sXlabel);
    if (sXlabel.empty()) {
        msgM.msg(_msg::eMsg::MID,"reset xlabel");
        this->sXlabel=" ";
    }
}

template<typename _T>
void _marker<_T>::set_ylabel(const std::string& sYlabel) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set ylabel:", sYlabel);
    if (sYlabel.empty()) {
        msgM.msg(_msg::eMsg::MID,"reset ylabel");
        this->sYlabel=" ";
    }
}


template<typename _T>
void _marker<_T>::set_output(const std::string& sFilename) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set output:", sFilename);
    
    if (sFilename.empty()) {
        this->sFilename="output.png";
        msgM.msg(_msg::eMsg::MID,"reset output to:", this->sFilename);
    }
    else 
        this->sFilename=sFilename;
    
    this->sExt=this->get_ext(sFilename);
    
    if ((sExt!="png" &&
        sExt!="jpg" &&
        sExt!="jpeg" &&
        sExt!="pdf") ||
        sExt.empty()) {
        this->sExt="png";
        msgM.msg(_msg::eMsg::ERROR,"reset extension to:", this->sExt);
        }
        
        msgM.msg(_msg::eMsg::MID,"set extension to:", this->sExt);
}

template<typename _T>
void _marker<_T>::set_output(const std::string& sFilename, const int iDpi) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set output:", sFilename, "and dpi:", iDpi);
    if (iDpi<50) {
        msgM.msg(_msg::eMsg::MID,"reset dpi to:", 50);
        this->iDpi=50;
    }
    else
        this->iDpi=iDpi;
    
    set_output(sFilename);    
}



template<typename _T>
void _marker<_T>::set_continuum(const _T TContinuum) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set continnum:", TContinuum);
    this->TYcontinuum=TContinuum;
}


template<typename _T>
void _marker<_T>::set_supp(const _T TXmin, const _T TXmax) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID, "set support: (", TXmin,",", TXmax,")");
    this->TXmin=TXmin;
    this->TXmax=TXmax;
}

template<typename _T>
void _marker<_T>::set_xmin(const _T TXmin) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set xmin:", TXmin);
    this->TYmin=TYmin;
}

template<typename _T>
void _marker<_T>::set_xmax(const _T TXmax) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set xmax:", TXmax);
    this->TXmax=TXmax;
}


template<typename _T>
void _marker<_T>::set_ymin(const _T TYmin) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set ymin:", TYmin);
    this->TYmin=TYmin;
}

template<typename _T>
void _marker<_T>::set_ymax(const _T TYmax) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set ymax:", TYmax);
    this->TYmax=TYmax;
}


template<typename _T>
void _marker<_T>::set_figsize(int iHeight, int iWidth) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set figsize:", "(", iHeight, ",", iWidth,")");
    if (iHeight>0 && iWidth>0) {
        this->iHeight=iHeight;
        this->iWidth=iWidth;
        bIsset_fig_size=true;
    }
    else
        msgM.msg(_msg::eMsg::ERROR,"invalid size");
}


template<typename _T>
void _marker<_T>::set_scriptname(const std::string &sScriptname) {
    if (sScriptname.empty()) 
        msgM.msg(_msg::eMsg::MID,"reset script name to:", this->sScriptname);
    else
        this->sScriptname=sScriptname;
}

template<typename _T>
void _marker<_T>::add_line(_T TWl, const std::string &sName) {
    if (TWl<0 || sName.empty())
        msgM.msg(_msg::eMsg::ERROR,"invalid line");
    else {
        if (bVerbose)
            msgM.msg(_msg::eMsg::MID,"add",sName,"at",TWl);
        vllSet.push_back({TWl, sName});
    }
    
}

template<typename _T>
_T _marker<_T>::get_continuum() const {
    return this->TYcontinuum;
}

template<typename _T>
const std::string& _marker<_T>::get_scriptname() const {
    if (this->sScriptname.empty()) 
        msgM.msg(_msg::eMsg::ERROR,"empty script name");
   
   return this->sScriptname;
}

template<typename _T>
const std::string& _marker<_T>::get_output() const {
        if (this->sFilename.empty()) 
            msgM.msg(_msg::eMsg::ERROR,"empty output name");
    
    return this->sFilename;
}

template<typename _T>
int* _marker<_T>::get_figsize() const {
    static int iSize[2];
    iSize[0]=this->iHeight;
    iSize[1]=this->iWidth;
    return iSize;
}

template<typename _T>
int _marker<_T>::get_dpi() const {
    if (this->iDpi<50)
        return 50;
    else
        return this->iDpi;
}

template<typename _T>
bool _marker<_T>::make() {
    
    if (this->X.empty() || this->Y.empty()) {
        msgM.msg(_msg::eMsg::ERROR,"make: empty data");
        return false;
    }
    
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"write data");
    
    std::fstream sfFlux_d(".data.csv", std::ios::out | std::ios::trunc);
    
     if (sfFlux_d) {
        if (bVerbose)
            msgM.msg(_msg::eMsg::MID,"write data");
        
        int iSize=this->X.size();
        for(int i=0; i<iSize; i++) 
            sfFlux_d << this->X[i] << " " << this->Y[i] << "\n";
        
        sfFlux_d.close();
     }
     else {
        msgM.msg(_msg::eMsg::ERROR,"cannot write data");
        return false;
     }
    
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"make python script");
        
    add_cmd("import matplotlib.pyplot as plt");
    add_cmd("import numpy as np");
    add_cmd("import csv\n");
    
    add_cmd("x=[]");
    add_cmd("y=[]\n");

    add_cmd("with open('.data.csv', 'r') as csvfile:");
    add_cmd("   plots= csv.reader(csvfile, delimiter=' ')");
    add_cmd("   for row in plots:");
    add_cmd("       x.append(float(row[0]))");
    add_cmd("       y.append(float(row[1]))\n");
    
    if (bIsset_fig_size)
        add_cmd("fig=plt.figure(figsize=("+
                std::to_string(get_figsize()[0])+","+
                std::to_string(get_figsize()[1])+"))\n");
    else
        add_cmd("fig=plt.figure()\n");
        
    add_cmd("grid = fig.add_gridspec(nrows=1, ncols=1)");
    add_cmd("ax0=fig.add_subplot(grid[0,0])\n");
    add_cmd("ax0.plot(x,y,'-b')\n");
    
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"add markers");
    
    for(auto line: vllSet)
        add_cmd("ax0.plot(["+
        std::to_string(line.TWl) + "," +
        std::to_string(line.TWl) + "], [0," +
        std::to_string(get_continuum()) +
        "],  color='r', linestyle=':', zorder=2, linewidth=1)");
    
    add_cmd(" ");
    
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"write savefig");
    
    add_cmd("fig.savefig('"+
            get_output()+"',dpi="+
            std::to_string(get_dpi())+")\n");
    
    std::fstream sfFlux(get_scriptname(), std::ios::out);
    
    if (sfFlux) {
        if (bVerbose)
            msgM.msg(_msg::eMsg::MID,"write script:",get_scriptname());
        sfFlux << get_cmd();
        sfFlux.close();
        return true;
    }
    else
        msgM.msg(_msg::eMsg::ERROR,"cannot open file:", get_scriptname());
    return false;
}

template<typename _T>
void _marker<_T>::plot(){
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"run python script");
    system(("chmod +x "+get_scriptname()+" > /dev/null 2>&1").c_str());
    system(("./"+get_scriptname()+" > /dev/null 2>&1").c_str());
}


template<typename _T>
std::string _marker<_T>::get_ext(const std::string& sS) const {
    return sS.substr(sS.find_last_of(".")+1);
}

template<typename _T>
std::string _marker<_T>::get_cmd() const {
    return this->sExec;
}

template<typename _T>
void _marker<_T>::add_cmd(const std::string &sCmd) {
    this->sExec+=sCmd+"\n";
}
