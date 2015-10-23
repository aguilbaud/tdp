#!/usr/bin/gnuplot

set term pdf
set output "dgemm_scalaire_mflops.pdf"

set xlabel "Taille matrice"
set ylabel "MFlop/s"

set logscale x
plot "data/dgemm_scalaires_mflops.dat" using 1:2 w lp title "(k,i,j)", "data/dgemm_scalaires_mflops.dat" using 1:3 w lp title "(i,j,k)", "data/dgemm_scalaires_mflops.dat" using 1:4 w lp title "(j,i,k)"
