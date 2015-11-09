#ifndef __WFCLASS_NINO_H__
#define __WFCLASS_NINO_H__

#include "WFClass.h"

using namespace std;

class WFClassNINO : public WFClass
{
public:
    //---ctors---
    WFClassNINO() {};
    WFClassNINO(int polarity, float tUnit);

    //---getters---
    float                 GetSignalIntegral(int thr, int min) override;
};

#endif
