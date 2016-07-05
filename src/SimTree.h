//
//  SimTree.h
//  simBAMM
//
//  Created by Dan Rabosky on 12/3/14.
//  Copyright (c) 2014 Dan Rabosky. All rights reserved.
//

#ifndef __simBAMM__SimTree__
#define __simBAMM__SimTree__

#include <iostream>
#include <set>
#include <vector>
#include <sstream>

class Node;
class BranchEvent;
class MbRandom;
class Settings;

class SimTree
{
private:
    
    MbRandom* _random;
    Settings* _settings;
    
    Node* _root;
    BranchEvent* _rootEvent;
    
    std::vector<BranchEvent*> _eventSet; // holds all non-root events
    std::vector<Node*> _nodes;
    
    double  _maxTime;

    double  _maxTimeForEvent;
    int     _maxNumberOfNodes;
    
    bool    _isTreeBad;
    
    double  _inc;
    
    double _multipliermin;
    double _multipliermax;
    
    double _epsmin;
    double _epsmax;
    double _rmin;
    double _rmax;
    
    double _lambda_rate; //rate params of exponential distribution
    double _mu_rate;
    
public:
    
    SimTree(MbRandom* random, Settings* settings);
    SimTree(const SimTree&) = delete;
    SimTree& operator=(const SimTree&) = delete;
    ~SimTree();

    void simulateStep(Node* p, std::string direction);

    int getEventType(double a, double b, double c);
    
    void writeTree(Node* p, std::ostream& ss);
    void setTipNames(void);
    Node* getRoot();
    void printTipLambda();
    
    void getEventDataString(int index, std::ostream& ss);
    
    bool getIsTreeBad();
    int getNumberOfTips();
    int getNumberOfShifts();
    
    void recursiveCheckTime();
    void recursiveSetTime(Node * x);
    void checkBranchLengths();
    
    double getTreeAge();

};


inline Node* SimTree::getRoot()
{
    return _root;
}

inline bool SimTree::getIsTreeBad()
{
    return _isTreeBad;
}


#endif /* defined(__simBAMM__SimTree__) */
