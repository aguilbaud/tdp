#TP2
##Compilation
* `make driver`: generate the senquential version of the simulation
* `make driver_mpi`: generate the MPI version of the simulation

##Execution
###Sequential
`./driver -s <file> [OPTIONS]` file: input file
* `-i <nb_iterations>` Number of iterations
* `-o <file>`          Create a file containing the evolution of the positions of the particles   
* `-p`                 Output the number of particles and the total time of the simulation on the standard output (useful for performance tests)


Exemples:
* `./driver -g <file> -n <nb_particles>`: generate a ramdom universe of `nb_particles` and store it in the given file
* `./driver -s <in_file> -i <nb_iterations> -o <out_file>`: runs `nb_iterations` on the `in_file` and store the positions in `out_file`


###MPI

