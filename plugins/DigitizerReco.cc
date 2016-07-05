#include "DigitizerReco.h"

//----------Utils-------------------------------------------------------------------------
bool DigitizerReco::Begin(CfgManager& opts, uint64* index)
{
    //---inputs---
    channelsNames_ = opts.GetOpt<vector<string> >(instanceName_+".channelsNames");
    timeRecoTypes_ = opts.GetOpt<vector<string> >(instanceName_+".timeRecoTypes");
    nSamples_ = opts.GetOpt<int>(instanceName_+".nSamples");
    tUnit_ = opts.GetOpt<float>(instanceName_+".tUnit");

    //---channels setup
    string templateTag="";
    if(opts.OptExist(instanceName_+".templateTags"))
        for(auto& tag : opts.GetOpt<vector<string> >(instanceName_+".templateTags"))
            for(auto& run : opts.GetOpt<vector<string> >(tag+".runList"))
                if(run == opts.GetOpt<string>("h4reco.run"))
                    templateTag = tag;
    for(auto& channel : channelsNames_)
    {
        if(opts.OptExist(channel+".type") && opts.GetOpt<string>(channel+".type") == "NINO")
            WFs[channel] = new WFClassNINO(opts.GetOpt<int>(channel+".polarity"), tUnit_);
        else
            WFs[channel] = new WFClass(opts.GetOpt<int>(channel+".polarity"), tUnit_);
        if(opts.OptExist(channel+".templateFit"))
        {            
            TFile* templateFile = TFile::Open(opts.GetOpt<string>(channel+".templateFit.file", 0).c_str(), ".READ");
            TH1* wfTemplate=(TH1*)templateFile->Get((opts.GetOpt<string>(channel+".templateFit.file", 1)+
                                                     +"_"+templateTag).c_str());
            WFs[channel]->SetTemplate(wfTemplate);
            WFViewers[channel]=WFViewer(channel, (TH1F*)wfTemplate);
            RegisterSharedData(&WFViewers[channel], channel+"_view", true);
            templateFile->Close();
        }
        //---keep track of all the possible time reco method requested
        for(auto type_name : timeRecoTypes_)
        {
            if(opts.OptExist(channel+"."+type_name))
                timeOpts_[channel+"."+type_name] = opts.GetOpt<vector<float> >(channel+"."+type_name);
        }
        RegisterSharedData(WFs[channel], channel, false);
    }
    
    //---outputs---
    string recoTreeName = opts.OptExist(instanceName_+".recoTreeName") ?
        opts.GetOpt<string>(instanceName_+".recoTreeName") : "digi";
    RegisterSharedData(new TTree(recoTreeName.c_str(), "digi_tree"), "digi_tree", true);
    recoTree_ = DigiTree(index, (TTree*)data_.back().obj);
    recoTree_.Init(channelsNames_, timeRecoTypes_);
    if(opts.GetOpt<int>(instanceName_+".fillWFtree"))
    {
        string wfTreeName = opts.OptExist(instanceName_+".wfTreeName") ?
            opts.GetOpt<string>(instanceName_+".wfTreeName") : "wf";
        RegisterSharedData(new TTree(wfTreeName.c_str(), "wf_tree"), "wf_tree", true);
        outWFTree_ = WFTree(channelsNames_.size(), nSamples_, index, (TTree*)data_.back().obj);
        outWFTree_.Init();
    }

    return true;
}

