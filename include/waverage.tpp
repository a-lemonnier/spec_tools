// --------------------------
// _IO                      -
// --------------------------

void hello() {
    std::cout << "******************************\n";
    std::cout << "*  Weighted arithmetic mean  *\n";
    std::cout << "******************************\n\n";   
}

template<typename _T> _io<_T>::_io(): exists_SNR(true), Precision(PRECISION) { }

template<typename _T> _io<_T>::_io(Vvv &VvvSpectr): exists_SNR(true), Precision(PRECISION) { 
    this->VvvSpectr=VvvSpectr;
}

template<typename _T> _io<_T>::~_io() { 
    std::cout << "\n- flushing data...";
    this->vvSpec.clear();
    this->VvvSpec.clear();
    std::cout << " done.\n";
}

template<typename _T> bool _io<_T>::read(std::string sFilename) { return true; }

template<typename _T> 
bool _io<_T>::read_dir(std::string sExtension) {
    bool bStatus=true;
    
    std::cout << "- read_dir(): start reading directory.\n";
    
    // -- Flush previous data
    std::cout << "\t\t-> flushing previous data...";
    this->vvSpec.clear();
    this->VvvSpec.clear();
    std::cout << " done.\n";
    // -- 
    
    // -- List of files
    std::cout << "\t\t-> creating the sorted list of files... ";
    for(const auto &file: fs::directory_iterator("./")) {
        std::string sFName(file.path());
        if (sFName.find(sExtension)!=std::string::npos) 
            this->vsFileList.emplace_back(sFName);
    }
    std::sort(this->vsFileList.begin(),this->vsFileList.end());
    std::cout << " done.\n";
    // -- 
    
    // -- Read and store data
    for(const auto sFName: this->vsFileList) {
        std::vector<vec> vvSpecTmp;
        
        std::fstream fFlux(sFName, std::ios::in);
        
        if (fFlux.is_open()) {
            std::cout << "\t\t-> file '" << sFName << "' is open.\n";
            std::string sLine;
            
            std::cout << "\t\t-> reading and converting data...";
            
            while(std::getline(fFlux, sLine)) {
                std::vector<std::string> vsLine;
                boost::algorithm::split(vsLine, sLine,  
                                        boost::is_any_of(" "), 
                                        boost::token_compress_on); // row to vector separator is ' '
                                        vvSpecTmp.emplace_back(vec{ std::stod(vsLine[0]), 
                                            std::stod(vsLine[1]) }); 
            }
            std::cout << vvSpecTmp.size()  << " lines done.\n";
            fFlux.close();
        }
        else {
            std::cerr << "/!\\ cannot open '" << sFName << "'\n";
            bStatus=true;
        }
        
        this->VvvSpec.emplace_back(vvSpecTmp);
        std::cout << "\t\t-> spectrum stacked.\n";
    }
    // --
    std::cout << "\t\t-> read_dir(): all done.\n\n";
    
    return bStatus;
}

template<typename _T> void _io<_T>::set_fileIn(std::string sFilename) { }

template<typename _T> void _io<_T>::set_fileOut(std::string sFilename) {
    if (!sFilename.empty())
        this->sFnameOut=sFilename;
    else
        std::cerr << "/!\\ set_fileOut(): empty string\n\n";
}

template<typename _T> bool _io<_T>::write() const { 
    fs::create_directory("converted");
    std::cout << "- write(): start writing all spectra in './converted'.\n";
    bool bStatus=true;
    for(const auto file: this->vsFileList)
        bStatus&=this->write(file);
    std::cout << "- write(): all done.\n\n";
    return bStatus; 
}

