#ifndef __T1065Reco__
#define __T1065Reco__

#include <iostream>

#include "interface/PluginBase.h"
#include "interface/DigiTree.h"
#include "interface/T1065Tree.h"
#include "interface/WFTree.h"
#include "interface/WFClass.h"
#include "interface/WFClassNINO.h"
#include "interface/WFViewer.h"

class T1065Reco: public PluginBase
{
public:
    //---ctors---
    T1065Reco(){};

    //---dtor---
    ~T1065Reco(){};

    //---utils---
    bool Begin(CfgManager& opts, uint64* index);
    bool ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts);
    
private:    
    //---internal data
    string                      srcInstance_;
    vector<string>              channelsNames_;
    vector<string>              timeRecoTypes_;
    map<string, vector<float> > timeOpts_;
    DigiTree                    digiTree_;
    T1065Tree                   t1065Tree_;
    WFTree                      outWFTree_;
    map<string, WFClass*>       WFs_;
    map<string, TH1*>           templates_;
    int                         eventCount_;
};

DEFINE_PLUGIN(T1065Reco);

#endif
