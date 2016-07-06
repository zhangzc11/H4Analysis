#ifndef __FFT_Analyzer__
#define __FFT_Analyzer__

#include <iostream>
#include <math.h>

#include "TVirtualFFT.h"

#include "interface/PluginBase.h"
#include "interface/DigiTree.h"
#include "interface/WFTree.h"
#include "interface/WFClass.h"
#include "interface/WFClassNINO.h"

class FFTAnalyzer: public PluginBase
{
public:
    //---ctors---
    FFTAnalyzer(){};

    //---dtor---
    ~FFTAnalyzer(){};

    //---utils---
    bool Begin(CfgManager& opts, uint64* index);
    bool ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts);
    
private:    
    //---internal data
    unsigned int     n_tot_;
    unsigned int     n_freqs_;
    int*             current_ch_;
    float*           freqs_;
    float*           amplitudes_;
    float*           phases_;
    string           digiInstance_;
    vector<string>   channelsNames_;
    map<string, int> channelsMap_;
    TTree*           fftTree_;         
};

DEFINE_PLUGIN(FFTAnalyzer);

#endif
