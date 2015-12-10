#ifndef __WIRE_RECO__
#define __WIRE_RECO__

#include "PluginBase.h"
#include "interface/PositionTree.h"

using namespace std;

class WireReco: public PluginBase
{
public:
    //---ctors---
    WireReco() {};

    //---dtor---
    ~WireReco() {};

    //---utils---
    bool Begin(CfgManager& opts, int* index);
    bool ProcessEvent(const H4Tree& event, CfgManager& opts);
    
private:
    PositionTree     wireTree_;
    int              chXl_;
    int              chXr_;
    int              chYu_;
    int              chYd_;

    //---datamember for registration
    PLUGIN_REGISTER(WireReco)
};

DEFINE_PLUGIN(WireReco);


#endif
