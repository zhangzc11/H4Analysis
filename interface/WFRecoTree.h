#ifndef __WFRECO_TREE__
#define __WFRECO_TREE__

#include <string>
#include <vector>

#include "TTree.h"
#include "TString.h"

using namespace std;

typedef unsigned long int uint32;
typedef unsigned long long int uint64;
 
//****************************************************************************************

class WFRecoTree
{
public: 
    //---ctors---
  WFRecoTree(int nCh, int* idx, TString prefix="toy_");
    //---dtor---
  ~WFRecoTree();

    //---utils---
    void Fill() {tree_->Fill();};
    void Write(string name="toys_tree") {tree_->BuildIndex("index"); tree_->Write(name.c_str());};

    TTree* tree_; 
    TString prefix_;

    int*   index;
    uint64 time_stamp;
    unsigned int n_channels;
    float*       time;
    float*       time_chi2;
    float*       maximum;
    float*       amp_max;
    float*       charge_tot;
    float*       charge_sig;
    float*       fit_ampl;
    float*       fit_time;
    float*       fit_chi2;
};

WFRecoTree::WFRecoTree(int nCh, int* idx, TString prefix)
{
  prefix_=prefix;
  tree_ = new TTree();

    index=idx;
    time_stamp=0;
    //---set total number of WF samples 
    n_channels = nCh;
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
    tree_->Branch("time_stamp", &time_stamp, "time_stamp/l");
    tree_->Branch(Form("n_%schannels",prefix_.Data()), &n_channels, Form("n_%schannels/i",prefix_.Data()));
    tree_->Branch(Form("%stime",prefix_.Data()),      time,       Form("%stime[n_%schannels]/F",prefix_.Data(),prefix_.Data()));
    tree_->Branch(Form("%stime_chi2",prefix_.Data()),  time_chi2,  Form("%stime_chi2[n_%schannels]/F",prefix_.Data(),prefix_.Data()));
    tree_->Branch(Form("%smaximum",prefix_.Data()),    maximum,    Form("%smaximum[n_%schannels]/F",prefix_.Data(),prefix_.Data()));
    tree_->Branch(Form("%samp_max",prefix_.Data()),    amp_max,    Form("%samp_max[n_%schannels]/F",prefix_.Data(),prefix_.Data()));
    tree_->Branch(Form("%scharge_tot",prefix_.Data()), charge_tot, Form("%scharge_tot[n_%schannels]/F",prefix_.Data(),prefix_.Data()));
    tree_->Branch(Form("%scharge_sig",prefix_.Data()), charge_sig, Form("%scharge_sig[n_%schannels]/F",prefix_.Data(),prefix_.Data()));
    tree_->Branch(Form("%sfit_ampl",prefix_.Data()),   fit_ampl,   Form("%sfit_ampl[n_%schannels]/F",prefix_.Data(),prefix_.Data()));
    tree_->Branch(Form("%sfit_time",prefix_.Data()),   fit_time,   Form("%sfit_time[n_%schannels]/F",prefix_.Data(),prefix_.Data()));
    tree_->Branch(Form("%sfit_chi2",prefix_.Data()),   fit_chi2,   Form("%sfit_chi2[n_%schannels]/F",prefix_.Data(),prefix_.Data()));
}

WFRecoTree::~WFRecoTree()
{
    delete[]  time;
    delete[]  time_chi2;
    delete[]  maximum;
    delete[]  amp_max;
    delete[]  fit_ampl;
    delete[]  fit_time;
    delete[]  fit_chi2;

    delete tree_;
}

#endif
