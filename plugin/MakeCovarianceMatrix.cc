#include "plugin/MakeCovarianceMatrix.h"

//----------Utils-------------------------------------------------------------------------
bool MakeCovarianceMatrix::Begin(CfgManager& opts, int* index)
{
    digiInstance_ = opts.GetOpt<string>(instanceName_+".digiInstanceName");
    channelsNames_ = opts.GetOpt<vector<string> >(instanceName_+".channelsNames");
    firstSample_ = opts.GetOpt<int>(instanceName_+".firstSample");
    lastSample_ = opts.GetOpt<int>(instanceName_+".lastSample");

    sums_.resize(lastSample_-firstSample_);
    sum2s_.resize(lastSample_-firstSample_);
    mapCovariances_ = TH2F("CovariancesMap", "", lastSample_-firstSample_, 0, lastSample_-firstSample_-1,
                           lastSample_-firstSample_, 0, lastSample_-firstSample_-1);
    mapCovariances_.SetAxisRange(-1, 1, "Z");
    mapCovariances_.SetContour(10000);
    RegisterSharedData(&mapCovariances_, "CovariancesMap", true);
    
    return true;
}

bool MakeCovarianceMatrix::ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts)
{
    for(auto& channel : channelsNames_)
    {        
        WFClass* wf = (WFClass*)plugins[digiInstance_]->GetSharedData(digiInstance_+"_"+channel, "", false).at(0).obj;
        vector<float>* samples = wf->GetSamples();
        for(int iSample=firstSample_; iSample<lastSample_; ++iSample)
        {
            values_[iSample-firstSample_].push_back(samples->at(iSample));
            sums_[iSample-firstSample_] += samples->at(iSample);
            sum2s_[iSample-firstSample_] += samples->at(iSample)*samples->at(iSample);
        }
    }
    ++events_;

    return true;
}

bool MakeCovarianceMatrix::End(CfgManager& opts)
{
    map<int, float> mean, rms;
    //---compute mean and rms of each sample
    for(int iSample=0; iSample<sums_.size(); ++iSample)
    {
        mean[iSample] = sums_[iSample]/events_;
        rms[iSample] = sqrt(sum2s_[iSample]/events_ - mean[iSample]*mean[iSample]);
        // hMeans.Fill(mean[iSample]);
        // hRMSs.Fill(rms[iSample]);
    }
    //---compute covariances
    for(int iX=0; iX<sums_.size(); ++iX)
    {
        for(int iY=0; iY<iX; ++iY)
        {
            float rho=0;
            for(int iEvt=0; iEvt<events_; ++iEvt)
                rho += values_[iX][iEvt]*values_[iY][iEvt]
                    - values_[iX][iEvt]*mean[iY] - values_[iY][iEvt]*mean[iX];
            rho += events_*mean[iX]*mean[iY];
            rho = rho/((events_-1)*rms[iX]*rms[iY]);
            mapCovariances_.SetBinContent(iX+1, iY+1, rho);
            mapCovariances_.SetBinContent(iY+1, iX+1, rho);
        }
    }

    return true;
}
