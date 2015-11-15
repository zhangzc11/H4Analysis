#ifndef __H4_TREE__
#define __H4_TREE__

#include <map>
#include <string>

#include "TChain.h"
#include "TString.h"

using namespace std;

#define MAX_TDC_CHANNELS 20

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
    unsigned int  nTdcChannels;
    unsigned int* tdcChannel;
    unsigned int* tdcData;
    unsigned int  nPatterns;
    unsigned int* pattern;
    unsigned int* patternBoard;
    unsigned int* patternChannel;
    unsigned int  nDigiSamples;
    unsigned int* digiGroup;
    unsigned int* digiChannel;
    float*        digiSampleValue;

    map<pair<int, int>, int> digiMap;
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
    nPatterns = 0;
    nDigiSamples = 0;
    tree_->SetBranchAddress("nPatterns", &nPatterns);
    tree_->SetBranchAddress("nDigiSamples", &nDigiSamples);
    tree_->GetEntry(0);

    //---Wire chambers branches
    nTdcChannels = 0;
    tdcChannel = new unsigned int[MAX_TDC_CHANNELS];
    tdcData = new unsigned int[MAX_TDC_CHANNELS];
    tree_->SetBranchAddress("nTdcChannels", &nTdcChannels);
    tree_->SetBranchAddress("tdcChannel", tdcChannel);
    tree_->SetBranchAddress("tdcData", tdcData);
    
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
    //---fill map< <group, channel>, pointer to first sample>
    tree_->GetEntry(0);
    int currentDigiGroup=-1, currentDigiChannel=-1;
    for(unsigned int iSample=0; iSample<nDigiSamples; ++iSample)
    {
        if(digiChannel[iSample] != currentDigiChannel)
        {
            currentDigiChannel = digiChannel[iSample];
            if(digiGroup[iSample] != currentDigiGroup)
                currentDigiGroup = digiGroup[iSample];
            digiMap[make_pair(currentDigiGroup, currentDigiChannel)]=iSample;
        }
    }
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
