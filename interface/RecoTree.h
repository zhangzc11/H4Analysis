#ifndef __RECO_TREE__
#define __RECO_TREE__

#include <string>
#include <vector>

#include "TTree.h"
#include "TString.h"

using namespace std;

typedef unsigned long int uint32;
typedef unsigned long long int uint64;
 
//****************************************************************************************

class RecoTree
{
public: 
    //---ctors---
    RecoTree(vector<string> names, int nSamples, bool H4hodo, int nWireCh, int* idx);
    //---dtor---
    ~RecoTree();

    //---utils---
    void Fill() {tree_->Fill();};
    void Write(string name="reco_tree") {tree_->Write(name.c_str());}
    void AddFriend(string friend_name="wf_tree") {tree_->AddFriend(friend_name.c_str());}

    TTree* tree_; 

    int*         index;
    uint64       start_time;
    uint64       time_stamp;
    unsigned int run;
    unsigned int spill;
    unsigned int event;   
    unsigned int n_channels;
    int*         channels;
    float*       b_charge;
    float*       b_slope;
    float*       b_rms;
    float*       time;
    float*       time_chi2;
    float*       maximum;
    float*       amp_max;
    float*       charge_tot;
    float*       charge_sig;
    float*       fit_ampl;
    float*       fit_time;
    float*       fit_chi2;
    int          n_wchamber;
    float*       wireX;
    float*       wireY;
    float        hodoX1;
    float        hodoY1;
    float        hodoX2;
    float        hodoY2;
};

RecoTree::RecoTree(vector<string> names, int nSamples, bool H4hodo, int nWireCh, int* idx)
{
    tree_ = new TTree();
    unsigned int nCh=names.size();

    index=idx;
    start_time=0;
    time_stamp=0;
    run=0;
    spill=0;
    event=0;   
    //---allocate enough space for all channels
    n_channels = nCh;
    channels = new int[nCh];
    b_charge = new float[nCh];
    b_slope = new float[nCh];
    b_rms = new float[nCh];
    time = new float[nCh];
    time_chi2 = new float[nCh];
    maximum = new float[nCh];
    amp_max = new float[nCh];
    charge_tot = new float[nCh];
    charge_sig = new float[nCh];
    fit_ampl = new float[nCh];
    fit_time = new float[nCh];
    fit_chi2 = new float[nCh];
    //---global branches
    tree_->Branch("index", index, "index/I");
    tree_->Branch("start_time", &start_time, "start_time/l");
    tree_->Branch("time_stamp", &time_stamp, "time_stamp/l");
    tree_->Branch("run", &run, "run/i");
    tree_->Branch("spill", &spill, "spill/i");
    tree_->Branch("event", &event, "event/i");
    tree_->Branch("n_channels", &n_channels, "n_channels/i");
    tree_->Branch("b_charge", b_charge, "b_charge[n_channels]/F");
    tree_->Branch("b_slope", b_slope, "b_slope[n_channels]/F");
    tree_->Branch("b_rms", b_rms, "b_rms[n_channels]/F");
    tree_->Branch("time", time, "time[n_channels]/F");
    tree_->Branch("time_chi2", time_chi2, "time_chi2[n_channels]/F");
    tree_->Branch("maximum", maximum, "maximum[n_channels]/F");
    tree_->Branch("amp_max", amp_max, "amp_max[n_channels]/F");
    tree_->Branch("charge_tot", charge_tot, "charge_tot[n_channels]/F");
    tree_->Branch("charge_sig", charge_sig, "charge_sig[n_channels]/F");
    tree_->Branch("fit_ampl", fit_ampl, "fit_ampl[n_channels]/F");
    tree_->Branch("fit_time", fit_time, "fit_time[n_channels]/F");
    tree_->Branch("fit_chi2", fit_chi2, "fit_chi2[n_channels]/F");
    //---channels branches
    for(int iCh=0; iCh<nCh; iCh++)
    {
        channels[iCh]=iCh;
        tree_->Branch(names[iCh].c_str(), &(channels[iCh]), (names[iCh]+"/I").c_str());
    }
    //---wire chamber branches
    n_wchamber = nWireCh;
    wireX = new float[nWireCh];
    wireY = new float[nWireCh];
    tree_->Branch("n_wchamber", &n_wchamber, "n_wchamber/I");
    tree_->Branch("wireX", wireX, "wireX[n_wchamber]/F");
    tree_->Branch("wireY", wireY, "wireY[n_wchamber]/F");
    //---hodoscope branches
    hodoX1=0;
    hodoY1=0; 
    hodoX2=0;
    hodoY2=0;
    if(H4hodo)
    {
        tree_->Branch("hodoX1", &hodoX1, "hodoX1/F");
        tree_->Branch("hodoY1", &hodoY1, "hodoY1/F");
        tree_->Branch("hodoX2", &hodoX2, "hodoX2/F");
        tree_->Branch("hodoY2", &hodoY2, "hodoY2/F");
    }
}

RecoTree::~RecoTree()
{
    delete[] channels;
    delete[] b_charge;
    delete[] b_slope;
    delete[] b_rms;
    delete[] time;
    delete[] amp_max;
    delete[] maximum;
    delete[] charge_tot;
    delete[] charge_sig;
    delete[] fit_ampl;
    delete[] fit_time;
    delete[] fit_chi2;

    delete tree_;
}

#endif
