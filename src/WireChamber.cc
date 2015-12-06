#include "interface/WireChamber.h"

//----------constructor-------------------------------------------------------------------
WireChamber::WireChamber(H4Tree* h4Tree, int chXl, int chXr, int chYu, int chYd):
    chXl_(chXl), chXr_(chXr), chYu_(chYu), chYd_(chYd), recoX_(0), recoY_(0)
{
    h4Tree_ = h4Tree;
}

//----------utils-------------------------------------------------------------------------
void WireChamber::Unpack()
{
    //---search the first (in time) hit for each channel
    vector<float> timeL, timeR, timeU, timeD;
    for(int iCh=0; iCh<h4Tree_->nTdcChannels; ++iCh)
    {
        if(h4Tree_->tdcChannel[iCh]==chXl_)
            timeL.push_back(h4Tree_->tdcData[iCh]);
        if(h4Tree_->tdcChannel[iCh]==chXr_)
            timeR.push_back(h4Tree_->tdcData[iCh]);
        if(h4Tree_->tdcChannel[iCh]==chYu_)
            timeU.push_back(h4Tree_->tdcData[iCh]);
        if(h4Tree_->tdcChannel[iCh]==chYd_)
            timeD.push_back(h4Tree_->tdcData[iCh]);
    }

    //---compute X and Y from channels times
    if(timeR.size()!=0 && timeL.size()!=0)
        recoX_ = (*min_element(timeR.begin(), timeR.begin()+timeR.size()) -
                  *min_element(timeL.begin(), timeL.begin()+timeL.size()))*0.005;
    else
        recoX_ = -1000;
    if(timeU.size()!=0 && timeD.size()!=0)
        recoY_ = (*min_element(timeU.begin(), timeU.begin()+timeU.size()) -
                  *min_element(timeD.begin(), timeD.begin()+timeD.size()))*0.005;
    else
        recoY_ = -1000;
    
    return;
}


        
