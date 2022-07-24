/*
Copyright (C) 2022 - Mahesh Patil <mbpatil@ee.iitb.ac.in>
This file is part of GSEIM.

GSEIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <vector>

using namespace std;

void user_f_1(
   double* x,
   double* y,
   vector<int> &iprm,
   vector<double> &rprm) {

   double a;

   if (iprm[0] == 0) {
     a = 0.5;
   } else {
     a =2.0;
   }

   y[0] = a*rprm[0]*x[0] + rprm[1];
   return;
}

void user_f_mux_1(
   const double time0,
   double* x,
   double* y,
   vector<int> &iprm,
   vector<double> &rprm) {

   double x1,x2,t0;

   x1 = x[0];
   x2 = x[1];
   t0 = rprm[0];

   if (time0 <= t0) {
     y[0] = x1;
     y[1] = 0.0;
   } else {
     y[0] = 0.0;
     y[1] = x2;
   }

   return;
}

void user_f_vctr_1(
   double* x,
   double* y,
   vector<int> &iprm,
   vector<double> &rprm) {

   double sigma,ls,vdc;
   double k;
   double tr,w2,wrm,wmr,isd,isq,imr,eff_d,eff_q;
   double k1,k2;
   int poles;

   poles = iprm[0];

   sigma = rprm[0];
   ls    = rprm[1];  
   vdc   = rprm[2];  
   tr    = rprm[3];

   wrm = x[0];
   isd = x[1];
   isq = x[2];
   imr = x[3];

   if (fabs(imr) < 1.0e-2) {
     w2 = 1.0;
   } else {
     w2 = isq/(tr*imr);
   }

   wmr = 0.5*((double)(poles))*wrm + w2;

   k = sigma*ls/(0.5*vdc);
   eff_d = k*wmr*isq;

   k1 = sigma*ls/(0.5*vdc);
   k2 = (1.0-sigma)*ls/(0.5*vdc);
   eff_q = k1*wmr*isd + k2*wmr*imr;

   y[0] = eff_d;
   y[1] = eff_q;
   y[2] = wmr;

   return;
}

void user_function(
   const int index_fn,
   const double time0,
   double* x,
   double* y,
   vector<int> &iprm,
   vector<double> &rprm) {

// allow up to 2 iprms and 10 rprms

   double a = 2.0;

   switch(index_fn)
   {
   case 1 :
//    simple example:
//    y[0] = a*rprm[0]*x[0] + rprm[1];

      user_f_1(x,y,iprm,rprm);
      break;
   case 2 :
      user_f_mux_1(time0,x,y,iprm,rprm);
      break;
   case 3 :
      user_f_vctr_1(x,y,iprm,rprm);
      break;
   default :
      cout << "user_function: Check index_fn. Halting..." << endl;
      exit (1);
   }
   return;
}