template<typename _T> bool _io<_T>::write(std::string sName) const {
    std::cout << "- write(): writing " << sName << ".\n";
    int n=this->get_FileIndex(sName);
    
    std::fstream fFlux(sName+".csv", std::ios::out);
    
    if (fFlux.is_open() && n>-1) {
        std::cout << "\t\t-> " << sName << ".csv" << " opened.\n";
        if (VvvSpec[n][0].SNR>0)
            for(const auto Spec: this->VvvSpec[n])
                fFlux << std::left 
                << std::setprecision(Precision) << Spec.x << " " 
                << Spec.y << " "                          
                << Spec.SNR << "\n";
            
            else
                for(const auto Spec: this->VvvSpec[n])
                    fFlux << std::left 
                    << std::setprecision(Precision) << Spec.x << " " 
                    << Spec.y << "\n";
                
                fFlux.close();
            
            fs::path Path(sName+".csv");
            std::cout << "\t\t-> deleting previous file " << ("converted/"+Path.filename().string()) << "... ";
            
            if (fs::exists("converted/"+Path.filename().string()))
                fs::remove("converted/"+Path.filename().string());
            std::cout << "done.\n";
            std::cout << "\t\t-> moving file... ";
            fs::copy(sName+".csv","converted");
            fs::remove(sName+".csv");
            std::cout << "done.\n";
    }
    
    return true;
}

template<typename _T>
int _io<_T>::get_FileIndex(std::string sName) const {
    int iCount=0;
    int iPos=-1;
    for(const auto file: this->vsFileList) {
        if(file.find(sName)!=std::string::npos) {
            iPos=iCount;
            break;
        }
        iCount++;
    }
    return iPos;
}

template<typename _T> 
const std::vector<typename _io<_T>::vec> _io<_T>::get_vector() const {
    return this->vvSpec;
}

template<typename _T> 
const std::vector<std::vector<typename _io<_T>::vec> > _io<_T>::get_vectors() const {
    return this->VvvSpec;
}

template<typename _T> 
const std::valarray<std::valarray<_T> > _io<_T>::get_valarray() {
    // empty vectors do not matter
    std::cout << "- get_valarray(): start converting vector to valarray.\n";
    
    std::cout << "\t\t-> initializing array...";
    std::valarray<std::valarray<_T> > vvRes(3); 
    for(auto &vV: vvRes)
        vV=std::valarray<_T>(this->vvSpec.size());
    std::cout << " done.\n";
    
    std::cout << "\t\t-> filling array... ";
    int iiDim=this->vvSpec.size();
    for(int i=0; i<iiDim; i++) {
        vvRes[0][i]=this->vvSpec[i].x;
        vvRes[1][i]=this->vvSpec[i].y;
        vvRes[2][i]=this->vvSpec[i].SNR;
    }
    std::cout << iiDim << " values done.\n\n";    
    return vvRes;
}

template<typename _T> 
const std::vector<std::valarray<std::valarray<_T> > > _io<_T>::get_valarrays()  {
    
    std::cout << "- get_valarrays(): start converting vectors to valarrays\n";
    
    std::cout << "\t\t-> initializing arrays...";
    std::vector<std::valarray<std::valarray<_T> > > VvvRes;
    
    int iNbSpec=this->VvvSpec.size();
    for(int i=0; i<iNbSpec;i++) 
        VvvRes.emplace_back(std::valarray<std::valarray<_T> >(3)); // X,Y,SNR
        
        for(auto &Vvv: VvvRes) { // heap
            int i=0;
            for(auto &vV: Vvv) {// spectrum: vV[0], vV[1]... to init
                vV=std::valarray<_T>(this->VvvSpec[i].size());
                i++;
            }
        }
        std::cout << " done.\n";
    
    for(int n=0; n<iNbSpec;n++) {
        std::cout << "\t\t-> filling array with "<< VvvRes[n].size() <<" columns... ";
        int iiDim=this->VvvSpec[n].size();
        for(int i=0; i<iiDim; i++) {
            VvvRes[n][0][i]=this->VvvSpec[n][i].x;
            VvvRes[n][1][i]=this->VvvSpec[n][i].y;
            VvvRes[n][2][i]=this->VvvSpec[n][i].SNR; // Check this !
        }
        std::cout << " done.\n";
    }
    std::cout << "\t\t-> total: " << iNbSpec << " arrays done.\n\n";  
    return VvvRes;     
}

