#include "interface/InfoTree.h"

InfoTree::InfoTree(int* idx, TTree* tree)
{
    tree_ = tree ? tree : new TTree();

    index=idx;
    start_time=0;
    time_stamp=0;
    run=0;
    spill=0;
    event=0;   
    //---global branches
    tree_->Branch("index", index, "index/I");
    tree_->Branch("start_time", &start_time, "start_time/l");
    tree_->Branch("time_stamp", &time_stamp, "time_stamp/l");
    tree_->Branch("run", &run, "run/i");
    tree_->Branch("spill", &spill, "spill/i");
    tree_->Branch("event", &event, "event/i");
    // //---wire chamber branches
    // n_wchamber = nWireCh;
    // wireX = new float[nWireCh];
    // wireY = new float[nWireCh];
    // tree_->Branch("n_wchamber", &n_wchamber, "n_wchamber/I");
    // tree_->Branch("wireX", wireX, "wireX[n_wchamber]/F");
    // tree_->Branch("wireY", wireY, "wireY[n_wchamber]/F");
    // //---hodoscope branches
    // hodoX1=0;
    // hodoY1=0; 
    // hodoX2=0;
    // hodoY2=0;
    // if(H4hodo)
    // {
    //     tree_->Branch("hodoX1", &hodoX1, "hodoX1/F");
    //     tree_->Branch("hodoY1", &hodoY1, "hodoY1/F");
    //     tree_->Branch("hodoX2", &hodoX2, "hodoX2/F");
    //     tree_->Branch("hodoY2", &hodoY2, "hodoY2/F");
    // }
}

InfoTree::~InfoTree()
{
    delete tree_;
}
