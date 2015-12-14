#include "HodoBTFReco.h"

//**********Utils*************************************************************************
//----------Begin*************************************************************************
bool HodoBTFReco::Begin(CfgManager& opts, int* index)
{

    //---create a position tree
    RegisterSharedData(new TTree("hodo", "hodo_tree"), "hodo_tree", true);
    hodoTree_ = PositionTree(index, (TTree*)data_.back().obj, 2);
    hodoTree_.Init();

    return true;
}

bool HodoBTFReco::ProcessEvent(const H4Tree& h4Tree, CfgManager& opts)
{

    return true;
}
