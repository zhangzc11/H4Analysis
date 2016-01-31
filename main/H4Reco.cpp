#ifndef __H4_RECO__
#define __H4_RECO__

#include <unistd.h>
#include <csignal>
#include <iostream>
#include <fstream>
#include <string>

#include "TFile.h"
#include "TChain.h"

#include "interface/CfgManager.h"
#include "interface/CfgManagerT.h"
#include "interface/RecoTree.h"
#include "plugins/PluginBase.h"

typedef map<string, PluginBase*(*)()> pluginsMap;
pluginsMap* PluginBaseFactory::map_;

//----------Simple function to track memory and CPU usage---------------------------------
void TrackProcess(float* cpu, float* mem, float* vsz, float* rss)
{
    string dummy1, dummy2, dummy3, dummy4, dummy5;
    string time;
    //---get cpu/mem info
    int pid = getpid();
    string ps_command = "ps up "+to_string(pid)+" >.H4Reco_proc.tmp";
    system(ps_command.c_str());
    ifstream proc_tmp(".H4Reco_proc.tmp", ios::in);
    getline(proc_tmp, dummy1);
    proc_tmp >> dummy1 >> dummy2 >> cpu[0] >> mem[0] >> vsz[0] >> rss[0]
             >> dummy3 >> dummy4 >> dummy5 >> time;
    vsz[0] = vsz[0]/1000;
    rss[0] = rss[0]/1000;
    proc_tmp.close();
    if(cpu[0]>cpu[1])
        cpu[1] = cpu[0];
    if(mem[0]>mem[1])
        mem[1] = mem[0];
    if(vsz[0]>vsz[1])
        vsz[1] = vsz[0];
    if(rss[0]>rss[1])
        rss[1] = rss[0];
    //---print statistics
    cout << "-----Machine stats---current/max-----" << endl
         << "CPU(%): " << cpu[0] << "/" << cpu[1] << endl
         << "MEM(%): " << mem[0] << "/" << mem[1] << endl
         << "VSZ(M): " << vsz[0] << "/" << vsz[1] << endl
         << "RSS(M): " << rss[0] << "/" << rss[1] << endl
         << "time lasted: " << time << endl;
}
                  
//----------Get input files---------------------------------------------------------------
void ReadInputFiles(CfgManager& opts, TChain* inTree)
{
    int nFiles=0;
    string ls_command;
    string file;
    string path=opts.GetOpt<string>("h4reco.path2data");
    string run=opts.GetOpt<string>("h4reco.run");

    //---Get file list searching in specified path (eos or locally)
    if(path.find("/eos/cms") != string::npos)
        ls_command = string("gfal-ls root://eoscms/"+path+run+" | grep 'root' > tmp/"+run+".list");
    else
        ls_command = string("ls "+path+run+" | grep 'root' > tmp/"+run+".list");
    system(ls_command.c_str());
    ifstream waveList(string("tmp/"+run+".list").c_str(), ios::in);
    while(waveList >> file && (opts.GetOpt<int>("h4reco.maxFiles")<0 || nFiles<opts.GetOpt<int>("h4reco.maxFiles")) )
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

    return;
}

//**********MAIN**************************************************************************
int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cout << argv[0] << " cfg file " << "[run]" << endl; 
        return -1;
    }

    //---memory consumption tracking---
    float cpu[2]{0}, mem[2]={0}, vsz[2]={0}, rss[2]={0};

    //---load options---    
    CfgManager opts;
    opts.ParseConfigFile(argv[1]);

    //-----input setup-----    
    if(argc > 2)
    {
        vector<string> run(1, argv[2]);
        opts.SetOpt("h4reco.run", run);
    }
    string outSuffix = opts.GetOpt<string>("h4reco.outNameSuffix");
    string run = opts.GetOpt<string>("h4reco.run");
    TChain* inTree = new TChain("H4tree");
    ReadInputFiles(opts, inTree);
    H4Tree h4Tree(inTree);

    //-----output setup-----
    uint64 index=stoul(run)*1e9;
    TFile* outROOT = new TFile("ntuples/"+outSuffix+TString(run)+".root", "RECREATE");
    RecoTree mainTree(&index);

    //---Get plugin sequence---
    map<string, PluginBase*> pluginMap;
    vector<PluginBase*> pluginSequence;
    vector<string> pluginList = opts.GetOpt<vector<string> >("h4reco.pluginList");    
    //---plugin creation
    for(auto& plugin : pluginList)
    {
        cout << ">>> Loading plugin <" << plugin << ">" << endl;
        PluginBase* newPlugin = PluginBaseFactory::CreateInstance(opts.GetOpt<string>(plugin+".pluginType"));
        if(newPlugin)
        {
            pluginSequence.push_back(newPlugin);
            pluginSequence.back()->SetInstanceName(plugin);
            pluginMap[plugin] = pluginSequence.back();
        }
        else
        {
            cout << ">>> ERROR: plugin type " << opts.GetOpt<string>(plugin+".pluginType") << " is not defined." << endl;
            return 0;
        }
    }
    
    //---begin
    for(auto& plugin : pluginSequence)
    {
        plugin->Begin(opts, &index);
        for(auto& shared : plugin->GetSharedData("", "TTree", true))
        {
            TTree* tree = (TTree*)shared.obj;
            tree->SetMaxVirtualSize(10000);
            tree->SetDirectory(outROOT);
        }
    }
            
    //---events loop
    int maxEvents=opts.GetOpt<int>("h4reco.maxEvents");
    cout << ">>> Processing H4DAQ run #" << run << " <<<" << endl;
    while(h4Tree.NextEntry() && (index-stoul(run)*1e9<maxEvents || maxEvents==-1))
    {
        if(index % 1000 == 0)
        {
            cout << ">>>Processed events: " << index-stoul(run)*1e9 << "/"
                 << (maxEvents<0 ? h4Tree.GetEntries() : min((int)h4Tree.GetEntries(), maxEvents))
                 << endl;
            TrackProcess(cpu, mem, vsz, rss);
        }
        //---call ProcessEvent for each plugin
        for(auto& plugin : pluginSequence)
            plugin->ProcessEvent(h4Tree, pluginMap, opts);

        //---fill the main tree with info variables and increase event counter
        mainTree.time_stamp = h4Tree.evtTimeStart;
        mainTree.run = h4Tree.runNumber;
        mainTree.spill = h4Tree.spillNumber;
        mainTree.event = h4Tree.evtNumber;
        mainTree.Fill();        
        ++index;
    }

    //---end
    for(auto& plugin : pluginSequence)
    {
        //---call endjob for each plugin
        plugin->End(opts);
        //---get permanent data from each plugin and store them in the out file
        for(auto& shared : plugin->GetSharedData())
        {
            if(shared.obj->IsA()->GetName() == string("TTree"))
            {
                TTree* currentTree = (TTree*)shared.obj;
                outROOT->cd();
                currentTree->BuildIndex("index");
                currentTree->Write(currentTree->GetName(), TObject::kOverwrite);
                mainTree.AddFriend(currentTree->GetName());
            }
            else
            {
                outROOT->cd();
                shared.obj->Write(shared.tag.c_str(), TObject::kOverwrite);
            }
        }
    }
    
    //---close
    mainTree.Write();
    opts.Write("cfg");
    outROOT->Close();
    TrackProcess(cpu, mem, vsz, rss);

    exit(0);
}    

#endif
