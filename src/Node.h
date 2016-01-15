//
//  Node.h
//  simBAMM
//
//  Created by Dan Rabosky on 12/2/14.
//  Copyright (c) 2014 Dan Rabosky. All rights reserved.
//

#ifndef __simBAMM__Node__
#define __simBAMM__Node__

#include <iostream>

class BranchEvent;

class Node
{
private:
    Node*   _lfDesc;
    Node*   _rtDesc;
    Node*   _anc;
    double  _time;
    double  _brlen;
    double  _tmp;
    
    bool    _isExtant;
    bool    _isTip;
    
    BranchEvent* _nodeEvent;

    std::string _name;

public:
    Node();
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node* anc, double time, BranchEvent* be);
    
    void    setAnc(Node * x);
    Node*   getAnc();
    
    void    setLfDesc(Node* x);
    Node*   getLfDesc();

    void    setRtDesc(Node* x);
    Node*   getRtDesc();

    void    setTime(double x);
    double  getTime();

    void    setBrlen(double x);
    double  getBrlen();

    bool    getIsExtant();
    void    setIsExtant(bool x);

    bool    getIsTip();
    void    setIsTip(bool x);
    
    double  getTmp();
    void    setTmp(double x);

    BranchEvent* getNodeEvent();
    void    setNodeEvent(BranchEvent* x);
    
    void    setName(std::string x);
    std::string getName();
    
    std::string getRandomTipRight(Node* x);
    std::string getRandomTipLeft(Node* x);


};

inline void Node::setTmp(double x)
{
    _tmp = x;
}

inline double Node::getTmp()
{
    return _tmp;
}


inline void Node::setAnc(Node* x)
{
    _anc = x;
}

inline Node* Node::getAnc()
{
    return _anc;
}

inline void Node::setLfDesc(Node* x)
{
    _lfDesc = x;
}

inline void Node::setRtDesc(Node* x)
{
    _rtDesc = x;
}

inline Node* Node::getLfDesc()
{
    return _lfDesc;
}

inline Node* Node::getRtDesc()
{
    return _rtDesc;
}

inline double Node::getTime()
{
    return _time;
}

inline void Node::setTime(double x)
{
    _time = x;
}

inline double Node::getBrlen()
{
    return _brlen;
}

inline void Node::setBrlen(double x)
{
    _brlen = x;
}

inline bool Node::getIsExtant()
{
    return _isExtant;
}

inline void Node::setIsExtant(bool x)
{
    _isExtant = x;
}


inline bool Node::getIsTip()
{
    return _isTip;
}

inline void Node::setIsTip(bool x)
{
    _isTip = x;
}

inline BranchEvent* Node::getNodeEvent()
{
    return _nodeEvent;
}

inline void Node::setNodeEvent(BranchEvent* x)
{
    _nodeEvent = x;
}

inline void Node::setName(std::string x)
{
    _name = x;
}

inline std::string Node::getName()
{
    return _name;
}


#endif /* defined(__simBAMM__Node__) */







