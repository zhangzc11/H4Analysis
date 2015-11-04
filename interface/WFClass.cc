#include "../interface/WFClass.h"

//**********Constructors******************************************************************
WFClass::WFClass(int polarity, float tUnit):
    polarity_(polarity), tUnit_(tUnit), sWinMin_(-1), sWinMax_(-1), 
    bWinMin_(-1), bWinMax_(-1),  maxSample_(-1), fitAmpMax_(-1), baseline_(-1), bRMS_(-1),
    cfSample_(-1), cfFrac_(-1), cfTime_(-1), chi2cf_(-1), chi2le_(-1),
    fWinMin_(-1), fWinMax_(-1), tempFitTime_(-1), tempFitAmp_(-1)
{}
//**********Getters***********************************************************************

//----------Get the max/min amplitude wrt polarity----------------------------------------
float WFClass::GetAmpMax(int min, int max)
{
    //---check if signal window is valid
    if(min==max && max==-1 && sWinMin_==sWinMax_ && sWinMax_==-1)
        return -1;
    //---setup signal window
    if(min!=-1 && max!=-1)
        SetSignalWindow(min, max);
    //---return the max if already computed
    else if(maxSample_ != -1)
        return samples_.at(maxSample_);

    //---find the max
    maxSample_=sWinMin_;
    for(int iSample=sWinMin_; iSample<sWinMax_; iSample++)
    {
        if(samples_.at(iSample) > samples_.at(maxSample_)) 
	    maxSample_ = iSample;
    }    
    return samples_.at(maxSample_);
}

//----------Get the interpolated max/min amplitude wrt polarity---------------------------
float WFClass::GetInterpolatedAmpMax(int min, int max, int nFitSamples)
{
    //---check if already computed
    if(min==-1 && max==-1 && fitAmpMax_!=-1)
        return fitAmpMax_;
    //---check if signal window is valid
    if(min==max && max==-1 && sWinMin_==sWinMax_ && sWinMax_==-1)
        return -1;
    //---setup signal window
    if(min!=-1 && max!=-1)
        SetSignalWindow(min, max);
    //---return the max if already computed
    else if(maxSample_ == -1)
        GetAmpMax(min, max);

    //---fit the max
    TH1F h_max("h_max", "", nFitSamples, maxSample_-nFitSamples/2, maxSample_+nFitSamples/2);
    TF1 f_max("f_max", "pol2", maxSample_-nFitSamples/2, maxSample_+nFitSamples/2);

    int bin=1;
    for(int iSample=maxSample_-nFitSamples/2; iSample<=maxSample_+nFitSamples/2; ++iSample)
    {
        h_max.SetBinContent(bin, samples_[iSample]);
        h_max.SetBinError(bin, GetBaselineRMS());
        ++bin;
    }
    h_max.Fit(&f_max, "QR");

    return fitAmpMax_ = f_max.Eval(-f_max.GetParameter(1)/(2*f_max.GetParameter(2)));
}

//----------Get time with the specified method--------------------------------------------
float WFClass::GetTime(string method, vector<float>& params)
{
    //---CFD
    if(method == "CFD")
    {
        if(params.size()<1)
            cout << ">>>ERROR: to few arguments passed for CFD time computation" << endl;
        else if(params.size()<2)
            return GetTimeCF(params[0]);
        else if(params.size()<3)
            return GetTimeCF(params[0], params[1]);
        else
            return GetTimeCF(params[0], params[1], params[2], params[3]);

    }
    //---LED
    else if(method == "LED")
    {
        if(params.size()<1)
            cout << ">>>ERROR: to few arguments passed for LED time computation" << endl;
        else if(params.size()<2)
            return GetTimeLE(params[0]);
        else if(params.size()<4)
            return GetTimeLE(params[0], params[1], params[2]);
        else
            return GetTimeLE(params[0], params[1], params[2], params[3], params[4]);

    }
    else
    {
        cout << ">>>ERROR: time reconstruction method <" << method << "> not supported" << endl;
        return -1000;
    }    
}

