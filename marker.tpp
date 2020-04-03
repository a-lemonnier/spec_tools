template<typename _T>
_marker<_T>::_marker():
    bVerbose(true),
    X(0), Y(0),
    vllSet(vlList(0)),
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
    
    if (vTX.size()!=vTY.size())
        msgM.msg(_msg::eMsg::ERROR,"dimension mismatches");
    
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
void _marker<_T>::make() const {

    
}

template<typename _T>
void _marker<_T>::plot() const {

    
}

template<typename _T>
void _marker<_T>::save() const {

    
}

template<typename _T>
std::string _marker<_T>::get_ext(const std::string& sS) const {
    return sS.substr(sS.find_last_of(".")+1);
}
 
