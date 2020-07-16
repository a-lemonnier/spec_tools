#include <iostream>
#include <filesystem>
#include <fstream>

#include <cmath>

#include <string>
#include <sstream>
#include <iomanip>

#include <algorithm> 
#include <vector>
#include <valarray>
#include <tuple>

#include <waverage.hpp>



int main(int argc, char **argv) {
    std::cout << "******************************\n";
    std::cout << "*  Weighted arithmetic mean  *\n";
    std::cout << "******************************\n\n";
    
    _io<double> io;

    io.read_fits_dir(".fits");
    
    

    return EXIT_SUCCESS;
}

