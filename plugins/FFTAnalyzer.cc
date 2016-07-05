#include "FFTAnalyzer.h"

//----------Utils-------------------------------------------------------------------------
bool FFTAnalyzer::Begin(CfgManager& opts, uint64* index)
{
    //---get all needed information from DigitizerReco
    //   n_channels is fixed by DigiReco since we want to use
    //   the same channel number <-> name mapping
    if(!opts.OptExist(instanceName_+".digiInstanceName"))
    {
        cout << ">>>FFTAnalyzer ERROR: no DigitizerReco plugin specified" << endl;
        return false;
    }
    digiInstance_ = opts.GetOpt<string>(instanceName_+".digiInstanceName");
    vector<string> digiChannels = opts.GetOpt<vector<string> >(digiInstance_+".channelsNames");
    int n_channels = digiChannels.size();
    n_freqs_ = (opts.GetOpt<int>(digiInstance_+".nSamples"))/2;
    channelsNames_ = opts.GetOpt<vector<string> >(instanceName_+".channelsNames");
    for(auto& channel : channelsNames_)
    {
        auto point = std::find(digiChannels.begin(), digiChannels.end(), channel);
        if(point != digiChannels.end())
            channelsMap_[channel] = point-digiChannels.begin();            
    }
    
    //---register output data tree
    string fftTreeName = opts.OptExist(instanceName_+".fftTreeName") ?
        opts.GetOpt<string>(instanceName_+".fftTreeName") : "fft";
    RegisterSharedData(new TTree(fftTreeName.c_str(), "fft_tree"), "fft_tree", true);
    //---create tree branches:
    //   array size is determined by DigitizerReco channels
    current_ch_ = 0;
    n_tot_ = n_channels*n_freqs_;
    amplitudes_ = new float[n_tot_];
    phases_ = new float[n_tot_];
    fftTree_ = (TTree*)data_.back().obj;
    fftTree_->Branch("index", index, "index/l");
    fftTree_->Branch("n_tot", &n_tot_, "n_tot/i");
    fftTree_->Branch("n_freqs", &n_freqs_, "n_freqs/i");
    fftTree_->Branch("ch", &current_ch_, "ch[n_tot]/I");
    fftTree_->Branch("ampl", &amplitudes_, "ampl[n_tot]/F");
    fftTree_->Branch("phi", &phases_, "phi[n_tot]/F");
    //---set default values
    for(int i=0; i<n_tot_; ++i)
    {
        amplitudes_[i]=-1;
        phases_[i]=-1;
    }
    
    return true;
}

bool FFTAnalyzer::ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts)
{
    for(auto& channel : channelsNames_)
    {
        //---get WFs from DigitizerREco instance data
        WFClass* wf = (WFClass*)plugins[digiInstance_]->GetSharedData(digiInstance_+"_"+channel, "", false).at(0).obj;
        vector<double>* samples = wf->GetSamples();

        //---build the FFT
        int N = n_freqs_*2;
        double Re[n_freqs_], Im[n_freqs_];
        TVirtualFFT *fftr2c = TVirtualFFT::FFT(1, &N, "R2C");
        fftr2c->SetPoints(samples->data());
        fftr2c->Transform();
        fftr2c->GetPointsComplex(Re, Im);
        for(int k=0; k<n_freqs_; ++k)
        {
            current_ch_ = channelsMap_[channel];
            int index =  current_ch_ * n_freqs_ + k;
            amplitudes_[index] = sqrt(pow(Re[k], 2) + pow(Im[k],2));
            phases_[index] = atan(Im[k]/Re[k]);
        }        
    }
    fftTree_->Fill();
    
    return true;
}
