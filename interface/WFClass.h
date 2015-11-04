#ifndef __WFCLASS_H__
#define __WFCLASS_H__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

#include "Math/Interpolator.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TMath.h"
#include "TH1F.h"
#include "TF1.h"

using namespace std;

#define MAX_INTERPOLATOR_POINTS 10000

struct WFFitResults
{
    double ampl;
    double time;
    double chi2;
};      

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
    inline float          GetBaseline() {return baseline_;}
    float                 GetAmpMax(int min=-1, int max=-1);
    float                 GetInterpolatedAmpMax(int min=-1, int max=-1, int nFitSamples=7);
    float                 GetTime(string method, vector<float>& params); 
    float                 GetTimeCF(float frac, int nFitSamples=5, int min=-1, int max=-1);
    float                 GetTimeLE(float thr, int nmFitSamples=1, int npFitSamples=3, int min=-1, int max=-1);
    float                 GetChi2(string type);
    float                 GetIntegral(int min=-1, int max=-1);
    float                 GetSignalIntegral(int riseWin, int fallWin);
    float                 GetModIntegral(int min=-1, int max=-1);
    float                 GetBaselineRMS();
    //---setters---
    void                  SetSignalWindow(int min, int max);
    void                  SetBaselineWindow(int min, int max);
    void                  SetTemplate(TH1* templateWF=NULL, int lW=0, int hW=0);
    //---utils---
    void                  AddSample(float sample) {samples_.push_back(polarity_*sample);};
    bool                  SubtractBaseline(int min=-1, int max=-1);
    WFFitResults          TemplateFit();

private:
    //---utils---
    double                TemplateChi2(const double* par=NULL);
    
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
    float         chi2cf_;
    float         chi2le_;
    int           fWinMin_;
    int           fWinMax_;
    float         tempFitTime_;
    float         tempFitAmp_;
    ROOT::Math::Interpolator* interpolator_;
};

#endif
