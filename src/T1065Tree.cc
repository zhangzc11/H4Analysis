#include "interface/T1065Tree.h"

T1065Tree::T1065Tree(uint64* idx, TTree* tree, string prefix)
{
    prefix_=prefix;
    tree_ = tree ? tree : new TTree();

    index=idx;
}

void T1065Tree::Init(vector<string>& names, vector<string>& timetypes)
{
    //---allocate enough space for all channels
    //n_times = timetypes.size()*n_channels;
//    n_channels = names.size();
//    n_groups = 4;
//    n_samples = 1024;

    //---channels branches
/*    for(int iCh=0; iCh<n_channels; iCh++)
    {
        channels[iCh]=iCh;
        tree_->Branch(names[iCh].c_str(), &(channels[iCh]), (names[iCh]+"/I").c_str());
    }
    //---time types branches
    for(int iT=0; iT<timetypes.size(); iT++)
    {
        time_types[iT]=iT*n_channels;
        tree_->Branch(timetypes[iT].c_str(), &(time_types[iT]), (timetypes[iT]+"/I").c_str());
    }
*/
 
  //---global branches    
    string size_channel_var = "n_"+prefix_+"channels";
    string size_group_var = "n_"+prefix_+"groups";
    string size_sample_var = "n_"+prefix_+"samples";
    string size_nc_x_ng_var = "n_"+prefix_+"ncgroups";
    string size_nc_x_ng_x_ns_var = "n_"+prefix_+"ncgsamples";
	
/*
	tree_->Branch(size_channel_var.c_str(), &n_channels, (size_channel_var+"/i").c_str());	
	tree_->Branch(size_group_var.c_str(), &n_groups, (size_group_var+"/i").c_str());	
	tree_->Branch(size_sample_var.c_str(), &n_samples, (size_sample_var+"/i").c_str());	
	
	tree_->Branch((prefix_+"b_c").c_str(), b_c, (prefix_+"b_c["+size_group_var+"]["+size_channel_var+"]["+size_sample_var+"]/S").c_str());
	tree_->Branch((prefix_+"tc").c_str(), tc, (prefix_+"tc["+size_group_var+"]/S").c_str());
	tree_->Branch((prefix_+"time").c_str(), time, (prefix_+"time["+size_group_var+"]["+size_sample_var+"]/F").c_str());
	tree_->Branch((prefix_+"raw").c_str(), raw, (prefix_+"raw["+size_nc_x_ng_var+"]["+size_sample_var+"]/S").c_str());
	tree_->Branch((prefix_+"channel").c_str(), channel, (prefix_+"channel["+size_group_var+"]["+size_sample_var+"]/S").c_str());
	tree_->Branch((prefix_+"channelCorrected").c_str(), channelCorrected, (prefix_+"channelCorrected["+size_nc_x_ng_var+"]["+size_sample_var+"]/F").c_str());
	tree_->Branch((prefix_+"xmin").c_str(), xmin, (prefix_+"xmin["+size_nc_x_ng_var+"]/F").c_str());
	tree_->Branch((prefix_+"base").c_str(), base, (prefix_+"base["+size_nc_x_ng_var+"]/F").c_str());
	tree_->Branch((prefix_+"amp").c_str(), amp, (prefix_+"amp["+size_nc_x_ng_var+"]/F").c_str());
	tree_->Branch((prefix_+"integral").c_str(), integral, (prefix_+"integral["+size_nc_x_ng_var+"]/F").c_str());
	tree_->Branch((prefix_+"integralFull").c_str(), integralFull, (prefix_+"integralFull["+size_nc_x_ng_var+"]/F").c_str());
	tree_->Branch((prefix_+"gauspeak").c_str(), gauspeak, (prefix_+"gauspeak["+size_nc_x_ng_var+"]/F").c_str());
	tree_->Branch((prefix_+"linearTime0").c_str(), linearTime0, (prefix_+"linearTime0["+size_nc_x_ng_var+"]/F").c_str());
	tree_->Branch((prefix_+"linearTime15").c_str(), linearTime15, (prefix_+"linearTime15["+size_nc_x_ng_var+"]/F").c_str());
	tree_->Branch((prefix_+"linearTime30").c_str(), linearTime30, (prefix_+"linearTime30["+size_nc_x_ng_var+"]/F").c_str());
	tree_->Branch((prefix_+"linearTime45").c_str(), linearTime45, (prefix_+"linearTime45["+size_nc_x_ng_var+"]/F").c_str());
	tree_->Branch((prefix_+"linearTime60").c_str(), linearTime60, (prefix_+"linearTime60["+size_nc_x_ng_var+"]/F").c_str());
	tree_->Branch((prefix_+"t").c_str(), t, (prefix_+"t["+size_nc_x_ng_x_ns_var+"]/I").c_str());
	tree_->Branch((prefix_+"t0").c_str(), t0, (prefix_+"t0["+size_sample_var+"]/I").c_str());
*/

  tree_->Branch("ngroups",   &n_groups, "ngroups/i");
  tree_->Branch("nsamples",   &n_samples, "nsamples/i");
  tree_->Branch("nchannels",   &n_channels, "nchannels/i");
  tree_->Branch("tc",   tc, "tc[4]/s");
  tree_->Branch("b_c",  b_c, "b_c[36864]/s"); //this is for 9 channels per group
  tree_->Branch("raw", raw, "raw[36][1024]/S");   
  tree_->Branch("t",  t, "t[36864]/I");    
  tree_->Branch("channel", channel, "channel[36][1024]/S");
  tree_->Branch("channelCorrected", channelCorrected, "channelCorrected[36][1024]/S");
  tree_->Branch("t0",  t0, "t0[1024]/I");
  tree_->Branch("time", time, "time[4][1024]/F");
  tree_->Branch("xmin", xmin, "xmin[36]/F");
  tree_->Branch("amp", amp, "amp[36]/F");
  tree_->Branch("base", base, "base[36]/F");
  tree_->Branch("int", integral, "int[36]/F");
  tree_->Branch("intfull", integralFull, "intfull[36]/F");
  tree_->Branch("gauspeak", gauspeak, "gauspeak[36]/F");
  tree_->Branch("linearTime0", linearTime0, "linearTime0[36]/F");
  tree_->Branch("linearTime15", linearTime15, "linearTime15[36]/F");
  tree_->Branch("linearTime30", linearTime30, "linearTime30[36]/F");
  tree_->Branch("linearTime45", linearTime45, "linearTime45[36]/F");
  tree_->Branch("linearTime60", linearTime60, "linearTime60[36]/F");

}

T1065Tree::~T1065Tree()
{}
