#include "interface/H4Tree.h"

H4Tree::H4Tree(TChain* tree)
{
    tree_ = tree;
    currentEntry_=-1;

    //---global branches
    evtTime = 0;
    runNumber = 0;
    spillNumber = 0;
    evtNumber =0;
    tree_->SetBranchAddress("evtTime", &evtTime);
    tree_->SetBranchAddress("runNumber", &runNumber);
    tree_->SetBranchAddress("spillNumber", &spillNumber);
    tree_->SetBranchAddress("evtNumber", &evtNumber);

    //---size branches
    nAdcChannels = 0;
    nPatterns = 0;
    nDigiSamples = 0;
    tree_->SetBranchAddress("nAdcChannels", &nAdcChannels);
    tree_->SetBranchAddress("nPatterns", &nPatterns);
    tree_->SetBranchAddress("nDigiSamples", &nDigiSamples);
    tree_->GetEntry(0);

    //---ADC branches
    adcBoard = new unsigned int[nAdcChannels]{0};
    adcChannel = new unsigned int[nAdcChannels]{0};
    adcData = new unsigned int[nAdcChannels]{0};
    tree_->SetBranchAddress("adcBoard", adcBoard);
    tree_->SetBranchAddress("adcChannel", adcChannel);
    tree_->SetBranchAddress("adcData", adcData);

    //---Wire chambers branches
    nTdcChannels = 0;
    tdcChannel = new unsigned int[MAX_TDC_CHANNELS]{0};
    tdcData = new unsigned int[MAX_TDC_CHANNELS]{0};
    tree_->SetBranchAddress("nTdcChannels", &nTdcChannels);
    tree_->SetBranchAddress("tdcChannel", tdcChannel);
    tree_->SetBranchAddress("tdcData", tdcData);
    
    //---Pattern branches
    pattern = new unsigned int[nPatterns]{0};
    patternBoard = new unsigned int[nPatterns]{0};
    patternChannel = new unsigned int[nPatterns]{0};
    tree_->SetBranchAddress("pattern", pattern);
    tree_->SetBranchAddress("patternBoard", patternBoard);
    tree_->SetBranchAddress("patternChannel", patternChannel);
    
    //---digitizer branches
    digiGroup = new unsigned int[nDigiSamples]{0};
    digiChannel = new unsigned int[nDigiSamples]{0};
    digiSampleValue = new float[nDigiSamples]{0};
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

H4Tree::~H4Tree()
{
    delete[] tdcChannel;
    delete[] tdcData;
    delete[] pattern;
    delete[] patternBoard;
    delete[] patternChannel;
    delete[] digiGroup;
    delete[] digiChannel;
    delete[] digiSampleValue;
}

bool H4Tree::NextEvt(int64_t entry)
{
    if(entry > -1)
        currentEntry_ = entry;
    
    ++currentEntry_;    
    if(currentEntry_ < tree_->GetEntries())
    {
        tree_->GetEntry(currentEntry_);
        return true;
    }
    
    currentEntry_=-1;
    return false;
}
