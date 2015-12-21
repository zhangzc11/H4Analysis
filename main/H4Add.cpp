#ifndef __H4_ADD__
#define __H4_ADD__

#include "TFile.h"
#include "TKey.h"
#include "TChain.h"

#include "interface/CfgManager.h"
#include "plugins/PluginBase.h"

typedef map<string, PluginBase*(*)()> pluginsMap;
pluginsMap* PluginBaseFactory::map_;

//**********MAIN**************************************************************************
int main(int argc, char* argv[])
{
    vector<TChain*> trees;
    TFile* mergedFile = new TFile(argv[1], "RECREATE");
    TFile* headFile = new TFile(argv[2], "READ");
    headFile->cd();
    trees.push_back(new TChain("h4", "main_h4reco_tree"));
    TDirectory *currentDir = gDirectory;    
    TIter nextKey(currentDir->GetListOfKeys());
    TKey *key, *oldkey=0;
    while((key = (TKey*)nextKey()))
    {
        //---keep only the highest cycle number for each key and only TTree keys
        if((oldkey && !strcmp(oldkey->GetName(), key->GetName())) ||
           key->GetName() == string("h4"))
            continue;
        TObject *obj = key->ReadObj();
        if(obj->IsA()->GetName() == string("TTree"))
        {
            trees.push_back(new TChain(key->GetName(), key->GetTitle()));
            trees.back()->SetDirectory(0);
        }
        else
        {
            mergedFile->cd();
            obj->Write(key->GetName());
        }
    }            
    for(int iFile=2; iFile<argc; ++iFile)
        for(auto& tree : trees)
            tree->AddFile(argv[iFile]);
    for(int iTree=1; iTree<trees.size(); ++iTree)
    {
        mergedFile->cd();
        trees[iTree]->Merge(mergedFile, 0, "keep");
        trees[iTree]->BuildIndex("index");
        //trees[iTree]->Write(trees[iTree]->GetName(), TObject::kOverwrite);
        trees[0]->AddFriend(trees[iTree]->GetName());
    }

    mergedFile->cd();
    trees[0]->BuildIndex("index");
    trees[0]->Merge(mergedFile, 0, "keep");

    mergedFile->Close();
}

#endif
