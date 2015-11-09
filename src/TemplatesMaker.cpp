#ifndef __MAIN__
#define __MAIN__

#include <string>

#include "TString.h"
#include "TFile.h"
#include "TProfile.h"

#include "interface/CfgManager.h"
#include "interface/CfgManagerT.h"
#include "interface/H4Tree.h"
#include "interface/WFClass.h"

using namespace std;

//----------DFT---------------------------------------------------------------------------
pair<TH1F, TH1F> DFT_cut(TProfile* inWave, string name, float fCut=800)
{
    float *Re, *Im, *mod;
    int nbins = inWave->GetNbinsX();
    Re = (float*)malloc(sizeof(float)*nbins);
    Im = (float*)malloc(sizeof(float)*nbins);
    mod = (float*)malloc(sizeof(float)*nbins);
    string nameDFT = "DFT_"+name;
    name = "cleaned_"+name;
    TH1F outDFT(nameDFT.c_str(), nameDFT.c_str(), nbins/2, 0, nbins/2);
    TH1F outWave(name.c_str(), name.c_str(), nbins, -40, 160); 
    for(int k=0; k<nbins/2; k++) 
    { 
        Re[k]=0;
        Im[k]=0; 
	mod[k]=0;
        for(int t=1; t<=nbins; t++) 
        { 
            float angle = 2 * TMath::Pi() * t * k / (nbins); 
            Re[k] += 2*inWave->GetBinContent(t) * cos(angle)/(nbins); 
            Im[k] += -2*inWave->GetBinContent(t) * sin(angle)/(nbins);
        }	    
	if(k == 0 || k == nbins-1)
	{
	    Re[k] = Re[k]/2;
	    Im[k] = Im[k]/2;
	}
	mod[k] = TMath::Sqrt(TMath::Abs(pow(Re[k],2) - pow(Im[k],2)));
        outDFT.SetBinContent(k, mod[k]);
    }
    for(int t=1; t<=nbins; t++) 
    { 
        float recoWave=0;
        for(int k=0; k<nbins/2; k++) 
        { 
	    if(k > fCut)
	    	continue;
            float angle = 2 * TMath::Pi() * t * k / nbins; 
	    recoWave += Re[k]*cos(angle) - Im[k]*sin(angle);
	}  
	outWave.SetBinContent(t, recoWave);
    }
    outWave.Scale(inWave->GetMaximum()/outWave.GetMaximum());

    return make_pair(outWave, outDFT);
}

