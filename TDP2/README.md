#TP2
##Compilation
* `make driver`: generate the senquential version of the simulation
* `make driver_mpi`: generate the MPI version of the simulation
* `make`: generate both


##Execution
###Sequential
`./driver -f <file> [OPTIONS]` file: input file
* `-g <file>`          Create a random file 
* `-n <nb_particles>`  Number of particles (only with -g option)
* `-i <nb_iterations>` Number of iterations
* `-o <file>`          Create a file containing the evolution of the positions of the particles   
* `-p`                 Output the number of particles and the total time of the simulation on the standard output (useful for performance tests)


###MPI
`mpiexec -n <nb_proc> ./driver_mpi -f <file> [OPTIONS]` file: input file
* `-i <nb_iterations>` Number of iterations
* `-p`                 Output the number of particles and the total time of the simulation on the standard output (useful for performance tests)


###Exemples:
* `./driver -g <file> -n <nb_particles>`: generate a ramdom universe of `nb_particles` and store it in the given file
* `./driver -s <in_file> -i <nb_iterations> -o <out_file>`: runs `nb_iterations` on the `in_file` and store the positions in `out_file`
* `mpiexec -n 10 ./driver_mpi -f <in_file> -i <nb_iterations> -p`: runs `nb_iterations` on the `in_file`


##Input file format
A very simple format is used to store the particles.<br/>
**WARNING**: when executing the MPI version of the programm, the number of particles should be a multiple of the number of processes.
<pre><code>
  number_of_particles
  mass_p1 position_x_p1 position_y_p2 velocity_x_p1 velocity_y_p1
  mass_p1 position_x_p2 position_y_p2 velocity_x_p2 velocity_y_p2
  ...
  mass_pn position_x_pn position_y_pn velocity_x_pn velocity_y_pn
</pre></code>


A few examples can be found in the /data directory.


##Settings
If you want to change the values of mass, position or velocity used during the generation of a random file, you can edit this values in util.h. If the distance between particles is large enough, you can also modify the default dt value in either driver.c or driver_mpi.c.


##Exploiting the output results
The sequential programm can ouptut a file containing multiples snapshots of the simulation (-o option).
The MPI version automatically generates one of this file for each of the process (if the -p otion is not specified); these files can be found in the /output directory.


With those files, you can generate a gif to visualize the simulation. A commented example on the way to achieve it can be found in perf.sh


Examples of gif that can be created with the script
![](http://zippy.gfycat.com/OrangeUnpleasantIrrawaddydolphin.gif)</br>
![](http://zippy.gfycat.com/CircularFewAfricanjacana.gif)


##Performance comparison

Executed on Plafrim on a single NUMA node

| Type            | Time(ms)      |  Nb particles   |   Nb iterations | Time/particle (ms)  |
| --------------- | ------------- | --------------- | --------------- | ------------------- |
| Sequential      |    10981      |       5016      |      10         |       2.18          |
| MPI (24 cores)  |     467       |        5016     |      10         |       0.09          |

