#include "HodoBTFReco.h"

//**********Utils*************************************************************************
//----------Begin*************************************************************************
bool HodoBTFReco::Begin(CfgManager& opts, uint64* index)
{

    //---create a position tree
    RegisterSharedData(new TTree("hodo", "hodo_tree"), "hodo_tree", true);
    hodoTree_ = PositionTree(index, (TTree*)data_.back().obj, 2);
    hodoTree_.Init();

    return true;
}

bool HodoBTFReco::ProcessEvent(const H4Tree& h4Tree, map<string, PluginBase*>& plugins, CfgManager& opts)
{

    return true;
}
