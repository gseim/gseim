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

#include "global.h"
#include "xbeusr.h"
#include "xbejac.h"
#include "utils.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#ifndef USER_FUNCTION
#define USER_FUNCTION
void user_function(
   const int index_fn,
   const double time0,
   double* x,
   double* y,
   vector<int> &iprm,
   vector<double> &rprm);
#endif
void x_abc_to_dq(Global &G,XbeUsr &X,XbeJac &J) {
   static double k1=0,k2=0,k3=0;
   double van,vbn,vcn;
   double vqs,vds;
   const int nvr_van = 0;
   const int nvr_vbn = 1;
   const int nvr_vcn = 2;
   const int nvr_vqs = 3;
   const int nvr_vds = 4;
   const int no_van = 0;
   const int no_vbn = 1;
   const int no_vcn = 2;
   const int no_vqs = 3;
   const int no_vds = 4;
   const int ng_1 = 0;
   const int ng_2 = 1;
// cout << "abc_to_dq.xbe" << endl;

   if (G.flags[G.i_one_time_parms]) {
     k1 = 2.0/3.0;
     k2 = 1.0/3.0;
     k3 = 1.0/(sqrt(3.0));
     return;
   }
   if (G.flags[G.i_init_guess]) {
     van = X.val_vr[nvr_van];
     vbn = X.val_vr[nvr_vbn];
     vcn = X.val_vr[nvr_vcn];

     X.val_vr[nvr_vqs] = k1*van - k2*(vbn + vcn);
     X.val_vr[nvr_vds] = - k3*(vbn - vcn);
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     van = X.val_vr[nvr_van];
     vbn = X.val_vr[nvr_vbn];
     vcn = X.val_vr[nvr_vcn];
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_vqs] = k1*van - k2*(vbn + vcn);
       X.val_vr[nvr_vds] = - k3*(vbn - vcn);
     } else if (G.flags[G.i_implicit]) {
       vqs = X.val_vr[nvr_vqs];
       vds = X.val_vr[nvr_vds];
       if (G.flags[G.i_function]) {
         X.g[ng_1] = vqs -(k1*van - k2*(vbn + vcn));
         X.g[ng_2] = vds -(- k3*(vbn - vcn));
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_vqs] = 1.0;
         J.dgdvr[ng_1][nvr_van] = -k1;
         J.dgdvr[ng_1][nvr_vbn] =  k2;
         J.dgdvr[ng_1][nvr_vcn] =  k2;

         J.dgdvr[ng_2][nvr_vds] = 1.0;
         J.dgdvr[ng_2][nvr_vbn] =  k3;
         J.dgdvr[ng_2][nvr_vcn] = -k3;
       }
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_van] = X.val_vr[nvr_van];
     X.outprm[no_vbn] = X.val_vr[nvr_vbn];
     X.outprm[no_vcn] = X.val_vr[nvr_vcn];
     X.outprm[no_vqs] = X.val_vr[nvr_vqs];
     X.outprm[no_vds] = X.val_vr[nvr_vds];
     return;
   }
   return;
}
void x_abc_to_dq_2(Global &G,XbeUsr &X,XbeJac &J) {
   static double k4=0;
   double a,b,c,cost,sint;
   double d,q;
   double alpha,beta;
   const int nvr_a = 0;
   const int nvr_b = 1;
   const int nvr_c = 2;
   const int nvr_cost = 3;
   const int nvr_sint = 4;
   const int nvr_d = 5;
   const int nvr_q = 6;
   const int no_a = 0;
   const int no_b = 1;
   const int no_c = 2;
   const int no_d = 3;
   const int no_q = 4;
   const int na_alpha = 0;
   const int na_beta = 1;
   const int ng_1 = 0;
   const int ng_2 = 1;
   const int ng_3 = 2;
   const int ng_4 = 3;
   if (G.flags[G.i_one_time_parms]) {
     k4 = 0.5*(sqrt(3.0));
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_d] = X.val_vr[nvr_d];
     X.outprm[no_q] = X.val_vr[nvr_q];
     X.outprm[no_a] = X.val_vr[nvr_a];
     X.outprm[no_b] = X.val_vr[nvr_b];
     X.outprm[no_c] = X.val_vr[nvr_c];
     return;
   }
   a = X.val_vr[nvr_a];
   b = X.val_vr[nvr_b];
   c = X.val_vr[nvr_c];
   cost = X.val_vr[nvr_cost];
   sint = X.val_vr[nvr_sint];

   if (G.flags[G.i_init_guess]) {
     alpha = 1.5*a;
     beta  = k4*(b-c);
 
     X.val_aux[na_alpha] = alpha;
     X.val_aux[na_beta ] = beta;

     X.val_vr[nvr_q] = -sint*alpha + cost*beta;
     X.val_vr[nvr_d] =  cost*alpha + sint*beta;

     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       alpha = 1.5*a;
       beta  = k4*(b-c);
       X.val_vr[nvr_q] = -sint*alpha + cost*beta;
       X.val_vr[nvr_d] =  cost*alpha + sint*beta;
     } else if (G.flags[G.i_implicit]) {
       alpha = X.val_aux[na_alpha];
       beta  = X.val_aux[na_beta ];

       if (G.flags[G.i_function]) {
         X.g[ng_1] = alpha - 1.5*a;
         X.g[ng_2] = beta - k4*(b-c);

         X.g[ng_3] = q + sint*alpha - cost*beta;
         X.g[ng_4] = d - cost*alpha - sint*beta;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdaux[ng_1][na_alpha] =  1.0;
         J.dgdvr [ng_1][nvr_a   ] = -1.5;

         J.dgdaux[ng_2][na_beta] =  1.0;
         J.dgdvr [ng_2][nvr_b  ] = -k4;
         J.dgdvr [ng_2][nvr_c  ] =  k4; 

         J.dgdvr [ng_3][nvr_q   ] =  1.0;
         J.dgdvr [ng_3][nvr_sint] =  alpha;
         J.dgdvr [ng_3][nvr_cost] = -beta;
         J.dgdaux[ng_3][na_alpha] =  sint;
         J.dgdaux[ng_3][na_beta ] = -cost;

         J.dgdvr [ng_4][nvr_d   ] =  1.0;
         J.dgdvr [ng_4][nvr_sint] = -beta;
         J.dgdvr [ng_4][nvr_cost] = -alpha;
         J.dgdaux[ng_4][na_alpha] = -cost;
         J.dgdaux[ng_4][na_beta ] = -sint;
       }
     }
     return;
   }
   return;
}
void x_and_2(Global &G,XbeUsr &X,XbeJac &J) {
   double y0;
   bool X1,X2,Y;
   double x1,x2;
   double y;
   double y_high,hb2;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y = 2;
   const int nr_y_high = 0;
   const int nr_hb2 = 1;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y = 2;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     y_high = X.rprm[nr_y_high];
     hb2 = 0.5*y_high;
     X.rprm[nr_hb2] = hb2;
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x1] = X.val_vr[nvr_x1];
     X.outprm[no_x2] = X.val_vr[nvr_x2];
     X.outprm[no_y ] = X.val_vr[nvr_y ];
     return;
   }
   y_high = X.rprm[nr_y_high];
   hb2 = X.rprm[nr_hb2];

   x1 = X.val_vr[nvr_x1];
   x2 = X.val_vr[nvr_x2];
   X1 = x1 > hb2;
   X2 = x2 > hb2;
   Y = X1 && X2;

   if (Y) {
     y0 = y_high;
   } else {
     y0 = 0.0;
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y ] =  1.0;
       }
     }
     return;
   }
   return;
}
void x_b_1(Global &G,XbeUsr &X,XbeJac &J) {
   double x1,x2;
   double y;
   double b_1_1,b_1_2;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y = 2;
   const int nr_b_1_1 = 0;
   const int nr_b_1_2 = 1;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y = 2;
   const int ng_1 = 0;
   return;
}
void x_b_2(Global &G,XbeUsr &X,XbeJac &J) {
   double x;
   double y1,y2;
   double b_2_1,b_2_2;
   const int nvr_x = 0;
   const int nvr_y1 = 1;
   const int nvr_y2 = 2;
   const int nr_b_2_1 = 0;
   const int nr_b_2_2 = 1;
   const int no_x = 0;
   const int no_y1 = 1;
   const int no_y2 = 2;
   const int ng_1 = 0;
   return;
}
void x_b_3(Global &G,XbeUsr &X,XbeJac &J) {
   double x;
   double y;
   double b_3_1;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int nr_b_3_1 = 0;
   const int no_x = 0;
   const int no_y = 1;
   const int ng_1 = 0;
   return;
}
void x_clock(Global &G,XbeUsr &X,XbeJac &J) {
   double y0,t0_new,delta_min,del1,del2,t_a,t_b,tnext_p;
   int n;
   double y;
   double T1,T2,L1,L2,t0,delta1,delta2,T,L0,tk1,tk2,tk3,tk4,tk5,slope1,slope2,
     epsl;
   const int nvr_y = 0;
   const int nr_T1 = 0;
   const int nr_T2 = 1;
   const int nr_L1 = 2;
   const int nr_L2 = 3;
   const int nr_t0 = 4;
   const int nr_delta1 = 5;
   const int nr_delta2 = 6;
   const int nr_T = 7;
   const int nr_L0 = 8;
   const int nr_tk1 = 9;
   const int nr_tk2 = 10;
   const int nr_tk3 = 11;
   const int nr_tk4 = 12;
   const int nr_tk5 = 13;
   const int nr_slope1 = 14;
   const int nr_slope2 = 15;
   const int nr_epsl = 16;
   const int no_y = 0;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     T1 = X.rprm[nr_T1];
     T2 = X.rprm[nr_T2];
     L1 = X.rprm[nr_L1];
     L2 = X.rprm[nr_L2];
     delta1 = X.rprm[nr_delta1];
     delta2 = X.rprm[nr_delta2];

//   make sure that there is a flat part in each half:
     delta_min = 0.1*min(delta1,delta2);
     del1 = T1-0.5*(delta1+delta2);
     del2 = T2-0.5*(delta1+delta2);

     if (del1 < delta_min) {
       cout << "clock.xbe: T1 is too small. Halting..." << endl; exit(1);
     }
     if (del2 < delta_min) {
       cout << "clock.xbe: T2 is too small. Halting..." << endl; exit(1);
     }
     T = T1 + T2;
     tk1 = 0.5*delta1;
     tk2 = T1 - 0.5*delta2;
     tk3 = T1 + 0.5*delta2;
     tk4 = T  - 0.5*delta1;
     tk5 = T  + 0.5*delta1;

     slope1 = (L1-L2)/delta1;
     slope2 = (L2-L1)/delta2;
     epsl = min(delta1,delta2)/10.0;
     L0 = 0.5*(L1+L2);

     X.rprm[nr_T] = T;
     X.rprm[nr_tk1] = tk1;
     X.rprm[nr_tk2] = tk2;
     X.rprm[nr_tk3] = tk3;
     X.rprm[nr_tk4] = tk4;
     X.rprm[nr_tk5] = tk5;
     X.rprm[nr_slope1] = slope1;
     X.rprm[nr_slope2] = slope2;
     X.rprm[nr_epsl] = epsl;
     X.rprm[nr_L0] = L0;

     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }

   t0   = X.rprm[nr_t0  ];
   T    = X.rprm[nr_T   ];
   epsl = X.rprm[nr_epsl];
   tk1  = X.rprm[nr_tk1 ];
   tk2  = X.rprm[nr_tk2 ];
   tk3  = X.rprm[nr_tk3 ];
   tk4  = X.rprm[nr_tk4 ];
   tk5  = X.rprm[nr_tk5 ];

   if (G.time_given_x < t0) {
     n = ((t0-G.time_given_x)/T) + 1;
     t0_new = t0-n*T;
   } else {
     t0_new = t0;
   }
   t_a = G.time_given_x-t0_new;
   t_b = fmod(t_a,T);

   if (abs(t_b-T) < epsl) t_b = 0.0;

   if (G.flags[G.i_next_time]) {
//   cout << " t0 = " << t0 << endl;
//   cout << " t0_new = " << t0_new << endl;
//   cout << " t_a = " << t_a << endl;
//   cout << " t_b = " << t_b << endl;
//   cout << " T = " << T << endl;

     if (t_b < tk1) {
//     cout << "clock: debug 1" << endl;
       tnext_p = tk1;
     } else if (t_b < tk2) {
//     cout << "clock: debug 2" << endl;
       tnext_p = tk2;
     } else if (t_b < tk3) {
//     cout << "clock: debug 3" << endl;
       tnext_p = tk3;
     } else if (t_b < tk4) {
//     cout << "clock: debug 4" << endl;
       tnext_p = tk4;
     } else {
//     cout << "clock: debug 5" << endl;
       tnext_p = tk5;
     }
     G.time_nextbreak_x = G.time_given_x + (tnext_p-t_b);
//   cout << " G.time_nextbreak_x = " << G.time_nextbreak_x << endl;
     return;
   }

