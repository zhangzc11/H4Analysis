#ifndef __MAKE_COVARIANCE_MATRIX__
#define __MAKE_COVARIANCE_MATRIX__

#include <iostream>

#include "TH2F.h"

#include "interface/WFClass.h"
#include "plugin/PluginBase.h"

class MakeCovarianceMatrix: public PluginBase
{
public:
    //---ctors---
    MakeCovarianceMatrix(){};

    //---dtor---
    ~MakeCovarianceMatrix(){};

    //---utils---
    bool Begin(CfgManager& opts, int* index);
    bool ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts);
    bool End(CfgManager& opts);
    
private:    
    //---internal data
    int                            events_;
    int                            firstSample_;
    int                            lastSample_;
    string                         digiInstance_;             
    vector<string>                 channelsNames_;
    map<string, TH2F>              mapCovariances_;
    map<string, vector<float> >            sums_;
    map<string, vector<float> >            sum2s_;
    map<string, map<int, vector<float> > > values_;

    //---datamember for registration
    PLUGIN_REGISTER(MakeCovarianceMatrix);
};

DEFINE_PLUGIN(MakeCovarianceMatrix);

#endif