//----------Get CF time for a given fraction and in a given range-------------------------
float WFClass::GetTimeCF(float frac, int nFitSamples, int min, int max)
{
    if(frac == cfFrac_ && cfSample_ != -1)
        return cfTime_;

    //---definitions---
    float xx= 0.;
    float xy= 0.;
    float Sx = 0.;
    float Sy = 0.;
    float Sxx = 0.;
    float Sxy = 0.;
    //---setups---
    int tStart=min;
    if(tStart == -1)
        tStart=sWinMin_ == -1 ? 0 : sWinMin_;
    cfSample_ = tStart;
    cfFrac_ = frac;
    if(fitAmpMax_ == -1)
        GetInterpolatedAmpMax(min, max);
    if(frac == 1) 
        return maxSample_;
    
    //---find first sample above Amax*frac
    for(int iSample=maxSample_; iSample>tStart; --iSample)
    {
        if(samples_.at(iSample) < fitAmpMax_*frac) 
        {
            cfSample_ = iSample;
            break;
        }
    }

    //---compute sums
    int usedSamples=0;
    for(int n=-(nFitSamples-1)/2; n<=(nFitSamples-1)/2; ++n)
    {
        if(cfSample_+n<0 || cfSample_+n>=maxSample_) 
	    continue;
        xx = (cfSample_+n)*(cfSample_+n)*tUnit_*tUnit_;
        xy = (cfSample_+n)*tUnit_*(samples_.at(cfSample_+n));
        Sx = Sx + (cfSample_+n)*tUnit_;
        Sy = Sy + samples_.at(cfSample_+n);
        Sxx = Sxx + xx;
        Sxy = Sxy + xy;
        ++usedSamples;
    }
    
    float Delta = usedSamples*Sxx - Sx*Sx;
    float A = (Sxx*Sy - Sx*Sxy) / Delta;
    float B = (usedSamples*Sxy - Sx*Sy) / Delta;

    //---compute chi2---
    chi2cf_ = 0.;
    float sigma2 = pow(tUnit_/sqrt(12)*B,2);
    for(int n=-(nFitSamples-1)/2; n<=(nFitSamples-1)/2; n++)
    {
        if(cfSample_+n<0 || cfSample_+n>=maxSample_) 
            continue;
        chi2cf_ = chi2cf_ + pow(samples_.at(cfSample_+n) - A - B*((cfSample_+n)*tUnit_),2)/sigma2;
    } 

    //---A+Bx = frac * amp
    cfTime_ = (fitAmpMax_ * frac - A) / B;

    return cfTime_;
}

//----------Get leading edge time at a given threshold and in a given range---------------
float WFClass::GetTimeLE(float thr, int nmFitSamples, int npFitSamples, int min, int max)
{
    //---check if signal window is valid
    if(min==max && max==-1 && sWinMin_==sWinMax_ && sWinMax_==-1)
        return -1;
    //---setup signal window
    if(min!=-1 && max!=-1)
        SetSignalWindow(min, max);
    //---return time value if already computed
    if(thr == leThr_ && leSample_ != -1)
        return leTime_;

    //---definitions---
    float xx= 0.;
    float xy= 0.;
    float Sx = 0.;
    float Sy = 0.;
    float Sxx = 0.;
    float Sxy = 0.;
    
    //---find first sample above thr
    for(int iSample=sWinMin_; iSample>sWinMax_; ++iSample)
    {
        if(samples_.at(iSample) > thr) 
        {
            leSample_ = iSample;
            break;
        }
    }

    //---compute sums
    int usedSamples=0;
    for(int n=-nmFitSamples; n<=npFitSamples; ++n)
    {
        if(leSample_+n<0 || leSample_+n>=maxSample_) 
	    continue;
        xx = (leSample_+n)*(leSample_+n)*tUnit_*tUnit_;
        xy = (leSample_+n)*tUnit_*(samples_.at(leSample_+n));
        Sx = Sx + (leSample_+n)*tUnit_;
        Sy = Sy + samples_.at(leSample_+n);
        Sxx = Sxx + xx;
        Sxy = Sxy + xy;
        ++usedSamples;
    }
    
    float Delta = usedSamples*Sxx - Sx*Sx;
    float A = (Sxx*Sy - Sx*Sxy) / Delta;
    float B = (usedSamples*Sxy - Sx*Sy) / Delta;

    //---compute chi2---
    chi2le_ = 0.;
    float sigma2 = pow(tUnit_/sqrt(12)*B,2);
    for(int n=nmFitSamples; n<=npFitSamples; n++)
    {
        if(leSample_+n<0 || leSample_+n>=maxSample_) 
            continue;
        chi2le_ = chi2le_ + pow(samples_.at(leSample_+n) - A - B*((leSample_+n)*tUnit_),2)/sigma2;
    } 

    //---A+Bx = frac * amp
    leTime_ = (thr - A) / B;
    return leTime_;
}

//----------Get Chi2 of the specified time extraction algorithm---------------------------
float WFClass::GetChi2(string type)
{
    if(type == "CFD")
        return chi2cf_;
    else if(type == "LED")
        return chi2le_;
    else
        cout << ">>>ERROR: algorithm <" << type << "> is not supported" << endl;

    return -1;
}

//----------Get the waveform integral in the given range----------------------------------
float WFClass::GetIntegral(int min, int max)
{
    //---compute integral
    float integral=0;
    for(int iSample=min; iSample<max; iSample++)
        integral += samples_.at(iSample);

    return integral;
}

//----------Get the signal integral around the the max-------------------------------------
float WFClass::GetSignalIntegral(int riseWin, int fallWin)
{
    //---compute position of the max
    if(maxSample_ == -1)
        GetAmpMax();

    //---compute integral
    float integral=0;
    for(int iSample=maxSample_-riseWin; iSample<maxSample_+fallWin; iSample++)
    {
        //---if signal window goes out of bound return a bad value
        if(iSample > samples_.size() || iSample < 0)
            return -1000;        
        integral += samples_.at(iSample);
    }

    return integral;
}


