#/bin/bash

./marker  --xmin $1 --xmax $2  \
--shift $3 \
-i HD87240_p1.synspec \
-i HD87240_p1.obs.norm \
-i HD87240_p1.sub.obs.norm  \
-l "HD87240" -l "HD87240 sub." \
-s ' ' -s '\t' -s '\t' \ -t " " \
--xlabel "$\\\\lambda$" --xunit "$\\\\AA$" \
--ylabel "Normalized flux" \
-o "plot_"$1"-"$2".pdf" \
--width 0.4 \
--elemlist line.list
