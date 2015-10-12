#ifndef __WFCLASS_H__
#define __WFCLASS_H__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

#include "TH1F.h"
#include "TF1.h"

using namespace std;

class WFClass 
{
public:
    //---ctors---
    WFClass() {};
    WFClass(int polarity, float tUnit);
    //---dtor---
    ~WFClass() {};

    //---getters---
    inline vector<float>* GetSamples() {return &samples_;};
    inline float          GetChi2() {return chi2_;};
    inline float          GetBaseline() {return baseline_;}
    float                 GetAmpMax(int min=-1, int max=-1);
    float                 GetInterpolatedAmpMax(int min=-1, int max=-1, int nFitSamples=7);
    float                 GetTime(string method, vector<float>& params); 
    float                 GetTimeCF(float frac, int nFitSamples=5, int min=-1, int max=-1);
    float                 GetTimeLE(float thr, int nmFitSamples=1, int npFitSamples=3, int min=-1, int max=-1); 
    float                 GetIntegral(int min=-1, int max=-1);
    float                 GetSignalIntegral(int riseWin, int fallWin);
    float                 GetModIntegral(int min=-1, int max=-1);
    float                 GetBaselineRMS();
    //---setters---
    void                  SetSignalWindow(int min, int max);
    void                  SetBaselineWindow(int min, int max);
    //---utils---
    void                  AddSample(float sample) {samples_.push_back(polarity_*sample);};
    bool                  SubtractBaseline(int min=-1, int max=-1);

private:
    vector<float> samples_;
    float         tUnit_;
    int           polarity_;
    int           sWinMin_;
    int           sWinMax_;
    int           bWinMin_;
    int           bWinMax_;
    int           maxSample_;
    float         fitAmpMax_;
    float         baseline_;
    float         bRMS_;
    int           cfSample_;
    float         cfFrac_;
    float         cfTime_;
    int           leSample_;
    float         leThr_;
    float         leTime_;
    float         chi2_;
};

#endif
