#ifndef __H4_TREE__
#define __H4_TREE__

#include <string>

#include "TChain.h"
#include "TString.h"

using namespace std;

typedef unsigned long int uint32;
typedef unsigned long long int uint64;
 
//****************************************************************************************

class H4Tree
{
public: 
    //---ctors---
    H4Tree() {};
    H4Tree(TChain* tree);
    //---dtor---
    ~H4Tree() {};

    //---utils---
    inline uint64 GetEntries(){return tree_->GetEntries();}
    bool          NextEvt(int64_t entry=-1);

    TChain* tree_; 
    int     currentEntry_;
    
    uint64        evtTime;
    unsigned int  runNumber;
    unsigned int  spillNumber;
    unsigned int  evtNumber;
    unsigned int  nPatterns;
    unsigned int* pattern;
    unsigned int* patternBoard;
    unsigned int* patternChannel;
    unsigned int  nDigiSamples;
    unsigned int* digiGroup;
    unsigned int* digiChannel;
    float*        digiSampleValue;
};

//---avoid multiple declaration and linking problems
#ifdef __MAIN__

H4Tree::H4Tree(TChain* tree)
{
    tree_ = tree;
    currentEntry_=-1;

    //---global branches
    evtTime = 0;
    runNumber = 0;
    spillNumber = 0;
    evtNumber =0;

    //---size branches
    nDigiSamples = 0;    
    nPatterns = 0;
    tree_->SetBranchAddress("nPatterns", &nPatterns);
    tree_->SetBranchAddress("nDigiSamples", &nDigiSamples);
    tree_->GetEntry(0);

    //---HODO branches
    pattern = new unsigned int[nPatterns];
    patternBoard = new unsigned int[nPatterns];
    patternChannel = new unsigned int[nPatterns];
    tree_->SetBranchAddress("pattern", pattern);
    tree_->SetBranchAddress("patternBoard", patternBoard);
    tree_->SetBranchAddress("patternChannel", patternChannel);
    
    //---digitizer branches
    digiGroup = new unsigned int[nDigiSamples];
    digiChannel = new unsigned int[nDigiSamples];
    digiSampleValue = new float[nDigiSamples];
    tree_->SetBranchAddress("evtTime", &evtTime);
    tree_->SetBranchAddress("runNumber", &runNumber);
    tree_->SetBranchAddress("spillNumber", &spillNumber);
    tree_->SetBranchAddress("evtNumber", &evtNumber);
    tree_->SetBranchAddress("digiGroup", digiGroup);
    tree_->SetBranchAddress("digiChannel", digiChannel);
    tree_->SetBranchAddress("digiSampleValue", digiSampleValue);
}

bool H4Tree::NextEvt(int64_t entry)
{
    if(entry > -1)
        currentEntry_ = entry;
    
    ++currentEntry_;    
    if(currentEntry_ < tree_->GetEntriesFast())
    {        
        tree_->GetEntry(currentEntry_);
        return true;
    }
    
    currentEntry_=-1;
    return false;
}

#endif //__MAIN__

#endif //__H4_TREE__
