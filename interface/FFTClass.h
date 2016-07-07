#ifndef __FFTCLASS_H__
#define __FFTCLASS_H__

#include <iostream>
#include <string>
#include <vector>

#include "TObject.h"

using namespace std;

class FFTClass : public TObject
{
public:
    //---ctors---
    FFTClass() {};
    FFTClass(int N, double* Re, double* Im);
    FFTClass(vector<double>& Re, vector<double>& Im);
    
    //---dtor---
    ~FFTClass() {};

    //---getters---
    vector<double>* GetRe() {return &Re_;};
    vector<double>* GetIm() {return &Im_;};
    
    //---setters---
    void SetPointsComplex(int N, double* Re, double* Im);
    void SetPointsComplex(vector<double>& Re, vector<double>& Im);

    //---utils---
    void Reset() {Re_.clear(); Im_.clear();};
    
private:
    vector<double> Re_;
    vector<double> Im_;
};

#endif