template<typename _T> 
bool _io<_T>::read_fits(std::string sFilename) {
    
    std::valarray<double> vWave;
    std::valarray<double> vFlux;
    
    //CCfits::FITS::setVerboseMode(true);
    
    std::cout << "- read_fits(): start reading fits: " << sFilename << ".\n";
    
    // -- Flush previous data
    std::cout << "\t\t-> flushing previous data...";
    this->vvSpec.clear();
    this->VvvSpec.clear();
    std::cout << " done.\n";
    // -- 
    
    std::cout << "\t\t-> reading fits...";
    CCfits::FITS FIn(sFilename,CCfits::Read,true);
    CCfits::ExtHDU& table = FIn.extension(1);
    table.column("WAVE").read(vWave,1);
    table.column("FLUX_REDUCED").read(vFlux,1);
    std::cout << " done.\n";
    
    if (vWave.size()>0 && vWave.size()==vFlux.size()) {
        std::cout << "\t\t-> filling vector... ";
        for(int i=0; i<vWave.size(); i++)
            this->vvSpec.emplace_back(vec{vWave[i], vFlux[1]});
        std::cout << vWave.size() << " rows done.\n";
    }
    else {
        std::cerr << "/!\\ error while reading fits.\n";
        FIn.destroy();
        return false;
    }
    FIn.destroy();
    return true;
}

template<typename _T> 
bool _io<_T>::read_fits_dir(std::string sExtension) { return this->read_fits_dir("./", sExtension); }

template<typename _T> 
bool _io<_T>::read_fits_dir(std::string sDirectory, std::string sExtension) {
    
    std::cout << "- read__fits_dir(): start reading fits in the directory.\n";
    
    // -- Flush previous data
    std::cout << "\t\t-> flushing previous data...";
    this->vvSpec.clear();
    this->VvvSpec.clear();
    std::cout << " done.\n";
    // -- 
    
    // -- List of files
    std::cout << "\t\t-> creating the sorted list of files... ";
    for(const auto &file: fs::directory_iterator(sDirectory+"/")) {
        std::string sFName(file.path());
        if (sFName.find(sExtension)!=std::string::npos) 
            this->vsFileList.emplace_back(sFName);
    }
    std::sort(this->vsFileList.begin(),this->vsFileList.end());
    std::cout << " done.\n";
    // -- 
    
    // --
    for(const auto sFName: this->vsFileList) {
        std::vector<vec> vvSpecTmp;
        
        std::valarray<_T> vWave;
        std::valarray<_T> vFlux;
        std::valarray<_T> vSNR;
        std::valarray<_T> vExpTimeTmp;
        
        std::cout << "\t\t-> reading fits " << sFName << "...";
        
        //         CCfits::FITS::setVerboseMode(true);
        CCfits::FITS FIn(sFName,CCfits::Read,true);
        
        CCfits::ExtHDU& table = FIn.extension(1);
        table.column("WAVE").read(vWave,1);
        std::cout << " (SNR) ";
        
        table.column("FLUX_REDUCED").read(vFlux,1);
        std::cout << "(FLUX_REDUCED) ";
        
        this->exists_SNR=true;
        try { table.column("SNR").read(vSNR,1); }
        catch(...) { this->exists_SNR=false; }
        
        if (!this->exists_SNR) {
            this->exists_SNR=true;
            try { table.column("SNR_REDUCED").read(vSNR,1); }
            catch(...) { this->exists_SNR=false; }
            std::cout << "(SNR_REDUCED) ";
        }
        else
            std::cout << "(SNR) ";
        
        if (this->exists_SNR)
            std::cout << " done.\n";
        else
            std::cout << "\n\t-> no SNR found.\n";
        
        
        if (vWave.size()>0 && vWave.size()==vFlux.size()) {
            std::cout << "\t\t-> filling vector... ";
            if (this->exists_SNR)
                for(int i=0; i<vWave.size(); i++)
                    vvSpecTmp.emplace_back(vec{vWave[i], vFlux[i], vSNR[i]});
                else
                    for(int i=0; i<vWave.size(); i++)
                        vvSpecTmp.emplace_back(vec{vWave[i], vFlux[i]});
                    
                    std::cout << vWave.size() << " rows done.\n";
        }
        else {
            std::cerr << "/!\\ error while reading fits.\n";
            return false;
        }
        
        this->VvvSpec.emplace_back(vvSpecTmp);
        std::cout << "\t\t-> spectrum stacked.\n";
        FIn.destroy();
    }
    std::cout << "\t\t- read_fits_dir(): all done.\n\n";
    return true;
}


