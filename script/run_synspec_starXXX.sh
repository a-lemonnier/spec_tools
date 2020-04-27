#!/bin/bash
EXEC=synspec53r
NAME=HD87240
PART=p1
MODEL=HD87240_t13000g4v2.dat
DIR=stars
LINELIST=R3900_6800_08Jan2020.dat

rm fort.* nst r.dat > /dev/null 2>&1

ln -s $DIR/$NAME/$MODEL fort.8
ln -s line_list/$LINELIST fort.19

ln -s $DIR/$NAME/"unit.1" fort.1
ln -s $DIR/$NAME/"unit.5" fort.5
ln -s $DIR/$NAME/"unit_"$PART".55" fort.55
ln -s $DIR/$NAME/"unit.56" fort.56

ln -s $DIR/$NAME/$NAME".param" nst

ln -s $DIR/$NAME/$NAME"_"$PART".rotin" r.dat

./$EXEC<fort.5> fort.6

./rotin3 < r.dat

mv fort.6 $DIR/$NAME/$NAME"_"$PART".log"
mv fort.7 $DIR/$NAME/$NAME"_"$PART"_not_convolved.synspec"
mv fort.11 $DIR/$NAME/$NAME"_"$PART".synspec"
mv fort.12 $DIR/$NAME/$NAME"_"$PART".line_identif"

rm fort.* nst r.dat > /dev/null 2>&1

SUBCMD0="plot [$1:$2] [0.5:1.05]; "
SUBCMD1="@+ <'"$DIR/$NAME/$NAME"_"$PART".obs.norm';"
SUBCMD2="@+ <'"$DIR/$NAME/$NAME"_"$PART".sub.obs.norm';"
SUBCMD3="@+ <'"$DIR/$NAME/$NAME"_"$PART".synspec';"

CMD=$SUBCMD1$SUBCMD2$SUBCMD3$SUBCMD0

#fityk  -c "$CMD" 2>&1 &

./mark.sh $1 $2 $3 "stars"/$NAME/$NAME"_"$PART".synspec" "stars"/$NAME/$NAME"_"$PART".obs.norm" "stars"/$NAME/$NAME"_"$PART".sub.obs.norm" "$NAME" "$NAME sub" $4 $5 && okular plot_$1-$2.pdf
