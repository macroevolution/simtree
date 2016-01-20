//
//  BranchEvent.cpp
//  simBAMM
//
//  Created by Dan Rabosky on 12/3/14.
//  Copyright (c) 2014 Dan Rabosky. All rights reserved.
//

#include "BranchEvent.h"
#include "Node.h"


BranchEvent::BranchEvent()
  : _eventNode{nullptr},
    _eventTime{0.0},
    _lambdaInit{0.0},
    _lambdaShift{0.0},
    _muInit{0.0}
{

}

BranchEvent::BranchEvent(Node* node, double time, double lambdaInit,
                            double lambdaShift, double muInit)
  : _eventNode{node},
    _eventTime{time},
    _lambdaInit{lambdaInit},
    _lambdaShift{lambdaShift},
    _muInit{muInit}
{

}