template<typename _T> 
void _io<_T>::show_data() const {
    for(int i=0; i< this->VvvSpec.size(); i++)
        this->show_data(i);
}

template<typename _T> 
void _io<_T>::show_data(int n) const {
    if (n<this->VvvSpec.size()) {
        std::cout << "- showing spectrum " << n << "\n";
        std::cout << "---\nWAVE\t\tFLUX\t\tSNR\n---\n";
        if (VvvSpec[n][0].SNR>-1)
            for(const auto Spec: this->VvvSpec[n]) 
                std::cout << std::left 
                << std::setw(Precision) << std::setprecision(Precision) << Spec.x << "\t\t" 
                << std::setw(Precision)                                 << Spec.y 
                << std::setw(Precision) << "\t\t"                       << Spec.SNR << "\n";
            else
                for(const auto Spec: this->VvvSpec[n]) 
                    std::cout << std::left 
                    << std::setw(Precision) << std::setprecision(Precision) << Spec.x << "\t\t" 
                    << std::setw(Precision)                                 << Spec.y << "\n";
    }
    std::cout << "---\n\n";
}

template<typename _T> 
void _io<_T>::show_data(std::string sName) const {
    int iCount=0;
    for(const auto file: this->vsFileList) {
        if(file.find(sName)!=std::string::npos)
            show_data(iCount);
        iCount++;
    }
}

template<typename _T> 
void _io<_T>::set_WaveScale(_T Scale) {
    std::cout << "- set_WaveScale().\n";
    std::cout << "\t\t-> multiplying wavelengths by " << Scale << "... "; 
    this->Scale=Scale;
    for(auto &vvSpec: this->VvvSpec)
        for(auto &vSpec: vvSpec)
            vSpec.x*=Scale;
        std::cout << "done.\n\n";
}


template<typename _T> 
void _io<_T>::set_data(Vvv& VvvSpectr) {
    std::cout << "- set_data(): setting data.\n";
    
    std::cout << "\t\t-> clear data.\n";
    this->VvvSpec.clear();
    
    int iCount=0;
    for(auto const vvSpectr: VvvSpectr) {
        bool nHasSNR=vvSpectr.size()==3;
        if (nHasSNR) std::cout << "\t\t-> copy spectrum " << iCount << "(SNR)... ";
        else         std::cout << "\t\t-> copy spectrum " << iCount << "... ";
        int iDim=vvSpectr[0].size();
        
        std::vector<vec> vSpec;
        
        for(int i=0;i<iDim;i++) {
            if (nHasSNR) vSpec.emplace_back(vec{vvSpectr[0][i],vvSpectr[1][i],vvSpectr[2][i]});
            else         vSpec.emplace_back(vec{vvSpectr[0][i],vvSpectr[1][i],-1});
        }
        
        this->VvvSpec.emplace_back(vSpec);
        
        std::cout << " done.\n";
        
        iCount++;
    } 
    
    
    std::cout << "\t\t-> set_data(): all done.\n\n";
}


template<typename _T>
void _io<_T>::notify(std::string sTitle, std::string sMsg, int iTime) {
    
    #ifdef LNOTIFY
    notify_init("waverage");
    NotifyNotification* n = notify_notification_new (sTitle.c_str(), 
                                                     sMsg.c_str(), 0);
    notify_notification_set_timeout(n, iTime*1000); 
    notify_notification_show(n, 0);    
    #endif
}

