#ifndef __HODO_UTILS_H__
#define __HODO_UTILS_H__

#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include "interface/H4Tree.h"

//---hodoscope definitions
#define N_HODO_PLANES 4
#define N_HODO_FIBERS 64
#define HODO_X1 0
#define HODO_Y1 1
#define HODO_X2 2
#define HODO_Y2 3

void FillHodo(const H4Tree& h4Tree, std::map<int,std::map<int,bool> >& hodoFiberOn,
              std::vector<int>& hodoFiberOrderA, std::vector<int>& hodoFiberOrderB,
              std::map<int,float>& beamXY_hodo12);

void FillHodoFiberOrder(std::vector<int>& hodoFiberOrderA, std::vector<int>& hodoFiberOrderB);

#endif
