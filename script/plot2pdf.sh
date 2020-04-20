#!/bin/bash

# Make pdf tex of pdf plots made with marker.cpp
#
# Usage: ./plot2pdf "a text"
#
# A. Lemonnier
# 20/04/2020



#begin
begin() {
latex_begin="
%% auto generated\n
\\\documentclass[a4paper,10pt]{article}\n
\\\usepackage[utf8]{inputenc}\n
\\\usepackage[dvips,pdftex]{graphicx}\n
\\\begin{document}\n\n"

latex_begin_2="
%% auto generated\n
\\\documentclass[a4paper,10pt]{article}\n
\\\usepackage[utf8]{inputenc}\n
\\\usepackage[dvips,pdftex]{graphicx}\n
\\\usepackage[center]{caption}\n
\\\usepackage[left=1.9cm,right=1.9cm,top=1.9cm,bottom=1.9cm]{geometry}\n
\\\begin{document}\n\n
\\\begin{figure}\n
\\\centering\n"
}

#end
end() {
latex_end="\n\\\end{document}"
latex_end_2="\\\end{figure}\n\n\\\end{document}"
}

#fig
figure() {
LOCAL1=$1
LOCAL1=${LOCAL1//_/ }

latex_fig="\\\begin{figure}
\n\\\centering
\\\includegraphics[scale=0.7,keepaspectratio=true]{{{"$1"}}}\n
\\\caption{File "$3" \\\textbf{"$LOCAL1"}}\n
\\\label{fig:plot"$2$3"}\n
\\\end{figure}\n\n"

latex_fig_2="\\\includegraphics[scale=0.52,keepaspectratio=true]{{{"$1"}}}"

}

#break
breakimg () { 
break_img="\\\caption{ Pic "$(($1-1))" - "$1"}\n\\\end{figure}\n\n\\\begin{figure}\n\\\centering\n" 
}

STR=$1

if [ -z "$1" ]; then
    STR=0
fi

rm plot_$STR.pdf > /dev/null 2>&1
rm plot_2_$STR.pdf > /dev/null 2>&1

begin
end

echo -e $latex_begin >  plot_$STR.tex
echo -e $latex_begin_2 > plot_2_$STR.tex

COUNT=1
for FILE in ./*.pdf; do
    breakimg $COUNT
    
    echo "file "$COUNT": "$FILE
    
    NAME=$(basename $FILE)
    NAME="${NAME%.*}"
    
    figure $NAME $1 $COUNT
    
    echo -e $latex_fig >> plot_$STR.tex
    echo -e $latex_fig_2 >> plot_2_$STR.tex
    
    if ! (($COUNT % 2)); then
        echo -e $break_img >> plot_2_$STR.tex
    fi
    
    COUNT=$(($COUNT+1))
done

echo -e $latex_end >>  plot_$STR.tex
echo -e $latex_end_2 >>  plot_2_$STR.tex

pdflatex plot_$STR.tex plot_$STR.pdf > /dev/null 2>&1
pdflatex plot_2_$STR.tex plot_2_$STR.pdf > /dev/null 2>&1

rm plot_$STR.synctex.gz > /dev/null 2>&1
rm plot_$STR.bbl > /dev/null 2>&1
rm plot_$STR.blg > /dev/null 2>&1
rm plot_$STR.log > /dev/null 2>&1
rm plot_$STR.aux > /dev/null 2>&1

rm plot_2_$STR.synctex.gz > /dev/null 2>&1
rm plot_2_$STR.bbl > /dev/null 2>&1
rm plot_2_$STR.blg > /dev/null 2>&1
rm plot_2_$STR.log > /dev/null 2>&1
rm plot_2_$STR.aux > /dev/null 2>&1