// --------------------------
// _OP                      -
// --------------------------


template<typename _T> _op<_T>::_op() { }

template<typename _T> _op<_T>::_op(Vvv& VvvSpectr): VvvSpectr(VvvSpectr), 
bHaveSNR(true) { 
    std::cout << "- _op(): Initialisation with spectra assuming structure:\n";
    for(const auto vvSpectr: VvvSpectr) {
        this->bHaveSNR&=vvSpectr.size()==3;
        if (this->bHaveSNR) std::cout <<  "\t\t->\t(WAVE)\t(FLUX)\t(SNR)\n";
        else                std::cout <<  "\t\t->\t(WAVE)\t(FLUX)\n";
    }
    std::cout << "\n";
}
template<typename _T> _op<_T>::~_op() { this->VvvSpectr.clear(); }

template<typename _T> 
void _op<_T>::set_data(Vvv& VvvSpectr) { this->VvvSpectr=VvvSpectr; }

template<typename _T> 
std::pair<_T, _T> _op<_T>::get_wlRange(int n) const {
    if (n>-1 && n<this->VvvSpectr.size()) 
        return { this->VvvSpectr[n][0].min(),
            this->VvvSpectr[n][0].max()};
            return {-1,-1};
}

template<typename _T> 
std::pair<_T, _T> _op<_T>::get_wlRangeMin() const {
    std::vector<_T> vWl_min, vWl_max;
    for(int i=0; i<this->VvvSpectr.size(); i++) {
        auto [min, max]=this->get_wlRange(i);
        vWl_min.emplace_back(min);
        vWl_max.emplace_back(max);
    }
    return {*std::min_element(vWl_min.begin(),vWl_min.end()), 
        *std::max_element(vWl_max.begin(), vWl_max.end())};
}


template<typename _T> 
bool _op<_T>::resize_spectr() {
    std::cout << "- resize_spectr().\n";
    
    std::cout << "\t\t-> get best support\n.";
    auto [min,max]=this->get_wlRangeMin();
    
    if (min>=max) return false;
    
    std::cout << "\t\t-> get the minimum iDimension: ";
    int MiniDim=0;
    std::vector<int> viDim;
    for(const auto vvSpec: this->VvvSpectr) 
        viDim.emplace_back(vvSpec[0].size()); // assuming iDim(X)==iDim(Y)
        
        MiniDim=*std::min_element(viDim.begin(), viDim.end());
    std::cout << MiniDim << " .\n";
    
    int iCount=0;
    for(auto &vvSpec: this->VvvSpectr) {
        std::cout << "\t\t-> resizing spectrum "<< iCount <<": "
        << vvSpec[0].size() << " -> ";
        std::vector<_T> X, Y, SNR;
        
        for(int i=0; i<MiniDim; i++) {
            _T x=vvSpec[0][i], y=vvSpec[1][i];
            if (x>=min && x<=max) {
                X.emplace_back(x);
                Y.emplace_back(y);
                if (this->bHaveSNR)
                    SNR.emplace_back(vvSpec[2][i]);  
            }
        }
        vvSpec[0]=std::valarray<_T>(X.data(), X.size());
        vvSpec[1]=std::valarray<_T>(Y.data(), Y.size());
        if (this->bHaveSNR)
            vvSpec[2]=std::valarray<_T>(SNR.data(), SNR.size());
        
        std::cout << vvSpec[0].size() << ".\n";
        
        iCount++;
    }
    std::cout << "\t\t-> resize_spectr(): all done.\n\n";
    
    return true;
}


