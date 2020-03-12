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

#ifndef CSV_H
#define CSV_H

/**
 * \file csv.h
 * \brief A basic class for csv manipulation
 * 
 * \author Audric Lemonnier
 * \version 0.6
 * \date 12/03/2020
 * 
 */

#include <iostream>
#include <fstream>

// #include <execution> //Parallel STL disabled

#include <cmath>

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

#define PARALLEL_EXEC /**< If c++17 and TBB available */

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
     * \brief This is the default constructor without parameters. These parameters must be set after by methods. It will rise lot of errors if something is missing.
     */
    _csv();
    
    /**
     * Constructor
     * \fn _csv(const std::string &sFilename, const char &cSep)
     * \brief This is the constructor with two parameters such as the name of the working file and the separator character as usual with csv.
     * \param sFilename string Name of the input or output file with extension
     * \param cSep char Separator char between column
     */
    explicit _csv(const std::string &sFilename, const char &cSep);
    
    /**
     * \fn _csv(const std::vector<std::vector<_T> > &vvData)
     * \brief This is the constructor fed with external data.
     * \param vvData The data 
     */
    explicit _csv(const std::vector<std::vector<_T> > &vvData);
    
    /**
     * \fn _csv(const std::vector<std::string>& vsHeader, const std::vector<std::vector<_T> > &vvData)
     * \brief This is the constructor fed with external header and data.
     * \param vsHeader The vector of column name
     * \param vvData The data 
     */
    explicit _csv(const std::vector<std::string>& vsHeader, const std::vector<std::vector<_T> > &vvData);
    
    /**
     * \fn _csv(const std::vector<std::string>& vsHeader, const std::vector<std::vector<_T> > &vvData, const char &cSep)
     * \brief This is the constructor fed with external header and data.
     * \param vsHeader The vector of column name
     * \param vvData The data 
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
     * \param iLine_stop The line number where stop the display
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
     * \fn  const int get_header_size() const
     * \brief Get size of the header.
     * \return int
     */
     const int get_header_size() const;
    
    /**
     * \fn  const int get_data_size_i() const
     * \brief Get data line size.
     * \return int
     */
     const int get_data_size_i() const;
    
    /**
     * \fn const int get_data_size_j() const
     * \brief Get data column size.
     * \return int
     */
     const int get_data_size_j() const;
    
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
     * \fn bool(_T TVal)
     * \return true if all seems OK
     */
    bool shift(_T TVal);

    /**
     * \fn bool(_T TVal, int iCol)
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

    _csv( _csv& other);
    
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

    std::string sFilename,sFilename_out; /**< Store the filename  */
    char cSeparator; /**< Store the csv separator  */          
   
    eVerbose evVerbose; /**< Verbose define verbosity */
    bool bStatus; /**< Status is used to return error status  */
    
};

#include "csv.tpp"

#endif // CSV_H
