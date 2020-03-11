#!/bin/bash

./findncopy -i data -o selected -n star_list.txt -e sky

./threshold -i selected -o spectra -t 1

./der_snr -d data -o all_snr.csv
./der_snr -d spectra -o selected_snr.csv


