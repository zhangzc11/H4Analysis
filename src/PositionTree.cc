#include "interface/PositionTree.h"

PositionTree::PositionTree(uint64* idx, TTree* tree, int nPlanes)
{
    tree_ = tree ? tree : new TTree();

    index=idx;
    n_planes = nPlanes;
}

void PositionTree::Init()
{
    //---global branches
    tree_->Branch("index", index, "index/l");
    //---position branches
    X = new float[n_planes];
    Y = new float[n_planes];
    nFibresOnX = new int[n_planes];
    nFibresOnY = new int[n_planes];

    tree_->Branch("n_planes", &n_planes, "n_planes/I");
    tree_->Branch("X", X, "X[n_planes]/F");
    tree_->Branch("Y", Y, "Y[n_planes]/F");
    tree_->Branch("nFibresOnX", nFibresOnX, "nFibresOnX[n_planes]/I");
    tree_->Branch("nFibresOnY", nFibresOnY, "nFibresOnY[n_planes]/I");

}
