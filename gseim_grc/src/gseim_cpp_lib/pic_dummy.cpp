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
   #include "utils.h"
using namespace std;

class PICDummy{
public:
  void filter_compute_coef(){

// we will assume that the input coefficients (a0,a1,..,b0,b1,..)
// are in temp_coef_1.dat.

   double a0,a1,a2,a3,a4,a5,b0,b1,b2,b3,b4,b5;
   int degree_n,degree_d;

   const int m=5; // 5th order numerator/denominator

   double coef_n[m+1];
   double coef_d[m+1];

   double gain;
   double a0p;
   int pp_n_roots;
   std::complex<double> pp_coef[m];
   std::complex<double> pp_root[m];
   int pp_flag_real[m];
   int pp_sign_imag[m];
   int pp_power[m];
   int ppc_n_roots;
   std::complex<double> ppc_coef[m];
   std::complex<double> ppc_root[m];
   int ppc_flag_real[m];
   int ppc_power[m];
   double eps2=1.0e-5;

   std::fstream inf;
   std::ofstream outf;

   inf.open("temp_coef_1.dat",ios::in|ios::binary);
   if (!inf) {
       cout << "Unable to open temp_coef_1.dat" << endl;
       exit(1);
   }

   a0 = next_double_1(inf);
   a1 = next_double_1(inf);
   a2 = next_double_1(inf);
   a3 = next_double_1(inf);
   a4 = next_double_1(inf);
   a5 = next_double_1(inf);

   b0 = next_double_1(inf);
   b1 = next_double_1(inf);
   b2 = next_double_1(inf);
   b3 = next_double_1(inf);
   b4 = next_double_1(inf);
   b5 = next_double_1(inf);

   inf.close();

// get partial fractions:

   coef_n[0] = a0;
   coef_n[1] = a1;
   coef_n[2] = a2;
   coef_n[3] = a3;
   coef_n[4] = a4;
   coef_n[5] = a5;

   coef_d[0] = b0;
   coef_d[1] = b1;
   coef_d[2] = b2;
   coef_d[3] = b3;
   coef_d[4] = b4;
   coef_d[5] = b5;

   degree_n = -1;
   degree_d = -1;

   for (int i=m; i >= 0; i--) {
     if (coef_n[i] != 0.0) {
       degree_n = i; break;
     }
   }
   for (int i=m; i >= 0; i--) {
     if (coef_d[i] != 0.0) {
       degree_d = i; break;
     }
   }

   if (degree_d <= 0) {
     cout << "filter_compute_coef: check degree_d. Halting..." << endl;
     exit(1);
   }
   if (degree_n == -1) {
     cout << "filter_compute_coef: check degree_n. Halting..." << endl;
     exit(1);
   }
   if (degree_d < degree_n) {
     cout << "filter_compute_coef: degree_d < degree_n. Halting..." << endl;
     exit(1);
   }

   partial_fractions_1(
     degree_n,degree_d,coef_n,coef_d,gain,a0p,
     pp_n_roots, pp_coef,pp_root,
     pp_flag_real,pp_sign_imag, pp_power,
     ppc_n_roots,ppc_coef, ppc_root,
     ppc_flag_real,ppc_power,
     1.0e-5);

   outf.open("temp_coef_2.dat",ios::out);
   outf << scientific; outf << setprecision(5);

   outf << ppc_n_roots << "  " << gain << "  " << a0p << " --- n_roots, gain, a0p" << endl;
   for (int i = 0;i < ppc_n_roots;i++) {
     outf << ppc_flag_real[i] << "  " << ppc_power[i] << " --- flag_real, power" << endl;
     outf << ppc_root[i].real() << "  " << ppc_root[i].imag() << " --- root (real,imag)" << endl;
     outf << ppc_coef[i].real() << "  " << ppc_coef[i].imag() << " --- coef (real,imag)" << endl;
   }   

   outf.close();

  }
};
int main()
{
   return 0;
}
extern "C" {
   PICDummy* PICDummy_new(){
     return new PICDummy();
   }
   void PICDummy_filter_compute_coef(
     PICDummy* pic_dummy){

     pic_dummy->filter_compute_coef();
   }
}

