#ifndef __DIGI_TREE__
#define __DIGI_TREE__

#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include "TTree.h"
#include "TString.h"

using namespace std;

typedef unsigned long int uint32;
typedef unsigned long long int uint64;
 
//****************************************************************************************

class DigiTree
{
public: 
    //---ctors---
    DigiTree(){};
    DigiTree(uint64* idx, TTree* tree=NULL, string prefix="");
    //---dtor---
    ~DigiTree();

    //---utils---
    void Init(vector<string>& names);
    void Fill() {tree_->Fill();};
    
    TTree*        tree_; 
    string        prefix_;

    uint64*       index;
    uint64        time_stamp;
    unsigned int  n_channels;
    float*        b_charge;
    float*        b_slope;
    float*        b_rms;
    float*        time;
    float*        time_chi2;
    float*        maximum;
    float*        amp_max;
    float*        charge_tot;
    float*        charge_sig;
    float*        fit_ampl;
    float*        fit_time;
    float*        fit_chi2;
    int*          channels;
};

#endif
