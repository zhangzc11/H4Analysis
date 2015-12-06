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
    WFTree(){};
    WFTree(int nCh, int nSamples, int* idx, TTree* tree=NULL, string suffix="");
    //---dtor---
    ~WFTree(){};

    //---utils---
    void Init();
    void Fill() {tree_->Fill();};
    void Write(const char* name="wf_tree", const char* title="wf_tree")
        {tree_->BuildIndex("index"); tree_->SetTitle(title); tree_->Write(name);};

    TTree* tree_; 
    string suffix_;

    int*   index;
    uint64 time_stamp;
    int    WF_samples;
    int*   WF_ch; 
    float* WF_time;
    float* WF_val;
};

#endif