template<typename _T> 
void _op<_T>::remove_zero() {
    std::cout << "- remove_zero():\n";
    int iCount=0;
    for(auto &vvSpectr: this->VvvSpectr) {
        std::cout << "\t\t-> removing 0 from spectrum " << iCount << "\n";
        
        VV VVSpectr;
        if (this->bHaveSNR) VVSpectr=VV(3);
        else                VVSpectr=VV(2);
        
        if (this->bHaveSNR)
            std::cout << "\t\t-> size before: (" << vvSpectr[0].size() << ", " 
            << vvSpectr[1].size() << ", " << vvSpectr[2].size() << ").\n"; 
        else
            std::cout << "\t\t-> size before: (" << vvSpectr[0].size() << ", " 
            << vvSpectr[1].size() << ").\n"; 
        
        for(int i=0; i<vvSpectr[1].size(); i++) {
            _T x=vvSpectr[0][i], y=vvSpectr[1][i];
            if (y>0) {
                VVSpectr[0].emplace_back(x);
                VVSpectr[1].emplace_back(y);
                if (this->bHaveSNR)
                    VVSpectr[2].emplace_back(vvSpectr[2][i]);
            }
        }
        
        vv vvNewSpectr;
        if (this->bHaveSNR) vvNewSpectr=vv(3);
        else                vvNewSpectr=vv(2);
        
        vvNewSpectr[0]=std::valarray<_T>(VVSpectr[0].data(), VVSpectr[0].size());
        vvNewSpectr[1]=std::valarray<_T>(VVSpectr[1].data(), VVSpectr[1].size());
        if (this->bHaveSNR)
            vvNewSpectr[2]=std::valarray<_T>(VVSpectr[2].data(), VVSpectr[2].size());
        
        vvSpectr=vvNewSpectr;
        
        std::cout << "\t\t-> size after: (" << vvSpectr[0].size() 
        << ", " << vvSpectr[1].size() << ").\n";  
        
        iCount++;
    }
    std::cout << "\t\t-> remove_zero(): all done.\n\n";
}


template<typename _T> 
bool _op<_T>::rebuild_wlStep() {
    std::cout << "- rebuild_wlStep(): start rebuilding support.\n";
    
    int iCount=0;
    for(auto &vvSpectr: this->VvvSpectr) {
        std::vector<_T> X;
        
        auto [min, max]=this->get_wlRange(iCount);
        
        _T Step=(max-min)/vvSpectr[1].size();
        Step=ceil(10000*Step)/10000; // Check this !
        
        std::cout << "\t\t-> rebuilding with new step for spectrum "
        << iCount << ": " <<  Step << " for [" << min << ", " 
        << max << "] ...";
        
        for(_T f=min;f<max;f+=Step) X.push_back(f); // Check this !
        vvSpectr[0]=std::valarray<_T>(X.data(), X.size());
        
        std::cout << " done.\n";
        
        std::cout << "\t\t-> size is: (" << vvSpectr[0].size() 
        << ", " << vvSpectr[0].size() << ").\n";
        
        iCount++;
    }
    std::cout << "\t\t-> rebuild_wlStep(): all done.\n\n";
    return true;
}


template<typename _T> 
bool _op<_T>::filter_SG(int n) {
    if (n>this->VvvSpectr.size() || n<0)
        return false;

    int iDim=this->VvvSpectr[n][0].size();
    int iWindow=1000;
    int iPolyDeg=12;
    
    std::cout << "\n- filter_SG(): n=" << n << " - PolyDeg="<< iPolyDeg << "- \u03C3=" << (abs(VvvSpectr[n][0][1]-VvvSpectr[n][0][0])*iWindow) <<  "\u212B.\n";
    
    std::vector<_T> vRes;
    
//     std::cout << "\t\t-> computing...";
    for(unsigned int i=0;i<(iWindow-1)/2;++i) 
        vRes.push_back(VvvSpectr[n][1][i]);
    
    for(unsigned int i=(iWindow-1)/2;i<iDim-(iWindow-1)/2;i++) {
        // Calcul des coeffs à la volée en cas de changement de pas
        std::vector<_T> Coeff=this->SG_conv(VvvSpectr[n][0],
                                            i-(iWindow-1)/2,
                                            i+iWindow-(iWindow-1)/2, 
                                            iPolyDeg);
        _T mid=0;
        for(unsigned int j=0;j<iWindow;j++) 
            mid+=Coeff[j]*VvvSpectr[n][1][i+j-(iWindow-1)/2];
        vRes.push_back(mid);
    }
    
    for(unsigned int i=vRes.size();i<iDim;i++) 
        vRes.push_back(VvvSpectr[n][1][i]);
    
    for(int i=0; i<vRes.size(); i++)
        if (vRes[i]!=0) // bofbof voir precision
            VvvSpectr[n][1][i]/=vRes[i];
        
//     std::cout << " done.\n";
    return true;
}

