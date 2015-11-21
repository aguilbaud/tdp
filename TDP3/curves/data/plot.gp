#!/usr/bin/gnuplot
set terminal pdf

set output 'dgetrf_bs.pdf'

set logscale x
set xlabel 'Block size'
set ylabel 'Mflops/s'

plot 'dgetrf_SB_mflops.dat' title 'dgetrf' w lp
