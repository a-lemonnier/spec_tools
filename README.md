# spec_tools [![Build Status](https://travis-ci.com/a-lemonnier/spec_tools.svg?token=SBy1idJoqQDAzQdHoDpQ&branch=master)](https://travis-ci.com/a-lemonnier/spec_tools)


A set of codes for csv and for spectrum manipulation:

 - **threshold**.cpp: remove lines below a wavelength threshold
 - **trim**.cpp: cut spectrum between two wavelengths
 - **findncopy**.cpp: find and copy files from a file list
 - **der_snr**.cpp: compute the Signal-to-Noise of a spectrum

 TODO:
 - Merge partfiles of results
 - Additionnal parameter and file verifications
 - Implement boost:: recursive directory copy
 - Clean preprocessor intructions
 - Add C++11 and C++14 compatibility


---
Codes work on Gentoo x64 and Ubuntu x64: bionic and eoan.

Dependencies:
- g++: C++ >17
- Boost >1.40
- Boost::program_options
- Boost::spirit
- Boost::tokenizer

> Compiling:
```
gentoo - spec_tools/build % cmake .
-- The C compiler identification is GNU 9.2.1
-- The CXX compiler identification is GNU 9.2.1
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Boost version: 1.67.0
-- Found the following Boost libraries:
--   filesystem
--   program_options
--   timer
--   system
--   chrono
-- Configuring done
-- Generating done
-- Build files have been written to: /home/gentoo/Prog/cpp/spec_tools

gentoo - spec_tools/build % make
Scanning dependencies of target findncopy
[ 12%] Building CXX object CMakeFiles/findncopy.dir/findncopy.cpp.o
[ 25%] Linking CXX executable findncopy
[ 25%] Built target findncopy
Scanning dependencies of target threshold
[ 37%] Building CXX object CMakeFiles/threshold.dir/threshold.cpp.o
[ 50%] Linking CXX executable threshold
[ 50%] Built target threshold
Scanning dependencies of target trim
[ 62%] Building CXX object CMakeFiles/trim.dir/trim.cpp.o
[ 75%] Linking CXX executable trim
[ 75%] Built target trim
Scanning dependencies of target der_snr
[ 87%] Building CXX object CMakeFiles/der_snr.dir/der_snr.cpp.o
[100%] Linking CXX executable der_snr
[100%] Built target der_snr

```

---
> findncopy
```
gentoo - spec_tools/build % ./findncopy -h
Usage:
  -h [ --help ]                     Display this help message
  -n [ --namelist ] arg             Filename of the list of name to find
  -i [ --i_folder ] arg             Name of the folder where original data are
  -o [ --o_folder ] arg (=data_out) Name of the folder where files will be 
                                    copied
  -e [ --exclude ] arg              Exclude files matching the string

Example:
# ls
data findncopy star_list.txt
# cat tar_list.txt
HD87205
HD87309
HD304836
CPD-591662
...
# ./findncopy -n star_list.txt -i data -o spectra -e sky
▶ findncopy
⚐ parsing file: 36 lines
⚐ searching file: 240 files found
⚐ erasing string: 'data'
⚐ creating directory list: done
⚐ creating folders: done
⚐ copying files: 174 files copied
⚐ findncopy:  0.176172s wall, 0.130000s user + 0.040000s system = 0.170000s CPU (96.5%)
```

---
> trim
```
gentoo - spec_tools/build % ./trim -h
Usage:
  -h [ --help ]                         Display this help message
  -l [ --min ] arg                      Minimum wavelength
  -u [ --max ] arg                      Maximum wavelength
  -i [ --input_folder ] arg             Name of the folder where original data 
                                        are
  -o [ --output_folder ] arg (=data_out)
                                        Set the directory where store new data.

Example:
./trim -l 4700 -u 4800 -i data -o spectra_trimmed
▶ trim
⚐ trim: starting 8 async threads
⚐ trim(2416052): 521 files parsed.
⚐ trim(2416051): 521 files parsed.
⚐ trim(2416054): 521 files parsed.
⚐ trim(2416053): 521 files parsed.
⚐ trim(2416055): 521 files parsed.
⚐ trim(2416050): 521 files parsed.
⚐ trim(2416056): 521 files parsed.
⚐ trim(2416057): 524 files parsed.
⚐ trim:  52.909230s wall, 296.410000s user + 0.710000s system = 297.120000s CPU (561.6%)
```

---
> threshold
```
gentoo - spec_tools/build % ./threshold -h

Usage:
  -h [ --help ]                         Display this help message
  -i [ --input_folder ] arg             Set the input directory.
  -o [ --output_folder ] arg (=data_out)
                                        Set the directory where set the 
                                        threshold.
  -t [ --threshold ] arg                Apply a threshold in all 2D spectrum 
                                        data.
                                        f<=threshold will be deleted.

Example:
./threshold -i data -o spectra -t 0
▶ threshold
⚐ threshold: starting 8 async threads
⚐ trim(2470785): 521 files parsed.
⚐ trim(2470786): 521 files parsed.
⚐ trim(2470787): 521 files parsed.
⚐ trim(2470784): 521 files parsed.
⚐ trim(2470783): 521 files parsed.
⚐ trim(2470788): 521 files parsed.
⚐ trim(2470789): 521 files parsed.
⚐ trim(2470790): 524 files parsed.
⚐ threshold:  57.269018s wall, 331.160000s user + 1.350000s system = 332.510000s CPU (580.6%)
```
---
> der_snr
```
gentoo - spec_tools/build % ./der_snr -h

Usage:
  -h [ --help ]                     Display this help message
  -f [ --filename ] arg             Filename of the spectrum
  -d [ --directory ] arg            Directory where compute the S/N
  -o [ --output ] arg (=output.csv) Filename of results
  -s [ --separator ] arg (=     )       The column separator. Do not set this 
                                    option for \tab.
  -e [ --exclude ] arg              Exclude a string in filenames
  
Examples:
./der_snr -f CPD-591792.obs
- CPD-591792.obs: S/N = 95.68

./der_snr -d data
▶ der_snr
⚐ der_snr: starting 8 async threads
⚐ compute(806557): compute S/N for 521 files
⚐ compute(806558): compute S/N for 521 files
⚐ compute(806559): compute S/N for 521 files
⚐ compute(806560): compute S/N for 521 files
⚐ compute(806561): compute S/N for 521 files
⚐ compute(806562): compute S/N for 521 files
⚐ compute(806564): compute S/N for 524 files
⚐ compute(806563): compute S/N for 521 files
⚐ der_snr:  56.395825s wall, 323.050000s user + 0.450000s system = 323.500000s CPU (573.6%)
⚐ der_snr: output: output.csv

Ref:
F. Stoehr et al: DER_SNR: A Simple & General Spectroscopic Signal-to-Noise Measurement Algorithm
394, Astronomical Data Analysis Software and Systems (ADASS) XVII
2008ASPC..394..505S

```
