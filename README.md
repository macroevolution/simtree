## SIMTREE USER GUIDE
####INTRODUCTION

simtree is a C++11 program written by Dan Rabosky to simulate phylogenies with rate shifts. simtree stochastically draws a net diversification rate (r) and relative extinction rate (epsilon) for the root process of the tree, then evolves two clades under those rates. A Poisson process governs how often each clade undergoes a shift in rates (in which the shifted clade draws new parameters for r and epsilon). See [Input](#input) for details on how to change the parameters.

simtree generates output trees with extinct regimes, and shifts freely occur everywhere on the tree. This means that the simulated trees can be pruned using software such as `ape` or `phytools` in `R` and the pruned trees can be analyzed using BAMM. The comparison of the BAMM analysis with the simulated rate regimes can thus serve as a test for BAMM’s accuracy. Although note that, currently, rates are not time-variable within clades simulated using simtree. People interested in testing BAMM can certainly use this to examine how well time-variable BAMM performs when the true generating process involved time-homogenous rates, however allowing time-variable estimation greatly increases run time.

simtree outputs a file containing the simulated trees, and a file containing the event data. See [Output](#output) for details.

####Requirements

In order to compile simtree, you need [CMake](http://www.cmake.org) and a C++11 compiler. You also need a Unix shell (e.g., `bash`) to run the following commands.


####Installation
Install simtree by opening your terminal and navigating to the folder in which you’d like to place the source files, for instance `/Applications`, then entering the following into the terminal:

	git clone https://github.com/macroevolution/simtree.git

Navigate to the simtree directory

	cd simtree

And create a build directory

	mkdir build
	cd build

Now, you need [CMake](https://cmake.org/) and a C++11 compiler installed. Run:

	cmake ..
	make -j

This will compile simtree on your computer.

The final executable will be named simtree. You may run simtree from this directory, or you may install it in a more permanent location. To do this, run the following command within the build directory:

	sudo make install
	
You may now run simtree from any directory in your system.

Running simtree requires a control file. If your control file is called `control.txt`, you can then run the following:

	simtree -c control.txt

If simtree is not installed in a common location, you will need to specify its location. For example:

	./simtree -c control.txt

...if the control file is in the same directory as the program, or
	
	/Applications/simtree -c control.txt

...if the control file is in a different location from the program, and simtree is in a directory called `/Applications`.


#####Input<a name="input"></a>
The input values given to simtree are all stored in the control file. simtree comes with an example control file with a list of arguments that control different aspects of the simulation. Several arguments control the basic constraints of the simulations:

	mintaxa = 100
	maxtaxa = 2000
	minNumberOfShifts = 1
	maxNumberOfShifts = 20
	maxTime = 100

There are also key probabilistic arguments like:

	eventRate = 0.01

`eventRate` is the rate of the Poisson process that generates shifts. The expected number of shifts is `1/(eventRate)`. So an eventRate of 0.01 will run a simulation using an expected number of shifts of 100, while an eventRate of 1 will simulate using an expected number of shifts of 1.

	rain = 0.01
	rmax = 0.5
	epsmin = 0
	epsmax = 1

These values constrain the values of the net diversification rate (r) and relative extinction rate (eps for epsilon) of the new shifts. Net diversification (r) is speciation - extinction, while relative extinction (epsilon) is extinction/speciation. By tweaking these parameters, shifts are limited in how rapidly they can increase in diversity and how likely they are to go extinct.

Note that setting an `rmin` that is negative will allow some clades to shift into highly extinction-prone regimes, which allows users to robustly test how sensitive BAMM is to the assumption of “no shifts on extinct lineages”.

There are also several parameters to control the number and names of the output files:

	numberOfSims = 10
	eventful = events.txt
	treefile = simtrees.txt

#####Output files<a name="output"></a>
simtree outputs two different files. One is a file with Newick style trees for each of the numberOfSims simulations. The other is a comma-separated file storing the location, timing and magnitude of the simulated shifts along each of the tree.

The trees can be read into `R` using `read.tree()` from the `ape` package, and will generate an object of the class `multiPhylo`. Extinct tips are labeled “D#” while tips still extant at the end of the simulation run are labeled “A#”. So to create an extant-only tree:

	trees <- read.tree(“simtrees.txt”)
	tree <- trees[[1]]
	extinct <- tree$tip.label[grep("D", tree$tip.label)]
	tree_extant <- drop.tip(tree, extinct)

The rate regimes on each tree are stored in a separate outfile. This is a matrix with a column stating which simulated tree each regime applies to (column 1, `sims`), and then other columns corresponding to the location of the shift (`leftchild`, `rightchild`, and `abstime`) as well as the actual parameters of the regime (`lambdainit` and `muinit`). Currently, simtree does not simulate time-variable models, so the `lambdashift` and `mushift` values should be zero.

Every tree will have a root regime, although if you analyze a pruned BAMM tree (with some or all extinct tips dropped) the left and right children of each shift will need to be redetermined using the `getDesc()` function in `BAMMtools` or `getDescendants()` function in `phytools`.

