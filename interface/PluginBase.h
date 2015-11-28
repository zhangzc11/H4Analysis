#include <string>
#include <map>

using namespace std;

class PluginBase
{
private:
    int hola;
};

template<typename T> PluginBase * CreatePlugin() { return new T; }

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
    static PluginRegister<NAME> regestry

#define DEFINE_PLUGIN(NAME) \
    PluginRegister<NAME> NAME::regestry(#NAME)
