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

Global::Global() {

   int a;

   pi = acos(-1.0);
   twopi = pi + pi;
   deg_to_rad = pi/180.0;
   rad_to_deg = 180.0/pi;

   gmin0 = 1.0e-12;

   a = 0; flags.clear();

   i_init_guess     = a; flags.push_back(false); a++;
   i_dc             = a; flags.push_back(false); a++;
   i_startup        = a; flags.push_back(false); a++;
   i_trns           = a; flags.push_back(false); a++;
   i_ssw            = a; flags.push_back(false); a++;
   i_one_time_parms = a; flags.push_back(false); a++;
   i_time_parms     = a; flags.push_back(false); a++;
   i_save_history   = a; flags.push_back(false); a++;
   i_next_time      = a; flags.push_back(false); a++;
   i_update_states  = a; flags.push_back(false); a++;
   i_evaluate       = a; flags.push_back(false); a++;
   i_reset_x        = a; flags.push_back(false); a++;
   i_outvar         = a; flags.push_back(false); a++;
   i_explicit       = a; flags.push_back(false); a++;
   i_implicit       = a; flags.push_back(false); a++;
   i_alg_loop       = a; flags.push_back(false); a++;
   i_function       = a; flags.push_back(false); a++;
   i_jacobian       = a; flags.push_back(false); a++;
   i_limit_newton   = a; flags.push_back(false); a++;
   i_slv_init       = a; flags.push_back(false); a++;
   i_slv_readfile   = a; flags.push_back(false); a++;
   i_slv_previous   = a; flags.push_back(false); a++;

   flags_n = flags.size();

// method flags (indices):
   a = 0;

   i_feuler    = a; a++;
   i_rk4       = a; a++;
   i_rkf45     = a; a++;
   i_bs23      = a; a++;
   i_meuler    = a; a++;
   i_heun      = a; a++;
   i_be        = a; a++;
   i_trz       = a; a++;
   i_trbdf2    = a; a++;
   i_be_auto   = a; a++;
   i_trz_auto  = a; a++;
   i_be_const  = a; a++;
   i_trz_const = a; a++;

   method_flags_n = a;

   flag_exp.resize(method_flags_n);

   flag_exp[i_feuler   ] = true;
   flag_exp[i_rk4      ] = true;
   flag_exp[i_rkf45    ] = true;
   flag_exp[i_bs23     ] = true;
   flag_exp[i_meuler   ] = true;
   flag_exp[i_heun     ] = true;
   flag_exp[i_be       ] = false;
   flag_exp[i_trz      ] = false;
   flag_exp[i_trbdf2   ] = false;
   flag_exp[i_be_auto  ] = false;
   flag_exp[i_trz_auto ] = false;
   flag_exp[i_be_const ] = false;
   flag_exp[i_trz_const] = false;

   n_samplers_max = 20;
   sampler_index_max = 20;
   sampler_flag.resize(sampler_index_max + 1);

   return;
}
// -----------------------------------------------------------------------------
std::string Global::var_flag_string(
   const int k) {

   std::string s;

   if (k == I_XVR) {
     s = "I_XVR";
   } else if (k == I_XAUX) {
     s = "I_XAUX";
   } else if (k == I_NV) {
     s = "I_NV";
   } else if (k == I_ESTV) {
     s = "I_ESTV";
   } else if (k == I_EAUX) {
     s = "I_EAUX";
   } else if (k == I_EAUXS) {
     s = "I_EAUXS";
   } else if (k == I_NDCUR) {
     s = "I_NDCUR";
   } else {
     s = "NOT_FOUND";
   }

   return s;
}
