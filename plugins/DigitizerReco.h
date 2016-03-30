#ifndef __DIGITIZER_RECO__
#define __DIGITIZER_RECO__

#include <iostream>

#include "interface/PluginBase.h"
#include "interface/DigiTree.h"
#include "interface/WFTree.h"
#include "interface/WFClass.h"
#include "interface/WFClassNINO.h"
#include "interface/WFViewer.h"

class DigitizerReco: public PluginBase
{
public:
    //---ctors---
    DigitizerReco(){};

    //---dtor---
    ~DigitizerReco(){};

    //---utils---
    bool Begin(CfgManager& opts, uint64* index);
    bool ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts);
    
private:    
    //---internal data
    int                         nSamples_;
    float                       tUnit_;
    vector<string>              channelsNames_;
    map<string, vector<float> > timeOpts_;
    DigiTree                    recoTree_;
    WFTree                      outWFTree_;
    map<string, WFClass*>       WFs;
    map<string, WFViewer>       WFViewers;
    map<string, TH1*>           templates;
};

DEFINE_PLUGIN(DigitizerReco);

#endif
