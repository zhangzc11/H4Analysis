#ifndef __CFG_MANAGER__
#define __CFG_MANAGER__

#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <algorithm> 
#include <iomanip>

#include "TObject.h"

using namespace std;

class CfgManager: public TObject
{
public:
    //---ctors---
    CfgManager() {};
    CfgManager(map<string, map<string, vector<string> > >* defaultCfg) {SetDefaultCfg(defaultCfg);};
    CfgManager(const char* file) {ParseConfigFile(file);};
    //---dtor---
    ~CfgManager() {};

    //---getters---
    template<typename T> T GetOpt(string block, string key, int opt=0);

    //---setters---
    inline void            SetDefaultCfg(map<string, map<string, vector<string> > >* defaultCfg)
        {opts_ = *defaultCfg;};

    //---utils---
    void                   Errors(string block, string key, int opt=0);
    inline void            ParseConfigFile(string* file) {ParseConfigFile(file->c_str());};
    void                   ParseConfigFile(const char* file);
    virtual void           Print(Option_t* option="") const;

    //---operators---
    friend ostream& operator<<(ostream& out, const CfgManager& obj);

private:
    map<string, map<string, vector<string> > >  opts_;

    //---ROOT dictionary
    ClassDef(CfgManager, 1)
};

#endif
