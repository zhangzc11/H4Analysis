#ifndef __MAIN__
#define __MAIN__

#include <string>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"

#include "interface/CfgManager.h"
#include "interface/CfgManagerT.h"
#include "interface/H4Tree.h"
#include "interface/RecoTree.h"
#include "interface/WFTree.h"
#include "interface/WFClass.h"
#include "interface/WFClassNINO.h"
#include "interface/WireChamber.h"
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
    cout << opts << endl;
    //---data opts
    string path=opts.GetOpt<string>("global.path2data");
    string run=opts.GetOpt<string>("global.run");

    if(argc > 2)
        run = argv[2];


    int maxEvents = opts.GetOpt<int>("global.maxEvents");

    //---channels setup
    int nSamples = opts.GetOpt<int>("global.nSamples");
    float tUnit = opts.GetOpt<float>("global.tUnit");

    bool useTrigRef = opts.GetOpt<int>("global.useTrigRef");
    vector<string> channelsNames = opts.GetOpt<vector<string>& >("global.channelsNames");
    map<string, WFClass*> WFs;
    map<string, vector<float> > timeOpts;
    for(auto& channel : channelsNames)
    {
        if(opts.GetOpt<bool>(channel+".type") && opts.GetOpt<string>(channel+".type") == "NINO")
            WFs[channel] = new WFClassNINO(opts.GetOpt<int>(channel+".polarity"), tUnit);
        else
            WFs[channel] = new WFClass(opts.GetOpt<int>(channel+".polarity"), tUnit);
        timeOpts[channel] = opts.GetOpt<vector<float> >(channel+".timeOpts");
        if(opts.GetOpt<bool>(channel+".templateFit"))
        {
            TFile* templateFile = TFile::Open(opts.GetOpt<string>(channel+".templateFit.file", 0).c_str(), ".READ");
            WFs[channel]->SetTemplate((TH1*)templateFile->Get(opts.GetOpt<string>(channel+".templateFit.file", 1).c_str()));
            templateFile->Close();
        }
    }
  
    //-----output setup-----
    int iEvent=1;
    TFile* outROOT = TFile::Open("ntuples/"+TString(run)+".root", "RECREATE");
    RecoTree outTree(channelsNames, nSamples, opts.GetOpt<bool>("global.H4hodo"),
                     opts.GetOpt<int>("global.nWireChambers"), &iEvent);
    WFTree outWFTree(channelsNames.size(), nSamples, &iEvent);
  
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

    int nFiles=0;
    while(waveList >> file && (opts.GetOpt<int>("global.maxFiles")<0 || nFiles<opts.GetOpt<int>("global.maxFiles")) )
    {
        if(path.find("/eos/cms") != string::npos)
	  {
	    std::cout << "+++ Adding file " << ("root://eoscms/"+path+run+"/"+file).c_str() << std::endl;
            inTree->AddFile(("root://eoscms/"+path+run+"/"+file).c_str());
	  }
        else
	  {
	    std::cout << "+++ Adding file " << (path+run+"/"+file).c_str() << std::endl;
            inTree->AddFile((path+run+"/"+file).c_str());
	  }
	++nFiles;
    }

    H4Tree h4Tree(inTree);
    WireChamber wires;

    if(opts.GetOpt<int>("global.nWireChambers") > 0)
    {
        wires = WireChamber(&h4Tree,
                            opts.GetOpt<int>("WireChamber.chXleft"),
                            opts.GetOpt<int>("WireChamber.chXright"),
                            opts.GetOpt<int>("WireChamber.chYup"),
                            opts.GetOpt<int>("WireChamber.chYdown"));
    }


    //---setup H4hodo
    vector<int> hodoFiberOrderA;
    vector<int> hodoFiberOrderB;
    if(opts.GetOpt<bool>("global.H4hodo"))
        FillHodoFiberOrder(hodoFiberOrderA,hodoFiberOrderB);

    //---process WFs---
    cout << ">>> Processing H4DAQ run #" << run << " <<<" << endl;
    while(h4Tree.NextEvt() && (iEvent<maxEvents || maxEvents==-1))
    {
        //---setup output event 
        int outCh=0;
        bool badEvent=false;
        bool fillWFtree=false;
        if(opts.GetOpt<int>("global.fillWFtree"))
            fillWFtree = iEvent % opts.GetOpt<int>("global.WFtreePrescale") == 0;

        //---read the wire chamber
        if(opts.GetOpt<int>("global.nWireChambers")>0)
        {
            wires.Unpack();
            outTree.wireX[0] = wires.GetX();
            outTree.wireY[0] = wires.GetY();
        }
        
        //---read the hodoscopes
        if(opts.GetOpt<bool>("global.H4hodo"))
        {
            map<int,std::map<int,bool> > hodoFiberOn;
            map<int,float> beamXY_hodo12;
            FillHodo(h4Tree, hodoFiberOn, hodoFiberOrderA, hodoFiberOrderB, beamXY_hodo12);
            outTree.hodoX1 = beamXY_hodo12[0];
            outTree.hodoY1 = beamXY_hodo12[1];
            outTree.hodoX2 = beamXY_hodo12[2];
            outTree.hodoY2 = beamXY_hodo12[3]; 
        }
        
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
            //---reset and read new WFs
            WFs[channel]->Reset();
            int digiGr = opts.GetOpt<int>(channel+".digiGroup");
            int digiCh = opts.GetOpt<int>(channel+".digiChannel");
            int offset = h4Tree.digiMap[make_pair(digiGr, digiCh)];
            for(int iSample=offset; iSample<offset+nSamples; ++iSample)
            {
                //---H4DAQ bug: sometimes ADC value is out of bound.
                if(h4Tree.digiSampleValue[iSample] > 4096)
                {
                    badEvent = true;
                    break;
                }
                WFs[channel]->AddSample(h4Tree.digiSampleValue[iSample]);
            }
            //---skip everything if one channel has trigger the badEvent flag 
            if(badEvent)
                break;
            //---compute reco variables
            WFs[channel]->SetBaselineWindow(opts.GetOpt<int>(channel+".baselineWin", 0), 
                                 opts.GetOpt<int>(channel+".baselineWin", 1));
            WFs[channel]->SetSignalWindow(opts.GetOpt<int>(channel+".signalWin", 0)+timeRef, 
                               opts.GetOpt<int>(channel+".signalWin", 1)+timeRef);
            WFBaseline baselineInfo = WFs[channel]->SubtractBaseline();
            pair<float, float> timeInfo = WFs[channel]->GetTime(opts.GetOpt<string>(channel+".timeType"), timeOpts[channel]);
            outTree.b_charge[outCh] = WFs[channel]->GetIntegral(opts.GetOpt<int>(channel+".baselineInt", 0), 
                                                     opts.GetOpt<int>(channel+".baselineInt", 1));
            outTree.b_slope[outCh] = baselineInfo.slope;
            outTree.b_rms[outCh] = baselineInfo.rms;
            outTree.time[outCh] = timeInfo.first;
            outTree.time_chi2[outCh] = timeInfo.second;
            outTree.maximum[outCh] = WFs[channel]->GetAmpMax();
            outTree.amp_max[outCh] = WFs[channel]->GetInterpolatedAmpMax(-1,-1,opts.GetOpt<int>(channel+".signalWin", 2));
            outTree.charge_tot[outCh] = WFs[channel]->GetModIntegral(opts.GetOpt<int>(channel+".baselineInt", 1), 
                                                          nSamples);
            outTree.charge_sig[outCh] = WFs[channel]->GetSignalIntegral(opts.GetOpt<int>(channel+".signalInt", 0), 
                                                             opts.GetOpt<int>(channel+".signalInt", 1));
            //---template fit (only specified channels)
            WFFitResults fitResults{-1, -1000, -1};
            if(opts.GetOpt<bool>(channel+".templateFit"))
            {
	      fitResults = WFs[channel]->TemplateFit(opts.GetOpt<float>(channel+".templateFit.fitWin", 0), opts.GetOpt<int>(channel+".templateFit.fitWin", 1), opts.GetOpt<int>(channel+".templateFit.fitWin", 2));
	      outTree.fit_ampl[outCh] = fitResults.ampl;
	      outTree.fit_time[outCh] = fitResults.time;
	      outTree.fit_chi2[outCh] = fitResults.chi2;
            }
                 
            //---WFs---
            if(fillWFtree)
            {
                vector<float>* analizedWF = WFs[channel]->GetSamples();
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

    //---write output file -- trees + configuration
    outROOT->cd();
    if(opts.GetOpt<int>("global.fillWFtree"))
    {
        outWFTree.Write();
        outTree.AddFriend();
    }
    outTree.Write();
    opts.Write("cfg");
    outROOT->Close();
}

#endif
