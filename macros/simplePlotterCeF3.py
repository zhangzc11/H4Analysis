import ROOT
import array
import itertools
ROOT.gROOT.SetBatch(True)

ROOT.gSystem.Load("lib/H4Dict.so")
ROOT.gSystem.Load("CfgManager/lib/CfgManagerDict.so")

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-r","--run")
(options,args)=parser.parse_args()

run=int(options.run)
file=ROOT.TFile("ntuples/analysis_"+str(run)+".root")
tree=file.Get("h4")

outfile=ROOT.TFile("plots/resolution_"+str(run)+".root","recreate")

h1=ROOT.TH1F("chint_4apd_1","chint_4apd_1",300,0,60000)
h1_hodosel=ROOT.TH1F("chint_4apd_1_hodosel","chint_4apd_1_hodosel",300,0,60000)
h1_hodosel.SetLineColor(ROOT.kBlue)
h1_hodosel_3x3=ROOT.TH1F("chint_4apd_1_hodosel_3x3","chint_4apd_1_hodosel_3x3",300,0,60000);
h1_hodosel_3x3.GetXaxis().SetTitle("Charge Integrated");
h1_hodosel_3x3.SetLineColor(ROOT.kRed)
h1_hodosel_3x3_calib=ROOT.TH1F("chint_4apd_1_hodosel_3x3_calib","chint_4apd_1_hodosel_3x3_calib",300,0,60000);
h1_hodosel_3x3_calib.GetXaxis().SetTitle("Charge Integrated");
h1_hodosel_3x3_calib.SetLineColor(ROOT.kRed)


h2_hodosel_3x3=ROOT.TH1F("chint_4apd_2_hodosel_3x3","chint_4apd_2_hodosel_3x3",300,0,60000);
h2_hodosel_3x3.GetXaxis().SetTitle("Charge Integrated");
h2_hodosel_3x3.SetLineColor(ROOT.kBlack)
h2_hodosel_3x3_calib=ROOT.TH1F("chint_4apd_2_hodosel_3x3_calib","chint_4apd_2_hodosel_3x3_calib",300,0,60000);
h2_hodosel_3x3_calib.GetXaxis().SetTitle("Charge Integrated");
h2_hodosel_3x3_calib.SetLineColor(ROOT.kBlack)


h3_hodosel_3x3=ROOT.TH1F("chint_4apd_3_hodosel_3x3","chint_4apd_3_hodosel_3x3",300,0,60000);
h3_hodosel_3x3.GetXaxis().SetTitle("Charge Integrated");
h3_hodosel_3x3.SetLineColor(ROOT.kViolet)
h3_hodosel_3x3_calib=ROOT.TH1F("chint_4apd_3_hodosel_3x3_calib","chint_4apd_3_hodosel_3x3_calib",300,0,60000);
h3_hodosel_3x3_calib.GetXaxis().SetTitle("Charge Integrated");
h3_hodosel_3x3_calib.SetLineColor(ROOT.kViolet)


h4_hodosel_3x3=ROOT.TH1F("chint_4apd_4_hodosel_3x3","chint_4apd_4_hodosel_3x3",300,0,60000);
h4_hodosel_3x3.GetXaxis().SetTitle("Charge Integrated");
h4_hodosel_3x3.SetLineColor(ROOT.kBlue)
h4_hodosel_3x3_calib=ROOT.TH1F("chint_4apd_4_hodosel_3x3_calib","chint_4apd_4_hodosel_3x3_calib",300,0,60000);
h4_hodosel_3x3_calib.GetXaxis().SetTitle("Charge Integrated");
h4_hodosel_3x3_calib.SetLineColor(ROOT.kBlue)


htot_hodosel_3x3=ROOT.TH1F("chint_4apd_hodosel_3x3","chint_4apd_hodosel_3x3",4*300,0,60000*4);
htot_hodosel_3x3.SetLineColor(ROOT.kBlack)

htot_hodosel_3x3_calib=ROOT.TH1F("chint_4apd_hodosel_3x3_calib","chint_4apd_hodosel_3x3_calib",4*300,0,60000*4);
htot_hodosel_3x3_calib.SetLineColor(ROOT.kBlack)

hmatrix_hodosel_3x3_matrix=ROOT.TH1F("chint_matrix_hodosel_3x3","chint_matrix_hodosel_3x3",4*300,0,60000*4);
htot_hodosel_3x3.SetLineColor(ROOT.kBlack)


for entry in tree:
#    print entry.event
    if entry.event == 1:
        #define here the xtals for a 3x3 matrix
        xtalMatrix = array.array('i',[tree.xtal1,tree.xtal2,tree.xtal3,tree.xtal6,tree.xtal4apd_1,tree.xtal4apd_2,tree.xtal4apd_3,tree.xtal4apd_4,tree.xtal11,tree.xtal14,tree.xtal15,tree.xtal16])

    h1.Fill(entry.charge_sig[entry.xtal4apd_1])
    if (entry.X[0]<-10 or entry.Y[0]<-10 or entry.X[1]<-10 or entry.Y[1]<-10):#hodo not reconstructed
        continue
    if (entry.nFibresOnX[0]!=2 or entry.nFibresOnX[1]!=2 or entry.nFibresOnY[0]!=2 or entry.nFibresOnY[1]!=2):#showering
        continue
    h1_hodosel.Fill(entry.charge_sig[entry.xtal4apd_1])
    if (ROOT.TMath.Abs(entry.X[0])>3 or ROOT.TMath.Abs(entry.X[1])>3 or ROOT.TMath.Abs(entry.Y[0])>3 or ROOT.TMath.Abs(entry.Y[1])>3):
        continue
    h1_hodosel_3x3.Fill(entry.charge_sig[entry.xtal4apd_1])
    h2_hodosel_3x3.Fill(entry.charge_sig[entry.xtal4apd_2])
    h3_hodosel_3x3.Fill(entry.charge_sig[entry.xtal4apd_3])
    h4_hodosel_3x3.Fill(entry.charge_sig[entry.xtal4apd_4])

    h1_hodosel_3x3_calib.Fill(entry.charge_sig[entry.xtal4apd_1]*entry.calibration[entry.xtal4apd_1])
    h2_hodosel_3x3_calib.Fill(entry.charge_sig[entry.xtal4apd_2]*entry.calibration[entry.xtal4apd_2])
    h3_hodosel_3x3_calib.Fill(entry.charge_sig[entry.xtal4apd_3]*entry.calibration[entry.xtal4apd_3])
    h4_hodosel_3x3_calib.Fill(entry.charge_sig[entry.xtal4apd_4]*entry.calibration[entry.xtal4apd_4])

    htot_hodosel_3x3.Fill(entry.charge_sig[entry.xtal4apd_1]+entry.charge_sig[entry.xtal4apd_2]+entry.charge_sig[entry.xtal4apd_3]+entry.charge_sig[entry.xtal4apd_4])
    htot_hodosel_3x3_calib.Fill(entry.charge_sig[entry.xtal4apd_1]*entry.calibration[entry.xtal4apd_1]+entry.charge_sig[entry.xtal4apd_2]*entry.calibration[entry.xtal4apd_2]+entry.charge_sig[entry.xtal4apd_3]*entry.calibration[entry.xtal4apd_3]+entry.charge_sig[entry.xtal4apd_4]*entry.calibration[entry.xtal4apd_4])
    matrixEn=0
    for xtal in xtalMatrix:
#        print xtal
        matrixEn+=entry.charge_sig[xtal]
    hmatrix_hodosel_3x3_matrix.Fill(matrixEn)

outfile.Write()
outfile.Close()
