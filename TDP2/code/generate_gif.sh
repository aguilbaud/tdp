#!/bin/bash


input_file=$1
output_gif=$2
output_plot="gif_plot.gp"
max_x=$(cut -d' ' -f 1 $input_file | sort -n | tail -n 1)
min_x=$(cut -d' ' -f 1 $input_file | sort -n | head -n 1)
max_y=$(cut -d' ' -f 2 $input_file | sort -n | tail -n 1)
min_y=$(cut -d' ' -f 2 $input_file | sort -n | head -n 1)
    
echo "#!/usr/bin/gnuplot" > $output_plot
echo "set terminal gif animate delay 1" >> $output_plot
echo "set output '$output_gif'" >> $output_plot
echo "set key above" >> $output_plot
echo "stats '$input_file' nooutput" >> $output_plot
echo "set xrange [$min_x:$max_x]" >> $output_plot
echo "set yrange [$min_y:$max_y]" >> $output_plot
echo "do for [i=1:int(STATS_blocks)] {" >> $output_plot
echo -e "\tplot '$input_file' index (i-1) title sprintf('%d',i ) w p lc palette" >> $output_plot
echo "}" >> $output_plot

chmod +x $output_plot
./$output_plot
rm gif.dat
