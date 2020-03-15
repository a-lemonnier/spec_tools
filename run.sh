#!/bin/bash

./genrandspec
./findncopy -i rand_spectra -o selected -n star_list.txt -e sky
./threshold -i selected -o spectra -t 1
./trim -l 4500 -u 5500 -i rand_spectra -o spectra_trimmed
./der_snr -d rand_spectra -o all_snr.csv
./der_snr -d spectra -o selected_snr.csv
./shift -w 0.1 -i rand_spectra

