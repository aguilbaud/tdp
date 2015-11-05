#!/bin/bash

function generate_gp_gif_script(){
    local input_file=$1
    local output_gif=$2
    local output_plot=$3
    local nb_img=$4
    local mpi=$5
    local i=0
        
    echo "#!/usr/bin/gnuplot" > $output_plot
    echo "set terminal gif giant notransparent animate delay 1 optimize size 720 540" >> $output_plot
    if [ $mpi -eq 0 ];then
	echo "stats '$input_file' nooutput" >> $output_plot
	echo "set xrange [STATS_min_x*1.05:STATS_max_x*1.05]" >> $output_plot
	echo "set yrange [STATS_min_y*1.05:STATS_max_y*1.05]" >> $output_plot
    fi
    echo "set output '$output_gif'" >> $output_plot
    echo "set key above" >> $output_plot
    echo "do for [i=1:int($nb_img)] {" >> $output_plot
    
    
    if [ $mpi -eq 1 ];then
	echo "set title sprintf('%d',i )" >> $output_plot
	for file in $(ls output/outputmpi*.dat); do
	    if [ $i -eq 0 ]; then
		echo -e "\tplot '$file' index (i-1) title sprintf('Process %d',$i ) w p pt 1 lc palette, \\" >> $output_plot
	    else
		echo -e "\t\t '$file' index (i-1) title sprintf('Process %d',$i ) w p pt $((1+$i)) lc palette, \\" >> $output_plot

	    fi
	    ((i++))
	done
    else
	echo -e "\tplot '$input_file' index (i-1) title sprintf('%d',i ) w p pt 6 lc palette" >> $output_plot
    fi
    echo "}" >> $output_plot
}

function generate_gp_perf_script(){
    local output_gp=$1
    local input_file=$2
    local output_png=$3
    echo "#!/usr/bin/gnuplot" > $output_gp
    echo "set terminal png" >> $output_gp
    echo "set output '$output_png'" >> $output_gp
    echo "set xlabel 'Number of particles'" >> $output_gp
    echo "set ylabel 'Time(ms)'" >> $output_gp
    echo "plot '$input_file' w lp" >> $output_gp

}

function perf(){
    input_data="output/test.dat"
    input_univ="data/junk.univ"
    i=1000
    max_part=1000
    increment=1.5
    output_gif="foobar.gif"
    output_plot="plot_graphic.gp"
    output_perf="perf.dat"
    output_perf_plot="plot_perf.gp"
    output_png="perf.png"
    progname=driver

    if [ ! -e $progname ];then
	echo "$progname not found. Consider using make."
	exit
    fi
    echo -n > $output_perf

    
    echo "Generating random universe sets."
    for ((n=100 ; n<max_part ; n=$(echo "$n*$increment" | bc | cut -d. -f1))); do
	./driver -g data/perf$n.univ -n $n
    done

    echo "Executing performance tests (this may take some time...)."
    for ((n=100 ; n<max_part ; n=$(echo "$n*$increment" | bc | cut -d. -f1))); do
	echo -ne "\rNumber of particles : $n"
	echo $(simulation data/perf$n.univ $i) >> $output_perf
    done

    echo -e "\nGeneration gnuplot script."
    generate_gp_perf_script $output_perf_plot $output_perf $output_png
    chmod +x $output_perf_plot
    ./$output_perf_plot

    echo "Removing universe sets."
    for ((n=100 ; n<max_part ; n=$(echo "$n*$increment" | bc | cut -d. -f1))); do
	rm data/perf$n.univ
    done

    eog $output_png
}

####################MAIN##################

nb_iter=100000
input="data/blackhole.univ"
seq_output="seq.dat"
seq_gif="seq.gif"
mpi_gif="mpi.gif"

./driver -f $input -i $nb_iter -o $seq_output -p
mpiexec -n 4 ./driver_mpi -f $input -i $nb_iter 

#mpi
#1st argument: the files created with the application (-o options in sequential) NOT USED WHEN LAST ARGUMENT IS 1
#2nd: output file (gif)
#3rd: gnuplot script file tha will be generated
#4th: number of snapchot ( IF (number of iterations < 1000) : nb of iteration ELSE 1000)
#5th: 0 if seq // 1 if mpi
generate_gp_gif_script "anything" $mpi_gif "testmpigif.gp" 1000 1

#generate_gp_gif_script "seq.dat" $mpi_gif "testmpigif.gp" 1000 0

#seq
generate_gp_gif_script $seq_output $seq_gif "testgif.gp" 1000 0

chmod +x *.gp
echo "You can execute ./testmpigif.gp to create the gif generated with the mpi programm ($mpi_gif)"
echo "You can execute ./testgif.gp to create the gif generated with the programm ($seq_gif)"