template<typename _T> 
void _op<_T>::filter_SG_th(int n) {
        this->filter_SG(n);
}


template<typename _T> 
bool _op<_T>::filter_SG() {
    
    int max_thread=std::thread::hardware_concurrency();
    bool bStatus=true;
    
#if defined (__linux__)
    std::cout << "- CPU load: "<< static_cast<int>(CPU_utilization()) << "%\n";
    if (CPU_utilization()>50) {
        max_thread=1;
        std::cout << "\t\t-> reducing max thread.\n";
    }
#endif
    
    if (max_thread>1) {
        std::launch flag=std::launch::async | std::launch::deferred;
        std::vector<std::future<void> > thread;
        
        for(int i=0; i<this->VvvSpectr.size();i++)
            thread.emplace_back(std::async(flag, &_op<_T>::filter_SG_th, this, i));   // solve the non-static error
        std::for_each(thread.begin(), thread.end(), [](std::future<void> &th) { th.get(); });
        
        std::cout << "- filter_SG(): all done.\n\n";
    }
    else {
        std::cout << "- filter_SG(): filtering all spectra.\n";
        for(int i=0; i<this->VvvSpectr.size();i++)
            bStatus&=this->filter_SG(i);
        std::cout << "- filter_SG(): all done.\n\n";
    }
    
    return bStatus;
}


template<typename _T> 
void _op<_T>::compute_wmean() {
    std::cout << "- compute_wmean(): computing weighted mean.\n";
    
    int iNbSpec=this->VvvSpectr.size();
    int iRow=this->VvvSpectr[0][0].size(); // assuming all equal rows
    _T SNR_t=0;
    
    vvWMean.resize(0);
    vvWMean=vv({std::valarray<_T>(iRow), std::valarray<_T>(iRow)});
    
    std::cout << "\t\t-> Number of spectra:\t" << iNbSpec << ".\n";
    std::cout << "\t\t-> Number of rows:\t" << iRow << ".\n";
    std::cout << "\t\t-> Computing...";
    for(int i=0; i<iRow; i++) {
        _T P=0;
        _T SNR=0;
        _T wl=0;
        for(int k=0; k<iNbSpec; k++) {
            _T SNR_tmp=sqrtl(this->VvvSpectr[k][2][i]);
            wl+=this->VvvSpectr[k][0][i];
            P+=this->VvvSpectr[k][1][i]*SNR_tmp;
            SNR+=SNR_tmp;
            SNR_t+=SNR_tmp;
        }
        
        vvWMean[0][i]=wl/iNbSpec;
        vvWMean[1][i]=P/SNR;
    }
    std::cout << " done.\n";
    
    std::cout << "\t\t-> <SNR>=" << pow(SNR_t/iRow/iNbSpec,2) << ".\n";
    std::cout << "\t\t-> Size of the result: (" << vvWMean[0].size() 
              << ", " << vvWMean[1].size() << ").\n";
    
    std::cout << "\t\t-> compute_wmean(): all done.\n\n";
}

template<typename _T> 
bool _op<_T>::write(_io<_T>& ioInterface) {
    ioInterface.set_data(VvvSpectr);
    return ioInterface.write();
}

