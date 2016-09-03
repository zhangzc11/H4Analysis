//C++ INCLUDES
#include <vector>
#include <fstream>
//ROOT INCLUDES
//#include <TSYSTEM.h>
#include <TSystem.h>
#include <TTree.h>
#include <TLatex.h>
#include <TString.h>
#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TRandom3.h>
#include <TLegend.h>
#include <TMath.h>
#include <TBox.h>
#include <TMath.h>
#include <TROOT.h>
#include <Math/GaussIntegrator.h>
#include <Math/IntegratorOptions.h>
//ROOFIT INCLUDES
#include <RooWorkspace.h>
#include <RooDataSet.h>
#include <RooRealVar.h>
#include <RooExponential.h>
#include <RooAddPdf.h>
#include <RooGaussian.h>
#include <RooMinimizer.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <RooExtendPdf.h>
#include <RooStats/SPlot.h>
#include <RooStats/ModelConfig.h>
#include <RooGenericPdf.h>
#include <RooFormulaVar.h>
#include <RooBernstein.h>
#include <RooMinuit.h>
#include <RooNLLVar.h>
#include <RooRandom.h>
#include <RooDataHist.h>
#include <RooHistPdf.h>
//#include <RealVar.h>
//LOCAL INCLUDES
#include "DrawTimeReso.hh"
#include "digiSettings.hh"

void drawWF(std::string fname)
{
	TFile* f = new TFile(fname.c_str() , "READ" );
	TTree* tree =  (TTree*)f->Get("wf");
	TCanvas *cv = new TCanvas("cv","cv",800,600);
	tree->Draw("WF_val:WF_time",("WF_ch=="+std::to_string(channelsNames::XTAL_C3)).c_str());
	cv->SaveAs("plots/test_WF_C3.pdf");
	cv->SaveAs("plots/test_WF_C3.png");
	
	tree->Draw("WF_val:WF_time",("WF_ch=="+std::to_string(channelsNames::XTAL_C2)).c_str());
	cv->SaveAs("plots/test_WF_C2.pdf");
	cv->SaveAs("plots/test_WF_C2.png");

	tree->Draw("WF_val:WF_time",("WF_ch=="+std::to_string(channelsNames::XTAL_C4)).c_str());
	cv->SaveAs("plots/test_WF_C4.pdf");
	cv->SaveAs("plots/test_WF_C4.png");

	tree->Draw("WF_val:WF_time",("WF_ch=="+std::to_string(channelsNames::XTAL_B2)).c_str());
	cv->SaveAs("plots/test_WF_B2.pdf");
	cv->SaveAs("plots/test_WF_B2.png");

	tree->Draw("WF_val:WF_time",("WF_ch=="+std::to_string(channelsNames::XTAL_B3)).c_str());
	cv->SaveAs("plots/test_WF_B3.pdf");
	cv->SaveAs("plots/test_WF_B3.png");

	tree->Draw("WF_val:WF_time",("WF_ch=="+std::to_string(channelsNames::XTAL_D2)).c_str());
	cv->SaveAs("plots/test_WF_D2.pdf");
	cv->SaveAs("plots/test_WF_D2.png");

	tree->Draw("WF_val:WF_time",("WF_ch=="+std::to_string(channelsNames::XTAL_D3)).c_str());
	cv->SaveAs("plots/test_WF_D3.pdf");
	cv->SaveAs("plots/test_WF_D3.png");

	tree->Draw("WF_val:WF_time",("WF_ch=="+std::to_string(channelsNames::XTAL_D4)).c_str());
	cv->SaveAs("plots/test_WF_D4.pdf");
	cv->SaveAs("plots/test_WF_D4.png");

};

