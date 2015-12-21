#ifndef __PLUGIN_BASE__
#define __PLUGIN_BASE__

#include <string>
#include <map>

#include "TObject.h"

#include "interface/CfgManager.h"
#include "interface/CfgManagerT.h"
#include "interface/H4Tree.h"

using namespace std;

//**********SHARED DATAFORMAT*************************************************************
struct SharedData
{    
    bool     permanent;
    string   tag;
    TObject* obj;
};

//**********PLUGIN BASE CLASS*************************************************************
class PluginBase
{
public:
    //---ctors---
    PluginBase(){};

    //---dtor---
    ~PluginBase(){};    

    //---setters---
    void SetInstanceName(string const& instance){instanceName_=instance;};    

    //---getters---
    string             GetInstanceName(){return instanceName_;};
    vector<SharedData> GetSharedData(string tag="", string type="", bool permanent=true);
    
    //---utils---
    virtual bool Begin(CfgManager& opts, uint64* index){};
    virtual bool ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts){};
    virtual bool End(CfgManager& opts){};

protected:
    //---utils---
    void RegisterSharedData(TObject* obj, string tag, bool isPermanent);

protected:
    //---keep track of the plugin name as defined in the cfg
    string instanceName_;
    //---shared data
    vector<SharedData> data_;
};

//**********REFLECTION********************************************************************
template<typename T> PluginBase* CreatePlugin() { return new T; }

struct PluginBaseFactory
{
    typedef map<string, PluginBase*(*)()> pluginsMap;

    static PluginBase* CreateInstance(string const& name)
        {
            pluginsMap::iterator it = GetMap()->find(name);
            if(it == GetMap()->end())
                return 0;
            return it->second();
        }

protected:
    static pluginsMap* GetMap()
        {
            if(!map_)
                map_ = new pluginsMap;
            return map_;
        }

private:
    static pluginsMap* map_;
};

template<typename T>
struct PluginRegister : PluginBaseFactory
{
    PluginRegister(string const& name)
        {
            GetMap()->insert(make_pair(name, &CreatePlugin<T>));
        }
};

#define PLUGIN_REGISTER(NAME) \
    static PluginRegister<NAME> registry;

#define DEFINE_PLUGIN(NAME) \
    PluginRegister<NAME> NAME::registry(#NAME)

#endif