//**********MAIN**************************************************************************
int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cout << argv[0] << " cfg file " << "[run]" << endl; 
        return -1;
    }

    //---load options---
    CfgManager opts;
    opts.ParseConfigFile(argv[1]);

    //---data opts
    string path=opts.GetOpt<string>("global.path2data");
    string run=opts.GetOpt<string>("global.run");
    string outSuffix=opts.GetOpt<string>("global.outFileSuffix");
    if(argc > 2)
        run = argv[2];
    int maxEvents = opts.GetOpt<int>("global.maxEvents");
    //---channels opts
    int nCh = opts.GetOpt<int>("global.nCh");
    int nSamples = opts.GetOpt<int>("global.nSamples");
    float tUnit = opts.GetOpt<float>("global.tUnit");
    string refChannel = opts.GetOpt<string>("global.refChannel");
    vector<string> channelsNames = opts.GetOpt<vector<string>& >("global.channelsNames");
    map<string, vector<float> > timeOpts;
    timeOpts[refChannel] = opts.GetOpt<vector<float> >(refChannel+".timeOpts");
    for(auto& channel : channelsNames)
         timeOpts[channel] = opts.GetOpt<vector<float> >(channel+".timeOpts");

    //---definitions---
    int iEvent=1;
  
    //-----output setup-----
    TFile* outROOT = TFile::Open("ntuples/Templates_"+TString(outSuffix)+".root", "RECREATE");
    outROOT->cd();
    map<string, TProfile*> templates;
    for(auto channel : channelsNames)
        templates[channel] = new TProfile(channel.c_str(), channel.c_str(),
                                          8000, -40, 160, -0.5, 1.1);
  
    //-----input setup-----
    TChain* inTree = new TChain("H4tree");
    string ls_command;
    if(path.find("/eos/cms") != string::npos)
        ls_command = string("gfal-ls root://eoscms/"+path+run+" | grep 'root' > tmp/"+run+".list");
    else
        ls_command = string("ls "+path+run+" | grep 'root' > tmp/"+run+".list");
    system(ls_command.c_str());
    ifstream waveList(string("tmp/"+run+".list").c_str(), ios::in);
    string file;
    while(waveList >> file)
    {
        if(path.find("/eos/cms") != string::npos)
            inTree->AddFile(("root://eoscms/"+path+run+"/"+file).c_str());
        else
            inTree->AddFile((path+run+"/"+file).c_str());
    }
    H4Tree h4Tree(inTree);

    //---process WFs---
    cout << ">>> Processing H4DAQ run #" << run << " <<<" << endl;
    while(h4Tree.NextEvt() && (iEvent<maxEvents || maxEvents==-1))
    {        
        //---setup output event 
        int outCh=0;
        bool badEvent=false;
        
        //---read the digitizer
        //---read time raference channel
        float refTime=0, refAmpl=0;
        WFClass WF(opts.GetOpt<int>(refChannel+".polarity"), tUnit);
        int digiGr = opts.GetOpt<int>(refChannel+".digiGroup");
        int digiCh = opts.GetOpt<int>(refChannel+".digiChannel");
        int offset = digiGr*9*nSamples + digiCh*nSamples;
        for(int iSample=offset; iSample<offset+nSamples; ++iSample)
        {
            //---H4DAQ bug: sometimes ADC value is out of bound.
            if(h4Tree.digiSampleValue[iSample] > 4096)
            {
                badEvent = true;
                break;
            }
            WF.AddSample(h4Tree.digiSampleValue[iSample]);
        }
        //---skip bad events
        if(badEvent)
            continue;
        WF.SetBaselineWindow(opts.GetOpt<int>(refChannel+".baselineWin", 0), 
                             opts.GetOpt<int>(refChannel+".baselineWin", 1));
        WF.SetSignalWindow(opts.GetOpt<int>(refChannel+".signalWin", 0), 
                           opts.GetOpt<int>(refChannel+".signalWin", 1));
        WF.SubtractBaseline();
        refAmpl = WF.GetInterpolatedAmpMax();            
        refTime = WF.GetTime(opts.GetOpt<string>(refChannel+".timeType"), timeOpts[refChannel]).first;
        //---require reference channel to be good
        if(refTime/tUnit < opts.GetOpt<int>(refChannel+".signalWin", 0) ||
           refTime/tUnit > opts.GetOpt<int>(refChannel+".signalWin", 1) ||
           refAmpl < opts.GetOpt<int>(refChannel+".amplitudeThreshold"))
            continue;
        
        //---template channels
        for(auto& channel : channelsNames)
        {
            //---read WFs
            WFClass WF(opts.GetOpt<int>(channel+".polarity"), tUnit);
            int digiGr = opts.GetOpt<int>(channel+".digiGroup");
            int digiCh = opts.GetOpt<int>(channel+".digiChannel");
            int offset = digiGr*9*nSamples + digiCh*nSamples;
            for(int iSample=offset; iSample<offset+nSamples; ++iSample)
            {
                //---H4DAQ bug: sometimes ADC value is out of bound.
                if(h4Tree.digiSampleValue[iSample] > 4096)
                {
                    badEvent = true;
                    break;
                }
                WF.AddSample(h4Tree.digiSampleValue[iSample]);
            }
            //---skip bad events
            if(badEvent)
                continue;
            //---compute reco variables
            float channelTime=0, channelAmpl=0;            
            WF.SetBaselineWindow(opts.GetOpt<int>(channel+".baselineWin", 0), 
                                 opts.GetOpt<int>(channel+".baselineWin", 1));
            WF.SetSignalWindow(opts.GetOpt<int>(channel+".signalWin", 0), 
                               opts.GetOpt<int>(channel+".signalWin", 1));
            WF.SubtractBaseline();
            channelAmpl = WF.GetInterpolatedAmpMax();            
            channelTime = WF.GetTime(opts.GetOpt<string>(channel+".timeType"), timeOpts[channel]).first;
            //---skip bad events or events with no signal
            if(channelTime/tUnit > opts.GetOpt<int>(channel+".signalWin", 0) &&
               channelTime/tUnit < opts.GetOpt<int>(channel+".signalWin", 1) &&
               channelAmpl > opts.GetOpt<int>(channel+".amplitudeThreshold"))
            {                
                vector<float>* analizedWF = WF.GetSamples();
                for(int iSample=0; iSample<analizedWF->size(); ++iSample)
                    templates[channel]->Fill(iSample*tUnit-refTime, analizedWF->at(iSample)/channelAmpl);
            }
        }
    }   

    //---clean templates with DFT analysis and store them
    outROOT->cd();
    for(auto& channel : channelsNames)
    {
        pair<TH1F, TH1F> dft = DFT_cut(templates[channel], channel, 800);
        dft.first.Write();
        dft.second.Write();
        templates[channel]->Write();
    }
    opts.Write("cfg");
    outROOT->Close();
}

#endif
