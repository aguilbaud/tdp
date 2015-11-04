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
    echo "set output '$output_gif'" >> $output_plot
    echo "set key above" >> $output_plot
    # echo "stats '$input_file' nooutput" >> $output_plot
    # echo "set xrange [STATS_min_x*1.05:STATS_max_x*1.05]" >> $output_plot
    # echo "set yrange [STATS_min_y*1.05:STATS_max_y*1.05]" >> $output_plot
    echo "do for [i=1:int($nb_img)] {" >> $output_plot
    echo "set title sprintf('%d',i )" >> $output_plot
    #seq
    if [ $mpi -eq 1 ];then
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

function simulation (){
    local input_file=$1
    nb_iter=$2
    
    ret=$(./driver -s $input_file -i $nb_iter -p)
    echo $ret
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


#mpi
generate_gp_gif_script "merged.dat" "mpi.gif" "testmpigif.gp" 100 1

#seq
#generate_gp_gif_script "gif.dat" "seq.gif" "testgif.gp" 0

chmod +x *.gp

#rm gif.dat "output/merged.dat"
# touch $input_data
# simulation $input_univ $i
# generate_gp_script $input_data $output_gif $output_plot
# ./$output_plot
# eog $output_gif
