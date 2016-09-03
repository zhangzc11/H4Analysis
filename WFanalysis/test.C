void test(){
	TFile* f = new TFile("../ntuples/analysis_5297.root" , "READ" );
        TTree* tree =  (TTree*)f->Get("wf");
        TCanvas *cv = new TCanvas("cv","cv",800,600);
	const int nSample = 1024;

	int WF_samples;
        float WF_val_tmp[1024000];
        int WF_ch_tmp[1024000];
        float WF_time_tmp[1024000];
        float WF_time_C3[nSample];
        float WF_val_C3[nSample] = {0.0};
        tree->SetBranchAddress("WF_samples", &WF_samples);
        tree->SetBranchAddress("WF_ch", WF_ch_tmp);
        tree->SetBranchAddress("WF_val", WF_val_tmp);
        tree->SetBranchAddress("WF_time", WF_time_tmp);
        int nEntries = tree->GetEntries();
        for(int i=0;i<nEntries;i++)
        {
                tree->GetEntry(i);
		int digi_thisCH=0;
                for(int digi=0;digi<WF_samples;digi++)
                {
			if(WF_ch_tmp[digi]==5)
			{
                        WF_val_C3[digi_thisCH] += abs(WF_val_tmp[digi])/nEntries;
			WF_time_C3[digi_thisCH] = WF_time_tmp[digi];
			digi_thisCH++;
			}
                }
        }

        TGraph *gr = new TGraph(nSample, WF_time_C3, WF_val_C3);
        gr->SetMarkerStyle(7);
        gr->GetXaxis()->SetRangeUser(0,200);
        gr->GetXaxis()->SetTitle("t[ns]");
	gr->Draw("APZ");
        cv->SaveAs("plots/test_noise_C3.pdf");
        cv->SaveAs("plots/test_noise_C3.png");
}