void drawTimeReso(std::string fname)
{
	TFile* f = new TFile(fname.c_str() , "READ" );
	TTree* tree =  (TTree*)f->Get("digi");
	TCanvas *cv = new TCanvas("cv","cv",800,600);
	
	tree->Draw(("fit_time["+std::to_string(channelsNames::XTAL_C3)+"]-time["+std::to_string(channelsNames::MCP1)+"]").c_str(),("amp_max["+std::to_string(channelsNames::MCP1)+"]>100 && fit_ampl["+std::to_string(channelsNames::XTAL_C3)+"]>700").c_str());
	cv->SaveAs("plots/test_TimeReso_C3.pdf");
	cv->SaveAs("plots/test_TimeReso_C3.png");
	
	tree->Draw(("fit_time["+std::to_string(channelsNames::XTAL_C2)+"]-time["+std::to_string(channelsNames::MCP1)+"]").c_str(),("amp_max["+std::to_string(channelsNames::MCP1)+"]>100 && fit_ampl["+std::to_string(channelsNames::XTAL_C2)+"]>700").c_str());
	cv->SaveAs("plots/test_TimeReso_C2.pdf");
	cv->SaveAs("plots/test_TimeReso_C2.png");
	
	tree->Draw(("fit_time["+std::to_string(channelsNames::XTAL_C4)+"]-time["+std::to_string(channelsNames::MCP1)+"]").c_str(),("amp_max["+std::to_string(channelsNames::MCP1)+"]>100 && fit_ampl["+std::to_string(channelsNames::XTAL_C4)+"]>700").c_str());
	cv->SaveAs("plots/test_TimeReso_C4.pdf");
	cv->SaveAs("plots/test_TimeReso_C4.png");

	tree->Draw(("fit_time["+std::to_string(channelsNames::XTAL_B2)+"]-time["+std::to_string(channelsNames::MCP1)+"]").c_str(),("amp_max["+std::to_string(channelsNames::MCP1)+"]>100 && fit_ampl["+std::to_string(channelsNames::XTAL_B2)+"]>700").c_str());
	cv->SaveAs("plots/test_TimeReso_B2.pdf");
	cv->SaveAs("plots/test_TimeReso_B2.png");
	
	tree->Draw(("fit_time["+std::to_string(channelsNames::XTAL_B3)+"]-time["+std::to_string(channelsNames::MCP1)+"]").c_str(),("amp_max["+std::to_string(channelsNames::MCP1)+"]>100 && fit_ampl["+std::to_string(channelsNames::XTAL_B3)+"]>700").c_str());
	cv->SaveAs("plots/test_TimeReso_B3.pdf");
	cv->SaveAs("plots/test_TimeReso_B3.png");

	tree->Draw(("fit_time["+std::to_string(channelsNames::XTAL_D2)+"]-time["+std::to_string(channelsNames::MCP1)+"]").c_str(),("amp_max["+std::to_string(channelsNames::MCP1)+"]>100 && fit_ampl["+std::to_string(channelsNames::XTAL_D2)+"]>700").c_str());
	cv->SaveAs("plots/test_TimeReso_D2.pdf");
	cv->SaveAs("plots/test_TimeReso_D2.png");
	
	tree->Draw(("fit_time["+std::to_string(channelsNames::XTAL_D3)+"]-time["+std::to_string(channelsNames::MCP1)+"]").c_str(),("amp_max["+std::to_string(channelsNames::MCP1)+"]>100 && fit_ampl["+std::to_string(channelsNames::XTAL_D3)+"]>700").c_str());
	cv->SaveAs("plots/test_TimeReso_D3.pdf");
	cv->SaveAs("plots/test_TimeReso_D3.png");
	
	tree->Draw(("fit_time["+std::to_string(channelsNames::XTAL_D4)+"]-time["+std::to_string(channelsNames::MCP1)+"]").c_str(),("amp_max["+std::to_string(channelsNames::MCP1)+"]>100 && fit_ampl["+std::to_string(channelsNames::XTAL_D4)+"]>700").c_str());
	cv->SaveAs("plots/test_TimeReso_D4.pdf");
	cv->SaveAs("plots/test_TimeReso_D4.png");
	
};


void drawNoise(std::string fname)
{
	TFile* f = new TFile(fname.c_str() , "READ" );
        TTree* tree =  (TTree*)f->Get("wf");
        TCanvas *cv = new TCanvas("cv","cv",800,600);
	
	float WF_val_tmp[nSample];
	float WF_time_tmp[nSample];
	float WF_val_AVE[nSample] = {0.0};
 	tree->SetBranchAddress("WF_val", WF_val_tmp);
 	tree->SetBranchAddress("WF_time", WF_time_tmp);
	int nEntries = tree->GetEntries();
	for(int i=0;i<nEntries;i++)
	{
		tree->GetEntry(i);
		for(int digi=0;digi<nSample;digi++)
		{
			WF_val_AVE[digi] += WF_val_tmp[digi]/nEntries;	
		}
	}

	TGraph *gr = new TGraph(nSample, WF_time_tmp, WF_val_AVE);
	gr->Draw();	
	cv->SaveAs("plots/test_noise_C3.pdf");
	cv->SaveAs("plots/test_noise_C3.png");


};

 
