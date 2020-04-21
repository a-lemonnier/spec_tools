/**
 * \file csv.h
 * \brief A basic class for csv manipulation
 * 
 * \author Audric Lemonnier
 * \version 0.9
 * \date 07/04/2020
 * 
 */

#ifndef CSV_H
#define CSV_H

#include <iostream>
#include <fstream>

#include <chrono>

#include <cmath>
#include <numeric>
#include <random>

#include <cstdarg>

#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>

#include <string>
#include <iomanip>
#include <regex>

#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_numeric.hpp>

#include <boost/tokenizer.hpp>

/**
 * \class _csv
 * \brief This is the templated _csv class, initialized with double by default. STL parallel execution policy does not provide enhancements for simple operations.
 * 
 */
template<typename _T=double> 
class _csv {
public:
    
    /**
     * \enum eVerbose
     * \brief Define verbosity values
     */
    typedef enum { QUIET, DEBUG } eVerbose;  

    /**
     * Default constructor
     * \fn _csv()
     * \brief Default constructor without parameters. These parameters must be set after by methods. It will rise lot of errors if something is missing.
     */
    _csv();
    
    /**
     * Constructor
     * \fn explicit _csv(const std::string &sFilename, const char &cSep)
     * \brief Constructor with two parameters such as the name of the working file and the separator character as usual with csv.
     * \param sFilename string Name of the input or output file with extension
     * \param cSep char Separator char between column
     */
    explicit _csv(const std::string &sFilename, const char &cSep);
    
    /**
     * \fn explicit _csv(const std::string &sFilename, const std::string &sSep)
     * \param sFilename string Name of the input or output file with extension
     * \param sSep string Separator char between column
     */
    explicit _csv(const std::string &sFilename, const std::string &sSep);
    
    /**
     * \fn explicit _csv(const std::vector<std::vector<_T> > &vvData)
     * \brief Constructor fed with external data.
     * \param vvData the data 
     */
    explicit _csv(const std::vector<std::vector<_T> > &vvData);
    
    /**
     * \fn explicit _csv(const std::vector<std::string>& vsHeader, const std::vector<std::vector<_T> > &vvData)
     * \brief Constructor fed with external header and data.
     * \param vsHeader The vector of column name
     * \param vvData the data 
     */
    explicit _csv(const std::vector<std::string>& vsHeader, const std::vector<std::vector<_T> > &vvData);
    
    /**
     * \fn explicit _csv(const std::vector<std::string>& vsHeader, const std::vector<std::vector<_T> > &vvData, const char &cSep)
     * \brief Constructor fed with external header and data.
     * \param vsHeader the vector of column name
     * \param vvData the data 
     * \param cSep char Separator char between column
     */
    explicit _csv(const std::vector<std::string>& vsHeader, const std::vector<std::vector<_T> > &vvData, const char &cSep);
        
    virtual ~_csv();
    
    /**
     * \fn bool read();
     * \brief Read the content of the file given to the constructor using boost. It detects the header and data consistency with digit sequence: {0123456789eE+-. tab std::endl} and basic regex and dimension matching between header and data line. It is able to recover basic errors such as 'tab'==' '. The method put NaN in the grid if an unrecoverable error appends. Data will be store in private variables.
     * \return true if all seems OK
     */   
    bool read();
    
    /**
     * \fn void show() const
     * \brief Show whole data, i.e. the header and data with no restriction on length or terminal size. It uses boost::format in order to correct spacing of number and strings.
     * \return true if all seems OK
     */    
    bool show() const;
    
    /**
     * \fn bool show(int iLine_stop) const
     * \brief Show the header and data until "line_stop" line. Print all columns with terminal end-of-line. It uses boost::format in order to correct spacing of number and strings.
     * \param iLine_stop The number of lines where stop the display
     * \return true if all seems OK
     */
    bool show(int iLine_stop) const;
    
    /**
     * \fn bool write()
     * \brief Write on disk what data are store
     * \return true if all seems OK
     */
    bool write();
    
    /**
     * \fn const std::vector<_T>& select_line(int iLine) const
     * \brief Select the line "line" in data.
     * \param iLine The line to select
     * \return std::vector<_T>
     */
    const std::vector<_T> select_line(int line) const;
    
    /**
     * \fn const std::vector<_T>& select_column(int iCol) const
     * \brief Select the column "col" in data.
     * \param iCol The column to select
     * \return std::vector<_T>
     */
    const std::vector<_T> select_column(int iCol) const;
    
    /**
     * \fn const std::vector<std::vector<_T> >& select(int iLine_min, int iLine_max, int iCol_min, int iCol_max) const
     * \brief Select a sub grid in data, i.e. trim data to the rectangular \f$[i_{min}, i_{max}]\times [j_{min}, j_{max}]\f$
     * \param iLine_min upper line \f$i_{min}\f$
     * \param iLine_max lower line \f$i_{max}\f$
     * \param iCol_min left column \f$j_{min}\f$
     * \param iCol_max right column \f$j_{max}\f$
     * \return std::vector<std::vector<_T> >
     */
    const std::vector<std::vector<_T> > select(int iLine_min, int iLine_max, int iCol_min, int iCol_max) const;
    
