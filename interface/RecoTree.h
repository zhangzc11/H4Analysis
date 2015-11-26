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
    RecoTree(int nCh, int* idx, string prefix="");
    //---dtor---
    ~RecoTree();

    //---utils---
    void Fill() {tree_->Fill();};
    void Write(const char* name="reco_tree", const char* title="reco_tree")
        {tree_->BuildIndex("index"); tree_->SetTitle(title); tree_->Write(name);};
    void AddFriend(const char* friend_name="wf_tree = wf") {tree_->AddFriend(friend_name, "");};
    
    TTree* tree_; 
    string prefix_;

    int*         index;
    uint64       time_stamp;
    unsigned int n_channels;
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
};

#endif
