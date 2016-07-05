//
//  main.cpp
//  simBAMM
//
//  Created by Dan Rabosky on 12/2/14.
//  Copyright (c) 2014 Dan Rabosky. All rights reserved.
//

#include <iostream>
#include <chrono>
#include <vector>
#include <sstream>
#include <fstream>

#include "CommandLineProcessor.h"
#include "Log.h"
#include "Settings.h"
#include "MbRandom.h"
#include "SimTree.h"
#include "SimTreeEngine.h"

long int getPrecisionTime();


int main(int argc, char * argv[])
{

    CommandLineProcessor commandLine(argc, argv);    
    
    Settings mySettings(commandLine.controlFileName(), commandLine.parameters());
    
    // Use high-precision chronos library for seed.
    // Requires C++11.
    
    long int seed = mySettings.get<long int>("seed");
    
    if (seed == -1){
        long int seed = getPrecisionTime();
    }
 
    
    MbRandom myRNG;
    myRNG.setSeed(seed);
    
    // warmup
    for (int i = 0; i < 5000; i++){
        myRNG.uniformRv();
    }
 
    
    std::cout << "Simulating....\n";
 
    SimTreeEngine simengine(&mySettings, &myRNG);
    
    return 0;
    
    
    
}

long int getPrecisionTime()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto dur = now.time_since_epoch();
    
    long int rmd = (long int)dur.count() %  1000000;

    //typedef std::chrono::high_resolution_clock::period period_t;
    //auto dur = now.time_since_epoch();
    //std::cout << "High-res clock reports " << dur.count() << " ticks of 1/" << period_t::den << " seconds \n";
    
    return rmd;
}