    /**
     * \fn void set_data(const std::vector<std::vector<_T> > &vvData)
     * \brief Set data with a vector of a vector.
     * \param vvData std::vector<std::vector<_T> > grid
     * \return true if all seems OK
     */
    bool set_data(const std::vector<std::vector<_T> > &vvData);
    
    /**
     * \fn bool set_column(const std::vector<_T>& vCol, int iCol)
     * \brief Set a column with a vector.
     * \param vCol std::vector<_T> vCol
     * \param iCol Select a column 
     * \return true if all seems OK
     */
    bool set_column(const std::vector<_T>& vCol, int iCol);
    
    /**
     * \fn bool set_column(const std::vector<_T>& vRow, int iRow)
     * \brief Set a row with a vector.
     * \param vRow std::vector<_T> vRow
     * \param iRow Select a row 
     * \return true if all seems OK
     */
    bool set_row(const std::vector<_T>& vRow, int iRow);
    
    /**
     * \fn bool set_header(const std::vector<std::string> &vsHeader)
     * \brief Set the header: the first line containing column name
     * \param vsHeader string vector
     * \return true if all seems OK
     */
    bool set_header(const std::vector<std::string> &vsHeader);
    
    /**
     * \fn bool set_filename(const std::string &sFilename)
     * \brief Set the filename for output or input. The fstream do not care about extension...
     * \param sFilename The filename with extension or not.
     * \return true if all seems OK
     */
    bool set_filename(const std::string &sFilename);
    
    
    /**
     * \fn bool set_filename_out(const std::string &sFilename)
     * \brief Set the filename for output. The fstream do not care about extension...
     * \param sFilename The filename with extension or not.
     * \return true if all seems OK
     */
    bool set_filename_out(const std::string &sFilename);
    
    /**
     * \fn bool set_separator(const char &cSep)
     * \brief Set the csv separator. Usually: '\\t', ' ', ',', ';' ... 
     * \param cSep The sep character: '\\t' for tabulation
     * \return true if all seems OK
     */
    bool set_separator(const char &cSep);
    
    /**
     * \fn bool set_separator(const std::string &sSep)
     * \brief Set the csv separator. Usually: '\\t', ' ', ',', ';' ... 
     * \todo
     * \param sSep The sep character: '\\t' for tabulation
     * \return true if all seems OK
     */
    bool set_separator(const std::string &sSep);
    
    /**
     * \fn bool set_verbose(eVerbose evV)
     * \brief Set the verbose mode for debug. It does not deactivate error raising.
     * \param evV eVerbose::DEBUG for verbose mode and eVerbose::QUIET to keep quiet
     */
    void set_verbose(eVerbose evV);
    
    /**
     * \fn const std::string get_filename() const
     * \brief Get the filename.
     * \return std::string
     */
     const std::string get_filename() const;
    
    /**
     * \fn const std::string get_filename_out() const
     * \brief Get the output filename.
     * \return std::string
     */
     const std::string get_filename_out() const;
    
    
    /**
     * \fn  const char get_separator() const
     * \brief Get the separator.
     * \return char
     */
     const char get_separator() const;
    
    /**
     * \fn  const size_t get_header_size() const
     * \brief Get size of the header.
     * \return size_t
     */
     const size_t get_header_size() const;
    
    /**
     * \fn  const size_t get_data_size_i() const
     * \brief Get data line size.
     * \return size_t
     */
     const size_t get_data_size_i() const;
    
    /**
     * \fn const size_t get_data_size_j() const
     * \brief Get data column size.
     * \return size_t
     */
     const size_t get_data_size_j() const;
    
    /**
     * \fn const std::vector<std::vector<_T> >& get_data() const
     * \brief Get data and return it as a vector of vector.
     * \return std::vector<std::vector<_T> >
     */
    const std::vector<std::vector<_T> >& get_data() const;
    
    /**
     * \fn const std::vector<_T>& get_header() const
     * \brief Get column names and return it in a vector.
     * \return std::vector<_T>
     */
    const std::vector<std::string>& get_header() const;
    
    /**
     * \fn bool empty() const
     * \brief Check if data are empty, and the emptiness of the first line, i.e. this->data[0].
     * \return true if data are empty
     */
    bool empty() const;
    
    /**
     * \fn bool check_dim()
     * \brief Check data dimension consistency, i.e. if all line dimensions are all equal.
     * \return true if dimensions seem OK
     */
    bool check_dim();
    
    /**
     * \fn [[deprecated("will be moved")]] bool genrandspec(_T TMin, _T TMax, _T TStep)
     * \brief Generate a normalized random spectrum with random gaussian absoption profiles. For TStep < 0.05, you may want to initialize with double.
     */
    [[deprecated("will be moved")]] bool genrandspec(_T TMin, _T TMax, _T TStep);
    
    /**
     * \fn bool transform_lin(_T TA, _T TB, int iCol)
     * \brief Do Y=aX+b to the iCol-column
     * \return true if all seems OK
     */    
    bool transform_lin(_T TA, _T TB, int iCol);
    
