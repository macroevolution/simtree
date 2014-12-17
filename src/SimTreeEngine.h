//
//  SimTreeEngine.h
//  simBAMM
//
//  Created by Dan Rabosky on 12/4/14.
//  Copyright (c) 2014 Dan Rabosky. All rights reserved.
//

#ifndef __simBAMM__SimTreeEngine__
#define __simBAMM__SimTreeEngine__

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

class SimTree;
class MbRandom;
class Settings;

class SimTreeEngine
{
    
private:
    Settings* _settings;
    MbRandom* _random;
    
    int _numberOfSims;
    int _BADMAX;
    int _mintaxa;
    int _maxtaxa;
    int _minNumberOfShifts;
    int _maxNumberOfShifts;
    
    std::string _treefile;
    std::string _eventfile;
    
    std::vector<SimTree*> _simtrees;

    
public:
    SimTreeEngine(Settings* settings, MbRandom* random);
    ~SimTreeEngine();
    
    SimTree* getTreeInstance(void);
    bool isTreeValid(SimTree* x);

    void writeTrees();
    void writeEventData();


};





#endif /* defined(__simBAMM__SimTreeEngine__) */
