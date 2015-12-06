#include "PluginBase.h"

//**********Getters***********************************************************************
vector<SharedData> PluginBase::GetSharedData(string tag, string type, bool permanent)
{
    vector<SharedData> outData;
    for(auto& data : data_)
    {
        if(data.obj && data.permanent==permanent && (tag=="" || data.tag==tag) &&
           (type=="" || type==data.obj->IsA()->GetName()))
            outData.push_back(data);
    }

    return outData;
}

//**********Utils (private)***************************************************************
void PluginBase::RegisterSharedData(TObject* obj, string tag, bool isPermanent)
{
    if(obj)
        data_.push_back(SharedData{isPermanent, this->instanceName_+"_"+tag, obj});

    return;
}