    /**
     * \fn bool(_T TVal)
     * \brief Shift first column 
     * \return true if all seems OK
     */
    bool shift(_T TVal);

    /**
     * \fn bool(_T TVal, int iCol)
     * \brief Shift the column iCol by TVal 
     * \return true if all seems OK
     */
    bool shift(_T TVal, int iCol);

    /**
     * \fn bool apply_max_threshold(_T TVal)
     * \brief Delete \f$i\f$ line from the grid where \f$\mathbf{data}[i][j]>val\f$.
     * \param TVal The max threshold
     * \return true if all seems OK
     */
    bool apply_max_threshold(_T TVal);
    
    /**
     * \fn bool apply_min_threshold(_T TVal)
     * \brief Delete \f$i\f$ line from the grid where \f$\mathbf{data}[i][j]<val\f$.
     * \param TVal The min threshold
     * \return true if all seems OK
     */
    bool apply_min_threshold(_T TVal);
    
    /**
     * \fn bool apply_max_threshold(_T TVal, int iCol)
     * \brief Delete \f$i\f$ line from the grid where \f$\mathbf{data}[i][j\neq list]>val\f$.
     * \param TVal The max threshold
     * \param iCol Select a column
     * \return true if all seems OK
     */
    bool apply_max_threshold(_T TVal, int iCol);
    
    /**
     * \fn bool apply_min_threshold(_T TVal, int iCol)
     * \brief Delete \f$i\f$ line from the grid where \f$\mathbf{data}[i][j\neq list]<val\f$.
     * \param TVal The min threshold
     * \param iCol Select a column
     * \return true if all seems OK
     */
    bool apply_min_threshold(_T TVal,int iCol);
    
    /**
     * \fn void zeroize()
     * \brief Set to zero data. One should find this useful...
     */
    void zeroize();

    /**
     * \fn void clear()
     * \brief Delete data and header.
     */
    void clear();
    
    _csv& operator=(const _csv& other) const; 

    bool operator==(const _csv& other) const;
    bool operator!=(const _csv& other) const;
    
    /**
     * \fn _csv& operator+(const _csv& other) const
     * \brief Sum with the 2nd column
     */
    _csv& operator+(const _csv& other) const;
        /**
     * \fn _csv& operator+(const _T &other) const
     * \brief Add a constant to the 2nd column
     */
    _csv& operator+(const _T &other) const;
    
    /**
     * \fn _csv& operator-(const _csv& other) const
     * \brief Sum with the 2nd column
     */
    _csv& operator-(const _csv& other) const;
    
    /** \fn _csv& operator-(const _T &other) const
     *  \brief Substract a constant to the 2nd column
     */
    _csv& operator-(const _T &other) const;
    
    /**
     * \fn _csv& operator*(const _csv& other) const
     * \brief Inner product with the 2nd column
     */
    _csv& operator*(const _csv& other) const;
    
    /** \fn _csv& operator*(const _T &other) const
     *  \brief Multiply by a constant the 2nd column
     */
    _csv& operator*(const _T &other) const;
    
    /**
     * \fn _csv& operator/(const _csv& other) const
     * \brief Divide element by element the two columns
     */
    _csv& operator/(const _csv& other) const;
    
    /** \fn _csv& operator/(const _T &other) const
     *  \brief Divide by a non zero constant the 2nd column
     */
    _csv& operator/(const _T &other) const;
    
private:
    /**
     * \fn void debug(const std::string &sMsg) const
     * \brief Send debug message to the standard output.
     * \param sMsg std::string
     * \todo Variadic arguments but seem not crucial atm.
     */
    void debug(const std::string &sMsg) const;
    
    /**
     * \fn void error(const std::string &sMsg) const
     * \brief Send error message to std err output
     * \param sMsg std::string
     * \todo Variadic arguments but seem not crucial.
     */
    void error(const std::string &sMsg) const;
    
    /**
     * \fn bool is_float(const std::string &val) const
     * \brief Check if a string is a float and does not contain non digit characters. It uses boost::qi.
     * \param TVal The string  
     * \return true if float
     */
    bool is_float(const std::string &sVal) const;
    
    std::vector<std::vector<_T> > vvData; /**< Data contains the 2D-vector and is private */     
    std::vector<std::string> vsHeader; /**<  vsHeader is a vector of column std::string name */

    std::vector<std::tuple<std::string, char> > vcSeplist; /**<  vcSeplist is a vector of allowed seperators */
    
    std::string sFilename,sFilename_out; /**< Store the filename  */
    char cSeparator; /**< Store the csv separator  */          
   
    eVerbose evVerbose; /**< Verbose define verbosity */
    bool bStatus; /**< Status is used to return error status  */
    
    /**
     * \fn _T Gaussian(_T x, _T x0, _T sigma) const
     * \brief Gaussian function used for spectra generation
     */
    _T Gaussian(_T x, _T x0, _T sigma) const;

    
};

#include "csv.tpp"

#endif // CSV_H
