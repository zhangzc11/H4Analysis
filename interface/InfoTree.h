#ifndef __INFO_TREE__
#define __INFO_TREE__

#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"

using namespace std;

typedef unsigned long int uint32;
typedef unsigned long long int uint64;
 
//****************************************************************************************

class InfoTree
{
public: 
    //---ctors---
    InfoTree(){};
    InfoTree(int* idx, TTree* tree=NULL);
    //---dtor---
    ~InfoTree();

    //---utils---
    void Fill() {tree_->Fill();};
    void Write(const char* name="h4", const char* title="main_h4reco_tree")
        {tree_->BuildIndex("index"); tree_->SetTitle(title); tree_->Write(name);};
    void AddFriend(const char* friend_name="wf_tree = wf") {tree_->AddFriend(friend_name, "");};

    TTree* tree_; 

    int*         index;
    uint64       start_time;
    uint64       time_stamp;
    unsigned int run;
    unsigned int spill;
    unsigned int event;       
    // int          n_wchamber;
    // float*       wireX;
    // float*       wireY;
    // float        hodoX1;
    // float        hodoY1;
    // float        hodoX2;
    // float        hodoY2;
};

#endif
