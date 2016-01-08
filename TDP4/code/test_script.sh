#!/bin/bash

function unit_test {
    local A=$1
    local B=$2
    local C=$3
    np_procs=( 1 4 9 16 25 )
    mpirun -n 4 ./driver $A $B $C
}

function perf_test {
    local output="data/perf_test.mat"
    local dat1="output/perf.dat"
   
    nb_procs=( 4 9 16 )
    #nb_procs=( 16 ) 25 36 49 64 81
    ppcm=144
    echo -e > $dat1

    for var in ${nb_procs[@]}; do
	for i in $(seq 1 1 10); do
	    size=$(($ppcm*$i))
	    #size=$(($var*$i))
	    ./gen_mat $size $output
	    res=$(mpirun -n $var ./timing $output $output $output)
	    #res=$(./sequential $output)
	    
	    echo -n $res >> $dat1
	    echo " $size" >> $dat1
	done
	echo >> $dat1
	echo >> $dat1
    done

    
}


perf_test
