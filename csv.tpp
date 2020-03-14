/*
 * csv: A basic class for csv manipulation
 * Copyright (C) 2020  A. Lemonnier <lemonnier.audric@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


template<typename _T> 
_csv<_T>::_csv(): 
      vvData(std::vector<std::vector<_T> >(0))
    , sFilename("out.csv")
    , cSeparator('\t')
    , evVerbose(QUIET)
    , bStatus(true)
{
    debug("initing csv with empty parameters: fill them");

}

template<typename _T> 
_csv<_T>::_csv(const std::string &sFilename, const char &cSep):
     vvData(std::vector<std::vector<_T> >(0))
    , evVerbose(QUIET)
    , bStatus(true)
{
    debug("initing csv");
    
    set_filename(sFilename);
    set_separator(cSep);

}

template<typename _T> 
_csv<_T>::_csv(const std::vector<std::vector<_T> > &vvData): 
      sFilename("out.csv")
    , cSeparator('\t')
    , evVerbose(QUIET)
    , bStatus(true)
    , vvData(std::vector<std::vector<_T> >(0))
{
    debug("initing csv with empty parameters: fill them");
    
    set_data(vvData);
}

template<typename _T> 
_csv<_T>::_csv(const std::vector<std::string>& vsHeader, 
               const std::vector<std::vector<_T> > &vvData): 
      sFilename("out.csv")
    , cSeparator('\t')
    , evVerbose(DEBUG)
    , bStatus(true)
    , vvData(std::vector<std::vector<_T> >(0))
{
    debug("initing csv with empty parameters: fill them");
    
    set_data(vvData);
    set_header(vsHeader);
}

template<typename _T> 
_csv<_T>::_csv(const std::vector<std::string>& vsHeader, 
               const std::vector<std::vector<_T> > &vvData, 
               const char &cSep): 
      sFilename("out.csv")
    , cSeparator('\t')
    , evVerbose(DEBUG)
    , bStatus(true)
    , vvData(std::vector<std::vector<_T> >(0))
{
    debug("initing csv with empty parameters: fill them");
    
    set_data(vvData);
    set_header(vsHeader);
    set_separator(cSep);    
}

template<typename _T> 
_csv<_T>::_csv( _csv& other ) {
    bStatus=true;
    debug("copying");
        if (other.sFilename.empty()) {
            error("copy(): empty sFilename");
            this->set_filename("error.csv");
            bStatus=false;
        }
        else 
            this->set_filename(other.get_filename());
        
        if (other.cSeparator=='\0') {
            error("copy(): empty cSeparator");
            this->set_separator('\t');
            bStatus=false;
        }
        else
            this->set_separator(other.get_separator());
        
        if (!other.empty())
            this->set_data(other.data);
}

template<typename _T> 
_csv<_T>::~_csv() {
    clear();
    debug("destruction");
}

template<typename _T> 
bool _csv<_T>::read() {
    
     bStatus=false;
     std::fstream sFlux_in(get_filename(), std::ios::in);
    
     if (sFlux_in) {
         
         char cSep=get_separator();
         std::string sSep(std::string(1,cSep));

         debug("file opened");
         
         clear();
         
         debug("parsing header");
         boost::char_separator<char> bcSep(sSep.c_str());
         std::string sLine;
         
         getline(sFlux_in,sLine);
         boost::tokenizer<boost::char_separator<char> > btTok0(sLine,bcSep);
         
         std::string sDigit="0123456789eE+-. \n"+sSep;
         
         int iSkip_line=1; 
         
         for(auto sS: btTok0) {
             if (sS.find_first_not_of(sDigit)!=std::string::npos) {
                 vsHeader.emplace_back(sS);
             }
             else { 
                 debug("unnamed data: continue parsing");
                 iSkip_line=-1;
                 break;                
             }
         }
         
         sLine.clear();
         sFlux_in.clear();
         sFlux_in.seekg (0, std::ios::beg);
         
         debug("parsing and checking data");
         
         // define regex to correct errors. not mandatory
         std::regex regex_test;
         if (cSep==' ') regex_test=std::regex("\t");
         if (cSep=='\t') regex_test=std::regex(" ");
         
         int iCount=0;
         while(getline(sFlux_in,sLine)) {
             
             // try to correct errors with \t or ' '
             sLine=std::regex_replace(sLine, regex_test, sSep);    
             
             boost::tokenizer<boost::char_separator<char> > btTok(sLine,bcSep);
             std::vector<_T> vLine;
             if (iCount>iSkip_line) {
                 for(auto sS: btTok) {

                     if (!is_float(sS) || sS.empty()) {
                         error("read("+get_filename()+"): '"+sS+"' at line: "+std::to_string(iCount));
                         vLine.emplace_back(std::nan(sS.c_str()));
                     }
                     else
                         vLine.emplace_back(std::stof(sS));
                 }
                 
                 vvData.emplace_back(vLine);
             }
             iCount++;
         }
         
         bStatus=true;
         
         if (!check_dim()) {
             error("read("+get_filename()+"): dimension error");
             bStatus=false;
         }
         if (!vsHeader.empty()) {
             if (vsHeader.size()!=vvData[iSkip_line].size()) {
                 error("read("+get_filename()+"): header and data line mismatch");
                 bStatus=false;
             }
         }
         
         sFlux_in.close();         
     }
     else 
         error("read(): cannot open file "+get_filename());
         
    return bStatus;
}


template<typename _T> 
bool _csv<_T>::show() const{
    bStatus=!this->vvData.empty();
    if (bStatus) {
        debug("showing data");
        std::cout << "\033[1;34m\u2022 \033[1;30m\033[0m data:\n";
        
        std::ostringstream table;
        
        // *********** header
        if (!this->vsHeader.empty()) {
            int iHeader_size=get_header_size();          
            
            for(int i=0; i<iHeader_size; i++)
               table << std::setw(25) << this->get_header()[i];
            
            table << "\n" ;
        }
        table << std::fixed;
     
        // *********** data
        int iData_size_i=get_data_size_i();
        int iData_size_j=get_data_size_j();
        for(int i=0; i<iData_size_i; i++) {
            std::string tmp;
            
            for(int j=0; j<iData_size_j; j++) 
                table << std::setw(25) << get_data()[i][j] ;
            
            table << "\n" ;
        }
        std::cout << table.str();
    }
    else
        error("show(): data are empty");
    
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::show(int iiLine_stop) const {

    bStatus=!this->vvData.empty();
    if (bStatus) {
        debug("showing data");
        std::cout << "\033[1;34m\u2022\033[0m data:\n";

        std::ostringstream table;
        
        // *********** header
        if (!this->vsHeader.empty()) {
            int iHeader_size=get_header_size(); 
            for(int i=0; i<iHeader_size; i++)
               table << std::setw(25) << get_header()[i];

            table << "\n" ;
        }
        table << std::fixed;
        
        // *********** data
        int iData_size_j=get_data_size_j();
        for(int i=0; i<iiLine_stop; i++) {
            std::string tmp;
            
            for(int j=0; j<iData_size_j; j++) {
                table << std::setw(25) << get_data()[i][j] ;
            }
            table << "\n" ;
        }
        std::cout << table.str();
    }
    else
        error("show(): data are empty");
    
    return bStatus;
}


template<typename _T> 
bool _csv<_T>::write() {
    bStatus=false;
    
    if (!this->empty()) {
        
        std::fstream f_out(get_filename_out(), std::ios::out);
        
        if (f_out.is_open()) {
            debug("writing data");
            
            char sep=get_separator();
            
            if (!vsHeader.empty()) {
                
                std::unique_ptr<std::vector<std::string> > Header(new std::vector<std::string>(vsHeader));
                
                for(auto h: *Header)
                    f_out << h << sep;
                f_out << "\n";
            }
            
            std::unique_ptr<std::vector<std::vector<_T> > >  Data(new std::vector<std::vector<_T> >(this->vvData));
            for(auto g: *Data) {
                for(auto v: g)
                    f_out << v << sep;
                f_out <<"\n";
            }
            
            f_out.close();
            bStatus=true;
        }
        else
            error("write(): cannot open "+get_filename());
    }
    else bStatus=true;
    
    return bStatus;
}

template<typename _T> 
const std::vector<_T> _csv<_T>::select_line(int line) const {
    if (line<0 || line>get_data_size_i()) {
        error("select_line(): invalid line number");
        return std::vector<_T>(0);
    }
    
    std::vector<_T> vvRes;
    
    int iData_size_j=get_data_size_j();
    for(int j=0;j<iData_size_j;j++)
        vvRes.emplace_back(vvData[line][j]);
    
    return vvRes;    
}

template<typename _T> 
const std::vector<_T> _csv<_T>::select_column(int col) const {
    if (col<0 || col>get_data_size_j()) {
        error("select_column(): invalid column number");
        return std::vector<_T>(0);
    }
    
    std::vector<_T> vvRes;
    int iData_size_i=get_data_size_i();
    
    for(int i=0;i<iData_size_i;i++)
        vvRes.emplace_back(vvData[i][col]);
    
    return vvRes;    
}



template<typename _T> 
const std::vector<std::vector<_T> > _csv<_T>::select(int iLine_min, int iLine_max, int iCol_min, int iCol_max) const {
    if (iLine_min<0 || iCol_min<0 || iLine_max>get_data_size_i() ||  iCol_max>get_data_size_j()) {
        error("select(): invalid selection");
        std::vector<std::vector<_T> >(0);
    }
    
    std::vector<std::vector<_T> >vvRes;
    
    for(int i=iLine_min;i<iLine_max;i++)
        for(int j=iCol_min;i<iCol_max;j++)
            vvRes.emplace_back(vvData[i][j]);
        
    return vvRes; 
    
}


template<typename _T> 
bool _csv<_T>::set_data(const std::vector<std::vector<_T> > &vvData) {
    bStatus=true;
    debug("setting data");
    if (this->empty()) {
        bStatus=false;
        error("set_data(): data are empty");
        return bStatus;
    }
    if (this->check_dim()) {
        bStatus=false;
        error("set_data(): data dimensions mismatch");
        return bStatus;
    }
    
    std::copy(vvData.begin(), vvData.end(), this->vvData.begin());
    
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::set_column(const std::vector<_T>& vCol, int iCol) {
    bStatus=false;
    
    if (vCol.empty()) {
        debug("set_column(): column is empty");
        return bStatus;
    }
    
    int iSize=vCol.size();
    int iSize_i=get_data_size_i();
    int iSize_j=get_data_size_j();
    
    if (iCol>iSize_j) {
        error("set_column(): column number out of range");
        return bStatus;
    }
    
    if (iSize!=iSize_i) {
        error("set_column(): column size mismatches");
        return bStatus;
    }
    else {
        for(int i=0; i<iSize; i++)
            this->vvData[i][iCol]=vCol[i];       
        
        bStatus=true;       
    }
    
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::set_row(const std::vector<_T>& vRow, int iRow) {
    bStatus=true;
    
     if (vRow.empty()) {
        debug("set_row(): row is empty");
        return bStatus;
    }
    
    int iSize=vRow.size();
    int iSize_i=get_data_size_i();
    int iSize_j=get_data_size_j();
    
    if (iRow>iSize_i+1) {
        error("set_row(): invalid row number");
        return bStatus;
    }   
    
    if (iSize!=iSize_j) {
        error("set_row(): row size mismatches");
        return bStatus;
    }
    
    if (iSize==iSize_j && iRow==iSize_i+1) {
        this->vvData.emplace_back(vRow);
        bStatus=true;       
    }
    
    if (iSize==iSize_j && iRow<iSize_i) {
        this->vvData[iRow]=vRow;       
        bStatus=true;       
    }
    
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::set_header(const std::vector<std::string> &iCol_name) {
    bStatus=false;
    debug("setting the header");
    if (!iCol_name.empty()) {
        this->vsHeader.clear();
        this->vsHeader=iCol_name;
        bStatus=true;
    }
    else
        error("set_header(): the new header is empty");
        
    return bStatus;
}


template<typename _T> 
bool _csv<_T>::set_filename(const std::string &sFilename) {
    bStatus=true;
    if (!sFilename.empty()) {
    
    debug("setting sFilename: '"+sFilename+"'");
    this->sFilename=sFilename;
    }
    else {
        error("set_filename(): empty sFilename");
        set_filename("error.csv");
        bStatus=false;
    }
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::set_filename_out(const std::string &sFilename) {
    bStatus=true;
    if (!sFilename.empty()) {
    
    debug("setting output sFilename: '"+sFilename+"'");
    this->sFilename_out=sFilename;
    }
    else {
        error("set_filename_out(): empty sFilename");
        set_filename("error.csv");
        bStatus=false;
    }
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::set_separator(const char &sep) {
    bStatus=true;
    if (sep!='\0') {
        debug("seting cSeparator: '"+std::string(1,sep)+"'");
        this->cSeparator=sep;
    }
    else {
        error("set_separator(): invalid cSeparator");
        set_separator('\t');
        bStatus=false;        
    }
    return bStatus;
}


template<typename _T> 
void _csv<_T>::set_verbose(eVerbose evV) {
    evVerbose=evV;    
}

template<typename _T>
const std::string _csv<_T>::get_filename() const {
    return this->sFilename;
}

template<typename _T>
const std::string _csv<_T>::get_filename_out() const {
    if (sFilename_out.empty()) {
        debug("empty output sFilename: using input sFilename instead");
        return get_filename();
    }
    return this->sFilename_out;
}

template<typename _T>
const char _csv<_T>::get_separator() const {
    return this->cSeparator;
}

template<typename _T>
const int _csv<_T>::get_header_size() const {
    return vsHeader.size();
}

template<typename _T>
const int _csv<_T>::get_data_size_i() const {
    return vvData.size();
}

template<typename _T>
const int _csv<_T>::get_data_size_j() const {
    return vvData[0].size();
}

template<typename _T>
const std::vector<std::vector<_T> >& _csv<_T>::get_data() const {
    if (this->empty()) 
        error("get_data(): data are empty");    
    
    return this->vvData;
}

template<typename _T>
const std::vector<std::string>& _csv<_T>::get_header() const {
    if (this->vsHeader.empty()) 
        error("get_header(): header is empty");

    return vsHeader;
}

template<typename _T>
bool _csv<_T>::empty() const {
    return this->vvData.empty() || this->vvData[0].empty();
}

template<typename _T>
bool _csv<_T>::check_dim() {
    bStatus=false;
    
    if (!this->empty()) {
        bStatus=true;
        int size=vvData.size()-2;
        for (int i=0;i<size;i++) {
            bStatus &= vvData[i].size()==vvData[i+1].size();
        }
    }
    else 
        debug("check_dim(): data is empty");
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::shift(_T TVal) {
    bStatus=false;
    
    if (this->get_data_size_i()>0) {
        debug("add "+std::to_string(TVal)+" to the first column");

        std::vector<_T> vTmp(this->select_column(0));
        
        std::transform(vTmp.begin(), vTmp.end(),
                       vTmp.begin(),
                       std::bind(std::plus<_T>(), std::placeholders::_1, TVal));
        
         set_column(vTmp,0);
        
        bStatus=true;
    }
    else 
        debug("shift(): column is empty");    
    
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::shift(_T TVal, int iCol) {
    bStatus=true;
    if (iCol==0)
        shift(TVal);
    if (iCol==1) {
        debug("add "+std::to_string(TVal)+" to the 2nd column");
        this->data+=TVal;
    }
    if (iCol>1) {
        if (iCol<get_data_size_j()) {
            error("shift(): bad column selected");
            bStatus=false;
        }
        else {
            debug("add "+std::to_string(TVal)+" to the "+std::to_string(iCol)+" column");
            
            std::vector<_T> vTmp(this->select_column(iCol));
            
            std::transform(vTmp.begin(), vTmp.end(),
                           vTmp.begin(),
                           std::bind(std::plus<_T>(), std::placeholders::_1, TVal));
            
            set_column(vTmp,iCol);
        }
    }
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::apply_max_threshold(_T TVal) {
    bStatus=true;
    
    int iPrev_size=get_data_size_i();
    
    vvData.erase(std::remove_if( vvData.begin(),vvData.end(),[&]( std::vector<_T> v){ 
        for(auto vv: v) 
            if (vv>TVal) return true; 
                              
        return false;
    }),vvData.end());
    
    debug(std::to_string(abs(get_data_size_i()-iPrev_size))+" line(s) erased");
    
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::apply_min_threshold(_T TVal) {
    bStatus=true;
    
    int iPrev_size=get_data_size_i();
    
    vvData.erase(std::remove_if( vvData.begin(), vvData.end(),[&]( std::vector<_T> v){ 
        for(auto vv: v) 
            if (vv<TVal) return true; 
                              
        return false;
    }),vvData.end());
    
    debug(std::to_string(abs(get_data_size_i()-iPrev_size))+" line(s) erased");
    
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::apply_max_threshold(_T TVal, int iCol) {
    bStatus=true;
    
    if (iCol<0 || iCol>get_data_size_j()) {
        error("apply_min_threshold(): invalid column");
        bStatus=false;
        return bStatus;
    }
    
    int iPrev_size=get_data_size_i();
    int iH_size=get_data_size_j();
    
    
    vvData.erase(std::remove_if( vvData.begin(),vvData.end(),[&]( std::vector<_T> v){ 
        for(int i=0;i<iH_size;i++) {
            if (v[i]>TVal && i==iCol)
                return true; 
        }
        
        return false;
    }),vvData.end());
    
    
    debug(std::to_string(abs(get_data_size_i()-iPrev_size))+" line(s) erased");
    
    return bStatus;
}

template<typename _T> 
bool _csv<_T>::apply_min_threshold(_T TVal, int iCol) {
    
    bStatus=true;
    if (iCol<0 || iCol>get_data_size_j()) {
        error("apply_min_threshold(): invalid column");
        bStatus=false;
        return bStatus;
    }
    
    int iPrev_size=get_data_size_i();
    int iH_size=get_data_size_j();
    
    vvData.erase(std::remove_if( vvData.begin(),vvData.end(),[&]( std::vector<_T> v){ 
        for(int i=0;i<iH_size;i++) {
            if (v[i]<TVal && i==iCol)
                return true; 
        }
        
        return false;
    }),vvData.end());
    
    debug(std::to_string(abs(get_data_size_i()-iPrev_size))+" line(s) erased");
    
    return bStatus;
}

template<typename _T> 
void _csv<_T>::zeroize() {
    debug("zeroizing data");
    std::fill(vvData.begin(), vvData.end(), 0);
}


template<typename _T> 
void _csv<_T>::clear() {
    debug("deleting header and data");
    vsHeader.clear();
    vvData.clear();
}

template<typename _T> 
_csv<_T>& _csv<_T>::operator=(const _csv<_T>& other) const {
        bStatus=true;
   
        debug("copying");
        if (other.sFilename.empty()) {
            error("copy: empty sFilename");
            this->set_filename("error.csv");
            bStatus=false;
        }
        else 
            this->set_filename(other.get_filename());
        
        if (other.cSeparator=='\0') {
            error("copy: empty cSeparator");
            this->set_separator('\t');
            bStatus=false;
        }
        else
            this->set_separator(other.get_separator());
        
        if (!other.empty())
            this->set_data(other.data);
}

template<typename _T> 
bool _csv<_T>::operator==(const _csv& other) const {
    bool vvRes=true;
    
    vvRes &= this->get_data() == other.get_data();
    vvRes &= this->get_header() == other.get_header();

    vvRes &= this->get_separator()   == other.get_separator();
    
    vvRes &= this->get_filename() == other.get_filename();
    vvRes &= this->get_filename_out() == other.get_filename_out();
    
    return vvRes;
}

template<typename _T> 
bool _csv<_T>::operator!=(const _csv& other) const {
    return !(this==other);
}

template<typename _T> 
void _csv<_T>::debug(const std::string &msg) const { 
    if (evVerbose==DEBUG)
        std::cout << "\033[3;32m\u2690\033[0m \033[1;30m_csv<>()\033[0m: " << msg << ".\n"; // 
}

template<typename _T> 
void _csv<_T>::error(const std::string &msg) const { 
    std::cerr << "\033[5;31m\u2639\033[0m \033[1;30m_csv<>()\033[0m: " << msg << ".\n";
}

template<typename _T> 
bool _csv<_T>::is_float(const std::string &val) const {
   std::string::const_iterator first(val.begin()), last(val.end());
   return boost::spirit::qi::parse(first, last, boost::spirit::double_) && first == last;
}

template<typename _T>
_csv<_T>& _csv<_T>::operator+(const _csv& other) const {
    if(other.get_data_size_j()<1 || other.get_data_size_j()>2) {
        error("operator+(): implementation allows only operation on the 2nd column which must exist");
        return this;
    }
    else if (this->get_data_size_i()!=other.get_data_size_i()) {
        error("operator+(): column size mismatch");
        return this;   
    }
    else {
        std::vector<_T> vvRes;
        vvRes.vvReserve(this->get_data_size_i());
        
        std::transform(this->get_sdata()[1].begin(),this->get_data()[1].end(),
                       other.get_data()[1].begin(),other.get_data()[1].end(),
                       std::back_inserter(vvRes), std::plus<_T>());
        return vvRes;
    }
}

template<typename _T>
_csv<_T>& _csv<_T>::operator+(const _T &other) const {
    if(this->get_data_size_j()<1 || this->get_data_size_j()>2) {
        error("operator+(): implementation allows only operation on the 2nd column which must exist");
        return this;
    }
    
    std::vector<_T> vvRes;
    vvRes.vvReserve(other.iData_size_i());
    
    std::for_each(this->get_data()[1].begin(), this->get_data()[1].end(),
                  [&](std::vector<_T> &v) { 
                      v+=other;
                  });
    return vvRes;   
}

template<typename _T>
_csv<_T>& _csv<_T>::operator-(const _csv& other) const {
        if(other.get_data_size_j()<1 || other.get_data_size_j()>2) {
        error("operator-(): implementation allows only operation on the 2nd column which must exist");
        return this;
    }
    else if (this->get_data_size_i()!=other.get_data_size_i()) {
        error("operator-(): column size mismatch");
        return this;   
    }
    else {
        
        std::vector<_T> vvRes;
        vvRes.vvReserve(this->get_data_size_i());
        
        std::transform(this->get_data()[1].begin(),this->get_data()[1].end(),
                       other.get_data()[1].begin(),other.get_data()[1].end(),
                       std::back_inserter(vvRes), std::minus<_T>());
        return vvRes;
    }
}

template<typename _T>
_csv<_T>& _csv<_T>::operator-(const _T &other) const {
    if(this->get_data_size_j()<1 || this->get_data_size_j()>2) {
        error("operator-(): implementation allows only operation on the 2nd column which must exist");
        return this;
    }
    
    std::vector<_T> vvRes;
    vvRes.vvReserve(other.iData_size_i());
    
    std::for_each(this->get_data()[1].begin(), this->get_data()[1].end(),
                  [&](std::vector<_T> &v) { 
                      v-=other;
                  });
    return vvRes; 
}

template<typename _T>
_csv<_T>& _csv<_T>::operator*(const _csv& other) const {
    if(other.get_data_size_j()<1 || other.get_data_size_j()>2) {
        error("operator*(): implementation allows only operation on the 2nd column which must exist");
        return this;
    }
    else if (this->get_data_size_i()!=other.get_data_size_i()) {
        error("operator*(): column size mismatch");
        return this;   
    }
    else {
        
        std::vector<_T> vvRes;
        vvRes.vvReserve(this->get_data_size_i());
        
        std::transform(this->get_data()[1].begin(),this->get_data()[1].end(),
                       other.get_data()[1].begin(),other.get_data()[1].end(),
                       std::back_inserter(vvRes), std::multiplies<_T>());
        
        return vvRes;
    }
}

template<typename _T>
_csv<_T>& _csv<_T>::operator*(const _T &other) const {
    if(this->get_data_size_j()<1 || this->get_data_size_j()>2) {
        error("operator*(): implementation allows only operation on the 2nd column which must exist");
        return this;
    }
    
    std::vector<_T> vvRes;
    vvRes.vvReserve(other.iData_size_i());
    
    std::for_each(this->get_data()[1].begin(), this->get_data()[1].end(),
                  [&](std::vector<_T> &v) { 
                      v*=other;
                  });
    return vvRes;  
}

template<typename _T>    
_csv<_T>& _csv<_T>::operator/(const _csv& other) const {
        if(other.get_data_size_j()<1 || other.get_data_size_j()>2) {
        error("operator/(): implementation allows only operation on the 2nd column which must exist");
        return this;
    }
    else if (this->get_data_size_i()!=other.get_data_size_i()) {
        error("operator/(): column size mismatch");
        return this;   
    }
    else {
        
        std::vector<_T> vvRes;
        vvRes.vvReserve(this->get_data_size_i());
        
        std::transform(this->get_data()[1].begin(),this->get_data()[1].end(),
                       other.get_data()[1].begin(),other.get_data()[1].end(),
                       std::back_inserter(vvRes), std::divides<_T>());
        
        return vvRes;
    }

    
}


template<typename _T>    
_csv<_T>& _csv<_T>::operator/(const _T &other) const {
    if (other==0) {
        error("operator/(): division by 0");
        return this;
    }
    
    std::vector<_T> vvRes;
    vvRes.vvReserve(other.iData_size_i());
    
    std::for_each(this->get_data()[1].begin(), this->get_data()[1].end(),
                  [&](std::vector<_T> &v) { 
                      v/=other;
                  });
    return vvRes;  
}

