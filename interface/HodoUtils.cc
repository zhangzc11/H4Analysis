#include "interface/HodoUtils.h"

void FillHodo(const H4Tree& h4Tree, std::map<int,std::map<int,bool> >& hodoFiberOn,
              std::vector<int>& hodoFiberOrderA, std::vector<int>& hodoFiberOrderB,
              std::map<int,float>& beamXY_hodo12)
{
    for(unsigned int i = 0; i < N_HODO_PLANES; ++i)
        for(unsigned int j = 0; j < N_HODO_FIBERS; ++j)
            hodoFiberOn[i][j] = 0;
  
    std::map<int,std::vector<int> > fibersOn;
  
    for(unsigned int i = 0; i < h4Tree.nPatterns; ++i)
    {
        if(h4Tree.patternBoard[i] == 0x08020001 ||
           h4Tree.patternBoard[i] == 0x08020002)
        {
            int pos = -1; // here is where the real hodoscope mapping is done
      
            if(h4Tree.patternBoard[i] == 0x08020001)
                pos = (h4Tree.patternChannel[i] < 2) ? HODO_Y2 : HODO_X2;
            else if(h4Tree.patternBoard[i] == 0x08020002)
                pos = (h4Tree.patternChannel[i] < 2) ? HODO_Y1 : HODO_X1;
      
            std::vector<int>* fiberorder = (bool)(h4Tree.patternChannel[i]&0b1) ? &hodoFiberOrderB : &hodoFiberOrderA;
      
            for(unsigned int j = 0; j < 32; ++j)
            {
                bool thisfibon = (h4Tree.pattern[i]>>j)&0b1;
                hodoFiberOn[pos][fiberorder->at(j)-1] = thisfibon;
                if(thisfibon) fibersOn[pos].push_back(fiberorder->at(j)-1);
            }
        }
    }
  
    for(int i = 0; i < N_HODO_PLANES; ++i)
    {
        if(fibersOn[i].size() == 0 || fibersOn[i].size() > 3)
            beamXY_hodo12[i] = -999.;
    
        else if(fibersOn[i].size() == 1)
            beamXY_hodo12[i] = 0.5 * (fibersOn[i].at(0) - 32.);
    
        else if(fibersOn[i].size() == 2)
        {
            float val1 = fibersOn[i].at(0);
            float val2 = fibersOn[i].at(1);
            if(fabs(val2-val1) == 1) 
                beamXY_hodo12[i] = 0.5 * (0.5*(val1+val2) - 32.);
            else
                beamXY_hodo12[i] = -999.;
        }
    
        else if(fibersOn[i].size() == 3)
        {
            std::vector<float> vals;
            vals.push_back(fibersOn[i].at(0));
            vals.push_back(fibersOn[i].at(1));
            vals.push_back(fibersOn[i].at(2));
            std::sort(vals.begin(),vals.end());
            if(fabs(vals.at(1)-vals.at(0)) == 1 && fabs(vals.at(2)-vals.at(1))) 
                beamXY_hodo12[i] = 0.5 * (vals.at(1) - 32.);
            else                              
                beamXY_hodo12[i] = -999.;
        }
    }  
    return;
}



void FillHodoFiberOrder(std::vector<int>& hodoFiberOrderA, std::vector<int>& hodoFiberOrderB)
{
    hodoFiberOrderA.clear();
    hodoFiberOrderB.clear();
  
    hodoFiberOrderA.push_back(31);
    hodoFiberOrderA.push_back(29);
    hodoFiberOrderA.push_back(23);
    hodoFiberOrderA.push_back(21);
    hodoFiberOrderA.push_back(5);
    hodoFiberOrderA.push_back(7);
    hodoFiberOrderA.push_back(15);
    hodoFiberOrderA.push_back(13);
    hodoFiberOrderA.push_back(1);
    hodoFiberOrderA.push_back(3);
    hodoFiberOrderA.push_back(11);
    hodoFiberOrderA.push_back(9);
    hodoFiberOrderA.push_back(6);
    hodoFiberOrderA.push_back(8);
    hodoFiberOrderA.push_back(16);
    hodoFiberOrderA.push_back(14);
    hodoFiberOrderA.push_back(17);
    hodoFiberOrderA.push_back(27);
    hodoFiberOrderA.push_back(19);
    hodoFiberOrderA.push_back(25);
    hodoFiberOrderA.push_back(24);
    hodoFiberOrderA.push_back(22);
    hodoFiberOrderA.push_back(32);
    hodoFiberOrderA.push_back(30);
    hodoFiberOrderA.push_back(4);
    hodoFiberOrderA.push_back(2);
    hodoFiberOrderA.push_back(12);
    hodoFiberOrderA.push_back(10);
    hodoFiberOrderA.push_back(20);
    hodoFiberOrderA.push_back(18);
    hodoFiberOrderA.push_back(28);
    hodoFiberOrderA.push_back(26);

    hodoFiberOrderB.push_back(54);
    hodoFiberOrderB.push_back(64);
    hodoFiberOrderB.push_back(56);
    hodoFiberOrderB.push_back(62);
    hodoFiberOrderB.push_back(49);
    hodoFiberOrderB.push_back(51);
    hodoFiberOrderB.push_back(59);
    hodoFiberOrderB.push_back(57);
    hodoFiberOrderB.push_back(53);
    hodoFiberOrderB.push_back(55);
    hodoFiberOrderB.push_back(63);
    hodoFiberOrderB.push_back(61);
    hodoFiberOrderB.push_back(45);
    hodoFiberOrderB.push_back(47);
    hodoFiberOrderB.push_back(37);
    hodoFiberOrderB.push_back(39);
    hodoFiberOrderB.push_back(34);
    hodoFiberOrderB.push_back(42);
    hodoFiberOrderB.push_back(36);
    hodoFiberOrderB.push_back(44);
    hodoFiberOrderB.push_back(50);
    hodoFiberOrderB.push_back(52);
    hodoFiberOrderB.push_back(58);
    hodoFiberOrderB.push_back(60);
    hodoFiberOrderB.push_back(38);
    hodoFiberOrderB.push_back(48);
    hodoFiberOrderB.push_back(40);
    hodoFiberOrderB.push_back(46);
    hodoFiberOrderB.push_back(41);
    hodoFiberOrderB.push_back(43);
    hodoFiberOrderB.push_back(33);
    hodoFiberOrderB.push_back(35);
  
    return;
}
