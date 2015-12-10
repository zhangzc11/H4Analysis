#ifndef __POSITION_TREE__
#define __POSITION_TREE__

#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"

using namespace std;

class PositionTree
{
public: 
    //---ctors---
    PositionTree(){};
    PositionTree(int* idx, TTree* tree=NULL, int nPlanes=1);
    //---dtor---
    ~PositionTree(){};
    
    //---utils---
    void Init();
    void Fill() {tree_->Fill();};

    TTree* tree_; 

    int*   index;
    int    n_planes;
    float* X;
    float* Y;
};

#endif
