#ifndef __T1065_TREE__
#define __T1065_TREE__

#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include "TTree.h"
#include "TString.h"

using namespace std;

typedef unsigned long int uint32;
typedef unsigned long long int uint64;
 
//****************************************************************************************

class T1065Tree
{
public: 
    //---ctors---
    T1065Tree(){};
    T1065Tree(uint64* idx, TTree* tree=NULL, string prefix="");
    //---dtor---
    ~T1065Tree();

    //---utils---
    void Init(vector<string>& names, vector<string>& timetypes);
    void Fill() {tree_->Fill();};
    TTree*        tree_; 
    string        prefix_;
    uint64*       index;

  unsigned int  n_channels = 9;
  unsigned int  n_samples = 1024;
  unsigned int  n_groups = 4;

/*
  short b_c[4][9][1024] = {0}, tc[4] = {0}; 
  float time[4][1024] = {0};
  short raw[36][1024] = {0};
  short channel[36][1024] = {0};
  float channelCorrected[36][1024] = {0};
  float xmin[36] = {0};
  float base[36] = {0};
  float amp[36] = {0};
  float integral[36] = {0};
  float integralFull[36] = {0};
  float gauspeak[36] = {0};
  float linearTime0[36] = {0};
  float linearTime15[36] = {0};
  float linearTime30[36] = {0};
  float linearTime45[36] = {0};
  float linearTime60[36] = {0};
  int t[36864] = {0};
  int t0[1024] = {0};
*/

  int   event;
  short b_c[4][9][1024], tc[4]; 
  float time[4][1024];
  short raw[36][1024];
  short channel[36][1024];
  float channelCorrected[36][1024];
  float xmin[36];
  float base[36];
  float amp[36];
  float integral[36];
  float integralFull[36];
  float gauspeak[36];
  float linearTime0[36];
  float linearTime15[36];
  float linearTime30[36];
  float linearTime45[36];
  float linearTime60[36];
  int t[36864];
  int t0[1024];

};

#endif
