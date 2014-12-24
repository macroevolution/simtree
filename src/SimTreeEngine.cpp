//
//  SimTreeEngine.cpp
//  simBAMM
//
//  Created by Dan Rabosky on 12/4/14.
//  Copyright (c) 2014 Dan Rabosky. All rights reserved.
//

#include "SimTreeEngine.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "SimTree.h"
#include "Settings.h"
#include "MbRandom.h"


SimTreeEngine::SimTreeEngine(Settings* settings, MbRandom* random)
{
    _settings = settings;
    _random = random;
    _numberOfSims = _settings->get<int>("numberOfSims");
    _treefile = _settings->get<std::string>("treefile");
    _eventfile = _settings->get<std::string>("eventfile");
    
    _BADMAX = 2000;
    
    _mintaxa = _settings->get<int>("mintaxa");
    _maxtaxa = _settings->get<int>("maxtaxa");
    _minNumberOfShifts = _settings->get<int>("minNumberOfShifts");
    _maxNumberOfShifts = _settings->get<int>("maxNumberOfShifts");
    
    
    for (int i = 0; i < _numberOfSims; i++){
         _simtrees.push_back(getTreeInstance());
 
        //_simtrees[i]->recursiveCheckTime();
        //_simtrees[i]->checkBranchLengths();
        
        std::cout << "tree " << i << " has << ";
        std::cout << _simtrees[i]->getNumberOfTips() << " >> tips";
        std::cout << "\tshifts: " << _simtrees[i]->getNumberOfShifts() << std::endl;
        
    }
    
    // Data output
    
    writeTrees();
    writeEventData();
}


SimTreeEngine::~SimTreeEngine()
{
    for (int i = 0; i < (int)_simtrees.size(); i++){
        delete _simtrees[i];
    }
}


SimTree* SimTreeEngine::getTreeInstance()
{
    bool isBad = true;
    int badctr = 0;
    while (isBad){
        SimTree* myTree = new SimTree(_random, _settings);
        if (isTreeValid(myTree)){
            return myTree;
        }else{
            badctr++;
        }
        if (badctr > _BADMAX){
            std::cout << "cannot simulate valid tree with params" << std::endl;
            std::cout << "MAXBAD exceeded" << std::endl;
            exit(0);
        
        }
    }
}


bool SimTreeEngine::isTreeValid(SimTree* x)
{
    if (x->getIsTreeBad()){
        return false;
    }
    
    int tips = x->getNumberOfTips();
    int shifts = x->getNumberOfShifts();
    
    bool isGood = (tips >= _mintaxa & tips <= _maxtaxa
                   & shifts >= _minNumberOfShifts & shifts <= _maxNumberOfShifts);
        
    return isGood;
}



void SimTreeEngine::writeTrees()
{
    std::string outname = _treefile;
    std::ofstream outStream;
    outStream.open(outname.c_str());
    outStream.close();
    
    for (int i = 0; i < (int)_simtrees.size(); i++){
        std::ofstream tmpstream;
        tmpstream.open(outname.c_str(), std::ofstream::out | std::ofstream::app);
        
        std::stringstream ss;
        _simtrees[i]->writeTree(_simtrees[i]->getRoot(), ss);
        ss << ";";
        
        tmpstream << ss.str() << std::endl;
        
        tmpstream.close();
    }
    
}


void SimTreeEngine::writeEventData()
{
    std::string outname = _eventfile;
    std::ofstream outStream;
    outStream.open(outname.c_str());
    // Write header:
    outStream << "sim,leftchild,rightchild,abstime,lambdainit,lambdashift,muinit\n";
    outStream.close();
    
    for (int i = 0; i < (int)_simtrees.size(); i++){
        std::ofstream tmpstream;
        tmpstream.open(outname.c_str(), std::ofstream::out | std::ofstream::app);
        _simtrees[i]->getEventDataString(i+1, tmpstream);
    
        tmpstream.close();
    }
    
    
    
    
}

















