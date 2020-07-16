// --------------------------
// _IO                      -
// --------------------------


template<typename _T> _io<_T>::_io() { }
template<typename _T> _io<_T>::~_io() { 
    std::cout << "- flushing data...";
    this->vvSpec.clear();
    this->vvvSpec.clear();
    std::cout << " done\n";
}

template<typename _T> bool _io<_T>::read(std::string sFilename) { return true; }

template<typename _T> 
bool _io<_T>::read_dir(std::string sExtension) {
    bool bStatus=true;
    
    std::cout << "- read_dir(): start reading directory.\n";
    
    // -- Flush previous data
    std::cout << "\t-> flushing previous data...";
    this->vvSpec.clear();
    this->vvvSpec.clear();
    std::cout << " done\n";
    // -- 
    
    // -- List of files
    std::cout << "\t-> creating the sorted list of files... ";
    for(const auto &file: fs::directory_iterator("./")) {
        std::string sFName(file.path());
        if (sFName.find(sExtension)!=std::string::npos) 
            this->vsFileList.emplace_back(sFName);
    }
    std::sort(this->vsFileList.begin(),this->vsFileList.end());
    std::cout << " done\n";
    // -- 
    
    // -- Read and store data
    for(const auto sFName: this->vsFileList) {
        std::vector<vec> vvSpecTmp;
        
        std::fstream fFlux(sFName, std::ios::in);
        
        if (fFlux.is_open()) {
            std::cout << "\t-> file '" << sFName << "' is open.\n";
            std::string sLine;
            
            std::cout << "\t-> reading and converting data...";
            
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
        
        this->vvvSpec.emplace_back(vvSpecTmp);
        std::cout << "\t-> spectrum stacked.\n";
    }
    // --
    std::cout << "\t- read_dir(): all done.\n\n";
    
    return bStatus;
}

template<typename _T> void _io<_T>::set_fileIn(std::string sFilename) { }

template<typename _T> void _io<_T>::set_fileOut(std::string sFilename) {
    if (!sFilename.empty())
        this->sFnameOut=sFilename;
    else
        std::cerr << "/!\\ set_fileOut(): empty string\n";
}

template<typename _T> bool _io<_T>::write(std::string sFilename) { return true; }

template<typename _T> 
const std::vector<typename _io<_T>::_vec> _io<_T>::get_vector() const {
    return this->vvSpec;
}

template<typename _T> 
const std::vector<std::vector<typename _io<_T>::_vec> > _io<_T>::get_vectors() const {
    return this->vvvSpec;
}

template<typename _T> 
const std::valarray<std::valarray<_T> > _io<_T>::get_valarray() {
    // empty vectors do not matter
    std::cout << "- get_valarray(): start converting vector to valarray.\n";
    
    std::cout << "\t-> initializing array...";
    std::valarray<std::valarray<_T> > vvRes(2); // add SNR there and below?
    for(auto &vV: vvRes)
        vV=std::valarray<_T>(this->vvSpec.size());
    std::cout << " done\n";
    
    std::cout << "\t-> filling array... ";
    int iDim=this->vvSpec.size();
    for(int i=0; i<iDim; i++) {
        vvRes[0][i]=this->vvSpec[i].x;
        vvRes[1][i]=this->vvSpec[i].y;
    }
    std::cout << iDim << " values done.\n";    
    return vvRes;
}

template<typename _T> 
const std::vector<std::valarray<std::valarray<_T> > > _io<_T>::get_valarrays()  {
    
    std::cout << "- get_valarrays(): start converting vectors to valarrays.\n";
    
    std::cout << "\t-> initializing arrays...";
    std::vector<std::valarray<std::valarray<_T> > > vvvRes;
    
    int iNbSpec=this->vvvSpec.size();
    for(int i=0; i<iNbSpec;i++) 
        vvvRes.emplace_back(std::valarray<std::valarray<_T> >(2));
    
    for(auto &vvV: vvvRes) { // heap
        int i=0;
        for(auto &vV: vvV) {// spectrum: vV[0], vV[1]... to init
            vV=std::valarray<_T>(this->vvvSpec[i].size());
            i++;
        }
    }
    std::cout << " done\n";
    
    std::cout << "\t-> filling arrays... ";
    for(int n=0; n<iNbSpec;n++) {
        int iDim=this->vvvSpec[n].size();
        for(int i=0; i<iDim; i++) {
            vvvRes[n][0][i]=this->vvvSpec[n][i].x;
            vvvRes[n][1][i]=this->vvvSpec[n][i].y;
        }
    }
    std::cout << iNbSpec << " arrays done.\n";  
    return vvvRes;     
}

template<typename _T> 
bool _io<_T>::read_fits(std::string sFilename) {
    
    std::valarray<double> vWave;
    std::valarray<double> vFlux;
    
    CCfits::FITS::setVerboseMode(true);
    
    std::cout << "- read_fits(): start reading fits: " << sFilename << ".\n";
    
    // -- Flush previous data
    std::cout << "\t-> flushing previous data...";
    this->vvSpec.clear();
    this->vvvSpec.clear();
    std::cout << " done\n";
    // -- 
    
    std::cout << "\t-> reading fits...";
    CCfits::FITS FIn(sFilename,CCfits::Read,true);
    CCfits::ExtHDU& table = FIn.extension(1);
    table.column("WAVE").read(vWave,1);
    table.column("FLUX_REDUCED").read(vFlux,1);
    std::cout << " done\n";
    
    if (vWave.size()>0 && vWave.size()==vFlux.size()) {
        std::cout << "\t-> filling vector... ";
        for(int i=0; i<vWave.size(); i++)
            this->vvSpec.emplace_back(vec{vWave[i], vFlux[1]});
        std::cout << vWave.size() << " rows done\n";
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
bool _io<_T>::read_fits_dir(std::string sExtension) {
    
    std::cout << "- read__fits_dir(): start reading fits in the directory.\n";
    
    // -- Flush previous data
    std::cout << "\t-> flushing previous data...";
    this->vvSpec.clear();
    this->vvvSpec.clear();
    std::cout << " done\n";
    // -- 
    
    // -- List of files
    std::cout << "\t-> creating the sorted list of files... ";
    for(const auto &file: fs::directory_iterator("./")) {
        std::string sFName(file.path());
        if (sFName.find(sExtension)!=std::string::npos) 
            this->vsFileList.emplace_back(sFName);
    }
    std::sort(this->vsFileList.begin(),this->vsFileList.end());
    std::cout << " done\n";
    // -- 
    
    // --
    for(const auto sFName: this->vsFileList) {
        std::vector<vec> vvSpecTmp;
        
        std::valarray<double> vWave;
        std::valarray<double> vFlux;
        
        std::cout << "\t-> reading fits " << sFName << "...";
        
        CCfits::FITS::setVerboseMode(true);
        CCfits::FITS FIn(sFName,CCfits::Read,true);
        
        CCfits::ExtHDU& table = FIn.extension(1);
        table.column("WAVE").read(vWave,1);
        table.column("FLUX_REDUCED").read(vFlux,1);
        std::cout << " done\n";
        
        if (vWave.size()>0 && vWave.size()==vFlux.size()) {
            std::cout << "\t-> filling vector... ";
            for(int i=0; i<vWave.size(); i++)
                vvSpecTmp.emplace_back(vec{vWave[i], vFlux[1]});
            std::cout << vWave.size() << " rows done\n";
        }
        else {
            std::cerr << "/!\\ error while reading fits.\n";
            return false;
        }
        
        this->vvvSpec.emplace_back(vvSpecTmp);
        std::cout << "\t-> spectrum stacked.\n";
        FIn.destroy();
    }
    
    std::cout << "\t- read_fits_dir(): all done.\n\n";
    
    return true;
}
