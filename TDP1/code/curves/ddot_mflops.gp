#!/usr/bin/gnuplot

set term pdf
set output "ddot_mflops.pdf"

set xlabel "Taille vecteur"
set ylabel "MFlop/s"

set logscale x
plot "data/ddot_mflops.dat" w lp
