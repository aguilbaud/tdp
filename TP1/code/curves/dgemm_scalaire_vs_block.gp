#!/usr/bin/gnuplot

set term pdf
set output "dgemm_scalaire_vs_block_mflops.pdf"

set xlabel "Taille matrice"
set ylabel "MFlop/s"

set logscale x
plot "data/dgemm_block_vs_scalaire_mflops.dat" using 1:2 w lp title "dgemm scalaire", "data/dgemm_block_vs_scalaire_mflops.dat" using 1:3 w lp title "dgemm block"
