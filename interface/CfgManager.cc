#include "../interface/CfgManager.h"

//**********getters***********************************************************************

//----------get option by name------------------------------------------------------------
template<> double CfgManager::GetOpt(string block, string key, int opt)
{
    Errors(block, key, opt);
    
    double opt_val;
    istringstream buffer(opts_[block].at(key).at(opt));
    buffer >> opt_val;
    
    return opt_val;
}

template<> float CfgManager::GetOpt(string block, string key, int opt)
{
    return (float)GetOpt<double>(block, key, opt);
}

template<> int CfgManager::GetOpt(string block, string key, int opt)
{
    return (int)GetOpt<double>(block, key, opt);
}

template<> string CfgManager::GetOpt(string block, string key, int opt)
{
    Errors(block, key, opt);
    return opts_[block].at(key).at(opt);
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

//**********utils*************************************************************************

void CfgManager::Errors(string block, string key, int opt)
{
    if(opts_.count(block) == 0)
    {
        cout << "> CfgManager --- ERROR: block '"<< block << "' not found" << endl;
        exit(-1);
    }
    if(opts_[block].count(key) == 0)
    {
        cout << "> CfgManager --- ERROR: option '"<< key << "' not found" << endl;
        exit(-1);
    }
    if(opt >= opts_[block].at(key).size())
    {
        cout << "> CfgManager --- ERROR: option '"<< key << "' as less then "
             << opt << "values (" << opts_[block].at(key).size() << ")" << endl;
        exit(-1);
    }
    return;
}

void CfgManager::ParseConfigFile(const char* file)
{
    ifstream cfgFile(file, ios::in);
    string buffer;
    string current_block="global";
    map<string, vector<string> > block_opts;
    while(getline(cfgFile, buffer))
    {
        if(buffer.size() == 0 || buffer.at(0) == '#')
            continue;        
        istringstream splitter(buffer);
        vector<string> tokens = vector<string>(istream_iterator<string>(splitter), 
                                               istream_iterator<string>());
        //---new block
        if(tokens.at(0).at(0) == '<')
        {
            if(tokens.at(0).at(1) == '/')
            {
                tokens.at(0).erase(tokens.at(0).begin(), tokens.at(0).begin()+2);
                tokens.at(0).erase(--tokens.at(0).end());
                if(tokens.at(0) == current_block)
                {
                    for(auto& opt : block_opts)
                        opts_[current_block][opt.first] = opt.second;
                    block_opts.clear();
                    current_block = "global";
                }
                else
                    cout << "> CfgManager --- ERROR: wrong closing block // " << tokens.at(0) << endl;
            }
            else
            {
                tokens.at(0).erase(tokens.at(0).begin());
                tokens.at(0).erase(--tokens.at(0).end());
                current_block = tokens.at(0);
            }
        }
        //---import cfg
        else if(tokens.at(0) == "importCfg")
        {
            tokens.erase(tokens.begin());
            for(auto& cfgFile: tokens)
                ParseConfigFile(cfgFile.c_str());
        }
        //---add key
        else 
        {
            string key=tokens.at(0);
            tokens.erase(tokens.begin());
            block_opts[key] = tokens;
        }
    }
    cfgFile.close();
}

//**********operators*********************************************************************

ostream& operator<<(ostream& out, const CfgManager& obj)
{
    map<string, map<string, vector<string> > >::const_iterator itBlock;
    //---banner
    out << "current configuration:" << endl;
    //---options
    for(itBlock=obj.opts_.begin(); itBlock!=obj.opts_.end(); ++itBlock)
    {
        out << setw(20) << itBlock->first;
        map<string, vector<string> >::const_iterator itOpt;
        for(itOpt=itBlock->second.begin(); itOpt!=itBlock->second.end(); ++itOpt)
        {
            for(int iOpt=0; iOpt<itOpt->second.size(); ++iOpt)
            {
                out << setw(itOpt->first.size()+3) << itOpt->first;
                out << setw(itOpt->second.at(iOpt).size()+3) << itOpt->second.at(iOpt);
                out << endl;
            }
        }
        out << endl;
    }
    return out;
}
