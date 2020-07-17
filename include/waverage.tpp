// --------------------------
// _IO                      -
// --------------------------

void hello() {
    std::cout << "******************************\n";
    std::cout << "*  Weighted arithmetic mean  *\n";
    std::cout << "******************************\n\n";   
}

template<typename _T> _io<_T>::_io(): exists_SNR(true), Precision(PRECISION) { }
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
            std::cout << "\t\t-> deleting previous file " << ("converted/"+Path.filename().string()) << " ... ";
            
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
    std::valarray<std::valarray<_T> > vvRes(2); // add SNR there and below?
    for(auto &vV: vvRes)
        vV=std::valarray<_T>(this->vvSpec.size());
    std::cout << " done.\n";
    
    std::cout << "\t\t-> filling array... ";
    int iDim=this->vvSpec.size();
    for(int i=0; i<iDim; i++) {
        vvRes[0][i]=this->vvSpec[i].x;
        vvRes[1][i]=this->vvSpec[i].y;
    }
    std::cout << iDim << " values done.\n\n";    
    return vvRes;
}

template<typename _T> 
const std::vector<std::valarray<std::valarray<_T> > > _io<_T>::get_valarrays()  {
    
    std::cout << "- get_valarrays(): start converting vectors to valarrays.\n";
    
    std::cout << "\t\t-> initializing arrays...";
    std::vector<std::valarray<std::valarray<_T> > > VvvRes;
    
    int iNbSpec=this->VvvSpec.size();
    for(int i=0; i<iNbSpec;i++) 
        VvvRes.emplace_back(std::valarray<std::valarray<_T> >(2));
    
    for(auto &Vvv: VvvRes) { // heap
        int i=0;
        for(auto &vV: Vvv) {// spectrum: vV[0], vV[1]... to init
            vV=std::valarray<_T>(this->VvvSpec[i].size());
            i++;
        }
    }
    std::cout << " done.\n";
    
    std::cout << "\t\t-> filling arrays... ";
    for(int n=0; n<iNbSpec;n++) {
        int iDim=this->VvvSpec[n].size();
        for(int i=0; i<iDim; i++) {
            VvvRes[n][0][i]=this->VvvSpec[n][i].x;
            VvvRes[n][1][i]=this->VvvSpec[n][i].y;
        }
    }
    std::cout << iNbSpec << " arrays done.\n\n";  
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
        if (VvvSpec[n][0].SNR>0)
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
    std::cout << "\t\t-> multiplying wavelengths by " << Scale << " ... "; 
    this->Scale=Scale;
    for(auto &vvSpec: this->VvvSpec)
        for(auto &vSpec: vvSpec)
            vSpec.x*=Scale;
        std::cout << "done.\n\n";
}


// --------------------------
// _OP                      -
// --------------------------

//     bool resize_spectr(); /**< resize all spectra to the same size (maximal). */
//     bool rebuild_wlStep(_T Step); /**< rebuild wavelength axis. */
// 
//     void remove_zero(); /**< trim spectra where flux is 0. */
//         
//     const vv& compute_mean() const; /**< compute arithmetic mean*/
//     const vv& compute_wmean() const; /**< compute weighted arithmetic mean */



template<typename _T> _op<_T>::_op() { }
template<typename _T> _op<_T>::_op(Vvv& VvvSpectr): VvvSpectr(VvvSpectr) { }
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
    auto [min,max]=this->get_wlRangeMin();
    
    if (min>=max)
        return false;
    
    for(auto &vvSpec: this->VvvSpectr) {
        std::vector<_T> X, Y;
        
        for(int i=0; i<vvSpec.size(); i++) {
            _T x=vvSpec[0][i], y=vvSpec[1][i];
            if (x>=min && x<=max) {
                X.emplace_back(x);
                Y.emplace_back(y);
            }
        }
        vvSpec[0]=std::valarray<_T>(X.data(), X.size());
        vvSpec[1]=std::valarray<_T>(Y.data(), Y.size());
    }
    
    
    return true;
}


template<typename _T> 
void _op<_T>::remove_zero() {
    std::cout << "- remove_zero():\n";
    int iCount=0;
    for(auto &vvSpectr: this->VvvSpectr) {
        std::cout << "\t\t-> removing 0 from spectrum " << iCount << "\n";
        VV VVSpectr(2);
        
        std::cout << "\t\t-> size before: (" << vvSpectr[0].size() << ", " << vvSpectr[1].size() << ").\n"; 
        
        for(int i=0; i<vvSpectr[1].size(); i++) {
            _T x=vvSpectr[0][i], y=vvSpectr[1][i];
            if (y>0) {
                VVSpectr[0].emplace_back(x);
                VVSpectr[1].emplace_back(y);
            }
        }
        
        vv vvNewSpectr(2);
        
        vvNewSpectr[0]=std::valarray<_T>(VVSpectr[0].data(), VVSpectr[0].size());
        vvNewSpectr[1]=std::valarray<_T>(VVSpectr[1].data(), VVSpectr[1].size());
        
        vvSpectr=vvNewSpectr;
        
        std::cout << "\t\t-> size after: (" << vvSpectr[0].size() << ", " << vvSpectr[1].size() << ").\n";  
        
        iCount++;
        
    }
    std::cout << "\t\t-> remove_zero(): all done.\n\n";
}


template<typename _T> 
bool _op<_T>::rebuild_wlStep() {
    
    int iCount=0;
     for(auto &vvSpectr: this->VvvSpectr) {
         std::vector<_T> X;
         
         auto [min, max]=this->get_wlRange(iCount);
         
         _T Step=(max-min)/vvSpectr[1].size();
         
         Step=ceil(10000*Step)/10000;
         
         std::cout << "new step for spectrum "<< iCount << ": " <<  Step << "\n";
         
         std::iota(X.begin(), X.end(), Step);
         
         vvSpectr[0]=std::valarray<_T>(X.data(), X.size());
         
         iCount++;
     }
    
    return true;
}
