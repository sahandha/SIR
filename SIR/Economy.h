//
//  Economy.h
//  SIR
//
//  Created by Sahand Hariri on 8/9/15.
//  Copyright (c) 2015 Sahand Hariri. All rights reserved.
//

#ifndef __SIR__Economy__
#define __SIR__Economy__

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "Person.h"

using namespace std;

class Economy {
public:
    
    Economy(double a, double alpha, double beta);
   
    
    // Setters
    void setA(double a);
    void setAlpha(double alpha);
    void setBeta(double beta);
    
    
    // Getters
    double getA();
    double getAlpha();
    double getBeta();
    double getGDP();
    double getDemand();
    
    // Utilities
    void computeGDP(vector<Person*> ppl, double oldGDP);
    
    
private:
    
    double A;
    double Alpha;
    double Beta;
    double GDP = 0;
    double Demand = 0;
    
    
};


#endif /* defined(__SIR__Economy__) */
