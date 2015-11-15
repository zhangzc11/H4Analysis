#include "CfgManager.h"

//**********getters***********************************************************************

//----------get option by name------------------------------------------------------------
template<typename T>
T CfgManager::GetOpt(string key, int opt)
{
    key = "opts."+key;    
    T opt_val;
    istringstream buffer(opts_[key][opt]);
    buffer >> opt_val;
    
    return opt_val;
}

template<> bool CfgManager::GetOpt(string key, int opt)
{
    key = "opts."+key;
    if(!OptExist(key))
        return false;
    else if(opts_[key][opt].size()==1)
    {        
        bool opt_val;
        istringstream buffer(opts_[key][opt]);
        buffer >> opt_val;

        return opt_val;
    }

    return true;
}   

template<> vector<float> CfgManager::GetOpt(string key, int opt)
{
    key = "opts."+key;
    Errors(key, opt);
    vector<float> optsVect;
    for(int iOpt=0; iOpt<opts_[key].size(); ++iOpt)
    {
        double opt_val;
        istringstream buffer(opts_[key][iOpt]);
        buffer >> opt_val;
        optsVect.push_back(opt_val);
    }
    
    return optsVect;
}    

template<> vector<string>& CfgManager::GetOpt(string key, int opt)
{
    key = "opts."+key;
    Errors(key, opt);
    
    return opts_[key];
}    
