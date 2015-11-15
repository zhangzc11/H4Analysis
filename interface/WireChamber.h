#ifndef __WIRE_CHAMBER_H__
#define __WIRE_CHAMBER_H__

#include <iostream>
#include <vector>
#include <algorithm>

#include "interface/H4Tree.h"

#define WIRE_CHAMBER_PITCH 0.005

class WireChamber
{
public:
    //---ctors---
    WireChamber();
    WireChamber(H4Tree* h4Tree, int chXl, int chXr, int chYu, int chYd);
    //---dtor---
    ~WireChamber(){};

    //---getters---
    float GetX(){return recoX_;};
    float GetY(){return recoY_;};

    //---utils---
    void Unpack();
    
private:
    H4Tree* h4Tree_;
    int    chXl_;
    int    chXr_;
    int    chYu_;
    int    chYd_;
    float  recoX_;
    float  recoY_;
};
    
#endif
