#include "PluginBase.h"

class DigitizerReco: public PluginBase
{
private:
    static PluginRegister<DigitizerReco> regestry;
    int ciccio;
};

DEFINE_PLUGIN(DigitizerReco);
