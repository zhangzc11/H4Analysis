#include "interface/PositionTree.h"

PositionTree::PositionTree(uint64* idx, TTree* tree, int nPlanes)
{
    tree_ = tree ? tree : new TTree();

    n_planes = nPlanes;
    n_hitsX=0;
    n_hitsY=0;
    
    index=idx;
}

void PositionTree::Init()
{
    //---global branches
    tree_->Branch("index", index, "index/l");
    X = new vector<float>[n_planes];
    Y = new vector<float>[n_planes];
    //---position branches
    tree_->Branch("n_planes", &n_planes, "n_planes/I");
    tree_->Branch("n_hitsX", &n_hitsX, "n_hitsX/I");
    tree_->Branch("n_hitsY", &n_hitsY, "n_hitsY/I");
    tree_->Branch("X", &X);
    tree_->Branch("Y", &Y);
}
