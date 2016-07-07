#include "FFTAnalyzer.h"

//----------Utils-------------------------------------------------------------------------
bool FFTAnalyzer::Begin(CfgManager& opts, uint64* index)
{
    //---get all needed information from DigitizerReco
    //   n_channels is fixed by DigiReco since we want to use
    //   the same channel number <-> name mapping
    //   NB: if src is FFTAnalyzer search keep searching for the DigiReco instance
    vector<string> srcChannels;
    int nChannels, nSamples;
    float tUnit;
    if(!opts.OptExist(instanceName_+".srcInstanceName"))
    {
        cout << ">>> FFTAnalyzer ERROR: no DigitizerReco plugin specified" << endl;
        return false;
    }    
    srcInstance_ = opts.GetOpt<string>(instanceName_+".srcInstanceName");
    if(opts.GetOpt<string>(srcInstance_+".pluginType") != "DigitizerReco")
    {
        string digiInstance = opts.GetOpt<string>(srcInstance_+".srcInstanceName");
        srcChannels = opts.GetOpt<vector<string> >(digiInstance+".channelsNames");
        nSamples = (opts.GetOpt<int>(digiInstance+".nSamples"));
        tUnit = (opts.GetOpt<int>(digiInstance+".tUnit"));
    }
    else
    {
        srcChannels = opts.GetOpt<vector<string> >(srcInstance_+".channelsNames");
        nSamples = (opts.GetOpt<int>(srcInstance_+".nSamples"));
        tUnit = (opts.GetOpt<int>(srcInstance_+".tUnit"));
    }
    nChannels = srcChannels.size();

    //---register shared FFTs
    //   nOutBins is divided by to if FFT is from time to frequency domain
    channelsNames_ = opts.GetOpt<vector<string> >(instanceName_+".channelsNames");
    fftType_ = opts.OptExist(instanceName_+".FFTType") ?
        opts.GetOpt<string>(instanceName_+".FFTType") : "T2F";
    if(fftType_ == "T2F")
    {
        nInBins_  = nSamples;
        nOutBins_ = nSamples/2;
    }
    else
    {
        nInBins_  = nSamples/2;
        nOutBins_ = nSamples;
    }
    bool storeFFT = opts.OptExist(instanceName_+".storeFFToutput") ?
        opts.GetOpt<bool>(instanceName_+".storeFFToutput") : false;
    for(auto& channel : channelsNames_)
    {
        if(fftType_ == "T2F")
        {
            FFTs_[channel] = new FFTClass();
            RegisterSharedData(FFTs_[channel], channel, storeFFT);
        }
        else
        {
            WFs_[channel] = new WFClass(1, tUnit);
            RegisterSharedData(WFs_[channel], channel, storeFFT);
        }
    }
    
    //---register output data tree if requested (default true)
    bool storeTree = opts.OptExist(instanceName_+".storeTree") && fftType_ == "T2F" ?
        opts.GetOpt<bool>(instanceName_+".storeTree") : false;
    if(storeTree)
    {
        for(auto& channel : channelsNames_)
        {
            auto point = std::find(srcChannels.begin(), srcChannels.end(), channel);
            if(point != srcChannels.end())
                channelsMap_[channel] = point-srcChannels.begin();            
        }
            
        string fftTreeName = opts.OptExist(instanceName_+".fftTreeName") ?
            opts.GetOpt<string>(instanceName_+".fftTreeName") : "fft";
        RegisterSharedData(new TTree(fftTreeName.c_str(), "fft_tree"), "fft_tree", storeTree);
        //---create tree branches:
        //   array size is determined by DigitizerReco channels
        n_tot_ = nChannels*nOutBins_;
        current_ch_ = new int[n_tot_];
        freqs_ = new float[n_tot_];
        amplitudes_ = new float[n_tot_];
        phases_ = new float[n_tot_];
        fftTree_ = (TTree*)data_.back().obj;
        fftTree_->Branch("index", index, "index/l");
        fftTree_->Branch("n_tot", &n_tot_, "n_tot/i");
        fftTree_->Branch("ch", current_ch_, "ch[n_tot]/I");
        fftTree_->Branch("freq", freqs_, "freq[n_tot]/F");
        fftTree_->Branch("ampl", amplitudes_, "ampl[n_tot]/F");
        fftTree_->Branch("phi", phases_, "phi[n_tot]/F");
        //---set default values
        for(int i=0; i<n_tot_; ++i)
        {
            current_ch_[i]=-1;
            freqs_[i] = (i%nOutBins_);
            amplitudes_[i]=-1;
            phases_[i]=-1;
        }
    }
    
    return true;
}

bool FFTAnalyzer::ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts)
{
    for(auto& channel : channelsNames_)
    {
        //---FFT from time to frequency domain /// T2F
        if(fftType_ == "T2F")
        {
            //---get WF from source instance data and reset FFT
            FFTs_[channel]->Reset();
            auto wf = (WFClass*)plugins[srcInstance_]->GetSharedData(srcInstance_+"_"+channel, "", false).at(0).obj;
            vector<double>* samples = wf->GetSamples();

            //---build the FFT
            double Re[nOutBins_], Im[nOutBins_];
            auto fftr2c = TVirtualFFT::FFT(1, &nInBins_, "R2C");
            fftr2c->SetPoints(samples->data());
            fftr2c->Transform();
            fftr2c->GetPointsComplex(Re, Im);
            FFTs_[channel]->SetPointsComplex(nOutBins_, Re, Im);
            if(fftTree_)
            {
                for(int k=0; k<nOutBins_; ++k)
                {
                    int index =  channelsMap_[channel] * nOutBins_ + k;
                    current_ch_[index] = channelsMap_[channel];
                    amplitudes_[index] = sqrt(pow(Re[k], 2) + pow(Im[k],2));
                    phases_[index] = atan(Im[k]/Re[k]);
                }
            }

            delete fftr2c;
        }
        //---FFT from frequency to time domain /// F2T
        else
        {
            //---get FFT from source instance data and reset old WF
            WFs_[channel]->Reset();
            auto fft = (FFTClass*)plugins[srcInstance_]->GetSharedData(srcInstance_+"_"+channel, "", false).at(0).obj;

            //---build the FFT
            double data[nOutBins_];
            auto Re = fft->GetRe();
            auto Im = fft->GetIm();
            auto fftc2r = TVirtualFFT::FFT(1, &nInBins_, "C2R");
            fftc2r->SetPointsComplex(Re->data(), Im->data());
            fftc2r->Transform();
            fftc2r->GetPoints(data);

            //---fill new WF
            for(int iSample=0; iSample<nOutBins_; ++iSample)
                WFs_[channel]->AddSample(data[iSample]/nOutBins_);

            delete fftc2r;
        }
    }
    //---fill FFT tree
    if(fftTree_)
        fftTree_->Fill();
        
    return true;
}
