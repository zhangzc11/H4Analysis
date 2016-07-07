//****************************************************************************************
//*** collection of variuos utilities used by different plugins
//***
//****************************************************************************************

#include <string>

#include "TH1F.h"
#include "TH2F.h"

using namespace std;

//----------Smart Profile-----------------------------------------------------------------
//---generate profile from TH2 istograms using an adaptive window instead of the full
//   Y range as in TProfile.
//   For the moment the parameters are not configurable...
TH1F* GetIterativeProfile(TH2* h2d, string name="");

//---------*------------------------------------------------------------------------------
