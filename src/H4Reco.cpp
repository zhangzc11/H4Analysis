#ifndef __MAIN__
#define __MAIN__

#include <string>

#include "TFile.h"
#include "TTree.h"

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
    //---mandatory opts
    string path=opts.GetOpt<string>("h4reco.path2data");
    string run=opts.GetOpt<string>("h4reco.run");
    if(argc > 2)
        run = argv[2];
    
    
