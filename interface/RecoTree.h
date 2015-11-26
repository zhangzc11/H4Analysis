#ifndef __RECO_TREE__
#define __RECO_TREE__

#include <string>
#include <vector>

#include "TTree.h"
#include "TString.h"

using namespace std;

typedef unsigned long int uint32;
typedef unsigned long long int uint64;
 
//****************************************************************************************

class RecoTree
{
public: 
    //---ctors---
    RecoTree(){};
    RecoTree(vector<string> names, int nSamples, bool H4hodo, int nWireCh, int* idx);
    //---dtor---
    ~RecoTree();

    //---utils---
    void Fill() {tree_->Fill();};
    void Write(string name="reco_tree") {tree_->Write(name.c_str());}
    void AddFriend(string friend_name="wf_tree") {tree_->AddFriend(friend_name.c_str());}

    TTree* tree_; 

    int*         index;
    uint64       start_time;
    uint64       time_stamp;
    unsigned int run;
    unsigned int spill;
    unsigned int event;   
    unsigned int n_channels;
    int*         channels;
    float*       b_charge;
    float*       b_slope;
    float*       b_rms;
    float*       time;
    float*       time_chi2;
    float*       maximum;
    float*       amp_max;
    float*       charge_tot;
    float*       charge_sig;
    float*       fit_ampl;
    float*       fit_time;
    float*       fit_chi2;
    int          n_wchamber;
    float*       wireX;
    float*       wireY;
    float        hodoX1;
    float        hodoY1;
    float        hodoX2;
    float        hodoY2;
};

#endif
