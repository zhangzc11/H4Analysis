#ifndef RunTwoFitMgg_HH
#define RunTwoFitMgg_HH
//C++ INCLUDES
#include <sstream>
#include <string>
//ROOT INCLUDES
#include <TTree.h>
#include <TString.h>
//ROOFIT INCLUDES
#include <RooRealVar.h>
#include <RooWorkspace.h>
#include <RooDataSet.h>
#include <RooAbsPdf.h>
//LOCAL INCLUDES
//

void drawWF(std::string fname);
void drawTimeReso(std::string fname);
void drawNoise(std::string fname);

#endif
