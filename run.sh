#!/bin/bash

./findncopy -i data -o selected -n star_list.txt -e sky

./threshold -i selected -o spectra -t 1

./trim -l 4700 -u 4800 -i data -o spectra_trimmed

./der_snr -d data -o all_snr.csv
./der_snr -d spectra -o selected_snr.csv

cat selected_snr.csv

