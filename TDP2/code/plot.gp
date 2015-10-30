#!/usr/bin/gnuplot

set terminal gif animate delay 1
set output 'foobar.gif'
set key above
stats 'output/test.dat' nooutput

max_x = system("cut -d' ' -f 1 output/test.dat | sort -n | tail -n 1")
min_x = system("cut -d' ' -f 1 output/test.dat | sort -n | head -n 1")
max_y = system("cut -d' ' -f 2 output/test.dat | sort -n | tail -n 1")
min_y = system("cut -d' ' -f 2 output/test.dat | sort -n | head -n 1")

set xrange [min_x:max_x]
set yrange [min_y:max_y]

do for [i=1:int(STATS_blocks)] {
   
    plot 'output/test.dat' index (i-1) title sprintf('%d',i ) w p lc palette
}