bool DigitizerReco::ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts)
{
    //---setup output event 
    int outCh=0;
    bool badEvent=false;
    bool fillWFtree=false;
    if(opts.GetOpt<int>(instanceName_+".fillWFtree"))
        fillWFtree = *recoTree_.index % opts.GetOpt<int>(instanceName_+".WFtreePrescale") == 0;
        
    //---read the digitizer
    //---set time reference from digitized trigger
    int timeRef=0;
    if(opts.GetOpt<int>(instanceName_+".useTrigRef"))
    {
        for(int iSample=nSamples_*8; iSample<nSamples_*9; ++iSample)
        {
            if(event.digiSampleValue[iSample] < 1000)
            {
                timeRef = iSample-nSamples_*8;
                break;
            }
        }
    }
    
    //---user channels
    for(auto& channel : channelsNames_)
    {
        //---reset and read new WFs
        WFs[channel]->Reset();
        int digiGr = opts.GetOpt<int>(channel+".digiGroup");
        int digiCh = opts.GetOpt<int>(channel+".digiChannel");
        int offset = event.digiMap.at(make_pair(digiGr, digiCh));
        for(int iSample=offset; iSample<offset+nSamples_; ++iSample)
        {
            //---H4DAQ bug: sometimes ADC value is out of bound.
            //---skip everything if one channel is bad
            if(event.digiSampleValue[iSample] > 10000)
            {
                cout << ">>>DigiReco WARNING: skipped event" << endl;
                return false;
            }
            WFs[channel]->AddSample(event.digiSampleValue[iSample]);
        }
    }
    
    //---compute reco variables
    for(auto& channel : channelsNames_)
    {
        //---subtract a specified channel if requested
        if(opts.OptExist(channel+".subtractChannel"))
            *WFs[channel] -= *WFs[opts.GetOpt<string>(channel+".subtractChannel")];        
        WFs[channel]->SetBaselineWindow(opts.GetOpt<int>(channel+".baselineWin", 0), 
                                        opts.GetOpt<int>(channel+".baselineWin", 1));
        WFs[channel]->SetSignalWindow(opts.GetOpt<int>(channel+".signalWin", 0)+timeRef, 
                                      opts.GetOpt<int>(channel+".signalWin", 1)+timeRef);
        WFBaseline baselineInfo = WFs[channel]->SubtractBaseline();
        WFFitResults interpolAmpMax = WFs[channel]->GetInterpolatedAmpMax(-1,-1,opts.GetOpt<int>(channel+".signalWin", 2));
        recoTree_.b_charge[outCh] = WFs[channel]->GetIntegral(opts.GetOpt<int>(channel+".baselineInt", 0), 
                                                             opts.GetOpt<int>(channel+".baselineInt", 1));        
        recoTree_.b_slope[outCh] = baselineInfo.slope;
        recoTree_.b_rms[outCh] = baselineInfo.rms;
        recoTree_.maximum[outCh] = WFs[channel]->GetAmpMax();
        recoTree_.time_maximum[outCh] = WFs[channel]->GetTimeCF(1).first;
        recoTree_.amp_max[outCh] = interpolAmpMax.ampl;
        recoTree_.time_max[outCh] = interpolAmpMax.time;
        recoTree_.chi2_max[outCh] = interpolAmpMax.chi2;
        recoTree_.charge_tot[outCh] = WFs[channel]->GetModIntegral(opts.GetOpt<int>(channel+".baselineInt", 1), 
                                                                 nSamples_);
        recoTree_.charge_sig[outCh] = WFs[channel]->GetSignalIntegral(opts.GetOpt<int>(channel+".signalInt", 0), 
                                                                     opts.GetOpt<int>(channel+".signalInt", 1));
        //---compute time with all the requested time reconstruction method
        for(int iT=0; iT<timeRecoTypes_.size(); ++iT)
        {
            //---compute time with selected method or store default value (-99)
            if(timeOpts_.find(channel+"."+timeRecoTypes_[iT]) != timeOpts_.end())
            {
                pair<float, float> timeInfo = WFs[channel]->GetTime(timeRecoTypes_[iT], timeOpts_[channel+"."+timeRecoTypes_[iT]]);
                recoTree_.time[outCh+iT*channelsNames_.size()] = timeInfo.first;
                recoTree_.time_chi2[outCh+iT*channelsNames_.size()] = timeInfo.second;
            }
            else
            {
                recoTree_.time[outCh+iT*channelsNames_.size()] = -99;
                recoTree_.time_chi2[outCh+iT*channelsNames_.size()] = -99;
            }
        }
        
        //---template fit (only specified channels)
        WFFitResults fitResults{-1, -1000, -1};
        if(opts.OptExist(channel+".templateFit"))
        {
            fitResults = WFs[channel]->TemplateFit(opts.GetOpt<float>(channel+".templateFit.fitWin", 0),
                                                   opts.GetOpt<int>(channel+".templateFit.fitWin", 1),
                                                   opts.GetOpt<int>(channel+".templateFit.fitWin", 2));
            recoTree_.fit_ampl[outCh] = fitResults.ampl;
            recoTree_.fit_time[outCh] = fitResults.time;
            recoTree_.fit_chi2[outCh] = fitResults.chi2;
        }            
	//---calibration constant for each channel if needed
	if(opts.OptExist(channel+".calibration.calibrationConst"))
	  recoTree_.calibration[outCh]=opts.GetOpt<float>(channel+".calibration.calibrationConst");
	else
	  recoTree_.calibration[outCh]=1;
	
        //---WFs---
        if(fillWFtree)
        {
            vector<double>* analizedWF = WFs[channel]->GetSamples();
            for(int jSample=0; jSample<analizedWF->size(); ++jSample)
            {
                outWFTree_.WF_ch[jSample+outCh*nSamples_] = outCh;
                outWFTree_.WF_time[jSample+outCh*nSamples_] = jSample*tUnit_;
                outWFTree_.WF_val[jSample+outCh*nSamples_] = analizedWF->at(jSample);
            }
        }
        //---increase output tree channel counter
        ++outCh;
    }

    //---fill the output trees if the event is good
    if(!badEvent)
    {
        //---reco var
        recoTree_.Fill();
        //---WFs
        if(fillWFtree)
            outWFTree_.Fill();
    }

    return true;
}