// compute y0, the y value at time_given_x:
// This should work even if L1 and L2 are both positive or both negative.

   L0 = X.rprm[nr_L0];
   L1 = X.rprm[nr_L1];
   L2 = X.rprm[nr_L2];
   slope1 = X.rprm[nr_slope1];
   slope2 = X.rprm[nr_slope2];

// cout << " L1 = " << L1 << endl;
// cout << " L2 = " << L2 << endl;
// cout << " slope1 = " << slope1 << endl;
// cout << " slope2 = " << slope2 << endl;

   if (t_b < tk1) {
     y0 = L0 + slope1*t_b;
//   cout << "clock: debug 1A: t_a = " << t_a << ", y0 =" << y0 << endl;
   } else if (t_b < tk2) {
     y0 = L1;
//   cout << "clock: debug 2A: t_a = " << t_a << ", y0 =" << y0 << endl;
   } else if (t_b < tk3) {
     y0 = L1 + slope2*(t_b-tk2);
//   cout << "clock: debug 3A: t_a = " << t_a << ", y0 =" << y0 << endl;
   } else if (t_b < tk4) {
     y0 = L2;
//   cout << "clock: debug 4A: t_a = " << t_a << ", y0 =" << y0 << endl;
   } else {
     y0 = L2 + slope1*(t_b-tk4);
//   cout << "clock: debug 5A: t_a = " << t_a << ", y0 =" << y0 << endl;
   }

   if (G.flags[G.i_init_guess]) {
     cout << "clock: init_guess: y0 =" << y0 << endl;
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_cmpr_1_1(Global &G,XbeUsr &X,XbeJac &J) {
   int iter;
   double y0,time0;
   double x;
   double y;
   int flag_invert,flag_quad;
   double x0,y_low,y_high,x_1,x_2,t_1,t_2,epsl,delt_min,delt_nrml;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int ni_flag_invert = 0;
   const int ni_flag_quad = 1;
   const int nr_x0 = 0;
   const int nr_y_low = 1;
   const int nr_y_high = 2;
   const int nr_x_1 = 3;
   const int nr_x_2 = 4;
   const int nr_t_1 = 5;
   const int nr_t_2 = 6;
   const int nr_epsl = 7;
   const int nr_delt_min = 8;
   const int nr_delt_nrml = 9;
   const int no_x = 0;
   const int no_y = 1;
   const int ng_1 = 0;
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }

   x0 = X.rprm[nr_x0];

   flag_invert = X.iprm[ni_flag_invert];
   flag_quad = X.iprm[ni_flag_quad];

   x = X.val_vr[nvr_x];

   if (G.flags[G.i_save_history]) {

     x_1 = X.rprm[nr_x_1];
     t_1 = X.rprm[nr_t_1];

     x_2 = x_1;
     t_2 = t_1;

     x_1 = x;
     t_1 = G.time_given_x;

     X.rprm[nr_x_1] = x_1;
     X.rprm[nr_x_2] = x_2;
     X.rprm[nr_t_1] = t_1;
     X.rprm[nr_t_2] = t_2;
     return;
   }
   if (G.flags[G.i_next_time]) {
     iter = G.iter_trns_x;
     time0 = G.time_given_x;

     x_1 = X.rprm[nr_x_1];
     x_2 = X.rprm[nr_x_2];
     t_1 = X.rprm[nr_t_1];
     t_2 = X.rprm[nr_t_2];
     epsl      = X.rprm[nr_epsl     ];
     delt_min  = X.rprm[nr_delt_min ];
     delt_nrml = X.rprm[nr_delt_nrml];

     G.time_nextbreak_x = get_tnext(iter,flag_quad,
       t_1,t_2,(x_1-x0),(x_2-x0),
       time0,(x-x0),epsl,delt_min,delt_nrml);

     return;
   }

   if (flag_invert == 0) {
     if (x >= x0) {
       y0 = X.rprm[nr_y_high];
     } else {
       y0 = X.rprm[nr_y_low];
     }   
   } else {
     if (x >= x0) {
       y0 = X.rprm[nr_y_low];
     } else {
       y0 = X.rprm[nr_y_high];
     }   
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_cmpr_1_2(Global &G,XbeUsr &X,XbeJac &J) {
   int iter;
   double y10,y20,time0;
   double x;
   double y1,y2;
   int flag_invert,flag_quad;
   double x0,y_low,y_high,x_1,x_2,t_1,t_2,epsl,delt_min,delt_nrml;
   const int nvr_x = 0;
   const int nvr_y1 = 1;
   const int nvr_y2 = 2;
   const int ni_flag_invert = 0;
   const int ni_flag_quad = 1;
   const int nr_x0 = 0;
   const int nr_y_low = 1;
   const int nr_y_high = 2;
   const int nr_x_1 = 3;
   const int nr_x_2 = 4;
   const int nr_t_1 = 5;
   const int nr_t_2 = 6;
   const int nr_epsl = 7;
   const int nr_delt_min = 8;
   const int nr_delt_nrml = 9;
   const int no_x = 0;
   const int no_y1 = 1;
   const int no_y2 = 2;
   const int ng_1 = 0;
   const int ng_2 = 1;
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y1] = X.val_vr[nvr_y1];
     X.outprm[no_y2] = X.val_vr[nvr_y2];
     return;
   }

   x0 = X.rprm[nr_x0];

   flag_invert = X.iprm[ni_flag_invert];
   flag_quad = X.iprm[ni_flag_quad];

   x = X.val_vr[nvr_x];

   if (G.flags[G.i_save_history]) {

     x_1 = X.rprm[nr_x_1];
     t_1 = X.rprm[nr_t_1];

     x_2 = x_1;
     t_2 = t_1;

     x_1 = x;
     t_1 = G.time_given_x;

     X.rprm[nr_x_1] = x_1;
     X.rprm[nr_x_2] = x_2;
     X.rprm[nr_t_1] = t_1;
     X.rprm[nr_t_2] = t_2;
     return;
   }
   if (G.flags[G.i_next_time]) {
     iter = G.iter_trns_x;
     time0 = G.time_given_x;

     x_1 = X.rprm[nr_x_1];
     x_2 = X.rprm[nr_x_2];
     t_1 = X.rprm[nr_t_1];
     t_2 = X.rprm[nr_t_2];
     epsl      = X.rprm[nr_epsl     ];
     delt_min  = X.rprm[nr_delt_min ];
     delt_nrml = X.rprm[nr_delt_nrml];

     G.time_nextbreak_x = get_tnext(iter,flag_quad,
       t_1,t_2,(x_1-x0),(x_2-x0),
       time0,(x-x0),epsl,delt_min,delt_nrml);

     return;
   }

   if (flag_invert == 0) {
     if (x >= x0) {
       y10 = X.rprm[nr_y_high];
       y20 = X.rprm[nr_y_low];
     } else {
       y10 = X.rprm[nr_y_low];
       y20 = X.rprm[nr_y_high];
     }   
   } else {
     if (x >= x0) {
       y10 = X.rprm[nr_y_low];
       y20 = X.rprm[nr_y_high];
     } else {
       y10 = X.rprm[nr_y_high];
       y20 = X.rprm[nr_y_low];
     }   
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y1] = y10;
     X.val_vr[nvr_y2] = y20;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y1] = y10;
       X.val_vr[nvr_y2] = y20;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y1] - y10;
         X.g[ng_2] = X.val_vr[nvr_y2] - y20;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y1] = 1.0;
         J.dgdvr[ng_2][nvr_y2] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_cmpr_2_1(Global &G,XbeUsr &X,XbeJac &J) {
   int iter;
   double y0,time0;
   double x1,x2;
   double y;
   int flag_invert,flag_quad;
   double y_low,y_high,x1_1,x1_2,x2_1,x2_2,t_1,t_2,epsl,delt_min,delt_nrml;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y = 2;
   const int ni_flag_invert = 0;
   const int ni_flag_quad = 1;
   const int nr_y_low = 0;
   const int nr_y_high = 1;
   const int nr_x1_1 = 2;
   const int nr_x1_2 = 3;
   const int nr_x2_1 = 4;
   const int nr_x2_2 = 5;
   const int nr_t_1 = 6;
   const int nr_t_2 = 7;
   const int nr_epsl = 8;
   const int nr_delt_min = 9;
   const int nr_delt_nrml = 10;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y = 2;
   const int ng_1 = 0;
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x1] = X.val_vr[nvr_x1];
     X.outprm[no_x2] = X.val_vr[nvr_x2];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }

   flag_invert = X.iprm[ni_flag_invert];
   flag_quad = X.iprm[ni_flag_quad];

   x1 = X.val_vr[nvr_x1];
   x2 = X.val_vr[nvr_x2];

   if (G.flags[G.i_save_history]) {

     x1_1 = X.rprm[nr_x1_1];
     x2_1 = X.rprm[nr_x2_1];
     t_1 = X.rprm[nr_t_1];

     x1_2 = x1_1;
     x2_2 = x2_1;
     t_2 = t_1;

     x1_1 = x1;
     x2_1 = x2;
     t_1 = G.time_given_x;

     X.rprm[nr_x1_1] = x1_1;
     X.rprm[nr_x1_2] = x1_2;
     X.rprm[nr_x2_1] = x2_1;
     X.rprm[nr_x2_2] = x2_2;
     X.rprm[nr_t_1] = t_1;
     X.rprm[nr_t_2] = t_2;
     return;
   }
   if (G.flags[G.i_next_time]) {
     iter = G.iter_trns_x;
     time0 = G.time_given_x;

     x1_1 = X.rprm[nr_x1_1];
     x1_2 = X.rprm[nr_x1_2];
     x2_1 = X.rprm[nr_x2_1];
     x2_2 = X.rprm[nr_x2_2];
     t_1  = X.rprm[nr_t_1 ];
     t_2  = X.rprm[nr_t_2 ];
     epsl      = X.rprm[nr_epsl     ];
     delt_min  = X.rprm[nr_delt_min ];
     delt_nrml = X.rprm[nr_delt_nrml];

     G.time_nextbreak_x = get_tnext(iter,flag_quad,
       t_1,t_2,(x1_1-x2_1),(x1_2-x2_2),
       time0,(x1-x2),epsl,delt_min,delt_nrml);

     return;
   }

   if (flag_invert == 0) {
     if (x1 >= x2) {
       y0 = X.rprm[nr_y_high];
     } else {
       y0 = X.rprm[nr_y_low];
     }   
   } else {
     if (x1 >= x2) {
       y0 = X.rprm[nr_y_low];
     } else {
       y0 = X.rprm[nr_y_high];
     }   
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_cmpr_2_2(Global &G,XbeUsr &X,XbeJac &J) {
   int iter;
   double y10,y20,time0;
   double x1,x2;
   double y1,y2;
   int flag_invert,flag_quad;
   double y_low,y_high,x1_1,x1_2,x2_1,x2_2,t_1,t_2,epsl,delt_min,delt_nrml;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y1 = 2;
   const int nvr_y2 = 3;
   const int ni_flag_invert = 0;
   const int ni_flag_quad = 1;
   const int nr_y_low = 0;
   const int nr_y_high = 1;
   const int nr_x1_1 = 2;
   const int nr_x1_2 = 3;
   const int nr_x2_1 = 4;
   const int nr_x2_2 = 5;
   const int nr_t_1 = 6;
   const int nr_t_2 = 7;
   const int nr_epsl = 8;
   const int nr_delt_min = 9;
   const int nr_delt_nrml = 10;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y1 = 2;
   const int no_y2 = 3;
   const int ng_1 = 0;
   const int ng_2 = 1;
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x1] = X.val_vr[nvr_x1];
     X.outprm[no_x2] = X.val_vr[nvr_x2];
     X.outprm[no_y1] = X.val_vr[nvr_y1];
     X.outprm[no_y2] = X.val_vr[nvr_y2];
     return;
   }

   flag_invert = X.iprm[ni_flag_invert];
   flag_quad = X.iprm[ni_flag_quad];

   x1 = X.val_vr[nvr_x1];
   x2 = X.val_vr[nvr_x2];

   if (G.flags[G.i_save_history]) {

     x1_1 = X.rprm[nr_x1_1];
     x2_1 = X.rprm[nr_x2_1];
     t_1 = X.rprm[nr_t_1];

     x1_2 = x1_1;
     x2_2 = x2_1;
     t_2 = t_1;

     x1_1 = x1;
     x2_1 = x2;
     t_1 = G.time_given_x;

     X.rprm[nr_x1_1] = x1_1;
     X.rprm[nr_x1_2] = x1_2;
     X.rprm[nr_x2_1] = x2_1;
     X.rprm[nr_x2_2] = x2_2;
     X.rprm[nr_t_1] = t_1;
     X.rprm[nr_t_2] = t_2;
     return;
   }
   if (G.flags[G.i_next_time]) {
     iter = G.iter_trns_x;
     time0 = G.time_given_x;

     x1_1 = X.rprm[nr_x1_1];
     x1_2 = X.rprm[nr_x1_2];
     x2_1 = X.rprm[nr_x2_1];
     x2_2 = X.rprm[nr_x2_2];
     t_1  = X.rprm[nr_t_1 ];
     t_2  = X.rprm[nr_t_2 ];
     epsl      = X.rprm[nr_epsl     ];
     delt_min  = X.rprm[nr_delt_min ];
     delt_nrml = X.rprm[nr_delt_nrml];

     G.time_nextbreak_x = get_tnext(iter,flag_quad,
       t_1,t_2,(x1_1-x2_1),(x1_2-x2_2),
       time0,(x1-x2),epsl,delt_min,delt_nrml);

     return;
   }

   if (flag_invert == 0) {
     if (x1 >= x2) {
       y10 = X.rprm[nr_y_high];
       y20 = X.rprm[nr_y_low];
     } else {
       y10 = X.rprm[nr_y_low];
       y20 = X.rprm[nr_y_high];
     }   
   } else {
     if (x1 >= x2) {
       y10 = X.rprm[nr_y_low];
       y20 = X.rprm[nr_y_high];
     } else {
       y10 = X.rprm[nr_y_high];
       y20 = X.rprm[nr_y_low];
     }   
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y1] = y10;
     X.val_vr[nvr_y2] = y20;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y1] = y10;
       X.val_vr[nvr_y2] = y20;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y1] - y10;
         X.g[ng_2] = X.val_vr[nvr_y2] - y20;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y1] = 1.0;
         J.dgdvr[ng_2][nvr_y2] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_cmpr_simple_2_1(Global &G,XbeUsr &X,XbeJac &J) {
   double y0;
   double x1,x2;
   double y;
   int flag_invert;
   double y_low,y_high;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y = 2;
   const int ni_flag_invert = 0;
   const int nr_y_low = 0;
   const int nr_y_high = 1;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y = 2;
   const int ng_1 = 0;
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x1] = X.val_vr[nvr_x1];
     X.outprm[no_x2] = X.val_vr[nvr_x2];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }

   flag_invert = X.iprm[ni_flag_invert];

   x1 = X.val_vr[nvr_x1];
   x2 = X.val_vr[nvr_x2];

   if (flag_invert == 0) {
     if (x1 >= x2) {
       y0 = X.rprm[nr_y_high];
     } else {
       y0 = X.rprm[nr_y_low];
     }   
   } else {
     if (x1 >= x2) {
       y0 = X.rprm[nr_y_low];
     } else {
       y0 = X.rprm[nr_y_high];
     }   
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = y0;
     return;
   }

   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_cmpr_simple_2_2(Global &G,XbeUsr &X,XbeJac &J) {
   double y10,y20;
   double x1,x2;
   double y1,y2;
   int flag_invert;
   double y_low,y_high;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y1 = 2;
   const int nvr_y2 = 3;
   const int ni_flag_invert = 0;
   const int nr_y_low = 0;
   const int nr_y_high = 1;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y1 = 2;
   const int no_y2 = 3;
   const int ng_1 = 0;
   const int ng_2 = 1;
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x1] = X.val_vr[nvr_x1];
     X.outprm[no_x2] = X.val_vr[nvr_x2];
     X.outprm[no_y1] = X.val_vr[nvr_y1];
     X.outprm[no_y2] = X.val_vr[nvr_y2];
     return;
   }

   flag_invert = X.iprm[ni_flag_invert];

   x1 = X.val_vr[nvr_x1];
   x2 = X.val_vr[nvr_x2];

   if (flag_invert == 0) {
     if (x1 >= x2) {
       y10 = X.rprm[nr_y_high];
       y20 = X.rprm[nr_y_low];
     } else {
       y10 = X.rprm[nr_y_low];
       y20 = X.rprm[nr_y_high];
     }   
   } else {
     if (x1 >= x2) {
       y10 = X.rprm[nr_y_low];
       y20 = X.rprm[nr_y_high];
     } else {
       y10 = X.rprm[nr_y_high];
       y20 = X.rprm[nr_y_low];
     }   
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y1] = y10;
     X.val_vr[nvr_y2] = y20;
     return;
   }

   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y1] = y10;
       X.val_vr[nvr_y2] = y20;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y1] - y10;
         X.g[ng_2] = X.val_vr[nvr_y2] - y20;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y1] = 1.0;
         J.dgdvr[ng_2][nvr_y2] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_cmprh_1_1(Global &G,XbeUsr &X,XbeJac &J) {
   int iter;
   double y0,delx,time0,offset1;
   double x;
   double y;
   int flag_invert,flag_quad;
   double x0,y_low,y_high,h,x_1,x_2,t_1,t_2,epsl,delt_min,delt_nrml,hby2,y_old,
     y_half;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int ni_flag_invert = 0;
   const int ni_flag_quad = 1;
   const int nr_x0 = 0;
   const int nr_y_low = 1;
   const int nr_y_high = 2;
   const int nr_h = 3;
   const int nr_x_1 = 4;
   const int nr_x_2 = 5;
   const int nr_t_1 = 6;
   const int nr_t_2 = 7;
   const int nr_epsl = 8;
   const int nr_delt_min = 9;
   const int nr_delt_nrml = 10;
   const int nr_hby2 = 11;
   const int nr_y_old = 12;
   const int nr_y_half = 13;
   const int no_x = 0;
   const int no_y = 1;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     h = X.rprm[nr_h];
     hby2 = 0.5*h;
     X.rprm[nr_hby2] = hby2;

     y_low  = X.rprm[nr_y_low ];
     y_high = X.rprm[nr_y_high];

     if (y_low >= y_high) {
       cout << "cmprh_1_1.xbe: y_low <= y_high?" << endl;
       cout << "  y_low=" << y_low << endl;
       cout << "  y_high=" << y_high << endl;
       cout << "  Halting..." << endl;
       exit (1);
     }
     y_half = 0.5*(y_high+y_low);
     X.rprm[nr_y_half] = y_half;
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }

   flag_invert = X.iprm[ni_flag_invert];
   flag_quad = X.iprm[ni_flag_quad];

   x = X.val_vr[nvr_x];

   if (G.flags[G.i_save_history]) {

     x_1 = X.rprm[nr_x_1];
     t_1 = X.rprm[nr_t_1];

     x_2 = x_1;
     t_2 = t_1;

     x_1 = x;
     t_1 = G.time_given_x;

     X.rprm[nr_x_1] = x_1;
     X.rprm[nr_x_2] = x_2;
     X.rprm[nr_t_1] = t_1;
     X.rprm[nr_t_2] = t_2;

     X.rprm[nr_y_old] = X.val_vr[nvr_y];
     return;
   }
   if (G.flags[G.i_next_time]) {
     iter = G.iter_trns_x;
     time0 = G.time_given_x;

     x0       = X.rprm[nr_x0    ];
     hby2     = X.rprm[nr_hby2  ];
     y_old    = X.rprm[nr_y_old ];
     y_half   = X.rprm[nr_y_half];

     if (flag_invert == 0) {
       if (y_old > y_half) {
         offset1 = x0 - hby2;
       } else {
         offset1 = x0 + hby2;
       }
     } else {
       if (y_old < y_half) {
         offset1 = x0 - hby2;
       } else {
         offset1 = x0 + hby2;
       }
     }
     x_1 = X.rprm[nr_x_1];
     x_2 = X.rprm[nr_x_2];
     t_1 = X.rprm[nr_t_1];
     t_2 = X.rprm[nr_t_2];

     epsl      = X.rprm[nr_epsl     ];
     delt_min  = X.rprm[nr_delt_min ];
     delt_nrml = X.rprm[nr_delt_nrml];

     G.time_nextbreak_x = get_tnext(iter,flag_quad,
       t_1,t_2,(x_1-offset1),(x_2-offset1),
       time0,(x-offset1),epsl,delt_min,delt_nrml);

     return;
   }
   hby2   = X.rprm[nr_hby2  ];
   y_low  = X.rprm[nr_y_low ];
   y_high = X.rprm[nr_y_high];
   y_half = X.rprm[nr_y_half];
   y_old  = X.rprm[nr_y_old ];
   x0     = X.rprm[nr_x0    ];

   if (G.flags[G.i_init_guess]) {
     delx = x-x0;

     if (flag_invert == 0) {
       if (delx > hby2) {
         y0 = y_high;
       } else if (delx < (-hby2)) {
         y0 = y_low;
       } else {
//       set arbitrarily to low
         y0 = y_low;
       }
     } else {
       if (delx > hby2) {
         y0 = y_low;
       } else if (delx < (-hby2)) {
         y0 = y_high;
       } else {
//       set arbitrarily to low
         y0 = y_low;
       }
     }
     X.val_vr[nvr_y] = y0;
     return;
   }

   delx = x-x0;

   if (flag_invert == 0) {
     if (y_old > y_half) {
       if (delx < (-hby2)) {
         y0 = y_low;
       } else {
         y0 = y_high;
       }
     } else {
       if (delx > hby2) {
         y0 = y_high;
       } else {
         y0 = y_low;
       }
     }
   } else {
     if (y_old < y_half) {
       if (delx < (-hby2)) {
         y0 = y_high;
       } else {
         y0 = y_low;
       }
     } else {
       if (delx > hby2) {
         y0 = y_low;
       } else {
         y0 = y_high;
       }
     }
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_cmprh_2_1(Global &G,XbeUsr &X,XbeJac &J) {
   int iter;
   double y0,delx,time0,offset1;
   double x1,x2;
   double y;
   int flag_invert,flag_quad;
   double x0,y_low,y_high,h,x1_1,x1_2,x2_1,x2_2,t_1,t_2,epsl,delt_min,delt_nrml,
     hby2,y_old,y_half;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y = 2;
   const int ni_flag_invert = 0;
   const int ni_flag_quad = 1;
   const int nr_x0 = 0;
   const int nr_y_low = 1;
   const int nr_y_high = 2;
   const int nr_h = 3;
   const int nr_x1_1 = 4;
   const int nr_x1_2 = 5;
   const int nr_x2_1 = 6;
   const int nr_x2_2 = 7;
   const int nr_t_1 = 8;
   const int nr_t_2 = 9;
   const int nr_epsl = 10;
   const int nr_delt_min = 11;
   const int nr_delt_nrml = 12;
   const int nr_hby2 = 13;
   const int nr_y_old = 14;
   const int nr_y_half = 15;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y = 2;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     h = X.rprm[nr_h];
     hby2 = 0.5*h;
     X.rprm[nr_hby2] = hby2;

     y_low  = X.rprm[nr_y_low ];
     y_high = X.rprm[nr_y_high];

     if (y_low >= y_high) {
       cout << "cmprh_2_1.xbe: y_low <= y_high?" << endl;
       cout << "  y_low=" << y_low << endl;
       cout << "  y_high=" << y_high << endl;
       cout << "  Halting..." << endl;
       exit (1);
     }
     y_half = 0.5*(y_high+y_low);
     X.rprm[nr_y_half] = y_half;
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x1] = X.val_vr[nvr_x1];
     X.outprm[no_x2] = X.val_vr[nvr_x2];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }

   flag_invert = X.iprm[ni_flag_invert];
   flag_quad = X.iprm[ni_flag_quad];

   x1 = X.val_vr[nvr_x1];
   x2 = X.val_vr[nvr_x2];

   if (G.flags[G.i_save_history]) {

     x1_1 = X.rprm[nr_x1_1];
     x2_1 = X.rprm[nr_x2_1];
     t_1 = X.rprm[nr_t_1];

     x1_2 = x1_1;
     x2_2 = x2_1;
     t_2 = t_1;

     x1_1 = x1;
     x2_1 = x2;
     t_1 = G.time_given_x;

     X.rprm[nr_x1_1] = x1_1;
     X.rprm[nr_x1_2] = x1_2;
     X.rprm[nr_x2_1] = x2_1;
     X.rprm[nr_x2_2] = x2_2;
     X.rprm[nr_t_1] = t_1;
     X.rprm[nr_t_2] = t_2;

     X.rprm[nr_y_old] = X.val_vr[nvr_y];
     return;
   }
   if (G.flags[G.i_next_time]) {
     iter = G.iter_trns_x;
     time0 = G.time_given_x;

     x0       = X.rprm[nr_x0    ];
     hby2     = X.rprm[nr_hby2  ];
     y_old    = X.rprm[nr_y_old ];
     y_half   = X.rprm[nr_y_half];

     if (flag_invert == 0) {
       if (y_old > y_half) {
         offset1 = x0 - hby2;
       } else {
         offset1 = x0 + hby2;
       }
     } else {
       if (y_old < y_half) {
         offset1 = x0 - hby2;
       } else {
         offset1 = x0 + hby2;
       }
     }
     x1_1 = X.rprm[nr_x1_1];
     x1_2 = X.rprm[nr_x1_2];
     x2_1 = X.rprm[nr_x2_1];
     x2_2 = X.rprm[nr_x2_2];
     t_1 = X.rprm[nr_t_1];
     t_2 = X.rprm[nr_t_2];

     epsl      = X.rprm[nr_epsl     ];
     delt_min  = X.rprm[nr_delt_min ];
     delt_nrml = X.rprm[nr_delt_nrml];

     G.time_nextbreak_x = get_tnext(iter,flag_quad,
       t_1,t_2,(x1_1-x2_1-offset1),(x1_2-x2_2-offset1),
       time0,(x1-x2-offset1),epsl,delt_min,delt_nrml);

     return;
   }
   hby2   = X.rprm[nr_hby2  ];
   y_low  = X.rprm[nr_y_low ];
   y_high = X.rprm[nr_y_high];
   y_half = X.rprm[nr_y_half];
   y_old  = X.rprm[nr_y_old ];
   x0     = X.rprm[nr_x0    ];

   if (G.flags[G.i_init_guess]) {
     delx = x1-x2-x0;

     if (flag_invert == 0) {
       if (delx > hby2) {
         y0 = y_high;
       } else if (delx < (-hby2)) {
         y0 = y_low;
       } else {
//       set arbitrarily to low
         y0 = y_low;
       }
     } else {
       if (delx > hby2) {
         y0 = y_low;
       } else if (delx < (-hby2)) {
         y0 = y_high;
       } else {
//       set arbitrarily to low
         y0 = y_low;
       }
     }
     X.val_vr[nvr_y] = y0;
     return;
   }

   delx = x1-x2-x0;

   if (flag_invert == 0) {
     if (y_old > y_half) {
       if (delx < (-hby2)) {
         y0 = y_low;
       } else {
         y0 = y_high;
       }
     } else {
       if (delx > hby2) {
         y0 = y_high;
       } else {
         y0 = y_low;
       }
     }
   } else {
     if (y_old < y_half) {
       if (delx < (-hby2)) {
         y0 = y_high;
       } else {
         y0 = y_low;
       }
     } else {
       if (delx > hby2) {
         y0 = y_low;
       } else {
         y0 = y_high;
       }
     }
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_const(Global &G,XbeUsr &X,XbeJac &J) {
   double k0;
   double y;
   double k,k_scale;
   const int nvr_y = 0;
   const int nr_k = 0;
   const int nr_k_scale = 1;
   const int no_y = 0;
   const int ng_1 = 0;
// cout << "const.xbe" << endl;
   k0 = X.rprm[nr_k_scale]*X.rprm[nr_k];
   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = k0;
//   cout << "const.xbe: k0 = " << k0 << endl;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = k0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - k0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }
   return;
}
void x_cosine(Global &G,XbeUsr &X,XbeJac &J) {
   double y0;
   double x;
   double y;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int no_x = 0;
   const int no_y = 1;
   const int ng_1 = 0;
   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = cos(X.val_vr[nvr_x]);
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = cos(X.val_vr[nvr_x]);
     } else if (G.flags[G.i_implicit]) {
       x = X.val_vr[nvr_x];
       y = X.val_vr[nvr_y];

       if (G.flags[G.i_function]) {
         X.g[ng_1] = y - cos(x);
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] =  1.0;
         J.dgdvr[ng_1][nvr_x] = sin(x);
       }
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }
   return;
}
void x_diff(Global &G,XbeUsr &X,XbeJac &J) {
   double x1,x2;
   double y;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y = 2;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y = 2;
   const int ng_1 = 0;
   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = X.val_vr[nvr_x1] - X.val_vr[nvr_x2];
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = X.val_vr[nvr_x1] - X.val_vr[nvr_x2];
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - X.val_vr[nvr_x1] + X.val_vr[nvr_x2];
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y ] =  1.0;
         J.dgdvr[ng_1][nvr_x1] = -1.0;
         J.dgdvr[ng_1][nvr_x2] =  1.0;
       }
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x1] = X.val_vr[nvr_x1];
     X.outprm[no_x2] = X.val_vr[nvr_x2];
     X.outprm[no_y ] = X.val_vr[nvr_y ];
     return;
   }
   return;
}
void x_dq_to_abc(Global &G,XbeUsr &X,XbeJac &J) {
   static double k1=0,k2=0,k3=0;
   double d,q,cost,sint;
   double a,b,c;
   double alpha,beta;
   const int nvr_d = 0;
   const int nvr_q = 1;
   const int nvr_cost = 2;
   const int nvr_sint = 3;
   const int nvr_a = 4;
   const int nvr_b = 5;
   const int nvr_c = 6;
   const int no_d = 0;
   const int no_q = 1;
   const int no_a = 2;
   const int no_b = 3;
   const int no_c = 4;
   const int na_alpha = 0;
   const int na_beta = 1;
   const int ng_1 = 0;
   const int ng_2 = 1;
   const int ng_3 = 2;
   const int ng_4 = 3;
   const int ng_5 = 4;
   if (G.flags[G.i_one_time_parms]) {
     k1 = 2.0/3.0;
     k2 = 1.0/3.0;
     k3 = 1.0/(sqrt(3.0));
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_d] = X.val_vr[nvr_d];
     X.outprm[no_q] = X.val_vr[nvr_q];
     X.outprm[no_a] = X.val_vr[nvr_a];
     X.outprm[no_b] = X.val_vr[nvr_b];
     X.outprm[no_c] = X.val_vr[nvr_c];
     return;
   }
   cost = X.val_vr[nvr_cost];
   sint = X.val_vr[nvr_sint];
   d = X.val_vr[nvr_d];
   q = X.val_vr[nvr_q];

   if (G.flags[G.i_init_guess]) {
     alpha = cost*d - sint*q;
     beta  = sint*d + cost*q;

     X.val_aux[na_alpha] = alpha;
     X.val_aux[na_beta ] = beta;
     X.val_vr[nvr_a] =  k1*alpha;
     X.val_vr[nvr_b] = -k2*alpha + k3*beta;
     X.val_vr[nvr_c] = -k2*alpha - k3*beta;

     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       alpha = cost*d - sint*q;
       beta  = sint*d + cost*q;
       X.val_vr[nvr_a] =  k1*alpha;
       X.val_vr[nvr_b] = -k2*alpha + k3*beta;
       X.val_vr[nvr_c] = -k2*alpha - k3*beta;
     } else if (G.flags[G.i_implicit]) {
       alpha = X.val_aux[na_alpha];
       beta  = X.val_aux[na_beta ];

       if (G.flags[G.i_function]) {
         X.g[ng_1] = alpha - cost*d + sint*q;
         X.g[ng_2] = beta - sint*d - cost*q;

         X.g[ng_3] = a - k1*alpha;
         X.g[ng_4] = b + k2*alpha - k3*beta;
         X.g[ng_5] = c + k2*alpha + k3*beta;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdaux[ng_1][na_alpha] =  1.0;
         J.dgdvr [ng_1][nvr_cost] = -d;
         J.dgdvr [ng_1][nvr_sint] =  q;
         J.dgdvr [ng_1][nvr_d   ] = -cost;
         J.dgdvr [ng_1][nvr_q   ] =  sint;

         J.dgdaux[ng_2][na_beta ] =  1.0;
         J.dgdvr [ng_2][nvr_cost] = -q;
         J.dgdvr [ng_2][nvr_sint] = -d;
         J.dgdvr [ng_2][nvr_d   ] = -sint;
         J.dgdvr [ng_2][nvr_q   ] = -cost;

         J.dgdvr [ng_3][nvr_a   ] =  1.0;
         J.dgdaux[ng_3][na_alpha] = -k1;

         J.dgdvr [ng_4][nvr_b   ] =  1.0;
         J.dgdaux[ng_4][na_alpha] =  k2;
         J.dgdaux[ng_4][na_beta ] = -k3;

         J.dgdvr [ng_5][nvr_c   ] =  1.0;
         J.dgdaux[ng_5][na_alpha] =  k2;
         J.dgdaux[ng_5][na_beta ] =  k3;
       }
     }
     return;
   }
   return;
}
void x_dummy(Global &G,XbeUsr &X,XbeJac &J) {
   double x;
   const int nvr_x = 0;
   if (G.flags[G.i_init_guess]) {
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     return;
   }
   if (G.flags[G.i_outvar]) {
     return;
   }
   return;
}
void x_dummy_sink(Global &G,XbeUsr &X,XbeJac &J) {
   double y;
   const int nvr_y = 0;
   const int no_y = 0;
   const int ng_1 = 0;
   return;
}
void x_dummy_source(Global &G,XbeUsr &X,XbeJac &J) {
   double y;
   const int nvr_y = 0;
   const int no_y = 0;
   const int ng_1 = 0;
   return;
}
void x_gs_add(Global &G,XbeUsr &X,XbeJac &J) {
   double x1,x2;
   double y;
   double k1,k2;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y = 2;
   const int nr_k1 = 0;
   const int nr_k2 = 1;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y = 2;
   const int ng_1 = 0;
   return;
}
void x_gs_add_3(Global &G,XbeUsr &X,XbeJac &J) {
   double x1,x2,x3;
   double y;
   double k1,k2,k3;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_x3 = 2;
   const int nvr_y = 3;
   const int nr_k1 = 0;
   const int nr_k2 = 1;
   const int nr_k3 = 2;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_x3 = 2;
   const int no_y = 3;
   const int ng_1 = 0;
   return;
}
void x_gs_mult(Global &G,XbeUsr &X,XbeJac &J) {
   double x1,x2;
   double y;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y = 2;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y = 2;
   const int ng_1 = 0;
   return;
}
void x_gs_source(Global &G,XbeUsr &X,XbeJac &J) {
   double y;
   const int nvr_y = 0;
   const int no_y = 0;
   const int ng_1 = 0;
   return;
}
void x_indmc1(Global &G,XbeUsr &X,XbeJac &J) {
   double ids,iqs,idr,iqr,wr,tem0;
   double p;
   static double k4=0;
   double ids_psids,ids_psidr,iqs_psiqs,iqs_psiqr;
   double idr_psids,idr_psidr,iqr_psiqs,iqr_psiqr;
   double tem0_iqs,tem0_idr,tem0_ids,tem0_iqr;
   double tem0_psids,tem0_psidr,tem0_psiqs,tem0_psiqr;
   double wr_wrm;
   double vqs,vds,tl;
   double wrm;
   double psids,psidr,psiqs,psiqr;
   int poles;
   double rs,lls,lm,llr,rr,j,ls,lr,le,l1,l2,l3,x1,x2;
   double psids0,psiqs0,psidr0,psiqr0,wrm0;
   const int nvr_vqs = 0;
   const int nvr_vds = 1;
   const int nvr_tl = 2;
   const int nvr_wrm = 3;
   const int ni_poles = 0;
   const int nr_rs = 0;
   const int nr_lls = 1;
   const int nr_lm = 2;
   const int nr_llr = 3;
   const int nr_rr = 4;
   const int nr_j = 5;
   const int nr_ls = 6;
   const int nr_lr = 7;
   const int nr_le = 8;
   const int nr_l1 = 9;
   const int nr_l2 = 10;
   const int nr_l3 = 11;
   const int nr_x1 = 12;
   const int nr_x2 = 13;
   const int nst_psids0 = 0;
   const int nst_psiqs0 = 1;
   const int nst_psidr0 = 2;
   const int nst_psiqr0 = 3;
   const int nst_wrm0 = 4;
   const int no_wrm = 0;
   const int no_tem = 1;
   const int no_vds = 2;
   const int no_vqs = 3;
   const int no_ia = 4;
   const int no_ib = 5;
   const int no_ic = 6;
   const int na_psids = 0;
   const int na_psidr = 1;
   const int na_psiqs = 2;
   const int na_psiqr = 3;
   const int nf_1 = 0;
   const int nf_2 = 1;
   const int nf_3 = 2;
   const int nf_4 = 3;
   const int nf_5 = 4;
   const int ng_1 = 0;
   const int ng_2 = 1;
   const int ng_3 = 2;
   const int ng_4 = 3;
   const int ng_5 = 4;
// cout << "indmc1.xbe" << endl;

   if (G.flags[G.i_one_time_parms]) {
     k4 = 0.5*(sqrt(3.0));

     lls = X.rprm[nr_lls];
     lm  = X.rprm[nr_lm ];
     llr = X.rprm[nr_llr];

     ls = lls + lm;
     lr = llr + lm;
     le = (ls*lr/lm) - lm;
     l1 = lr/(lm*le);
     l2 = 1.0 + (lls/lm);
     l3 = lls/lm;

     X.rprm[nr_ls] = ls;
     X.rprm[nr_lr] = lr;
     X.rprm[nr_le] = le;
     X.rprm[nr_l1] = l1;
     X.rprm[nr_l2] = l2;
     X.rprm[nr_l3] = l3;

     poles = X.iprm[ni_poles];
     p = (double)(poles);
     x1 = 0.75*p*lm;
     x2 = 0.5*p;

     X.rprm[nr_x1] = x1;
     X.rprm[nr_x2] = x2;

     return;
   }
   if (G.flags[G.i_init_guess]) {
//   for init guess, use start-up parameters (for this element)
//   can add separate igparms for this purpose later.
     X.val_aux[na_psids] = X.stprm[nst_psids0];
     X.val_aux[na_psiqs] = X.stprm[nst_psiqs0];
     X.val_aux[na_psidr] = X.stprm[nst_psidr0];
     X.val_aux[na_psiqr] = X.stprm[nst_psiqr0];
     X.val_vr [nvr_wrm ] = X.stprm[nst_wrm0  ];
     return;
   }
   if (G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_aux[na_psids] = X.stprm[nst_psids0];
       X.val_aux[na_psiqs] = X.stprm[nst_psiqs0];
       X.val_aux[na_psidr] = X.stprm[nst_psidr0];
       X.val_aux[na_psiqr] = X.stprm[nst_psiqr0];
       X.val_vr [nvr_wrm ] = X.stprm[nst_wrm0  ];
     } else {
       X.h[nf_1] = X.val_aux[na_psids] - X.stprm[nst_psids0];
       X.h[nf_2] = X.val_aux[na_psiqs] - X.stprm[nst_psiqs0];
       X.h[nf_3] = X.val_aux[na_psidr] - X.stprm[nst_psidr0];
       X.h[nf_4] = X.val_aux[na_psiqr] - X.stprm[nst_psiqr0];
       X.h[nf_5] = X.val_vr [nvr_wrm ] - X.stprm[nst_wrm0  ];
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_wrm] = X.val_vr[nvr_wrm];
     X.outprm[no_vds] = X.val_vr[nvr_vds];
     X.outprm[no_vqs] = X.val_vr[nvr_vqs];

     psids = X.val_aux[na_psids];
     psidr = X.val_aux[na_psidr];
     psiqs = X.val_aux[na_psiqs];
     psiqr = X.val_aux[na_psiqr];

     le  = X.rprm[nr_le];
     lm  = X.rprm[nr_lm];
     l1  = X.rprm[nr_l1];
     l2  = X.rprm[nr_l2];

     ids = (l1*psids) - (psidr/le);
     iqs = (l1*psiqs) - (psiqr/le);
     idr = (psids/lm) - (l2*ids);
     iqr = (psiqs/lm) - (l2*iqs);

     X.outprm[no_ia] = iqs;
     X.outprm[no_ib] = -0.5*iqs-k4*ids;
     X.outprm[no_ic] = -0.5*iqs+k4*ids;

     x1 = X.rprm[nr_x1];
     tem0 = x1*(iqs*idr-ids*iqr);
     X.outprm[no_tem] = tem0;

     return;
   }
   if (G.flags[G.i_trns]) {
     if (G.flags[G.i_explicit]) {
       if (G.flags[G.i_alg_loop]) {
         X.h[nf_1] = X.val_aux[na_psids] - X.val_aux_u[na_psids];
         X.h[nf_2] = X.val_aux[na_psiqs] - X.val_aux_u[na_psiqs];
         X.h[nf_3] = X.val_aux[na_psidr] - X.val_aux_u[na_psidr];
         X.h[nf_4] = X.val_aux[na_psiqr] - X.val_aux_u[na_psiqr];
         X.h[nf_5] = X.val_vr [nvr_wrm ] - X.val_vr_u [nvr_wrm ];
       } else {
         rs  = X.rprm[nr_rs ];
         lls = X.rprm[nr_lls];
         lm  = X.rprm[nr_lm ];
         rr  = X.rprm[nr_rr ];
         j   = X.rprm[nr_j  ];
         le  = X.rprm[nr_le ];
         l1  = X.rprm[nr_l1 ];
         l2  = X.rprm[nr_l2 ];
         l3  = X.rprm[nr_l3 ];
         x1  = X.rprm[nr_x1 ];
         x2  = X.rprm[nr_x2 ];

         vqs = X.val_vr[nvr_vqs];
         vds = X.val_vr[nvr_vds];
         wrm = X.val_vr[nvr_wrm];
         tl  = X.val_vr[nvr_tl ];

         psids = X.val_aux[na_psids];
         psidr = X.val_aux[na_psidr];
         psiqs = X.val_aux[na_psiqs];
         psiqr = X.val_aux[na_psiqr];

         ids = (l1*psids) - (psidr/le);
         iqs = (l1*psiqs) - (psiqr/le);

         idr = (psids/lm) - (l2*ids);
         iqr = (psiqs/lm) - (l2*iqs);

         tem0 = x1*(iqs*idr-ids*iqr);
         wr   = x2*wrm;

         X.f[nf_1] = vds-rs*ids;
         X.f[nf_2] = vqs-rs*iqs;
         X.f[nf_3] = (-wr)*psiqr-rr*idr;
         X.f[nf_4] = ( wr)*psidr-rr*iqr;
         X.f[nf_5] = (tem0-tl)/j;
       }
     } else {
//     There is some repetition here; never mind.
       rs  = X.rprm[nr_rs ];
       lls = X.rprm[nr_lls];
       lm  = X.rprm[nr_lm ];
       rr  = X.rprm[nr_rr ];
       j   = X.rprm[nr_j  ];
       le  = X.rprm[nr_le ];
       l1  = X.rprm[nr_l1 ];
       l2  = X.rprm[nr_l2 ];
       l3  = X.rprm[nr_l3 ];
       x1  = X.rprm[nr_x1 ];
       x2  = X.rprm[nr_x2 ];

       vqs = X.val_vr[nvr_vqs];
       vds = X.val_vr[nvr_vds];
       wrm = X.val_vr[nvr_wrm];
       tl  = X.val_vr[nvr_tl ];

       psids = X.val_aux[na_psids];
       psidr = X.val_aux[na_psidr];
       psiqs = X.val_aux[na_psiqs];
       psiqr = X.val_aux[na_psiqr];

       if (G.flags[G.i_function] || G.flags[G.i_jacobian]) {
         ids = (l1*psids) - (psidr/le);
         iqs = (l1*psiqs) - (psiqr/le);
         idr = (psids/lm) - (l2*ids);
         iqr = (psiqs/lm) - (l2*iqs);
         tem0 = x1*(iqs*idr-ids*iqr);
         wr = x2*wrm;

         if (G.flags[G.i_function]) {
           X.g[ng_1] = vds-rs*ids;
           X.g[ng_2] = vqs-rs*iqs;
           X.g[ng_3] = (-wr)*psiqr-rr*idr;
           X.g[ng_4] = ( wr)*psidr-rr*iqr;
           X.g[ng_5] = (tem0-tl)/j;
         }
       }
       if (G.flags[G.i_jacobian]) {
//       ids = (l1*psids) - (psidr/le);
         ids_psids = l1;
         ids_psidr = -1.0/le;

//       iqs = (l1*psiqs) - (psiqr/le);
         iqs_psiqs = l1;
         iqs_psiqr = -1.0/le;

//       idr = (psids/lm) - (l2*ids);
         idr_psids = (1.0/lm) - (l2*ids_psids);
         idr_psidr =          - (l2*ids_psidr);

//       iqr = (psiqs/lm) - (l2*iqs);
         iqr_psiqs = (1.0/lm) - (l2*iqs_psiqs);
         iqr_psiqr =          - (l2*iqs_psiqr);

//       tem0 = x1*(iqs*idr-ids*iqr);
         tem0_iqs =  x1*idr;
         tem0_idr =  x1*iqs;
         tem0_ids = -x1*iqr;
         tem0_iqr = -x1*ids;

         tem0_psids =
           tem0_idr*idr_psids +
           tem0_ids*ids_psids;

         tem0_psidr =
           tem0_idr*idr_psidr +
           tem0_ids*ids_psidr;

         tem0_psiqs =
           tem0_iqs*iqs_psiqs +
           tem0_iqr*iqr_psiqs;

         tem0_psiqr =
           tem0_iqs*iqs_psiqr +
           tem0_iqr*iqr_psiqr;

//       wr = x2*wrm;
         wr_wrm = x2;

//       X.g[ng_1] = vds-rs*ids;
         J.dgdvr[ng_1][nvr_vds] = 1.0;
         J.dgdaux[ng_1][na_psids] = -rs*ids_psids;
         J.dgdaux[ng_1][na_psidr] = -rs*ids_psidr;

//       X.g[ng_2] = vqs-rs*iqs;
         J.dgdvr[ng_2][nvr_vqs] = 1.0;
         J.dgdaux[ng_2][na_psiqs] = -rs*iqs_psiqs;
         J.dgdaux[ng_2][na_psiqr] = -rs*iqs_psiqr;

//       X.g[ng_3] = (-wr)*psiqr-rr*idr;
         J.dgdvr[ng_3][nvr_wrm] = (-wr_wrm)*psiqr;
         J.dgdaux[ng_3][na_psiqr] = (-wr);
         J.dgdaux[ng_3][na_psids] = -rr*idr_psids;
         J.dgdaux[ng_3][na_psidr] = -rr*idr_psidr;

//       X.g[ng_4] = ( wr)*psidr-rr*iqr;
         J.dgdvr[ng_4][nvr_wrm] = (wr_wrm)*psidr;
         J.dgdaux[ng_4][na_psidr] = wr;
         J.dgdaux[ng_4][na_psiqs] = -rr*iqr_psiqs;
         J.dgdaux[ng_4][na_psiqr] = -rr*iqr_psiqr;

//       X.g[ng_5] = (tem0-tl)/j;
         J.dgdvr[ng_5][nvr_tl] = -1.0/j;
         J.dgdaux[ng_5][na_psids] = tem0_psids/j;
         J.dgdaux[ng_5][na_psidr] = tem0_psidr/j;
         J.dgdaux[ng_5][na_psiqs] = tem0_psiqs/j;
         J.dgdaux[ng_5][na_psiqr] = tem0_psiqr/j;

//       cout << "J.dgdvr[ng_1][nvr_vds] = " << J.dgdvr[ng_1][nvr_vds] << endl;
//       cout << "J.dgdaux[ng_1][na_psids] = " << J.dgdaux[ng_1][na_psids] << endl;
//       cout << "J.dgdaux[ng_1][na_psidr] = " << J.dgdaux[ng_1][na_psidr] << endl;
//       cout << "J.dgdvr[ng_2][nvr_vqs] = " << J.dgdvr[ng_2][nvr_vqs] << endl;
//       cout << "J.dgdaux[ng_2][na_psiqs] = " << J.dgdaux[ng_2][na_psiqs] << endl;
//       cout << "J.dgdaux[ng_2][na_psiqr] = " << J.dgdaux[ng_2][na_psiqr] << endl;
//       cout << "J.dgdvr[ng_3][nvr_wrm] = " << J.dgdvr[ng_3][nvr_wrm] << endl;
//       cout << "J.dgdaux[ng_3][na_psiqr] = " << J.dgdaux[ng_3][na_psiqr] << endl;
//       cout << "J.dgdaux[ng_3][na_psids] = " << J.dgdaux[ng_3][na_psids] << endl;
//       cout << "J.dgdaux[ng_3][na_psidr] = " << J.dgdaux[ng_3][na_psidr] << endl;
//       cout << "J.dgdvr[ng_4][nvr_wrm] = " << J.dgdvr[ng_4][nvr_wrm] << endl;
//       cout << "J.dgdaux[ng_4][na_psidr] = " << J.dgdaux[ng_4][na_psidr] << endl;
//       cout << "J.dgdaux[ng_4][na_psiqs] = " << J.dgdaux[ng_4][na_psiqs] << endl;
//       cout << "J.dgdaux[ng_4][na_psiqr] = " << J.dgdaux[ng_4][na_psiqr] << endl;
//       cout << "J.dgdvr[ng_5][nvr_tl] = " << J.dgdvr[ng_5][nvr_tl] << endl;
//       cout << "J.dgdaux[ng_5][na_psids] = " << J.dgdaux[ng_5][na_psids] << endl;
//       cout << "J.dgdaux[ng_5][na_psidr] = " << J.dgdaux[ng_5][na_psidr] << endl;
//       cout << "J.dgdaux[ng_5][na_psiqs] = " << J.dgdaux[ng_5][na_psiqs] << endl;
//       cout << "J.dgdaux[ng_5][na_psiqr] = " << J.dgdaux[ng_5][na_psiqr] << endl;
       }
     }
     return;
   }

   return;
}
void x_integrator(Global &G,XbeUsr &X,XbeJac &J) {
   double x;
   double y;
   double k;
   double y_st;
   double y_ig;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int nr_k = 0;
   const int nst_y_st = 0;
   const int nig_y_ig = 0;
   const int no_x = 0;
   const int no_y = 1;
   const int nf_1 = 0;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     return;
   }
   if (G.flags[G.i_init_guess]) {
     X.val_vr [nvr_y] = X.igprm[nig_y_ig];
     return;
   }
   if (G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = X.stprm[nst_y_st];
//     cout << "integrator: X.val_vr[nvr_y] = " << X.val_vr[nvr_y] << endl;
     } else if (G.flags[G.i_implicit]) {
//     cout << "integrator: startup, implicit" << endl;
//     cout << "integrator: X.val_vr[nvr_x] = " << X.val_vr[nvr_x] << endl;
//     cout << "integrator: X.val_vr[nvr_y] = " << X.val_vr[nvr_y] << endl;
       X.h[nf_1] = X.val_vr[nvr_y] - X.stprm[nst_y_st];
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
//   cout << "integrator: outvar" << endl;
//   cout << "integrator: X.val_vr[nvr_x] = " << X.val_vr[nvr_x] << endl;
//   cout << "integrator: X.val_vr[nvr_y] = " << X.val_vr[nvr_y] << endl;
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }
   if (G.flags[G.i_trns]) {
     if (G.flags[G.i_explicit]) {
       if (G.flags[G.i_alg_loop]) {
//       use h[..] for both startup and explicit/alg loop:
         X.h[nf_1] = X.val_vr[nvr_y] - X.val_vr_u[nvr_y];
       } else {
         k = X.rprm[nr_k];
         x = X.val_vr[nvr_x];
         X.f[nf_1] = k*x;
//       cout << "integrator: x = " << x << " k = " << k << endl;
       }
     } else if (G.flags[G.i_implicit]) {
       k = X.rprm[nr_k];
       x = X.val_vr[nvr_x];
       if (G.flags[G.i_function]) {
         X.g[ng_1] = k*x;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_x] = k;
       }
     }
     return;
   }
   return;
}
void x_lag_1(Global &G,XbeUsr &X,XbeJac &J) {
   double y0;
   double x;
   double y;
   double tr,k;
   double y_st;
   double y_ig;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int nr_tr = 0;
   const int nr_k = 1;
   const int nst_y_st = 0;
   const int nig_y_ig = 0;
   const int no_x = 0;
   const int no_y = 1;
   const int nf_1 = 0;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     tr = X.rprm[nr_tr];
     k = 1.0/tr;
     X.rprm[nr_k] = k;
     return;
   }
   if (G.flags[G.i_init_guess]) {
     X.val_vr [nvr_y] = X.igprm[nig_y_ig];
     return;
   }
   if (G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = X.stprm[nst_y_st];
     } else if (G.flags[G.i_implicit]) {
       X.h[nf_1] = X.val_vr[nvr_y] - X.stprm[nst_y_st];
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }
   if (G.flags[G.i_trns]) {
     if (G.flags[G.i_explicit]) {
       if (G.flags[G.i_alg_loop]) {
         X.h[nf_1] = X.val_vr[nvr_y] - X.val_vr_u[nvr_y];
       } else {
         k = X.rprm[nr_k];
         x = X.val_vr[nvr_x];
         y = X.val_vr[nvr_y];
         X.f[nf_1] = k*(-y+x);
       }
     } else if (G.flags[G.i_implicit]) {
       k = X.rprm[nr_k];
       x = X.val_vr[nvr_x];
       y = X.val_vr[nvr_y];

       if (G.flags[G.i_function]) {
         X.g[ng_1] = k*(-y+x);
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_x] =  k;
         J.dgdvr[ng_1][nvr_y] = -k;
       }
     }
     return;
   }
   return;
}
void x_lag_2(Global &G,XbeUsr &X,XbeJac &J) {
   double tr,y0;
   double x;
   double y;
   double t_delay,k;
   double y_st;
   double y_ig;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int nr_t_delay = 0;
   const int nr_k = 1;
   const int nst_y_st = 0;
   const int nig_y_ig = 0;
   const int no_x = 0;
   const int no_y = 1;
   const int nf_1 = 0;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     tr = X.rprm[nr_t_delay]/log(2.0);
     k = 1.0/tr;
     X.rprm[nr_k] = k;
     return;
   }
   if (G.flags[G.i_init_guess]) {
     X.val_vr [nvr_y] = X.igprm[nig_y_ig];
     return;
   }
   if (G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = X.stprm[nst_y_st];
     } else if (G.flags[G.i_implicit]) {
       X.h[nf_1] = X.val_vr[nvr_y] - X.stprm[nst_y_st];
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }
   if (G.flags[G.i_trns]) {
     if (G.flags[G.i_explicit]) {
       if (G.flags[G.i_alg_loop]) {
         X.h[nf_1] = X.val_vr[nvr_y] - X.val_vr_u[nvr_y];
       } else {
         k = X.rprm[nr_k];
         x = X.val_vr[nvr_x];
         y = X.val_vr[nvr_y];
         X.f[nf_1] = k*(-y+x);
       }
     } else if (G.flags[G.i_implicit]) {
       k = X.rprm[nr_k];
       x = X.val_vr[nvr_x];
       y = X.val_vr[nvr_y];

       if (G.flags[G.i_function]) {
         X.g[ng_1] = k*(-y+x);
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_x] =  k;
         J.dgdvr[ng_1][nvr_y] = -k;
       }
     }
     return;
   }
   return;
}
void x_limiter(Global &G,XbeUsr &X,XbeJac &J) {
   double y0;
   double x;
   double y;
   double xmin,xmax;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int nr_xmin = 0;
   const int nr_xmax = 1;
   const int no_x = 0;
   const int no_y = 1;
   const int ng_1 = 0;
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }
   xmin = X.rprm[nr_xmin];
   xmax = X.rprm[nr_xmax];

   x = X.val_vr[nvr_x];
   if (x >= xmax) {
     y0 = xmax;
   } else if (x <= xmin) {
     y0 = xmin;
   } else {
     y0 = x;
   }   

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       y = X.val_vr[nvr_y];

       if (G.flags[G.i_function]) {
         X.g[ng_1] = y - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] =  1.0;
         if (x >= xmax) {
           J.dgdvr[ng_1][nvr_x] = 0.0;
         } else if (x <= xmin) {
           J.dgdvr[ng_1][nvr_x] = 0.0;
         } else {
           J.dgdvr[ng_1][nvr_x] = -1.0;
         }   
       }
     }
     return;
   }
   return;
}
void x_modulo(Global &G,XbeUsr &X,XbeJac &J) {
   double x;
   double x1,x2;
   const int nvr_x = 0;
   const int nr_x1 = 0;
   const int nr_x2 = 1;
   const int no_x = 0;
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     return;
   }
   x1 = X.rprm[nr_x1];
   x2 = X.rprm[nr_x2];

   if (G.flags[G.i_init_guess]) {
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     return;
   }
   return;
}
void x_modulo_twopi(Global &G,XbeUsr &X,XbeJac &J) {
   double x;
   double x1,x2;
   const int nvr_x = 0;
   const int nr_x1 = 0;
   const int nr_x2 = 1;
   const int no_x = 0;
   if (G.flags[G.i_one_time_parms]) {
     X.rprm[nr_x1] = -G.pi;
     X.rprm[nr_x2] =  G.pi;
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     return;
   }
   x1 = X.rprm[nr_x1];
   x2 = X.rprm[nr_x2];

   if (G.flags[G.i_init_guess]) {
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     return;
   }
   return;
}
void x_multscl(Global &G,XbeUsr &X,XbeJac &J) {
   double x;
   double y;
   double k;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int nr_k = 0;
   const int no_x = 0;
   const int no_y = 1;
   const int ng_1 = 0;
   k = X.rprm[nr_k];
   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = k*X.val_vr[nvr_x];
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = k*X.val_vr[nvr_x];
//     cout << "multscl.xbe: x = " << X.val_vr[nvr_x]
//      << " y = " << X.val_vr[nvr_y] << endl;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - k*X.val_vr[nvr_x];
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
         J.dgdvr[ng_1][nvr_x] = -k;
       }
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }
   return;
}
void x_mult_2(Global &G,XbeUsr &X,XbeJac &J) {
   double x1,x2;
   double y;
   double k;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y = 2;
   const int nr_k = 0;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y = 2;
   const int ng_1 = 0;
   k = X.rprm[nr_k];
   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = k*X.val_vr[nvr_x1]*X.val_vr[nvr_x2];
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = k*X.val_vr[nvr_x1]*X.val_vr[nvr_x2];
     } else if (G.flags[G.i_implicit]) {
       y = X.val_vr[nvr_y];
       x1 = X.val_vr[nvr_x1];
       x2 = X.val_vr[nvr_x2];

       if (G.flags[G.i_function]) {
         X.g[ng_1] = y - k*x1*x2;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y ] =  1.0;
         J.dgdvr[ng_1][nvr_x1] = -k*x2;
         J.dgdvr[ng_1][nvr_x2] = -k*x1;
       }
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x1] = X.val_vr[nvr_x1];
     X.outprm[no_x2] = X.val_vr[nvr_x2];
     X.outprm[no_y ] = X.val_vr[nvr_y ];
     return;
   }
   return;
}
void x_not(Global &G,XbeUsr &X,XbeJac &J) {
   double y0;
   bool X1,Y;
   double x;
   double y;
   double y_high,hb2;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int nr_y_high = 0;
   const int nr_hb2 = 1;
   const int no_x = 0;
   const int no_y = 1;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     y_high = X.rprm[nr_y_high];
     hb2 = 0.5*y_high;
     X.rprm[nr_hb2] = hb2;
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }
   y_high = X.rprm[nr_y_high];
   hb2 = X.rprm[nr_hb2];

   x = X.val_vr[nvr_x];
   X1 = x > hb2;
   Y = !X1;

   if (Y) {
     y0 = y_high;
   } else {
     y0 = 0.0;
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y ] =  1.0;
       }
     }
     return;
   }
   return;
}
void x_or_2(Global &G,XbeUsr &X,XbeJac &J) {
   double y0;
   bool X1,X2,Y;
   double x1,x2;
   double y;
   double y_high,hb2;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y = 2;
   const int nr_y_high = 0;
   const int nr_hb2 = 1;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y = 2;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     y_high = X.rprm[nr_y_high];
     hb2 = 0.5*y_high;
     X.rprm[nr_hb2] = hb2;
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x1] = X.val_vr[nvr_x1];
     X.outprm[no_x2] = X.val_vr[nvr_x2];
     X.outprm[no_y ] = X.val_vr[nvr_y ];
     return;
   }
   y_high = X.rprm[nr_y_high];
   hb2 = X.rprm[nr_hb2];

   x1 = X.val_vr[nvr_x1];
   x2 = X.val_vr[nvr_x2];
   X1 = x1 > hb2;
   X2 = x2 > hb2;
   Y = X1 || X2;

   if (Y) {
     y0 = y_high;
   } else {
     y0 = 0.0;
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y ] =  1.0;
       }
     }
     return;
   }
   return;
}
void x_pwl10_xy(Global &G,XbeUsr &X,XbeJac &J) {
   int indx,i;
// i_offset is the total number of points (=10 for pwl10)
   const int i_offset=10;
   double x1a,x2a,y1a,y2a,slope,y0;
   double x;
   double y;
   int n;
   double x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,y1,y2,y3,y4,y5,y6,y7,y8,y9,y10;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int ni_n = 0;
   const int nr_x1 = 0;
   const int nr_x2 = 1;
   const int nr_x3 = 2;
   const int nr_x4 = 3;
   const int nr_x5 = 4;
   const int nr_x6 = 5;
   const int nr_x7 = 6;
   const int nr_x8 = 7;
   const int nr_x9 = 8;
   const int nr_x10 = 9;
   const int nr_y1 = 10;
   const int nr_y2 = 11;
   const int nr_y3 = 12;
   const int nr_y4 = 13;
   const int nr_y5 = 14;
   const int nr_y6 = 15;
   const int nr_y7 = 16;
   const int nr_y8 = 17;
   const int nr_y9 = 18;
   const int nr_y10 = 19;
   const int no_x = 0;
   const int no_y = 1;
   const int ng_1 = 0;
// cout << "pwl10_xy.xbe" << endl;

   if (G.flags[G.i_one_time_parms]) {
     X.val_vr[nvr_y] = 0.0;
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }

   n = X.iprm[ni_n];
   x = X.val_vr[nvr_x];
   y = X.val_vr[nvr_y];

   indx = n-1;
   for (i=0; i < n; i++) {
     if (x <= X.rprm[i]) {
       indx = i-1;
       break;
     }
   }
   if (indx == (-1)) {
     y0 = X.rprm[i_offset];
   } else if (indx == (n-1)) {
     y0 = X.rprm[i_offset+n-1];
   } else {
     x1a = X.rprm[indx];
     x2a = X.rprm[indx+1];
     y1a = X.rprm[i_offset+indx];
     y2a = X.rprm[i_offset+indx+1];

     if (abs(x2a-x1a) < 1.0e-12) {
       cout << "pwl10_xy.xbe: x1 and x2 are too close!" << endl;
       cout << "   Halting.." << endl;
       exit (1);
     }
     slope = (y2a-y1a)/(x2a-x1a);
     y0 = y1a + slope*(x-x1a);
   }

   if (G.flags[G.i_init_guess]) {
//   cout << "pwl10_xy: init_guess: y0 =" << y0 << endl;
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_pwl20(Global &G,XbeUsr &X,XbeJac &J) {
   const int nmax=20;
   int i,intrvl;
   double time0,y0,slp1;
   const double epsl=1.0e-15;
   double y;
   int n;
   double t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,
     t20,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20;
   const int nvr_y = 0;
   const int ni_n = 0;
   const int nr_t1 = 0;
   const int nr_t2 = 1;
   const int nr_t3 = 2;
   const int nr_t4 = 3;
   const int nr_t5 = 4;
   const int nr_t6 = 5;
   const int nr_t7 = 6;
   const int nr_t8 = 7;
   const int nr_t9 = 8;
   const int nr_t10 = 9;
   const int nr_t11 = 10;
   const int nr_t12 = 11;
   const int nr_t13 = 12;
   const int nr_t14 = 13;
   const int nr_t15 = 14;
   const int nr_t16 = 15;
   const int nr_t17 = 16;
   const int nr_t18 = 17;
   const int nr_t19 = 18;
   const int nr_t20 = 19;
   const int nr_v1 = 20;
   const int nr_v2 = 21;
   const int nr_v3 = 22;
   const int nr_v4 = 23;
   const int nr_v5 = 24;
   const int nr_v6 = 25;
   const int nr_v7 = 26;
   const int nr_v8 = 27;
   const int nr_v9 = 28;
   const int nr_v10 = 29;
   const int nr_v11 = 30;
   const int nr_v12 = 31;
   const int nr_v13 = 32;
   const int nr_v14 = 33;
   const int nr_v15 = 34;
   const int nr_v16 = 35;
   const int nr_v17 = 36;
   const int nr_v18 = 37;
   const int nr_v19 = 38;
   const int nr_v20 = 39;
   const int no_y = 0;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }

   n = X.iprm[ni_n];
   time0 = G.time_given_x;

   if (G.flags[G.i_next_time]) {
     if (time0 >= X.rprm[n-1]) {
       G.time_nextbreak_x = G.time_end;
     } else if (time0 < X.rprm[0]) {
       G.time_nextbreak_x = X.rprm[0];
     } else {
       for (int i=1; i < n; i++) {
         if (time0 < X.rprm[i]) {
           if (abs(X.rprm[i]-time0) < epsl) {
             intrvl = i;
           } else {
             intrvl = i-1;
           }
           break;
         }
       }
       G.time_nextbreak_x = X.rprm[intrvl+1];
     }
     return;
   }
   if (time0 >= X.rprm[n-1]) {
     y0 = X.rprm[nmax+n-1];
   } else if (time0 < X.rprm[0]) {
     y0 = X.rprm[nmax];
   } else {
     for (int i=1; i < n; i++) {
       if (time0 < X.rprm[i]) {
         if (abs(X.rprm[i]-time0) < epsl) {
           intrvl = i;
         } else {
           intrvl = i-1;
         }
         break;
       }
     }
     slp1 = (X.rprm[nmax+intrvl+1]-X.rprm[nmax+intrvl])/
            (X.rprm[intrvl+1]-X.rprm[intrvl]);
     y0 = X.rprm[nmax+intrvl]+
             slp1*(time0-X.rprm[intrvl]);
   }

   if (G.flags[G.i_init_guess]) {
     cout << "pwl20: init_guess: y0 =" << y0 << endl;
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_sine(Global &G,XbeUsr &X,XbeJac &J) {
   double y0;
   double x;
   double y;
   const int nvr_x = 0;
   const int nvr_y = 1;
   const int no_x = 0;
   const int no_y = 1;
   const int ng_1 = 0;
   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = sin(X.val_vr[nvr_x]);
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = sin(X.val_vr[nvr_x]);
     } else if (G.flags[G.i_implicit]) {
       x = X.val_vr[nvr_x];
       y = X.val_vr[nvr_y];

       if (G.flags[G.i_function]) {
         X.g[ng_1] = y - sin(x);
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] =  1.0;
         J.dgdvr[ng_1][nvr_x] = -cos(x);
       }
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x] = X.val_vr[nvr_x];
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }
   return;
}
void x_srcac(Global &G,XbeUsr &X,XbeJac &J) {
   double v0;
   double y;
   double a,f_hz,phi_deg,t0,dc,omega,phi_rad;
   const int nvr_y = 0;
   const int nr_a = 0;
   const int nr_f_hz = 1;
   const int nr_phi_deg = 2;
   const int nr_t0 = 3;
   const int nr_dc = 4;
   const int nr_omega = 5;
   const int nr_phi_rad = 6;
   const int no_y = 0;
   const int ng_1 = 0;
// cout << "src_ac.xbe" << endl;

   if (G.flags[G.i_one_time_parms]) {
     f_hz    = X.rprm[nr_f_hz   ];
     phi_deg = X.rprm[nr_phi_deg];

     omega   = G.twopi*f_hz;
     phi_rad = G.deg_to_rad*phi_deg;

     X.rprm[nr_omega  ] = omega;
     X.rprm[nr_phi_rad] = phi_rad;

     return;
   }
   if (G.flags[G.i_init_guess]) {
     a       = X.rprm[nr_a      ];
     t0      = X.rprm[nr_t0     ];
     dc      = X.rprm[nr_dc     ];
     omega   = X.rprm[nr_omega  ];
     phi_rad = X.rprm[nr_phi_rad];

     v0 = a*sin(omega*(G.time_given_x-t0)+phi_rad) + dc;

     X.val_vr[nvr_y] = v0;
//   cout << "srcac.xbe: y=" << X.val_vr[nvr_y] << endl;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     a       = X.rprm[nr_a      ];
     t0      = X.rprm[nr_t0     ];
     dc      = X.rprm[nr_dc     ];
     omega   = X.rprm[nr_omega  ];
     phi_rad = X.rprm[nr_phi_rad];

     v0 = a*sin(omega*(G.time_given_x-t0)+phi_rad) + dc;
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = v0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - v0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }
   return;
}
void x_sum_2(Global &G,XbeUsr &X,XbeJac &J) {
   double x1,x2;
   double y;
   double k1,k2;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y = 2;
   const int nr_k1 = 0;
   const int nr_k2 = 1;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_y = 2;
   const int ng_1 = 0;
   k1 = X.rprm[nr_k1];
   k2 = X.rprm[nr_k2];
   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = k1*X.val_vr[nvr_x1] + k2*X.val_vr[nvr_x2];
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = k1*X.val_vr[nvr_x1] + k2*X.val_vr[nvr_x2];
//     cout << "sum_2: k1 = " << k1 << " k2 = " << k2
//       << " X.val_vr[nvr_x1] = " << X.val_vr[nvr_x1]
//       << " X.val_vr[nvr_x2] = " << X.val_vr[nvr_x2]
//       << " X.val_vr[nvr_y] = " << X.val_vr[nvr_y]
//       << endl;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - k1*X.val_vr[nvr_x1] - k2*X.val_vr[nvr_x2];
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y ] =  1.0;
         J.dgdvr[ng_1][nvr_x1] = -k1;
         J.dgdvr[ng_1][nvr_x2] = -k2;
       }
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x1] = X.val_vr[nvr_x1];
     X.outprm[no_x2] = X.val_vr[nvr_x2];
     X.outprm[no_y ] = X.val_vr[nvr_y ];
     return;
   }
   return;
}
void x_sum_3(Global &G,XbeUsr &X,XbeJac &J) {
   double x1,x2,x3;
   double y;
   double k1,k2,k3;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_x3 = 2;
   const int nvr_y = 3;
   const int nr_k1 = 0;
   const int nr_k2 = 1;
   const int nr_k3 = 2;
   const int no_x1 = 0;
   const int no_x2 = 1;
   const int no_x3 = 2;
   const int no_y = 3;
   const int ng_1 = 0;
   k1 = X.rprm[nr_k1];
   k2 = X.rprm[nr_k2];
   k3 = X.rprm[nr_k3];
   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = k1*X.val_vr[nvr_x1] + k2*X.val_vr[nvr_x2] + k3*X.val_vr[nvr_x3];
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = k1*X.val_vr[nvr_x1] + k2*X.val_vr[nvr_x2] + k3*X.val_vr[nvr_x3];
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y]
            - k1*X.val_vr[nvr_x1]
            - k2*X.val_vr[nvr_x2]
            - k3*X.val_vr[nvr_x3];
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y ] =  1.0;
         J.dgdvr[ng_1][nvr_x1] = -k1;
         J.dgdvr[ng_1][nvr_x2] = -k2;
         J.dgdvr[ng_1][nvr_x3] = -k3;
       }
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_x1] = X.val_vr[nvr_x1];
     X.outprm[no_x2] = X.val_vr[nvr_x2];
     X.outprm[no_x3] = X.val_vr[nvr_x3];
     X.outprm[no_y ] = X.val_vr[nvr_y ];
     return;
   }
   return;
}
void x_triangle_1(Global &G,XbeUsr &X,XbeJac &J) {
   double y0,t0_new,delta_min,t_a,t_b,tnext_p;
   int n;
   double y;
   double T1,T2,L1,L2,t0,T,slope1,slope2,epsl;
   const int nvr_y = 0;
   const int nr_T1 = 0;
   const int nr_T2 = 1;
   const int nr_L1 = 2;
   const int nr_L2 = 3;
   const int nr_t0 = 4;
   const int nr_T = 5;
   const int nr_slope1 = 6;
   const int nr_slope2 = 7;
   const int nr_epsl = 8;
   const int no_y = 0;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     T1 = X.rprm[nr_T1];
     T2 = X.rprm[nr_T2];
     L1 = X.rprm[nr_L1];
     L2 = X.rprm[nr_L2];

     T = T1 + T2;
     slope1 = (L2-L1)/T1;
     slope2 = (L1-L2)/T2;
     epsl = T/1000.0;

     X.rprm[nr_T] = T;
     X.rprm[nr_slope1] = slope1;
     X.rprm[nr_slope2] = slope2;
     X.rprm[nr_epsl] = epsl;

     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }

   t0   = X.rprm[nr_t0  ];
   T    = X.rprm[nr_T   ];
   T1   = X.rprm[nr_T1  ];
   epsl = X.rprm[nr_epsl];

   if (G.time_given_x < t0) {
     n = ((t0-G.time_given_x)/T) + 1;
     t0_new = t0-n*T;
   } else {
     t0_new = t0;
   }
   t_a = G.time_given_x-t0_new;
   t_b = fmod(t_a,T);

   if (abs(t_b-T) < epsl) t_b = 0.0;

   if (G.flags[G.i_next_time]) {
     if (t_b < T1) {
       tnext_p = T1;
     } else {
       tnext_p = T;
     }
     G.time_nextbreak_x = G.time_given_x + (tnext_p-t_b);
     return;
   }

   L1 = X.rprm[nr_L1];
   L2 = X.rprm[nr_L2];
   slope1 = X.rprm[nr_slope1];
   slope2 = X.rprm[nr_slope2];

   if (t_b < T1) {
     y0 = L1 + slope1*t_b;
   } else {
     y0 = L2 + slope2*(t_b-T1);
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_triangle_2(Global &G,XbeUsr &X,XbeJac &J) {
   double y0,t0_new,delta_min,t_a,t_b,tnext_p;
   int n;
   double y;
   double T,L1,L2,t0,slope1,slope2,epsl,T1,T2;
   const int nvr_y = 0;
   const int nr_T = 0;
   const int nr_L1 = 1;
   const int nr_L2 = 2;
   const int nr_t0 = 3;
   const int nr_slope1 = 4;
   const int nr_slope2 = 5;
   const int nr_epsl = 6;
   const int nr_T1 = 7;
   const int nr_T2 = 8;
   const int no_y = 0;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     T = X.rprm[nr_T];
     T1 = 0.5*T;
     T2 = T1;

     L1 = X.rprm[nr_L1];
     L2 = X.rprm[nr_L2];

     slope1 = (L2-L1)/T1;
     slope2 = (L1-L2)/T2;
     epsl = T/1000.0;

     X.rprm[nr_slope1] = slope1;
     X.rprm[nr_slope2] = slope2;
     X.rprm[nr_epsl] = epsl;

     X.rprm[nr_T1] = T1;
     X.rprm[nr_T2] = T2;

     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }

   t0   = X.rprm[nr_t0  ];
   T    = X.rprm[nr_T   ];
   T1   = X.rprm[nr_T1  ];
   epsl = X.rprm[nr_epsl];

   if (G.time_given_x < t0) {
     n = ((t0-G.time_given_x)/T) + 1;
     t0_new = t0-n*T;
   } else {
     t0_new = t0;
   }
   t_a = G.time_given_x-t0_new;
   t_b = fmod(t_a,T);

   if (abs(t_b-T) < epsl) t_b = 0.0;

   if (G.flags[G.i_next_time]) {
     if (t_b < T1) {
       tnext_p = T1;
     } else {
       tnext_p = T;
     }
     G.time_nextbreak_x = G.time_given_x + (tnext_p-t_b);
     return;
   }

   L1 = X.rprm[nr_L1];
   L2 = X.rprm[nr_L2];
   slope1 = X.rprm[nr_slope1];
   slope2 = X.rprm[nr_slope2];

   if (t_b < T1) {
     y0 = L1 + slope1*t_b;
   } else {
     y0 = L2 + slope2*(t_b-T1);
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_triangle_3(Global &G,XbeUsr &X,XbeJac &J) {
   double y0,t0_new,delta_min,t_a,t_b,tnext_p;
   int n;
   double y;
   double frequency,L1,L2,t0,slope1,slope2,epsl,T1,T2,T;
   const int nvr_y = 0;
   const int nr_frequency = 0;
   const int nr_L1 = 1;
   const int nr_L2 = 2;
   const int nr_t0 = 3;
   const int nr_slope1 = 4;
   const int nr_slope2 = 5;
   const int nr_epsl = 6;
   const int nr_T1 = 7;
   const int nr_T2 = 8;
   const int nr_T = 9;
   const int no_y = 0;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     frequency = X.rprm[nr_frequency];
     T = 1.0/frequency;
     T1 = 0.5*T;
     T2 = T1;

     L1 = X.rprm[nr_L1];
     L2 = X.rprm[nr_L2];

     slope1 = (L2-L1)/T1;
     slope2 = (L1-L2)/T2;
     epsl = T/1000.0;

     X.rprm[nr_slope1] = slope1;
     X.rprm[nr_slope2] = slope2;
     X.rprm[nr_epsl] = epsl;

     X.rprm[nr_T ] = T;
     X.rprm[nr_T1] = T1;
     X.rprm[nr_T2] = T2;

     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_y] = X.val_vr[nvr_y];
     return;
   }

   t0   = X.rprm[nr_t0  ];
   T    = X.rprm[nr_T   ];
   T1   = X.rprm[nr_T1  ];
   epsl = X.rprm[nr_epsl];

   if (G.time_given_x < t0) {
     n = ((t0-G.time_given_x)/T) + 1;
     t0_new = t0-n*T;
   } else {
     t0_new = t0;
   }
   t_a = G.time_given_x-t0_new;
   t_b = fmod(t_a,T);

   if (abs(t_b-T) < epsl) t_b = 0.0;

   if (G.flags[G.i_next_time]) {
     if (t_b < T1) {
       tnext_p = T1;
     } else {
       tnext_p = T;
     }
     G.time_nextbreak_x = G.time_given_x + (tnext_p-t_b);
     return;
   }

   L1 = X.rprm[nr_L1];
   L2 = X.rprm[nr_L2];
   slope1 = X.rprm[nr_slope1];
   slope2 = X.rprm[nr_slope2];

   if (t_b < T1) {
     y0 = L1 + slope1*t_b;
   } else {
     y0 = L2 + slope2*(t_b-T1);
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_y] = y0;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y] = y0;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y] - y0;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y] = 1.0;
       }
     }
     return;
   }
   return;
}
void x_user_fn_1_1(Global &G,XbeUsr &X,XbeJac &J) {
// declare large enough size to serve other user_fn_x_x
// elements as well

   double time0;
   double x_uf[20];
   double y_uf[20];
   double x1;
   double y1;
   int iprm1,iprm2,index_fn;
   double rprm1,rprm2,rprm3,rprm4,rprm5,rprm6,rprm7,rprm8,rprm9,rprm10;
   const int nvr_x1 = 0;
   const int nvr_y1 = 1;
   const int ni_iprm1 = 0;
   const int ni_iprm2 = 1;
   const int ni_index_fn = 2;
   const int nr_rprm1 = 0;
   const int nr_rprm2 = 1;
   const int nr_rprm3 = 2;
   const int nr_rprm4 = 3;
   const int nr_rprm5 = 4;
   const int nr_rprm6 = 5;
   const int nr_rprm7 = 6;
   const int nr_rprm8 = 7;
   const int nr_rprm9 = 8;
   const int nr_rprm10 = 9;
   const int no_y1 = 0;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_y1] = X.val_vr[nvr_y1];
     return;
   }
   if (G.flags[G.i_init_guess]) {
     return;
   }
   time0 = G.time_given_x;
   index_fn = X.iprm[ni_index_fn];
   x_uf[0] = X.val_vr[nvr_x1];
   user_function(index_fn,time0,x_uf,y_uf,X.iprm,X.rprm);

   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y1] = y_uf[0];
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y1] - y_uf[0];
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y1] =  1.0;
       }
     }
     return;
   }
   return;
}
void x_user_fn_2_1(Global &G,XbeUsr &X,XbeJac &J) {
// declare large enough size to serve other user_fn_x_x
// elements as well

   double time0;
   double x_uf[20];
   double y_uf[20];
   double x1,x2;
   double y1;
   int iprm1,iprm2,index_fn;
   double rprm1,rprm2,rprm3,rprm4,rprm5,rprm6,rprm7,rprm8,rprm9,rprm10;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_y1 = 2;
   const int ni_iprm1 = 0;
   const int ni_iprm2 = 1;
   const int ni_index_fn = 2;
   const int nr_rprm1 = 0;
   const int nr_rprm2 = 1;
   const int nr_rprm3 = 2;
   const int nr_rprm4 = 3;
   const int nr_rprm5 = 4;
   const int nr_rprm6 = 5;
   const int nr_rprm7 = 6;
   const int nr_rprm8 = 7;
   const int nr_rprm9 = 8;
   const int nr_rprm10 = 9;
   const int no_y1 = 0;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_y1] = X.val_vr[nvr_y1];
     return;
   }
   if (G.flags[G.i_init_guess]) {
     return;
   }
   time0 = G.time_given_x;
   index_fn = X.iprm[ni_index_fn];
   x_uf[0] = X.val_vr[nvr_x1];
   x_uf[1] = X.val_vr[nvr_x2];
   user_function(index_fn,time0,x_uf,y_uf,X.iprm,X.rprm);

   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y1] = y_uf[0];
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y1] - y_uf[0];
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y1] =  1.0;
       }
     }
     return;
   }
   return;
}
void x_user_fn_3_1(Global &G,XbeUsr &X,XbeJac &J) {
// declare large enough size to serve other user_fn_x_x
// elements as well

   double time0;
   double x_uf[20];
   double y_uf[20];
   double x1,x2,x3;
   double y1;
   int iprm1,iprm2,index_fn;
   double rprm1,rprm2,rprm3,rprm4,rprm5,rprm6,rprm7,rprm8,rprm9,rprm10;
   const int nvr_x1 = 0;
   const int nvr_x2 = 1;
   const int nvr_x3 = 2;
   const int nvr_y1 = 3;
   const int ni_iprm1 = 0;
   const int ni_iprm2 = 1;
   const int ni_index_fn = 2;
   const int nr_rprm1 = 0;
   const int nr_rprm2 = 1;
   const int nr_rprm3 = 2;
   const int nr_rprm4 = 3;
   const int nr_rprm5 = 4;
   const int nr_rprm6 = 5;
   const int nr_rprm7 = 6;
   const int nr_rprm8 = 7;
   const int nr_rprm9 = 8;
   const int nr_rprm10 = 9;
   const int no_y1 = 0;
   const int ng_1 = 0;
   if (G.flags[G.i_one_time_parms]) {
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_y1] = X.val_vr[nvr_y1];
     return;
   }
   if (G.flags[G.i_init_guess]) {
     return;
   }
   time0 = G.time_given_x;
   index_fn = X.iprm[ni_index_fn];
   x_uf[0] = X.val_vr[nvr_x1];
   x_uf[1] = X.val_vr[nvr_x2];
   x_uf[2] = X.val_vr[nvr_x3];
   user_function(index_fn,time0,x_uf,y_uf,X.iprm,X.rprm);

   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_y1] = y_uf[0];
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_y1] - y_uf[0];
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_y1] =  1.0;
       }
     }
     return;
   }
   return;
}
void x_vsi_3ph_1(Global &G,XbeUsr &X,XbeJac &J) {
   double val_a,val_b,val_c;
   double g1,g2,g3,g4,g5,g6;
   double va,vb,vc;
   double vdc,L,Lby2;
   const int nvr_g1 = 0;
   const int nvr_g2 = 1;
   const int nvr_g3 = 2;
   const int nvr_g4 = 3;
   const int nvr_g5 = 4;
   const int nvr_g6 = 5;
   const int nvr_va = 6;
   const int nvr_vb = 7;
   const int nvr_vc = 8;
   const int nr_vdc = 0;
   const int nr_L = 1;
   const int nr_Lby2 = 2;
   const int no_va = 0;
   const int no_vb = 1;
   const int no_vc = 2;
   const int no_g1 = 3;
   const int no_g2 = 4;
   const int no_g3 = 5;
   const int no_g4 = 6;
   const int no_g5 = 7;
   const int no_g6 = 8;
   const int ng_1 = 0;
   const int ng_2 = 1;
   const int ng_3 = 2;
   if (G.flags[G.i_one_time_parms]) {
     L = X.rprm[nr_L];
     Lby2 = 0.5*L;
     X.rprm[nr_Lby2] = Lby2;
     return;
   }

   g1 = X.val_vr[nvr_g1];
   g2 = X.val_vr[nvr_g2];
   g3 = X.val_vr[nvr_g3];
   g4 = X.val_vr[nvr_g4];
   g5 = X.val_vr[nvr_g5];
   g6 = X.val_vr[nvr_g6];

   vdc = X.rprm[nr_vdc];
   Lby2 = X.rprm[nr_Lby2];

   if (g1 > Lby2) {
     if (g4 > Lby2) {
       val_a = 0.5*vdc;
     } else {
       val_a = vdc;
     }
   } else {
     if (g4 > Lby2) {
       val_a = 0.0;
     } else {
       val_a = 0.5*vdc;
     }
   }
   if (g3 > Lby2) {
     if (g6 > Lby2) {
       val_b = 0.5*vdc;
     } else {
       val_b = vdc;
     }
   } else {
     if (g6 > Lby2) {
       val_b = 0.0;
     } else {
       val_b = 0.5*vdc;
     }
   }
   if (g5 > Lby2) {
     if (g2 > Lby2) {
       val_c = 0.5*vdc;
     } else {
       val_c = vdc;
     }
   } else {
     if (g2 > Lby2) {
       val_c = 0.0;
     } else {
       val_c = 0.5*vdc;
     }
   }

   if (G.flags[G.i_init_guess]) {
     X.val_vr[nvr_va] = val_a;
     X.val_vr[nvr_vb] = val_b;
     X.val_vr[nvr_vc] = val_c;
     return;
   }
   if (G.flags[G.i_trns] || G.flags[G.i_startup]) {
     if (G.flags[G.i_explicit]) {
       X.val_vr[nvr_va] = val_a;
       X.val_vr[nvr_vb] = val_b;
       X.val_vr[nvr_vc] = val_c;
     } else if (G.flags[G.i_implicit]) {
       if (G.flags[G.i_function]) {
         X.g[ng_1] = X.val_vr[nvr_va] - val_a;
         X.g[ng_2] = X.val_vr[nvr_vb] - val_b;
         X.g[ng_3] = X.val_vr[nvr_vc] - val_c;
       }
       if (G.flags[G.i_jacobian]) {
         J.dgdvr[ng_1][nvr_va] = 1.0;
         J.dgdvr[ng_2][nvr_vb] = 1.0;
         J.dgdvr[ng_3][nvr_vc] = 1.0;
       }
     }
     return;
   }
   if (G.flags[G.i_outvar]) {
     X.outprm[no_va] = X.val_vr[nvr_va];
     X.outprm[no_vb] = X.val_vr[nvr_vb];
     X.outprm[no_vc] = X.val_vr[nvr_vc];
     X.outprm[no_g1] = X.val_vr[nvr_g1];
     X.outprm[no_g2] = X.val_vr[nvr_g2];
     X.outprm[no_g3] = X.val_vr[nvr_g3];
     X.outprm[no_g4] = X.val_vr[nvr_g4];
     X.outprm[no_g5] = X.val_vr[nvr_g5];
     X.outprm[no_g6] = X.val_vr[nvr_g6];
     return;
   }
   return;
}