//----------Get the integral of Abs(WF) over the given range------------------------------
float WFClass::GetModIntegral(int min, int max)
{   
    float integral=0;
    for(int iSample=min; iSample<max; iSample++)
    {
	if(samples_.at(iSample) < 0)
	    integral -= samples_.at(iSample);
	else
	    integral += samples_.at(iSample);
    }
    return integral;
}

//----------compute baseline RMS (noise)--------------------------------------------------
float WFClass::GetBaselineRMS()
{
    if(bRMS_ != -1)
        return bRMS_;

    int nSample=0;
    float sum=0, sum2=0;
    for(int iSample=bWinMin_; iSample<bWinMax_; iSample++)
    {
        ++nSample;
        sum += samples_[iSample];
        sum2 += samples_[iSample]*samples_[iSample];
    }

    return bRMS_=sqrt(sum2/nSample - pow(sum/nSample, 2));
}


//**********Setters***********************************************************************

//----------Set the signal window---------------------------------------------------------
void WFClass::SetSignalWindow(int min, int max)
{
    sWinMin_=min;
    sWinMax_=max;
}

//----------Set the baseline window-------------------------------------------------------
void WFClass::SetBaselineWindow(int min, int max)
{
    bWinMin_=min;
    bWinMax_=max;
}

//----------Set the fit template----------------------------------------------------------
void WFClass::SetTemplate(TH1* templateWF, int lW, int hW)
{
    //---check input
    if(!templateWF)
    {
        cout << ">>>ERROR: template passed as input do not exist" << endl;
        return;
    }
    
    //---reset template fit variables
    if(interpolator_)
        delete interpolator_;
    interpolator_ = new ROOT::Math::Interpolator(MAX_INTERPOLATOR_POINTS, ROOT::Math::Interpolation::kCSPLINE);
    tempFitTime_ = templateWF->GetBinCenter(templateWF->GetMaximumBin());
    tempFitAmp_ = -1;
    //---set template fit window around maximum, [min, max) --- template X scale is in ns.
    GetBaselineRMS();
    GetAmpMax();    
    fWinMin_ = maxSample_ - lW;
    fWinMax_ = maxSample_ + hW;
    
    //---fill interpolator data
    vector<double> x, y;
    for(int iBin=1; iBin<=templateWF->GetNbinsX(); ++iBin)
    {
	x.push_back(templateWF->GetBinCenter(iBin));
	y.push_back(polarity_*templateWF->GetBinContent(iBin));
    }
    interpolator_->SetData(x, y);

    return;
}

//**********Utils*************************************************************************

//---------estimate the baseline in a given range and then subtract it from the signal----
bool WFClass::SubtractBaseline(int min, int max)
{
    if(min!=-1 && max==-1)
    {
        bWinMin_=min;
        bWinMax_=max;
    }
    //---compute baseline
    float baseline_=0;
    for(int iSample=bWinMin_; iSample<bWinMax_; iSample++)
    {
        baseline_ += samples_.at(iSample);
    }
    baseline_ = baseline_/((float)(bWinMax_-bWinMin_));
    //---subtract baseline
    for(int iSample=0; iSample<samples_.size(); iSample++)
        samples_.at(iSample) = (samples_.at(iSample) - baseline_);
    
    return true;
}

//----------template fit to the WF--------------------------------------------------------
WFFitResults WFClass::TemplateFit()
{
    if(tempFitAmp_ == -1)
    {
        ROOT::Math::Minimizer* minimizer = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

        minimizer->SetMaxFunctionCalls(100000);
        minimizer->SetMaxIterations(100);
        minimizer->SetTolerance(1e-3);
        minimizer->SetPrintLevel(0);

        ROOT::Math::Functor chi2(this, &WFClass::TemplateChi2, 2);
        minimizer->SetFunction(chi2);
        minimizer->SetLimitedVariable(0, "amplitude", GetAmpMax(), 1e-2, 0., 4000.);
        minimizer->SetLimitedVariable(1, "deltaT", maxSample_, 1e-2, 0., 1024.);

        minimizer->Minimize();
        tempFitAmp_ = minimizer->X()[0];
        tempFitTime_ += minimizer->X()[1];

        delete minimizer;        
    }
    
    return WFFitResults{tempFitAmp_, tempFitTime_, TemplateChi2()};
}
    
//----------chi2 for template fit---------------------------------------------------------
double WFClass::TemplateChi2(const double* par)
{
    double chi2 = 0;
    double delta = 0;
    for(int iSample=fWinMin_; iSample<fWinMax_; ++iSample)
    {
	if(iSample < 0 || iSample >= samples_.size())
        {
            cout << ">>>ERROR: template fit out of samples rage" << endl;
	    chi2 += 9999;
        }
	else
        {
            //---fit: par[0]*ref_shape(t-par[1]) par[0]=amplitude, par[1]=DeltaT
            //---if not fitting return chi2 value of best fit
            if(par)
                delta = (samples_[iSample] - par[0]*interpolator_->Eval(iSample*tUnit_-par[1]))/bRMS_;
            else
                delta = (samples_[iSample] - tempFitAmp_*interpolator_->Eval(iSample*tUnit_-tempFitTime_))/bRMS_;
	    chi2 += delta*delta;
        }
    }
    
    return chi2;
}
