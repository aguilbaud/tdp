#!/usr/bin/gnuplot
set terminal pdf

set output 'mpi.pdf'

set logscale y
set xlabel 'Nb procs'
set ylabel 'Time(s)'
set xtics ( 1 ,2 ,3 ,4 )
plot 'mpi_20.dat' title 'N=20' w lp, \
    'mpi_100.dat' title 'N=100' w lp, \
    'mpi_400.dat' title 'N=400' w lp, \
    'mpi_1000.dat' title 'N=1000' w lp, \
    'mpi_4000.dat' title 'N=4000' w lp



