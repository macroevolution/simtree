//
//  BranchEvent.h
//  simBAMM
//
//  Created by Dan Rabosky on 12/3/14.
//  Copyright (c) 2014 Dan Rabosky. All rights reserved.
//

#ifndef __simBAMM__BranchEvent__
#define __simBAMM__BranchEvent__

#include <iostream>
#include <set>


class Node;

class BranchEvent
{
    
private:
    
    Node*   _eventNode;
    double  _eventTime;
    double  _lambdaInit;
    double  _lambdaShift;
    double  _muInit;
    
public:
    
    BranchEvent();
    BranchEvent(Node* node, double time, double lambdaInit, double lambdaShift, double muInit);
    
    Node* getEventNode();
    void setEventNode(Node* x);

    double getEventTime();
    void setEventTime(double x);
    
    double getLambdaInit();
    void setLambdaInit(double x);
    
    double getLambdaShift();
    void setLambdaShift(double x);

    double getMuInit();
    void setMuInit(double x);


};

inline Node* BranchEvent::getEventNode()
{
    return _eventNode;
}

inline void BranchEvent::setEventNode(Node* x)
{
    _eventNode = x;
}

inline double BranchEvent::getEventTime()
{
    return _eventTime;
}

inline void BranchEvent::setEventTime(double x)
{
    _eventTime = x;
}

inline double BranchEvent::getLambdaInit()
{
    return _lambdaInit;
}

inline void BranchEvent::setLambdaInit(double x)
{
    _lambdaInit = x;
}

inline double BranchEvent::getLambdaShift()
{
    return _lambdaShift;
}

inline void BranchEvent::setLambdaShift(double x)
{
    _lambdaShift = x;
}

inline double BranchEvent::getMuInit()
{
    return _muInit;
}

inline void BranchEvent::setMuInit(double x)
{
    _muInit = x;
}











#endif /* defined(__simBAMM__BranchEvent__) */
