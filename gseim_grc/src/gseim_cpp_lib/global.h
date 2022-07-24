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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <math.h>

using std::vector;

using namespace std;

class Global {

  public:

// integer global constants:

   const int I_CLEAR = -1;
   const int I_NONE  = -1;

   const int I_XVR   = 1;
   const int I_XAUX  = 2;

   const int I_NV    = 3;
   const int I_ESTV  = 4;
   const int I_EAUX  = 5;
   const int I_EAUXS = 6;
   const int I_NDCUR = 7;

   const int I_EBE_KCL        = 1;
   const int I_EBE_NONKCL     = 2;
   const int I_EBE_STV        = 3;
   const int I_EBE_DDT_NONKCL = 4;
   const int I_EBE_DDT_STV    = 5;
   const int I_GBE            = 6;
   const int I_GBE_DDT        = 7;

   const int I_OV_XBE   = 1;
   const int I_OV_XVR   = 3;

   const int I_OV_EBE   = 2;
   const int I_OV_NODEV = 4;

   const int I_TRNS    = 1;
   const int I_STARTUP = 2;
   const int I_DC      = 3;
   const int I_SSW     = 4;

   const int I_LINES_LMT = 100000;

   const int I_INTEGRATE   = 1;
   const int I_EVAL_SRC    = 2;
   const int I_EVAL_NONSRC = 3;
   const int I_DELAY       = 4;

   const int I_RKF45_LTE  = 1;
   const int I_RKF45_SVEC = 2;
   const int I_RKF45_NORM = 3;

   const int I_BS23_LTE  = 4;
   const int I_BS23_SVEC = 5;
   const int I_BS23_NORM = 6;

   const int I_NO_LU_BSUB = 1;
   const int I_LU         = 2;
   const int I_BSUB       = 3;
   const int I_LU_BSUB    = 4;

   const int I_NO_GAUSS1  = 1;
   const int I_GAUSS1     = 2;
   const int I_GAUSS1A    = 3;

   const int I_NO_RHS_MTOW = 1;
   const int I_RHS_MTOW    = 2;

   const int I_NO_MAT_MTOW = 1;
   const int I_MAT_MTOW    = 2;

   const int I_NO_GAUSS2  = 1;
   const int I_GAUSS2     = 2;

   const int I_SYNC_X_E    = 1;
   const int I_SYNC_X_SLOW = 2;
   const int I_SYNC_E_SLOW = 3;

   const int I_COPY_0_TO_1 = 1;
   const int I_COPY_1_TO_0 = 2;
   const int I_COPY_1_TO_2 = 3;
   const int I_COPY_2_TO_1 = 4;
   const int I_COPY_2_TO_0 = 5;

// double global constants:

   double pi,twopi,deg_to_rad,rad_to_deg;
   double gmin0;

// flags for passing on to element routines, etc.
   vector<bool> flags;

   int flags_n;

// indices for flags:
   int i_init_guess,i_dc,i_startup,i_trns,i_ssw,i_one_time_parms,
       i_save_history,i_next_time,i_update_states,i_time_parms;
   int i_evaluate,i_reset_x;
   int i_outvar;
   int i_explicit,i_implicit,i_alg_loop;
   int i_function,i_jacobian;
   int i_limit_newton;
   int i_slv_init,i_slv_readfile,i_slv_previous;

// globally used variables:
   double time_given_x,time_nextbreak_x;
   double time_given_e,time_nextbreak_e;
   int iter_trns_x,iter_trns_e;
   double time_begin,time_end;
   bool flag_limit_tstep;

// indices for method flags:
   int i_feuler,i_rk4,i_rkf45,i_bs23,i_meuler,i_heun;
   int i_be,i_trz,i_trbdf2;
   int i_be_auto,i_trz_auto;
   int i_be_const,i_trz_const;

// size of method flags:
   int method_flags_n;

// flag to specify if a method is explicit
   vector<bool> flag_exp;

// variables for sampler elements:
   int sampler_index_max;
   int n_samplers_max;
   vector<int> sampler_flag;

  public:
   Global();

   std::string var_flag_string(
    const int k);

};
#endif
