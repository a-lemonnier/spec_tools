template<typename _T>
_marker<_T>::_marker():
    bVerbose(true),
    X(0), Y(0),
    vvvAdddata(0),
    vllSet(vlList(0)),
    sExec("\
#!/usr/bin/env python3\n\
# -*- coding: utf-8 -*-\n\
#auto generated script\n\n"),
    sScriptname(".plot.py"),
    sTitle("Plot"),
    sXlabel("x"), sYlabel("y"),
    TYcontinuum(1),
    TYmin(0), TYmax(1),
    iDpi(150),
    sColorline("black"),
    fLinewidth(0.3),
    iTitlesize(6),
    iLabelsize(6),
    iTicklabelsize(6),
    iAnnotatesize(6),
    iLegendsize(6),
    fContinnumsize(0.6),
    bIsset_fig_size(false)
{
    msgM.set_name("marker()");    
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
    if (bVerbose) msgM.msg(_msg::eMsg::MID,"set_verbose()");
}

template<typename _T>
void _marker<_T>::set_data(const std::vector<_T>& vTX, 
                           const std::vector<_T>& vTY) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_data()");
    if (vTX.empty() || vTY.empty()) 
        msgM.msg(_msg::eMsg::ERROR,"set_data(): empty vector");
    else if (vTX.size()!=vTY.size())
        msgM.msg(_msg::eMsg::ERROR,"set_data(): dimension mismatches");
    else {
        this->X=vTX;
        this->Y=vTY;
                
        set_xmin(*std::min_element(vTX.begin(), vTX.end()));
        set_xmax(*std::max_element(vTX.begin(), vTX.end()));
    }
}

template<typename _T>
void _marker<_T>::set_title(const std::string& sTitle) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_title(", sTitle,")");
    if (sTitle.empty()) {
        msgM.msg(_msg::eMsg::MID,"set_title(): reset title");
        this->sTitle="Plot";
    }
    else
        this->sTitle=sTitle;
}

template<typename _T>
void _marker<_T>::set_label(const std::string& sLabel) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_label(", sLabel,")");
    this->sLabel=sLabel;
}

template<typename _T>
void _marker<_T>::set_xlabel(const std::string& sXlabel) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_xlabel(", sXlabel,")");
    if (sXlabel.empty()) {
        msgM.msg(_msg::eMsg::MID,"set_xlabel(): reset xlabel");
        this->sXlabel=" ";
    }
    else
        this->sXlabel=sXlabel;
}

template<typename _T>
void _marker<_T>::set_ylabel(const std::string& sYlabel) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_ylabel(", sYlabel, ")");
    if (sYlabel.empty()) {
        msgM.msg(_msg::eMsg::MID,"set_ylabel(): reset ylabel");
        this->sYlabel=" ";
    }
    else
        this->sYlabel=sYlabel;
}

template<typename _T>
void _marker<_T>::set_xunit(const std::string& sXunit) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_xunit(", sXunit, ")");
    this->sXunit=sXunit;
}

template<typename _T>
void _marker<_T>::set_yunit(const std::string& sYunit) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_yunit(", sYunit, ")");
    this->sYunit=sYunit;
}

template<typename _T>
void _marker<_T>::set_output(const std::string& sFilename) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_output(", sFilename,")");
    
    if (sFilename.empty()) {
        this->sFilename="output.png";
        msgM.msg(_msg::eMsg::MID,"set_output(): reset output to:", 
                                 this->sFilename);
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
        msgM.msg(_msg::eMsg::ERROR,"set_output(): reset extension to:", 
                                    this->sExt);
        }
}

template<typename _T>
void _marker<_T>::set_output(const std::string& sFilename, 
                             const int iDpi) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_output(", sFilename, ",", iDpi,")");
    if (iDpi<50) {
        msgM.msg(_msg::eMsg::MID,"set_output(): reset dpi to:", 50);
        this->iDpi=50;
    }
    else
        this->iDpi=iDpi;
    
    set_output(sFilename);    
}