template<typename _T> 
bool _op<_T>::write_mean(std::string sFilename) const {
    std::cout << "- write_mean(): writing result in " << sFilename << ".\n";
    std::fstream fFlux(sFilename, std::ios::out);
    
    if (!fFlux.is_open()) {
        std::cerr << "/!\\ cannot open " << sFilename << ".\n\n";
        return false;
    }
    
    for(int i=0; i< this->vvWMean[0].size(); i++)
        fFlux << this->vvWMean[0][i] << " " << this->vvWMean[1][i] << "\n";
    
    fFlux.close();
    std::cout << "\t\t-> done.\n\n";
    
    return true;    
}


template<typename _T> 
std::vector<_T> _op<_T>::SG_conv(std::valarray<_T> &vX, 
                                 int i1, int i2, 
                                 int iPolyDeg) const {
    std::vector<_T> vRes;
    int TrimiDim=abs(i1-i2);
    
    std::valarray<int> vX_CR(TrimiDim);
    std::valarray<_T> vX_tmp(TrimiDim);
    
    Eigen::MatrixXd Jac(TrimiDim,iPolyDeg+1);
    Eigen::MatrixXd Conv(TrimiDim,iPolyDeg+1);
    
    vX_tmp=vX[std::slice(i1,TrimiDim,1)];
    
    _T mean=std::accumulate(std::begin(vX_tmp), 
                            std::end(vX_tmp), 0.0)/vX_tmp.size();
    
    // Centrage et reduisage de l'abscisse: -2 -1 0 1 2
    _T mStep=this->mean_step(vX_tmp);
    for(int i=0;i<TrimiDim;i++) 
        vX_CR[i]=round((vX_tmp[i]-mean)/mStep);
    
    // Ecriture du Jacobien discret (Vandermonde)
    for(int i=0;i<TrimiDim;i++) 
        for(int j=0;j<iPolyDeg+1;j++)  
            Jac(i,j)=static_cast<double>(pow(vX_CR[i],j));
        
        // Calcul des coeffs: (tJ.J)^-1 *tJ
        Conv=(Jac.transpose()*Jac).inverse()*Jac.transpose();
    
    for(int i=0;i<TrimiDim;i++) 
        vRes.emplace_back(Conv(0,i));
    
    return vRes;
}

template <typename _T>
_T _op<_T>::mean_step(const std::valarray<_T> &vArray) const {
    std::vector<_T> shift;
    for(unsigned int i=0; i<vArray.size()-1;i++) 
        shift.emplace_back(abs(vArray[i]-vArray[i+1]));
    return std::accumulate(shift.begin(), shift.end(), 0.0)/shift.size();
}

template <typename _T>
double long _op<_T>::CPU_utilization() {       
    auto [fTime_A, fIdle_A]=get_stat();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto [fTime_B, fIdle_B]=get_stat();
    return 100.*(1.-((fIdle_B-fIdle_A)/(fTime_B-fTime_A)));
}

template <typename _T>
std::tuple<double long, double long>  _op<_T>::get_stat() {
    std::fstream sfCpu("/proc/stat", std::ios::in);
    
    if (sfCpu) {

        std::string sLine;
        std::getline(sfCpu, sLine);
        
        // erase "cpu"
        sLine.erase(sLine.begin(), sLine.begin()+sLine.find_first_of("0123456789")); 
        
        // locate " " and push position into vec
        std::vector<int> vPos;
        std::vector<double long> vCol;
        
        vPos.push_back(0); 
        
        int iCount=0;
        for(auto cC: sLine) {
            if (cC==' ')
                vPos.push_back(iCount);
            iCount++;  
        }
        
        // slice
        for(int i=0; i<4; i++) {
            std::string sVal=sLine.substr(vPos[i], vPos[i+1]-vPos[i]);
            sVal.erase(std::remove(sVal.begin(), sVal.end(), ' '), sVal.end()); 
            vCol.push_back(std::stod(sVal));
        }
        
        sfCpu.close();
                
        return {static_cast<double long>(vCol[0]+vCol[1]+vCol[2]+vCol[3]), static_cast<double long>(vCol[3])};
    }
    else 
        std::cerr << "/!\\ cannot open /proc/stat.\n\n";
    
    return {-1,1};
}
