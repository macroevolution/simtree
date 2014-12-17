//
//  SimTree.cpp
//  simBAMM
//
//  Created by Dan Rabosky on 12/3/14.
//  Copyright (c) 2014 Dan Rabosky. All rights reserved.
//

#include <set>
#include <vector>
#include <sstream>

#include "SimTree.h"
#include "BranchEvent.h"
#include "MbRandom.h"
#include "Node.h"
#include "Settings.h"






SimTree::SimTree(MbRandom* random, Settings* settings)
{
    _random = random;
    _settings = settings;
    
    _isTreeBad = false;
    
    //_root = NULL;
    
    _root = new Node;
    
    BranchEvent* be = new BranchEvent;
    _rootEvent = be;
    
    // Initialize parameters of the root event:
    _rootEvent->setEventNode(_root);
    _rootEvent->setEventTime(0.0);
    
    // Rate distribution parameters
    
    
    _epsmin  = _settings->get<double>("epsmin");
    _epsmax = _settings->get<double>("epsmax");
    
    _rmin = _settings->get<double>("rmin");
    _rmax = _settings->get<double>("rmax");
    
    double lambdaInit = _settings->get<double>("lambdaInit0");
    double lambdaShift = _settings->get<double>("lambdaShift0");
    double muInit = _settings->get<double>("muInit0");
    
    if (lambdaInit <= 0){
        double eps = _random->uniformRv(_epsmin, _epsmax);
        double r = 0.0;
        if (_settings->get<bool>("rInitLogscale")){
            double tmp = _random->uniformRv(std::log(_rmin), std::log(_rmax));
            r = std::exp(tmp);
        }else{
            r = _random->uniformRv(_rmin, _rmax);
        }
        lambdaInit = r / (1 - eps);
        muInit = lambdaInit * eps;
            
    }
    if (muInit <= 0){
        double eps = _random->uniformRv();
        muInit = eps * lambdaInit;
    }
    
    if (lambdaShift < 0){
        lambdaShift = 0.0;
    }
    
    
    _rootEvent->setLambdaInit(lambdaInit);
    _rootEvent->setLambdaShift(lambdaShift);
    _rootEvent->setMuInit(muInit);
    
    _root->setAnc(NULL);
    _root->setTime(0.0);
    _root->setNodeEvent(be);

    _nodes.push_back(_root);
    
    _maxTime = _settings->get<double>("maxTime");
    _maxNumberOfNodes = _settings->get<double>("maxNumberOfNodes");
    _maxTimeForEvent = _settings->get<double>("maxTimeForEvent");
    _inc = _settings->get<double>("inc");
    
    simulateStep(_root, "right");
 
    if (!_isTreeBad){
        simulateStep(_root, "left");   
    }

    
    if (!_isTreeBad){
        setTipNames();
        //std::cout << "random tips: " << std::endl;
        //std::cout << _root->getRandomTipLeft(_root) << std::endl;
        //std::cout << _root->getRandomTipRight(_root) << std::endl;
    }
 

    
}


SimTree::~SimTree()
{

    
    for (int i = 0; i < (int)_eventSet.size(); ++i){
        delete _eventSet[i];
    }
    
    for (int i = 0; i < (int)_nodes.size(); ++i){
        delete _nodes[i];
    }
    
    delete _rootEvent;
     
}



// Recursive simulation engine

