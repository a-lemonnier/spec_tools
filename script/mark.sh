#/bin/bash

CFOLDER=$(pwd)

XMIN=$1
XMAX=$2
SHIFT=$3

SYNSPEC=$4
SPEC1=$5
SPEC2=$6

NAME1=$7
NAME2=$8

$CFOLDER/bin/./marker  --xmin $1 --xmax $2  \
-i "$4" \
-i "$5" \
-i "$6"  \
-s ' ' -s '\t' -s '\t' \ -t " " \
-l "$7" -l "$8" --width 0.8 \
--xlabel "$\\\\lambda$" --xunit "$\\\\AA$" \
--ylabel "Normalized flux" \
-o "plot_"$1"-"$2".pdf" \
--elemlist $CFOLDER/line.list \
--fontsize 7 \
--shift $3 \
$9 \
${10} \
${11}

echo " " >> $CFOLDER/hist.txt
echo "$CFOLDER/bin/./marker  --xmin $1 --xmax $2  \
-i "$4" \
-i "$5" \
-i "$6"  \
-s ' ' -s '\t' -s '\t' \ -t " " \
-l "$7" -l "$8" --width 0.8 \
--xlabel "$\\\\lambda$" --xunit "$\\\\AA$" \
--ylabel "Normalized flux" \
-o "plot_"$1"-"$2".pdf" \
--elemlist $CFOLDER/line.list \
--fontsize 7 \
--shift $3 \
$9 \
${10} \
${11}" >> hist.txt

echo "#!/bin/bash" > .tmp.txt
awk '!seen[$0]++' hist.txt >> .tmp.txt
mv .tmp.txt hist.txt

mv hist.txt hist.sh
chmod +x hist.sh