template<typename _T>
void _marker<_T>::set_continuum(const _T TContinuum) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_continuum(", TContinuum,")");
    this->TYcontinuum=TContinuum;
}

template<typename _T>
void _marker<_T>::set_supp(const _T TXmin, const _T TXmax) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID, "set_supp(", TXmin,",", TXmax,")");
    this->TXmin=TXmin;
    this->TXmax=TXmax;
}

template<typename _T>
void _marker<_T>::set_xmin(const _T TXmin) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_xmin(", TXmin,")");
    this->TXmin=TXmin;
}

template<typename _T>
void _marker<_T>::set_xmax(const _T TXmax) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_xmax(", TXmax,")");
    this->TXmax=TXmax;
}

template<typename _T>
void _marker<_T>::set_ymin(const _T TYmin) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_ymin(", TYmin,")");
    this->TYmin=TYmin;
}

template<typename _T>
void _marker<_T>::set_ymax(const _T TYmax) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_ymax(", TYmax,")");
    this->TYmax=TYmax;
}

template<typename _T>
void _marker<_T>::set_figsize(int iHeight, int iWidth) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_figsize(", iHeight, ",", iWidth,")");
    if (iHeight>0 && iWidth>0) {
        this->iHeight=iHeight;
        this->iWidth=iWidth;
        bIsset_fig_size=true;
    }
    else
        msgM.msg(_msg::eMsg::ERROR,"set_figsize(): invalid size");
}

template<typename _T>
void _marker<_T>::set_colorline(const std::string &sColor) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_colorline(", sColor,")");
    if (!sColor.empty())
        this->sColorline=sColor;
}

template<typename _T>
void _marker<_T>::set_linewidth(float fWidth) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set line width:",fWidth);
    if (fWidth>0)
        this->fLinewidth=fWidth;
}

template<typename _T>
void _marker<_T>::set_titlesize(int iSize) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_linewidth(",iSize,")");
    if (iSize>0)
        this->iTitlesize=iSize;    
}

template<typename _T>
void _marker<_T>::set_labelsize(int iSize) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_labelsize(",iSize,")");
    if (iSize>0)
        this->iLabelsize=iSize;    
}

template<typename _T>
void _marker<_T>::set_ticklabelsize(int iSize) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_ticklabelsize(",iSize,")");
    if (iSize>0)
        this->iTicklabelsize=iSize;    
}

template<typename _T>
void _marker<_T>::set_annotatesize(int iSize) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_annotatesize(",iSize,")");
    if (iSize>0)
        this->iAnnotatesize=iSize;    
}

template<typename _T>
void _marker<_T>::set_legendsize(int iSize) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_legendsize(",iSize,")");
     if (iSize>0)
        this->iLegendsize=iSize;    
}

template<typename _T>
void _marker<_T>::set_continnumsize(float fWidth) {
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"set_continnumsize(", fWidth,")");
    if (fWidth>0)
        this->fContinnumsize=fWidth;    
}

template<typename _T>
void _marker<_T>::set_scriptname(const std::string &sScriptname) {
    if (!sScriptname.empty())
        this->sScriptname=sScriptname;
    msgM.msg(_msg::eMsg::MID,"set_scriptname(:", this->sScriptname,")");
}

template<typename _T>
void _marker<_T>::add_line(_T TWl, const std::string &sName) {
    if (TWl<0 || sName.empty())
        msgM.msg(_msg::eMsg::ERROR,"add_line(): invalid line");
    else {
        if (bVerbose)
            msgM.msg(_msg::eMsg::MID,"add_line('",sName,"'", TWl);
        vllSet.push_back({TWl, sName});
    }
}