void SimTree::simulateStep(Node* p, std::string direction)
{
    
    if ((int)_nodes.size() > _maxNumberOfNodes){
        _isTreeBad = true;
        return;
    }
    
    double curTime = p->getTime();
    double dt = 0;
    double eventRate = _settings->get<double>("eventRate");
    
    BranchEvent* curEvent = p->getNodeEvent();
    double eventtime = curEvent->getEventTime();

    bool notDone = true;
    bool insertNewEvent = false;
 
    double lambdainit = curEvent->getLambdaInit();
    double lambdashift = curEvent->getLambdaShift();
    double mu = curEvent->getMuInit();   
    
    while (notDone){
        
        // get current parameters:
        double elapsed = curTime - eventtime;
        
        double lambda = lambdainit * std::exp(lambdashift * elapsed);

        if (curTime >= _maxTimeForEvent){
            eventRate = 0.0;
        }
                
        double totalRate = lambda + mu + eventRate;
        
        dt = _random->exponentialRv(totalRate);

        if (dt < _inc){
            // something happens
            curTime += dt;
            int eventtype = getEventType(lambda, mu, eventRate);
            
            if (eventtype <= (int)2){
            // speciation or extinction
                               
                notDone = false;
                
                Node* progeny = new Node(p, curTime, curEvent);
                _nodes.push_back(progeny);
                
                progeny->setBrlen((curTime - p->getTime()));
                
                if (direction == "right"){
                    p->setRtDesc(progeny);
                }else if (direction == "left"){
                    p->setLfDesc(progeny);
                }else{
                    throw;
                }
                
                if (insertNewEvent){
                    // Here we link the new node
                    //   to the new event that occurred on the branch
                    BranchEvent* nextEvent
                            = new BranchEvent(progeny, eventtime, lambdainit, lambdashift, mu);
                    
                    _eventSet.push_back(nextEvent);
                    progeny->setNodeEvent(nextEvent);
                }
                
                
                if (eventtype == (int)1){
                    simulateStep(progeny, "right");
                    simulateStep(progeny, "left");
                }else{
                    // extinction.
                    progeny->setIsExtant(false);
                    progeny->setIsTip(true);
                }
                

                
            }else if (eventtype == (int)3){
            // Rate shift but no speciationp-extinction
            
            // Must deal with possibility of multiple branch
            // shifts. These should be ignored: just retain
            // the most recent one.

                eventtime = curTime;
                
                double new_r = _random->uniformRv(_rmin, _rmax);
                double new_eps = _random->uniformRv(_epsmin, _epsmax);
    
                
                lambdainit = new_r / (1 - new_eps);
                mu = new_eps * lambdainit;

                insertNewEvent = true;
                
            }else{
                std::cout << "Problem in getting eventtype" << std::endl;
                throw;
            }
            
            
            
        }else if ((curTime + _inc) < _maxTime){
            // Lineage reaches end of interval but not end of simulation period
            //    Nothing happens except time gets incremented by inc
            
            curTime += _inc;
        
        }else if ((curTime + _inc) >= _maxTime){
            // lineage reaches max time
            // Create new terminal node and set as tip.
            
            curTime = _maxTime;
            notDone = false;
            Node* progeny = new Node(p, curTime, curEvent);
            _nodes.push_back(progeny);
            
            if (direction == "right"){
                p->setRtDesc(progeny);
            }else{
                p->setLfDesc(progeny);
            }
            
            progeny->setIsTip(true);
            progeny->setIsExtant(true);
            progeny->setBrlen((curTime - p->getTime()));
            
        }else{
            std::cout << "reached problem point in SimTree::simulateStep()" << std::endl;
            throw;
            
        }
        
        
        
    
    }
    

}



void SimTree::printTipLambda()
{
    for (int i = 0; i < (int)_nodes.size(); i++){
        if (_nodes[i]->getIsTip()){
            std::cout << i << "\t" << _nodes[i]->getNodeEvent()->getLambdaInit() << std::endl;
        }
    }

}


int SimTree::getEventType(double x, double y, double z)
{
    double total = x + y + z;
    
    double ran = _random->uniformRv();
    
    int eventtype = 0;
    
    if (ran <= (x/total)){
        eventtype = 1;
    }else if (ran <= ((x + y) / total)){
        eventtype = 2;
    }else {
        eventtype = 3;
    }
    return eventtype;
}


void SimTree::setTipNames()
{
    
    for (int i = 0; i < (int)_nodes.size(); i++){
        std::stringstream ss;
        if (_nodes[i]->getIsTip() & _nodes[i]->getIsExtant()){
            ss << "A" << i;
            _nodes[i]->setName(ss.str());
        }else if (_nodes[i]->getIsTip() & !_nodes[i]->getIsExtant()){
            ss << "D" << i;
            _nodes[i]->setName(ss.str());
        }else{
            ss << "I" << i;
            _nodes[i]->setName(ss.str());
        }
        
    }
}


void SimTree::writeTree(Node* p, std::ostream& ss)
{
    if (p->getLfDesc() == NULL && p-> getRtDesc() == NULL) {
        ss << p->getName() << ":" << p->getBrlen();
    } else {
        ss << "(";
        writeTree(p->getLfDesc(), ss);
        ss << ",";
        writeTree(p->getRtDesc(), ss);
        ss << "):" << p->getBrlen();
    }
}


int SimTree::getNumberOfTips()
{
    int ctr = 0;
    for (int i = 0; i < (int)_nodes.size(); i++){
        if (_nodes[i]->getLfDesc() == NULL & _nodes[i]->getRtDesc() == NULL){
            ctr++;
        }
    }
    return ctr;
}


int SimTree::getNumberOfShifts()
{
    int shifts = (int)_eventSet.size();
    return shifts;
}


void SimTree::getEventDataString(int index, std::ostream& ss)
{

    BranchEvent* be = _rootEvent;
    
    ss << index << ",";
    ss << be->getEventNode()->getRandomTipRight(be->getEventNode()) << ",";
    ss << be->getEventNode()->getRandomTipLeft(be->getEventNode()) << ",";
    ss << be->getEventTime() << ",";
    ss << be->getLambdaInit() << ",";
    ss << be->getLambdaShift() << ",";
    ss << be->getMuInit() << "\n";
    
    for (int i = 0; i < (int)_eventSet.size(); i++){
        be = _eventSet[i];
        ss << index << ",";
        ss << be->getEventNode()->getRandomTipRight(be->getEventNode()) << ",";
        ss << be->getEventNode()->getRandomTipLeft(be->getEventNode()) << ",";
        ss << be->getEventTime() << ",";
        ss << be->getLambdaInit() << ",";
        ss << be->getLambdaShift() << ",";
        ss << be->getMuInit() << "\n";
    }

}















