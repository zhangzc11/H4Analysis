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
//----------Tree reader class-------------------------------------------------------------
class H4Tree
{
public: 
    //---ctors---
    H4Tree() {};
    H4Tree(TChain* tree);
    //---dtor---
    ~H4Tree();

    //---utils---
    inline uint64 GetEntries(){return tree_->GetEntries();}
    bool          NextEvt(int64_t entry=-1);

    TChain* tree_; 
    int     currentEntry_;
    
    uint64        evtTime;
    unsigned int  runNumber;
    unsigned int  spillNumber;
    unsigned int  evtNumber;
    unsigned int  nAdcChannels;
    unsigned int* adcBoard;
    unsigned int* adcChannel;
    unsigned int* adcData;
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

#endif 
