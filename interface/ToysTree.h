#ifndef __TOYS_TREE__
#define __TOYS_TREE__

#include <string>
#include <vector>

#include "TTree.h"
#include "TString.h"

using namespace std;

typedef unsigned long int uint32;
typedef unsigned long long int uint64;
 
//****************************************************************************************

class ToysTree
{
public: 
    //---ctors---
    ToysTree(int nCh, int* idx);
    //---dtor---
    ~ToysTree();

    //---utils---
    void Fill() {tree_->Fill();};
    void Write(string name="toys_tree") {tree_->BuildIndex("index"); tree_->Write(name.c_str());};

    TTree* tree_; 

    int*   index;
    uint64 time_stamp;
    unsigned int n_toy_channels;
    float*       toy_time;
    float*       toy_time_chi2;
    float*       toy_maximum;
    float*       toy_amp_max;
    float*       toy_charge_tot;
    float*       toy_charge_sig;
    float*       toy_fit_ampl;
    float*       toy_fit_time;
    float*       toy_fit_chi2;
};

ToysTree::ToysTree(int nCh, int* idx)
{
    tree_ = new TTree();

    index=idx;
    time_stamp=0;
    //---set total number of WF samples 
    n_toy_channels = nCh;
    toy_time = new float[nCh];
    toy_time_chi2 = new float[nCh];
    toy_maximum = new float[nCh];
    toy_amp_max = new float[nCh];
    toy_charge_tot = new float[nCh];
    toy_charge_sig = new float[nCh];
    toy_fit_ampl = new float[nCh];
    toy_fit_time = new float[nCh];
    toy_fit_chi2 = new float[nCh];

    //---global branches
    tree_->Branch("index", index, "index/I");
    tree_->Branch("time_stamp", &time_stamp, "time_stamp/l");
    tree_->Branch("n_toy_channels", &n_toy_channels, "n_toy_channels/i");
    tree_->Branch("toy_time",       toy_time,       "toy_time[n_toy_channels]/F");
    tree_->Branch("toy_time_chi2",  toy_time_chi2,  "toy_time_chi2[n_toy_channels]/F");
    tree_->Branch("toy_maximum",    toy_maximum,    "toy_maximum[n_toy_channels]/F");
    tree_->Branch("toy_amp_max",    toy_amp_max,    "toy_amp_max[n_toy_channels]/F");
    tree_->Branch("toy_charge_tot", toy_charge_tot, "toy_charge_tot[n_toy_channels]/F");
    tree_->Branch("toy_charge_sig", toy_charge_sig, "toy_charge_sig[n_toy_channels]/F");
    tree_->Branch("toy_fit_ampl",   toy_fit_ampl,   "toy_fit_ampl[n_toy_channels]/F");
    tree_->Branch("toy_fit_time",   toy_fit_time,   "toy_fit_time[n_toy_channels]/F");
    tree_->Branch("toy_fit_chi2",   toy_fit_chi2,   "toy_fit_chi2[n_toy_channels]/F");
}

ToysTree::~ToysTree()
{
    delete[]  toy_time;
    delete[]  toy_time_chi2;
    delete[]  toy_maximum;
    delete[]  toy_amp_max;
    delete[]  toy_fit_ampl;
    delete[]  toy_fit_time;
    delete[]  toy_fit_chi2;

    delete tree_;
}

#endif
