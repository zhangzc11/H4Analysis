#ifndef __MAIN__
#define __MAIN__

#include <string>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"

#include "interface/CfgManager.h"
#include "interface/H4Tree.h"
#include "interface/RecoTree.h"
#include "interface/WFTree.h"
#include "interface/WFClass.h"
#include "interface/HodoUtils.h"

using namespace std;

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
    //cout << opts << endl;
    //---data opts
    string path=opts.GetOpt<string>("global", "path2data");
    string run=opts.GetOpt<string>("global", "run");
    if(argc > 2)
        run = argv[2];
    int maxEvents = opts.GetOpt<int>("global", "maxEvents");
    //---channels opts
    int nCh = opts.GetOpt<int>("global", "nCh");
    int nSamples = opts.GetOpt<int>("global", "nSamples");
    float tUnit = opts.GetOpt<float>("global", "tUnit");
    bool useTrigRef = opts.GetOpt<int>("global", "useTrigRef");
    vector<string> channelsNames = opts.GetOpt<vector<string>& >("global", "channelsNames");
    map<string, vector<float> > timeOpts;
    for(auto& channel : channelsNames)
         timeOpts[channel] = opts.GetOpt<vector<float> >(channel, "timeOpts");

    //---definitions---
    int iEvent=1;
  
    //-----output setup-----
    TFile* outROOT = TFile::Open("ntuples/"+TString(run)+".root", "RECREATE");
    outROOT->cd();
    RecoTree outTree(nCh, nSamples, channelsNames, &iEvent);
    WFTree outWFTree(nCh, nSamples, &iEvent);
  
    //-----input setup-----
    TChain* inTree = new TChain("H4tree");
    string ls_command = string("ls "+path+run+" | grep 'root' > tmp/"+run+".list");
    system(ls_command.c_str());
    ifstream waveList(string("tmp/"+run+".list").c_str(), ios::in);
    string file;
    while(waveList >> file)
        inTree->AddFile((path+run+"/"+file).c_str());
    H4Tree h4Tree(inTree);

    //-----setup hodo configuration
    vector<int> hodoFiberOrderA;
    vector<int> hodoFiberOrderB;
    FillHodoFiberOrder(hodoFiberOrderA,hodoFiberOrderB);

    //---process WFs---
    cout << ">>> Processing H4DAQ run #" << run << " <<<" << endl;
    while(h4Tree.NextEvt() && (iEvent<maxEvents || maxEvents==-1))
    {        
        //---setup output event 
        int outCh=0;
        bool badEvent=false;
        bool fillWFtree=false;
        if(opts.GetOpt<int>("global", "fillWFtree"))
            fillWFtree = iEvent % opts.GetOpt<int>("global", "WFtreePrescale") == 0;
        
        //---read the hodoscopes                                                                                                            $
        map<int,std::map<int,bool> > hodoFiberOn;
        map<int,float> beamXY_hodo12;
        FillHodo(h4Tree, hodoFiberOn, hodoFiberOrderA, hodoFiberOrderB, beamXY_hodo12);
        outTree.hodoX1 = beamXY_hodo12[0];
        outTree.hodoY1 = beamXY_hodo12[1];
        outTree.hodoX2 = beamXY_hodo12[2];
        outTree.hodoY2 = beamXY_hodo12[3]; 

        //---read the digitizer
        //---set time reference from digitized trigger
        int timeRef=0;
        if(useTrigRef)
        {
            for(int iSample=nSamples*8; iSample<nSamples*9; ++iSample)
            {
                if(h4Tree.digiSampleValue[iSample] < 1000)
                {
                    timeRef = iSample-nSamples*8;
                    break;
                }
            }
        }
        //---user channels
        for(auto& channel : channelsNames)
        {
            //---read WFs
            WFClass WF(opts.GetOpt<int>(channel, "polarity"), tUnit);
            int digiGr = opts.GetOpt<int>(channel, "digiGroup");
            int digiCh = opts.GetOpt<int>(channel, "digiChannel");
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
            //---skip everything if one channel has trigger the badEvent flag 
            if(badEvent)
                break;
            //---compute reco variables
            WF.SetBaselineWindow(opts.GetOpt<int>(channel, "baselineWin", 0), 
                                 opts.GetOpt<int>(channel, "baselineWin", 1));
            WF.SetSignalWindow(opts.GetOpt<int>(channel, "signalWin", 0)+timeRef, 
                               opts.GetOpt<int>(channel, "signalWin", 1)+timeRef);
            WF.SubtractBaseline();
            outTree.amp_max[outCh] = WF.GetAmpMax();
            outTree.fit_amp_max[outCh] = WF.GetInterpolatedAmpMax();            
            outTree.time[outCh] = WF.GetTime(opts.GetOpt<string>(channel, "timeType"), timeOpts[channel]);
            outTree.baseline[outCh] = WF.GetIntegral(opts.GetOpt<int>(channel, "baselineInt", 0), 
                                                     opts.GetOpt<int>(channel, "baselineInt", 1));
            outTree.charge_tot[outCh] = WF.GetModIntegral(opts.GetOpt<int>(channel, "baselineInt", 1), 
                                                          nSamples);
            outTree.charge_sig[outCh] = WF.GetSignalIntegral(opts.GetOpt<int>(channel, "signalInt", 0), 
                                                             opts.GetOpt<int>(channel, "signalInt", 1));
            //---WFs---
            if(fillWFtree)
            {
                vector<float>* analizedWF = WF.GetSamples();
                for(int jSample=0; jSample<analizedWF->size(); ++jSample)
                {
                    outWFTree.time_stamp = h4Tree.evtTime;
                    outWFTree.WF_ch[jSample+outCh*nSamples] = outCh;
                    outWFTree.WF_time[jSample+outCh*nSamples] = jSample*tUnit;
                    outWFTree.WF_val[jSample+outCh*nSamples] = analizedWF->at(jSample);
                }
            }
            //---increase output tree channel counter
            ++outCh;
        }

        //---fill the output trees if the event is good
        if(!badEvent)
        {
            if(iEvent % 1000 == 0)
                cout << ">>>Processed good events: " << iEvent << "/" << h4Tree.GetEntries() << endl;
            ++iEvent;            
            //---reco var
            outTree.time_stamp = h4Tree.evtTime;
            outTree.run = h4Tree.runNumber;
            outTree.spill = h4Tree.spillNumber;
            outTree.event = h4Tree.evtNumber;
            outTree.Fill();
            //---WFs
            if(fillWFtree)
                outWFTree.Fill();
        }
    }
    
    outROOT->cd();
    if(opts.GetOpt<int>("global", "fillWFtree"))
    {
        outWFTree.Write();
        outTree.AddFriend();
    }
    outTree.Write();
    opts.Write("cfg");
    outROOT->Close();
}

#endif
