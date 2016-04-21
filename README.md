simtree-BAMM
====

A program for simulating phylogenies under the BAMM process, with fossils

Requirements
------------

In order to compile simtree,
you need [CMake](http://www.cmake.org) and a C++11 compiler.
You also need a Unix shell (e.g., `bash`) to run the following commands.

Installation
------------

In the project's root directory,
create a new directory called `build` and go into it:

    mkdir build
    cd build

To compile simtree, run the following commands:

    cmake ..
    make -j

The final executable will be named `simtree`. You may run `simtree` from this directory, or you may install it in a more permanent location.
To do this, run the following command within the `build` directory:

    sudo make install

You may now run `simtree` from any directory in your system.
