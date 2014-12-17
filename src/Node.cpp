//
//  Node.cpp
//  simBAMM
//
//  Created by Dan Rabosky on 12/2/14.
//  Copyright (c) 2014 Dan Rabosky. All rights reserved.
//

#include "Node.h"


Node::Node()
{
    _lfDesc = NULL;
    _rtDesc = NULL;
    _anc = NULL;
    
    _brlen = 0.0;
    _time = 0.0;
    _isExtant = false;
    _isTip   = false;
    _nodeEvent = NULL;
    
}


Node::Node(Node* anc, double time, BranchEvent* be)
{
    _lfDesc = NULL;
    _rtDesc = NULL;
    _anc = anc;
    
    _brlen = 0.0;
    _time = time;
    _isExtant = false;
    _isTip   = false;
    _nodeEvent = be;
    
}


std::string Node::getRandomTipRight(Node* x){
    
 
    std::string name;
    if (x->getRtDesc() == NULL & x->getLfDesc() == NULL){
        name = x->getName();
    }else{
        name = getRandomTipRight(x->getRtDesc());
    }
     return name;
}

std::string Node::getRandomTipLeft(Node* x){
    
     
    std::string name;

    if (x->getRtDesc() == NULL & x->getLfDesc() == NULL){
        name = x->getName();
    }else{
        name = getRandomTipLeft(x->getLfDesc());
    }
    
     return name;
}