template<typename _T>
void _marker<_T>::add_data(const std::vector<_T>& vTX, 
                           const std::vector<_T>& vTY) {
    if (vTX.empty() || vTY.empty()) 
        msgM.msg(_msg::eMsg::ERROR,"add_data(): empty data");
    else if (vTX.size()!=vTY.size())
        msgM.msg(_msg::eMsg::ERROR,"add_data(): vector sizes mismatch");
    else {
        if (bVerbose)
            msgM.msg(_msg::eMsg::MID,"add_data()");
        
        if (X.empty() || Y.empty()) {
            set_data(vTX, vTY);
        }
        else {
            std::vector<std::vector<_T> > vData;
            vData.emplace_back(vTX);
            vData.emplace_back(vTY);
            vvvAdddata.emplace_back(vData);
        }
    }
}

template<typename _T>
void _marker<_T>::add_data(const std::vector<_T>& vTX, 
                           const std::vector<_T>& vTY, 
                           const std::string &sTitle) {
    
    if (bVerbose)
            msgM.msg(_msg::eMsg::MID,"add_data(): set label", sTitle);
    
    bool bFirstdef=false;
    if (X.empty() || Y.empty()) {
        set_data(vTX, vTY);
        set_label(sTitle);
        bFirstdef=true;
    }
    else
        add_data(vTX, vTY);
    if (!bFirstdef) {
        if (sTitle.empty())
            this->vsTitle.emplace_back(" ");
        else
            this->vsTitle.emplace_back(sTitle);
    }
}

template<typename _T>
_T _marker<_T>::get_continuum() const {
    return this->TYcontinuum;
}

template<typename _T>
_T* _marker<_T>::get_supp() const {
    if (this->TXmin>this->TXmax) {
        msgM.msg(_msg::eMsg::ERROR,"get_supp(): invalid support");
        return nullptr;
    }
        
    static _T TSupp[2]={this->TXmin,this->TXmax};

    return TSupp;
}

template<typename _T>
const std::string& _marker<_T>::get_scriptname() const {
    if (this->sScriptname.empty()) 
        msgM.msg(_msg::eMsg::ERROR,"get_scriptname(): empty script name");
   return this->sScriptname;
}

template<typename _T>
const std::string& _marker<_T>::get_output() const {
    if (this->sFilename.empty()) 
            msgM.msg(_msg::eMsg::ERROR,"get_output(): empty output name");
    return this->sFilename;
}

template<typename _T>
const std::string& _marker<_T>::get_title() const {
    return this->sTitle;
}

template<typename _T>
const std::string& _marker<_T>::get_label() const {
    return this->sLabel;
}

template<typename _T>
const std::string& _marker<_T>::get_xlabel() const {
    return this->sXlabel;
}

template<typename _T>
const std::string& _marker<_T>::get_xunit() const {
    return this->sXunit;
}

template<typename _T>
const std::string& _marker<_T>::get_ylabel() const {
    return this->sYlabel;
}

