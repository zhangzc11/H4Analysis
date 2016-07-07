#include "interface/FFTClass.h"

//----------Constructors------------------------------------------------------------------
FFTClass::FFTClass(int N, double* Re, double* Im)
{
    SetPointsComplex(N, Re, Im);
}

FFTClass::FFTClass(vector<double>& Re, vector<double>& Im):
    Re_(Re), Im_(Im)
{}
    
//----------Setters-----------------------------------------------------------------------
void FFTClass::SetPointsComplex(int N, double* Re, double* Im)
{
    for(int i=0; i<N; ++i)
    {
        Re_.push_back(Re[i]);
        Im_.push_back(Im[i]);
    }
}

void FFTClass::SetPointsComplex(vector<double>& Re, vector<double>& Im)
{
    Re_ = Re;
    Im_ = Im;
}
