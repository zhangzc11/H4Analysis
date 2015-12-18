#include "plugin/MakeCovarianceMatrix.h"

//----------Utils-------------------------------------------------------------------------
bool MakeCovarianceMatrix::Begin(CfgManager& opts, int* index)
{
    digiInstance_ = opts.GetOpt<string>(instanceName_+".digiInstanceName");
    channelsNames_ = opts.GetOpt<vector<string> >(instanceName_+".channelsNames");
    firstSample_ = opts.GetOpt<int>(instanceName_+".firstSample");
    lastSample_ = opts.GetOpt<int>(instanceName_+".lastSample");

    sums_.resize(lastSample_-firstSample_+1);
    sum2s_.resize(lastSample_-firstSample_+1);
    mapCovariances_ = TH2F("CovariancesMap", "", lastSample_-firstSample_+1, firstSample_, lastSample_,
                           lastSample_-firstSample_+1, firstSample_, lastSample_);
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
            values_[iSample].push_back(samples->at(iSample));
            sums_[iSample] += samples->at(iSample);
            sum2s_[iSample] += samples->at(iSample)*samples->at(iSample);
        }
    }
    ++events_;

    return true;
}

bool MakeCovarianceMatrix::End(CfgManager& opts)
{
    map<int, float> mean, rms;
    //---compute mean and rms of each sample
    for(int iSample=firstSample_; iSample<lastSample_; ++iSample)
    {
        mean[iSample] = sums_[iSample]/events_;
        rms[iSample] = sqrt(sum2s_[iSample]/events_ - mean[iSample]*mean[iSample]);
        // hMeans.Fill(mean[iSample]);
        // hRMSs.Fill(rms[iSample]);
    }
    //---compute covariances
    for(int iX=firstSample_; iX<lastSample_; ++iX)
    {
        for(int iY=firstSample_; iY<iX; ++iY)
        {
            float rho=0;
            for(int iEvt=0; iEvt<events_; ++iEvt)
                rho += values_[iX][iEvt]*values_[iY][iEvt]
                    - values_[iX][iEvt]*mean[iY] - values_[iY][iEvt]*mean[iX];
            rho += events_*mean[iX]*mean[iY];
            rho = rho/((events_-1)*rms[iX]*rms[iY]);
            mapCovariances_.SetBinContent(iX-firstSample_+1, iY-firstSample_+1, rho);
            mapCovariances_.SetBinContent(iY-firstSample_+1, iX-firstSample_+1, rho);
        }
    }

    return true;
}