template<typename _T>
const std::string& _marker<_T>::get_yunit() const {
    return this->sYunit;
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
        msgM.msg(_msg::eMsg::ERROR,"make(): empty data");
        return false;
    }
    
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"make(): write data");
    
    std::fstream sfFlux_d(".data.csv", std::ios::out | 
                                       std::ios::trunc);
    
    if (sfFlux_d) {
        if (bVerbose)
            msgM.msg(_msg::eMsg::MID,"make(): write data");
        
        int iSize=this->X.size();
        for(int i=0; i<iSize; i++) 
            sfFlux_d << this->X[i] << " " << this->Y[i] << "\n";
        
        sfFlux_d.close();
    }
    else {
        msgM.msg(_msg::eMsg::ERROR,"make(): cannot write data");
        return false;
    }
    
    int iCount=0;
    std::vector<std::string> vsAddfilename;
    for(auto vvPlot: this->vvvAdddata) {
        vsAddfilename.emplace_back(".data_"+std::to_string(iCount)+".csv");
        std::fstream sfFlux(vsAddfilename[iCount], std::ios::out);
        if (sfFlux) {
            int iSize=vvPlot[0].size();
            for(int i=0; i<iSize; i++) 
                sfFlux << vvPlot[0][i] << " " << vvPlot[1][i] << "\n";
         
            sfFlux.close();
        }
        else 
            msgM.msg(_msg::eMsg::ERROR,"make(): cannot open:", 
                                        vsAddfilename[iCount]);
        iCount++;
    }
    
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"make(): make python script");
        
    add_cmd("import matplotlib.colors as col");
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
    
    iCount=0;
    for(auto sFile: vsAddfilename) {
        add_cmd("x"+std::to_string(iCount)+"=[]");
        add_cmd("y"+std::to_string(iCount)+"=[]\n");

        add_cmd("with open('"+sFile+"', 'r') as csvfile:");
        add_cmd("   plots= csv.reader(csvfile, delimiter=' ')");
        add_cmd("   for row in plots:");
        add_cmd("       x"+std::to_string(iCount)+".append(float(row[0]))");
        add_cmd("       y"+std::to_string(iCount)+".append(float(row[1]))\n");
        
        iCount++;
    }
    
    if (bIsset_fig_size)
        add_cmd("fig=plt.figure(figsize=("+
                std::to_string(get_figsize()[0])+","+
                std::to_string(get_figsize()[1])+")\n");
    else
        add_cmd("fig=plt.figure()\n");
      
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"make(): add plots");
    add_cmd("grid = fig.add_gridspec(nrows=1, ncols=1)");
    add_cmd("ax0=fig.add_subplot(grid[0,0])\n");
    
    add_cmd("ax0.plot(x,y,'-', color='"+sColorline+
            "', linewidth="+std::to_string(fLinewidth)+
            ", zorder=10, label='"+get_label()+"')\n");

    int iR=0x00;
    int iG=0x00;
    int iB=0xff;
    int iCycle=0;
    if (!this->vsTitle.empty())
        for(int i=0; i< vsAddfilename.size(); i++) {
                        
            std::stringstream ssS;
            ssS << "#" << std::hex 
                << std::setw(2) << std::setfill('0') << iR  
                << std::setw(2) << std::setfill('0') << iG
                << std::setw(2) << std::setfill('0') << iB;
                        
            add_cmd("ax0.plot(x"+std::to_string(i)+
                            ",y"+std::to_string(i)+
                            ",'--'"+
                            ", color=\""+ssS.str()+"\""+
                            ", linewidth="+std::to_string(fLinewidth)+
                            ", zorder=15, label='"+
                            this->vsTitle[i]+"')\n");
            
            if (iCycle==0)
                iB+=(0xff-1)/2;

            if (iB>=0xff) {
                iCycle=1;
                iR=0;
                iB=0xff;
                iG+=(0xff-1)/2;
            }
            
            if (iG>=0xff) {
                iCycle=2;
                iG=0xff;
                iB=0;
                iR+=(0xff-1)/2;
            }
            
            if (iR>=0xff) {
                iCycle=0;
                iR=0;
                iG=0;
                iB=(0xff-1)/2;
            }
        }
    else {
        int iCycle=0;
        for(int i=0; i< vsAddfilename.size(); i++)  {
            std::stringstream ssS;
            ssS << "#" << std::hex 
                << std::setw(2) << std::setfill('0') << iR  
                << std::setw(2) << std::setfill('0') << iG 
                << std::setw(2) << std::setfill('0') << iB;

            add_cmd("ax0.plot(x"+std::to_string(i)+
                            ",y"+std::to_string(i)+
                            ",'--', color='"+ssS.str()+
                            "', linewidth="+std::to_string(fLinewidth)
                            +", zorder=15)\n");
            
            if (iCycle==0)
                iB+=(0xff-1)/2;

            if (iB>=0xff) {
                iCycle=1;
                iR=0;
                iB=0xff;
                iG+=(0xff-1)/2;
            }
            
            if (iG>=0xff) {
                iCycle=2;
                iG=0xff;
                iB=0;
                iR+=(0xff-1)/2;
            }
            
            if (iR>=0xff) {
                iCycle=0;
                iR=0;
                iG=0;
                iB=(0xff-1)/2;
            }
        }
    }
            
    add_cmd("ax0.set_xlim("+std::to_string(get_supp()[0])+
            ", "+std::to_string(get_supp()[1])+")");
    
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"add title and labels");
    add_cmd("plt.title('"+
            get_title()+ "', size="+
            std::to_string(iTitlesize)+")");
    if (!this->sXunit.empty())
        add_cmd("ax0.set_xlabel('"+
                get_xlabel()+" ("+
                get_xunit()+")', size="+
                std::to_string(iLabelsize)+")");
    else
        add_cmd("ax0.set_xlabel('"+
                get_xlabel()+"', size="+
                std::to_string(iLabelsize)+")");
    
    if (!this->sYunit.empty())
        add_cmd("ax0.set_ylabel('"+
                get_ylabel()+" ("+
                get_yunit()+")', size="+
                std::to_string(iLabelsize)+")");
    else
        add_cmd("ax0.set_ylabel('"+
                get_ylabel()+"', size="+
                std::to_string(iLabelsize)+")");
    
    add_cmd("ax0.tick_params(direction='out', labelsize="+
             std::to_string(iTicklabelsize)+
             ", length=1, width=1, grid_alpha=0.5)");
    
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"make(): add continuum");
    
    add_cmd("ax0.plot(["+
    std::to_string(get_supp()[0])+","+
    std::to_string(get_supp()[1])+"],["+
    std::to_string(get_continuum())+","+
    std::to_string(get_continuum())+"],"+
    "'-.', color='red', linewidth="+
    std::to_string(fContinnumsize)+", zorder=1)\n");
    
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"make(): add markers");
    
    for(auto line: vllSet) {
        add_cmd("ax0.plot(["+
        std::to_string(line.TWl) + ", " +
        std::to_string(line.TWl) + "], [0, " +
        std::to_string(get_continuum()) +
        "],  color='red', linestyle='--', zorder=2, linewidth=0.50)\n");
            
        std::stringstream ssS;
        ssS << std::setprecision(2) << std::fixed << line.TWl;

        add_cmd("ax0.annotate('"+
                line.sElem+"\\n"+
                "$\\\\lambda "+ssS.str()+ 
                "$', xytext=("+
                std::to_string(line.TWl)+","+
                std::to_string(iCount*0.05)+"), "+
                "xy=("+std::to_string(line.TWl)+", "+
                std::to_string(iCount*0.05)+"), "+
                "color = 'grey', arrowprops=dict(arrowstyle='->', connectionstyle='arc3', linewidth=0.30), bbox=dict(boxstyle='round,pad=0.4', fc='white', ec='white', lw=2),size='"+
                std::to_string(iAnnotatesize)+"', ha='center')");
        
        iCount++;
    }
    
    add_cmd(" ");
    
    add_cmd("ax0.legend(loc='best', fontsize=6)\n");
    
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"make(): write margins");
    add_cmd("fig.subplots_adjust(left=0.06, bottom=0.07, right=0.98, top=0.95, wspace=0.16)");
    
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"make(): write savefig");
    add_cmd("fig.savefig('"+
            get_output()+"',dpi="+
            std::to_string(get_dpi())+")\n");
    
    std::fstream sfFlux(get_scriptname(), std::ios::out);
    
    if (sfFlux) {
        if (bVerbose)
            msgM.msg(_msg::eMsg::MID,"make(): write script:",get_scriptname());
        sfFlux << get_cmd();
        sfFlux.close();
        return true;
    }
    else
        msgM.msg(_msg::eMsg::ERROR,"make(): cannot open file:", get_scriptname());
    return false;
}

template<typename _T>
void _marker<_T>::plot(){
    if (bVerbose)
        msgM.msg(_msg::eMsg::MID,"plot(): run python script");
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
