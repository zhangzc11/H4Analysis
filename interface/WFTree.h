#ifndef __WF_TREE__
#define __WF_TREE__

#include <string>
#include <vector>

#include "TTree.h"
#include "TString.h"

using namespace std;

typedef unsigned long int uint32;
typedef unsigned long long int uint64;
 
//****************************************************************************************

class WFTree
{
public: 
    //---ctors---
    WFTree(int nCh, int nSamples, int* idx);
    //---dtor---
    ~WFTree();

    //---utils---
    void Fill() {tree_->Fill();};
    void Write(string name="wf_tree") {tree_->BuildIndex("index"); tree_->Write(name.c_str());}

    TTree* tree_; 

    int*   index;
    uint64 time_stamp;
    int    WF_samples;
    int*   WF_ch; 
    float* WF_time;
    float* WF_val;
};

WFTree::WFTree(int nCh, int nSamples, int* idx)
{
    tree_ = new TTree();

    index=idx;
    time_stamp=0;
    //---set total number of WF samples 
    WF_samples = nSamples*nCh;
    WF_ch = new int[WF_samples];
    WF_time = new float[WF_samples];
    WF_val = new float[WF_samples];
    //---global branches
    tree_->Branch("index", index, "index/I");
    tree_->Branch("time_stamp", &time_stamp, "time_stamp/l");
    tree_->Branch("WF_samples", &WF_samples, "WF_samples/I");
    tree_->Branch("WF_ch", WF_ch, "WF_ch[WF_samples]/I");
    tree_->Branch("WF_time", WF_time, "WF_time[WF_samples]/F");
    tree_->Branch("WF_val", WF_val, "WF_val[WF_samples]/F");
}

WFTree::~WFTree()
{
    delete[] WF_ch; 
    delete[] WF_time;
    delete[] WF_val;

    delete tree_;
}

#endif
