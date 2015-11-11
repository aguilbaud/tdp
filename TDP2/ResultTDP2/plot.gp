#!/usr/bin/gnuplot
set terminal pdf
set output 'mpi_datatype/mpi_datatype.pdf'
set xlabel 'Number of particles'
set ylabel 'Time/iteration(ms)'
#set key right bottom
# set logscale y 2
# set ytics ( "256o" 256, "512o" 512, "1Kio" 1024, "256Kio" 262144, "512Kio" 524288, "1Mio" 1048576, "256Mio" 268435456, "512Mio" 536870912, "1Gio" 1073741824, "2Gio" 2147483648, "3Gio" 3221225472, "10Gio" 10737418240 )
plot 'mpi_datatype/mpi_heavy_datatype.dat' title 'Heavy Datatype' w lp, \
    'mpi_datatype/mpi_light_datatype.dat' title 'Light Datatype' w lp
#plot [1:10000] 20*19*x*10000/20*2*8 title '10000 particles on 20 proc' w lp
