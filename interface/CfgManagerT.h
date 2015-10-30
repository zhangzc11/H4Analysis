#include "CfgManager.h"

//**********getters***********************************************************************

//----------get option by name------------------------------------------------------------
template<typename T>
T CfgManager::GetOpt(string block, string key, int opt)
{
    Errors(block, key, opt);
    
    T opt_val;
    istringstream buffer(opts_[block].at(key).at(opt));
    buffer >> opt_val;
    
    return opt_val;
}


template<> vector<float> CfgManager::GetOpt(string block, string key, int opt)
{
    Errors(block, key, opt);
    vector<float> optsVect;
    for(int iOpt=0; iOpt<opts_[block].at(key).size(); ++iOpt)
    {
        double opt_val;
        istringstream buffer(opts_[block].at(key).at(iOpt));
        buffer >> opt_val;
        optsVect.push_back(opt_val);
    }
    return optsVect;
}    

template<> vector<string>& CfgManager::GetOpt(string block, string key, int opt)
{
    Errors(block, key, opt);
    return opts_[block].at(key);
}    
