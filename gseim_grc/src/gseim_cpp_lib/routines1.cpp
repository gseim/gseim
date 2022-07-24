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

#include "routines1.h"

// -----------------------------------------------------------------------------
void write_trns(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int n_var,i_ov;
   bool flag_write;

   if (cct_file.n_ov_ebe > 0) {
     ebe_ov_prm(ebe_lib,ebe_usr,ebe_jac,cct,cct_file,global);
   }
   if (cct_file.n_ov_xbe > 0) {
     xbe_ov_prm(xbe_lib,xbe_usr,xbe_jac,cct,cct_file,global);
   }

   for (int i_file=0; i_file < slv.n_outfile; i_file++) {
     if (slv.flag_solution[i_file]) continue;

     flag_write = false;

     if (slv.time_write <= slv.out_tend[i_file]) {
       if (slv.flag_out_delt_fixed[i_file]) {
         if (slv.time_write >= slv.out_tnext[i_file]) {
           flag_write = true;
           slv.out_tnext[i_file] = slv.time_write + slv.out_delt[i_file];
         }
       } else {
         if (slv.time_write >= slv.out_tstart[i_file]) {
           flag_write = true;
         }
       }
     }

     if (flag_write) {
       slv.total_lines[i_file]++;
       if (slv.total_lines[i_file] > slv.limit_lines[i_file]) {
         cout << "write_trns: limit_lines is exceeded." << endl;
         cout << "  index_solve = " << slv.index_solve
              << ", i_file = " << i_file << endl;
         cout << "  Halting..." << endl; exit(1);
       }
       n_var = slv.out_nvar[i_file];

       for (int i_var=0; i_var < n_var; i_var++) {
         i_ov = slv.out_var[i_file][i_var];
         assign_ov_prm(i_ov,i_file,i_var,xbe_usr,ebe_usr,
           cct,slv,cct_file,global);
       }
       slv.f_output[i_file]
         << setw(slv.outf_real_word_width)
         << slv.time_write << " ";
       for (int i_var=0; i_var < n_var; i_var++) {
         slv.f_output[i_file]
           << setw(slv.outf_real_word_width)
           << slv.outvar_temp[i_file][i_var] << " ";
       }
       slv.f_output[i_file] << endl;
     }
   }
   return;
} // end of write_trns
// -----------------------------------------------------------------------------
void write_dc_startup(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int n_var,i_ov;

   if (cct_file.n_ov_ebe > 0) {
     ebe_ov_prm(ebe_lib,ebe_usr,ebe_jac,cct,cct_file,global);
   }
   if (cct_file.n_ov_xbe > 0) {
     xbe_ov_prm(xbe_lib,xbe_usr,xbe_jac,cct,cct_file,global);
   }

   for (int i_file=0; i_file < slv.n_outfile; i_file++) {
     if (slv.flag_solution[i_file]) continue;
     n_var = slv.out_nvar[i_file];

     for (int i_var=0; i_var < n_var; i_var++) {
       i_ov = slv.out_var[i_file][i_var];

       assign_ov_prm(i_ov,i_file,i_var,xbe_usr,ebe_usr,
         cct,slv,cct_file,global);
     }
     for (int i_var=0; i_var < n_var; i_var++) {
       slv.f_output[i_file]
         << setw(slv.outf_real_word_width)
         << slv.outvar_temp[i_file][i_var] << " ";
     }
     slv.f_output[i_file] << endl;
   }
   return;
} // end of write_dc_startup
// -----------------------------------------------------------------------------
void solve_trns_x_exp(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   CctFile &cct_file,
   SysMat &smat,
   Global &global) {

   assign_const_1<bool>(global.flags,false);

   init_sol_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,slv,cct,smat,global);

   if (cct.flag_alg_loop) {
     smat.mat_startup_1_x(xbe_lib,xbe_usr,cct,cct_file);

     smat.w_x.allocate_1(smat.m_x.n_nz,smat.m_x.n_row,smat.m_x.n_col);
     smat.mo_x.allocate_1(smat.m_x.n_row);
   }

   if (slv.x_algo_feuler) {
     solve_trns_x_feuler(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       slv,cct,cct_file,smat,global);
   } else if (slv.x_algo_rk4) {
     solve_trns_x_rk4(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       slv,cct,cct_file,smat,global);
   } else if (slv.x_algo_rkf45) {
     solve_trns_x_rkf45(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       slv,cct,cct_file,smat,global);
   } else if (slv.x_algo_bs23) {
     solve_trns_x_bs23(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       slv,cct,cct_file,smat,global);
   } else if (slv.x_algo_meuler) {
     solve_trns_x_meuler(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       slv,cct,cct_file,smat,global);
   } else if (slv.x_algo_heun) {
     solve_trns_x_heun(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       slv,cct,cct_file,smat,global);
   } else {
     cout << "solve_trns_x_exp:" << endl;
     cout << "  wrong trns method. Halting..." << endl;
     exit(1);
   }

// clear flags once again
   assign_const_1<bool>(global.flags,false);

   return;
} // end of solve_trns_x_exp
// -----------------------------------------------------------------------------
void solve_trns_x_feuler(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   CctFile &cct_file,
   SysMat &smat,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached;

   cout << "solve_trns_x_feuler starts..." << endl;
   solve_trns_x_common(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     slv,cct,cct_file,global);

   flag_tend_reached = false;
   slv.iter_trns_x = -1;
   global.iter_trns_x = slv.iter_trns_x;

   while (!flag_tend_reached) {
     slv.iter_trns_x++;
     global.iter_trns_x = slv.iter_trns_x;
     write_iter_x(slv);

     if (slv.iter_trns_x != 0) {
       if (cct.flag_modulo_x) {
         xbe_modulo(cct,xbe_usr);
       }
     }

     slv.time_next_x  = slv.time_present_x + slv.delt_x;
     global.time_given_x = slv.time_next_x;

     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

//   evaluate/integrate and update:

     if (cct.flag_alg_loop) {
       xbe_feuler_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         cct,slv,smat,global);
     } else {
       xbe_feuler(xbe_lib,xbe_usr,xbe_jac,cct,slv,global);
     }

//   xelements using reset:
     if (cct.flag_reset_x) {
       xbe_reset_1(cct.flag_alg_loop,xbe_lib,xbe_usr,xbe_jac,cct,smat,global);
     }

     slv.time_present_x = slv.time_next_x;
     slv.time_write     = slv.time_next_x;

     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);

//   find the new time step if necessary

     slv.delt_x = slv.delt0_x;
     slv.delt_x = max(slv.delt_x,slv.delt_min_x);

     if (cct.flag_limit_tstep_x) {
       get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
     }
     time_next_1 = slv.time_present_x + slv.delt_x;
     if (time_next_1 >= global.time_end) {
       slv.delt_x = global.time_end - slv.time_present_x + slv.delt_small;
       slv.delt_x = max(slv.delt_x,slv.delt_min_x);
     }
     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_x > slv.itmax_trns) {
         cout << "solve_trns_x_feuler: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_x+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   cout << "solve_trns_x_feuler ends" << endl;
   return;
} // end of solve_trns_x_feuler
// -----------------------------------------------------------------------------
void solve_trns_x_rk4(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   CctFile &cct_file,
   SysMat &smat,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached;

   solve_trns_x_common(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     slv,cct,cct_file,global);

   cout << "solve_trns_x_rk4 starting..." << endl;
   cout << "solve_trns_x_rk4: slv.itmax_trns = " << slv.itmax_trns << endl;

   flag_tend_reached = false;
   slv.iter_trns_x = -1;
   global.iter_trns_x = slv.iter_trns_x;

   while (!flag_tend_reached) {
     slv.iter_trns_x++;
     global.iter_trns_x = slv.iter_trns_x;
     write_iter_x(slv);

     if (slv.iter_trns_x != 0) {
       if (cct.flag_modulo_x) {
         xbe_modulo(cct,xbe_usr);
       }
     }
     slv.time_next_x  = slv.time_present_x + slv.delt_x;

//   evaluate/integrate and update:

     if (cct.flag_alg_loop) {
       xbe_rk4_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         cct,slv,smat,global);
     } else {
       xbe_rk4(xbe_lib,xbe_usr,xbe_jac,cct,slv,global);
     }

     if (cct.flag_reset_x) {
       xbe_reset_1(false,xbe_lib,xbe_usr,xbe_jac,cct,smat,global);
     }

     slv.time_present_x = slv.time_next_x;
     slv.time_write     = slv.time_next_x;

     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);

//   find the new time step if necessary

     slv.delt_x = slv.delt0_x;
     slv.delt_x = max(slv.delt_x,slv.delt_min_x);

     if (cct.flag_limit_tstep_x) {
       get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
     }
     time_next_1 = slv.time_present_x + slv.delt_x;
     if (time_next_1 >= global.time_end) {
       slv.delt_x = global.time_end - slv.time_present_x + slv.delt_small;
       slv.delt_x = max(slv.delt_x,slv.delt_min_x);
     }

     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_x > slv.itmax_trns) {
         cout << "solve_trns_x_rk4: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_x+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   cout << "solve_trns_x_rk4 ends" << endl;

   return;
} // end of solve_trns_x_rk4
// -----------------------------------------------------------------------------
void solve_trns_x_rkf45(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   CctFile &cct_file,
   SysMat &smat,
   Global &global) {

   double time_next_1,t_n,delta;
   bool flag_tend_reached;

   cout << "solve_trns_x_rkf45 starts..." << endl;

   solve_trns_x_common(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     slv,cct,cct_file,global);

   slv.rkf45_n_accept = 0;
   slv.rkf45_n_reject = 0;

   xbeu_copy_1(xbe_lib,xbe_usr,cct);

   flag_tend_reached = false;
   slv.iter_trns_x = -1;
   global.iter_trns_x = slv.iter_trns_x;

   while (!flag_tend_reached) {
     slv.iter_trns_x++;
     global.iter_trns_x = slv.iter_trns_x;
     write_iter_x(slv);

     if (slv.iter_trns_x != 0) {
       if (cct.flag_modulo_x) {
         xbe_modulo(cct,xbe_usr);
       }
     }
     if (cct.flag_alg_loop) {
       xbe_rkf45_al(global.I_RKF45_LTE ,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         cct,slv,smat,global);
       xbe_rkf45_al(global.I_RKF45_NORM,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         cct,slv,smat,global);
     } else {
       xbe_rkf45(global.I_RKF45_LTE ,xbe_lib,xbe_usr,xbe_jac,cct,slv,global);
       xbe_rkf45(global.I_RKF45_NORM,xbe_lib,xbe_usr,xbe_jac,cct,slv,global);
     }

//   If LTE is not within tolerance, change delt_x;
//   If LTE is within tolerance, accept the time step.

     if (slv.rkf45_norm2 <= slv.rkf45_tolr) {
       slv.rkf45_n_accept++;
     } else {
       slv.rkf45_n_reject++;
       if (slv.delt_x == slv.delt_min_x) {
         cout << "solve_trns_x_rkf45:" << endl;
         cout << "  tolerance not met even with" << endl;
         cout << "  the smallest time step." << endl;
         cout << "  rkf45_norm2=" << slv.rkf45_norm2
           << ",  rkf45_tolr=" << slv.rkf45_tolr << endl;
         cout << "  delt_x=" << slv.delt_x
           << ",  delt_min_x=" << slv.delt_min_x << endl;
         cout << "  Halting..." << endl;
         exit(1);
       }
     }

     if (slv.rkf45_norm2 <= slv.rkf45_tolr) {
       t_n = slv.time_present_x + slv.delt_x;
       slv.time_present_x = t_n;
       slv.time_write     = t_n;

//     compute solution with the RK4 part of RKF45:

       if (cct.flag_alg_loop) {
         xbe_rkf45_al(global.I_RKF45_SVEC,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
           cct,slv,smat,global);
       } else {
         xbe_rkf45(global.I_RKF45_SVEC,xbe_lib,xbe_usr,xbe_jac,cct,slv,global);
       }

//     If the solution is accepted, treat intgrtr_reset kind of elements.

       if (cct.flag_reset_x) {
         xbe_reset_1(false,xbe_lib,xbe_usr,xbe_jac,cct,smat,global);
       }
       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);
       xbeu_copy_1(xbe_lib,xbe_usr,cct);
     }

//   compute the next time step (whether or not LTE is within tolerance)

//   this is used to control LTE, rather than LTE/h
     delta = 0.87*pow((slv.rkf45_tolr/slv.rkf45_norm2),0.2);

     if (delta < slv.rkf45_fctr_min) {
       slv.delt_x = slv.rkf45_fctr_min*slv.delt_x;
     } else if (delta >= slv.rkf45_fctr_max) {
       slv.delt_x = slv.rkf45_fctr_max*slv.delt_x;
     } else {
       slv.delt_x= delta*slv.delt_x;
     }

//   limit the time step:

     slv.delt_x = min(slv.delt_x,slv.delt_max_x);
     slv.delt_x = max(slv.delt_x,slv.delt_min_x);

//   check if the time step should be limited by elements:

     if (slv.delt_x > slv.delt_min_x) {
       if (cct.flag_limit_tstep_x) {
         get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
       }
     }
     time_next_1 = slv.time_present_x + slv.delt_x;
     if (time_next_1 >= global.time_end) {
       slv.delt_x = global.time_end - slv.time_present_x + slv.delt_small;
       slv.delt_x = max(slv.delt_x,slv.delt_min_x);
     }
     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_x > slv.itmax_trns) {
         cout << "solve_trns_x_rkf45: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_x+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   cout << "solve_trns_x_rkf45: rkf45_n_accept="
     << slv.rkf45_n_accept << endl;
   cout << "solve_trns_x_rkf45: rkf45_n_reject="
     << slv.rkf45_n_reject << endl;

   cout << "solve_trns_x_rkf45 ends" << endl;

   return;
} // end of solve_trns_x_rkf45
// -----------------------------------------------------------------------------
void solve_trns_x_bs23(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   CctFile &cct_file,
   SysMat &smat,
   Global &global) {

   double t_n,time_next_1,delta;
   bool flag_tend_reached;

   solve_trns_x_common(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     slv,cct,cct_file,global);

   slv.bs23_n_accept = 0;
   slv.bs23_n_reject = 0;

   xbeu_copy_1(xbe_lib,xbe_usr,cct);

   flag_tend_reached = false;
   slv.iter_trns_x = -1;
   global.iter_trns_x = slv.iter_trns_x;

   while (!flag_tend_reached) {
     slv.iter_trns_x++;
     global.iter_trns_x = slv.iter_trns_x;
     write_iter_x(slv);

     if (slv.iter_trns_x != 0) {
       if (cct.flag_modulo_x) {
         xbe_modulo(cct,xbe_usr);
       }
     }
//   compute local error estimate (denote it by slv.bs23_norm2)

     if (cct.flag_alg_loop) {
       xbe_bs23_al(global.I_BS23_LTE ,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         cct,slv,smat,global);
       xbe_bs23_al(global.I_BS23_NORM,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         cct,slv,smat,global);
     } else {
       xbe_bs23(global.I_BS23_LTE ,xbe_lib,xbe_usr,xbe_jac,cct,slv,global);
       xbe_bs23(global.I_BS23_NORM,xbe_lib,xbe_usr,xbe_jac,cct,slv,global);
     }

//   If LTE is not within tolerance, change delt_x;
//   If LTE is within tolerance, accept the time step.

     if (slv.bs23_norm2 <= slv.bs23_tolr) {
       slv.bs23_n_accept++;
     } else {
       slv.bs23_n_reject++;
       if (slv.delt_x == slv.delt_min_x) {
         cout << "solve_trns_x_bs23:" << endl;
         cout << "  tolerance not met even with" << endl;
         cout << "  the smallest time step." << endl;
         cout << "  bs23_norm2=" << slv.bs23_norm2
           << ",  bs23_tolr=" << slv.bs23_tolr << endl;
         cout << "  delt_x=" << slv.delt_x
           << ",  delt_min_x=" << slv.delt_min_x << endl;
         cout << "  Halting..." << endl;
         exit(1);
       }
     }

     if (slv.bs23_norm2 <= slv.bs23_tolr) {
       t_n = slv.time_present_x + slv.delt_x;
       slv.time_write     = t_n;
       slv.time_present_x = t_n;

//     compute solution with the 2nd order part of bs23:

       if (cct.flag_alg_loop) {
         xbe_bs23_al(global.I_BS23_SVEC,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
           cct,slv,smat,global);
       } else {
         xbe_bs23(global.I_BS23_SVEC,xbe_lib,xbe_usr,xbe_jac,cct,slv,global);
       }

       if (cct.flag_reset_x) {
         xbe_reset_1(false,xbe_lib,xbe_usr,xbe_jac,cct,smat,global);
       }

       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);

       xbeu_copy_1(xbe_lib,xbe_usr,cct);
     }
//   compute the next time step (whether or not LTE is within tolerance)
//   (1/2)**0.5 = 0.7071
//   (1/2)**0.333 = 0.7937

//   this is used to control LTE, rather than LTE/h
     delta = 0.7937*pow((slv.bs23_tolr/slv.bs23_norm2),0.3333333);
  
     if (delta < slv.bs23_fctr_min) {
       slv.delt_x = slv.bs23_fctr_min*slv.delt_x;
     } else if (delta >= slv.bs23_fctr_max) {
       slv.delt_x = slv.bs23_fctr_max*slv.delt_x;
     } else {
       slv.delt_x= delta*slv.delt_x;
     }

//   limit the time step:

     slv.delt_x = min(slv.delt_x,slv.delt_max_x);
     slv.delt_x = max(slv.delt_x,slv.delt_min_x);

//   check if the time step should be limited by elements:

     if (slv.delt_x > slv.delt_min_x) {
       if (cct.flag_limit_tstep_x) {
         get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
       }
     }
     time_next_1 = slv.time_present_x + slv.delt_x;
     if (time_next_1 >= global.time_end) {
       slv.delt_x = global.time_end - slv.time_present_x + slv.delt_small;
       slv.delt_x = max(slv.delt_x,slv.delt_min_x);
     }
     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_x > slv.itmax_trns) {
         cout << "solve_trns_x_bs23: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_x+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   cout << "solve_trns_x_bs23: bs23_n_accept="
     << slv.bs23_n_accept << endl;
   cout << "solve_trns_x_bs23: bs23_n_reject="
     << slv.bs23_n_reject << endl;

   cout << "solve_trns_x_bs23 ends" << endl;

   return;
} // end of solve_trns_x_bs23
// -----------------------------------------------------------------------------
void solve_trns_x_meuler(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   CctFile &cct_file,
   SysMat &smat,
   Global &global) {

   double t_n,time_next_1;
   bool flag_tend_reached;

   cout << "solve_trns_x_meuler starts..." << endl;

   solve_trns_x_common(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     slv,cct,cct_file,global);

   flag_tend_reached = false;
   slv.iter_trns_x = -1;
   global.iter_trns_x = slv.iter_trns_x;

   while (!flag_tend_reached) {
     slv.iter_trns_x++;
     global.iter_trns_x = slv.iter_trns_x;
     write_iter_x(slv);

     if (slv.iter_trns_x != 0) {
       if (cct.flag_modulo_x) {
         xbe_modulo(cct,xbe_usr);
       }
     }
     t_n = slv.time_present_x + slv.delt_x;

//   evaluate/integrate and update:

     if (cct.flag_alg_loop) {
       xbe_meuler_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         cct,slv,smat,global);
     } else {
       xbe_meuler(xbe_lib,xbe_usr,xbe_jac,cct,slv,global);
     }

     if (cct.flag_reset_x) {
       xbe_reset_1(false,xbe_lib,xbe_usr,xbe_jac,cct,smat,global);
     }

     slv.time_present_x = t_n;
     slv.time_write     = t_n;

     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);

//   find the new time step if necessary

     slv.delt_x = slv.delt0_x;
     slv.delt_x = max(slv.delt_x,slv.delt_min_x);

     if (cct.flag_limit_tstep_x) {
       get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
     }
     time_next_1 = slv.time_present_x + slv.delt_x;
     if (time_next_1 >= global.time_end) {
       slv.delt_x = global.time_end - slv.time_present_x + slv.delt_small;
       slv.delt_x = max(slv.delt_x,slv.delt_min_x);
     }
     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_x > slv.itmax_trns) {
         cout << "solve_trns_x_meuler: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_x+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   cout << "solve_trns_x_meuler ends" << endl;

   return;
} // end of solve_trns_x_meuler
// -----------------------------------------------------------------------------
void solve_trns_x_heun(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   CctFile &cct_file,
   SysMat &smat,
   Global &global) {

   double t_n,time_next_1;
   bool flag_tend_reached;

   cout << "solve_trns_x_heun starts..." << endl;

   solve_trns_x_common(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     slv,cct,cct_file,global);

   flag_tend_reached = false;
   slv.iter_trns_x = -1;
   global.iter_trns_x = slv.iter_trns_x;

   while (!flag_tend_reached) {
     slv.iter_trns_x++;
     global.iter_trns_x = slv.iter_trns_x;
     write_iter_x(slv);

     if (slv.iter_trns_x != 0) {
       if (cct.flag_modulo_x) {
         xbe_modulo(cct,xbe_usr);
       }
     }
     t_n  = slv.time_present_x + slv.delt_x;

//   evaluate/integrate and update:
     if (cct.flag_alg_loop) {
       xbe_heun_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         cct,slv,smat,global);
     } else {
       xbe_heun(xbe_lib,xbe_usr,xbe_jac,cct,slv,global);
     }

     if (cct.flag_reset_x) {
       xbe_reset_1(false,xbe_lib,xbe_usr,xbe_jac,cct,smat,global);
     }

     slv.time_present_x = t_n;
     slv.time_write     = t_n;

     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);

//   find the new time step if necessary

     slv.delt_x = slv.delt0_x;
     slv.delt_x = max(slv.delt_x,slv.delt_min_x);

     if (cct.flag_limit_tstep_x) {
       get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
     }
     time_next_1 = slv.time_present_x + slv.delt_x;
     if (time_next_1 >= global.time_end) {
       slv.delt_x = global.time_end - slv.time_present_x + slv.delt_small;
       slv.delt_x = max(slv.delt_x,slv.delt_min_x);
     }
     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_x > slv.itmax_trns) {
         cout << "solve_trns_x_heun: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_x+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   cout << "solve_trns_x_heun ends" << endl;

   return;
} // end of solve_trns_x_heun
// -----------------------------------------------------------------------------
void xbe_feuler(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_xbeu,i_xbel,n_f1;
   int var_flag,var_number;

   xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           xbe_usr[i_xbeu].val_vr[var_number] +=
             slv.delt_x*xbe_usr[i_xbeu].f[i_func];
         } else if (var_flag == global.I_XAUX) {
           xbe_usr[i_xbeu].val_aux[var_number] +=
             slv.delt_x*xbe_usr[i_xbeu].f[i_func];
         } else {
           cout << "xbe_feuler: var_flag = " << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }
   xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

   return;
} // end of xbe_feuler
// -----------------------------------------------------------------------------
void xbe_feuler_al(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   SysMat &smat,
   Global &global) {

   int i_xbeu,i_xbel,n_f1;
   int var_flag,var_number;

// evaluate ddt elements
   xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           xbe_usr[i_xbeu].val_vr[var_number] +=
             slv.delt_x*xbe_usr[i_xbeu].f[i_func];
           xbe_usr[i_xbeu].val_vr_u[var_number] =
           xbe_usr[i_xbeu].val_vr  [var_number];
         } else if (var_flag == global.I_XAUX) {
           xbe_usr[i_xbeu].val_aux[var_number] +=
             slv.delt_x*xbe_usr[i_xbeu].f[i_func];
           xbe_usr[i_xbeu].val_aux_u[var_number] =
           xbe_usr[i_xbeu].val_aux  [var_number];
         } else {
           cout << "xbe_feuler_al: var_flag = " << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }

   if (cct.flag_linear_x) {
     solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
   } else {
     solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
       smat,cct,slv,global);
   }
   return;
} // end of xbe_feuler_al
// -----------------------------------------------------------------------------
void xbe_rk4(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   double h;

   h = slv.delt_x;

// copy to old
   xbeu_copy_1(xbe_lib,xbe_usr,cct);

// RK4: stage 1

   global.time_given_x = slv.time_present_x;
   if (flag_nan(slv.time_present_x)) {
     cout << "xbe_rk4: slv.time_present_x is NAN. Halting..." << endl;
     exit(1);
   }
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }

   xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(0,xbe_lib,xbe_usr,cct);

   update_rk4(1,(0.5*h),xbe_lib,xbe_usr,cct,global);

// RK4: stage 2
   global.time_given_x = slv.time_present_x + 0.5*h;
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }

   xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(1,xbe_lib,xbe_usr,cct);

   update_rk4(2,(0.5*h),xbe_lib,xbe_usr,cct,global);

// RK4: stage 3

   xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(2,xbe_lib,xbe_usr,cct);

   update_rk4(3,h,xbe_lib,xbe_usr,cct,global);

   global.time_given_x = slv.time_present_x + h;
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }

   xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(3,xbe_lib,xbe_usr,cct);

   update_rk4(4,(h/6.0),xbe_lib,xbe_usr,cct,global);

// final update of non-integrator variables

// RK4: stage 4
   xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

   return;
} // end of xbe_rk4
// -----------------------------------------------------------------------------
void xbe_rk4_al(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   SysMat &smat,
   Global &global) {

   double h;

   h = slv.delt_x;

// copy to old
   xbeu_copy_1(xbe_lib,xbe_usr,cct);

// RK4: stage 1

   global.time_given_x = slv.time_present_x;
   if (flag_nan(slv.time_present_x)) {
     cout << "xbe_rk4_al: slv.time_present_x is NAN. Halting..." << endl;
     exit(1);
   }
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }
   xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(0,xbe_lib,xbe_usr,cct);

   update_rk4_al(1,(0.5*h),xbe_lib,xbe_usr,cct,global);

// RK4: stage 2
   global.time_given_x = slv.time_present_x + 0.5*h;
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }

   if (cct.flag_linear_x) {
     solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
   } else {
     solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
       smat,cct,slv,global);
   }

   xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(1,xbe_lib,xbe_usr,cct);

   update_rk4_al(2,(0.5*h),xbe_lib,xbe_usr,cct,global);

// RK4: stage 3

   if (cct.flag_linear_x) {
     solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
   } else {
     solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
       smat,cct,slv,global);
   }

   xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(2,xbe_lib,xbe_usr,cct);

   update_rk4_al(3,h,xbe_lib,xbe_usr,cct,global);

   global.time_given_x = slv.time_present_x + h;
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }

   if (cct.flag_linear_x) {
     solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
   } else {
     solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
       smat,cct,slv,global);
   }

   xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(3,xbe_lib,xbe_usr,cct);

   update_rk4_al(4,(h/6.0),xbe_lib,xbe_usr,cct,global);

// final update of non-integrator variables

// RK4: stage 4
   if (cct.flag_linear_x) {
     solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
   } else {
     solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
       smat,cct,slv,global);
   }

   return;
} // end of xbe_rk4_al
// -----------------------------------------------------------------------------
void xbe_rkf45(
   const int flag,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   double h;
   h = slv.delt_x;

   if (flag == global.I_RKF45_LTE) {
//   RKF45: stage 1

     global.time_given_x = slv.time_present_x;
     if (flag_nan(slv.time_present_x)) {
       cout << "xbe_rkf45: slv.time_present_x is NAN. Halting..." << endl;
       exit(1);
     }
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(0,xbe_lib,xbe_usr,cct);

     update_rkf45(1,h,xbe_lib,xbe_usr,cct,slv,global);

//   RKF45: stage 2
     global.time_given_x = slv.time_present_x + slv.rkf45_a1*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(1,xbe_lib,xbe_usr,cct);

     update_rkf45(2,h,xbe_lib,xbe_usr,cct,slv,global);

//   RKF45: stage 3
     global.time_given_x = slv.time_present_x + slv.rkf45_a2*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(2,xbe_lib,xbe_usr,cct);

     update_rkf45(3,h,xbe_lib,xbe_usr,cct,slv,global);

//   RKF45: stage 4
     global.time_given_x = slv.time_present_x + slv.rkf45_a3*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(3,xbe_lib,xbe_usr,cct);

     update_rkf45(4,h,xbe_lib,xbe_usr,cct,slv,global);

//   RKF45: stage 5
     global.time_given_x = slv.time_present_x + h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(4,xbe_lib,xbe_usr,cct);

     update_rkf45(5,h,xbe_lib,xbe_usr,cct,slv,global);

//   RKF45: stage 5A
     global.time_given_x = slv.time_present_x + slv.rkf45_a5*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(5,xbe_lib,xbe_usr,cct);
   } else if (flag == global.I_RKF45_SVEC) {
//   RKF45: stage 6
//   this stage computes the updated solvec (using the 4th order
//   part of RKF45) and is required only if the current step
//   is accepted.

     update_rkf45(6,h,xbe_lib,xbe_usr,cct,slv,global);

//   final update of non-integrator variables
//   Note that time_present_x has already been updated to (t + h).

     global.time_given_x = slv.time_present_x;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);
   } else if (flag == global.I_RKF45_NORM) {
//   compute norm_2 (only for the integrator variables)

     slv.rkf45_norm2 = 0.0;
     update_rkf45(7,h,xbe_lib,xbe_usr,cct,slv,global);
     slv.rkf45_norm2 = sqrt(slv.rkf45_norm2);
   }
   return;
} // end of xbe_rkf45
// -----------------------------------------------------------------------------
void xbe_rkf45_al(
   const int flag,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   SysMat &smat,
   Global &global) {

   double h;
   h = slv.delt_x;

   if (flag == global.I_RKF45_LTE) {
//   RKF45: stage 1

     global.time_given_x = slv.time_present_x;
     if (flag_nan(slv.time_present_x)) {
       cout << "xbe_rkf45_al: slv.time_present_x is NAN. Halting..." << endl;
       exit(1);
     }
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(0,xbe_lib,xbe_usr,cct);

     update_rkf45_al(1,h,xbe_lib,xbe_usr,cct,slv,global);

//   RKF45: stage 2
     global.time_given_x = slv.time_present_x + slv.rkf45_a1*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (cct.flag_linear_x) {
       solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);
     }

     xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(1,xbe_lib,xbe_usr,cct);

     update_rkf45_al(2,h,xbe_lib,xbe_usr,cct,slv,global);

//   RKF45: stage 3
     global.time_given_x = slv.time_present_x + slv.rkf45_a2*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (cct.flag_linear_x) {
       solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);
     }

     xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(2,xbe_lib,xbe_usr,cct);

     update_rkf45_al(3,h,xbe_lib,xbe_usr,cct,slv,global);

//   RKF45: stage 4
     global.time_given_x = slv.time_present_x + slv.rkf45_a3*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (cct.flag_linear_x) {
       solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);
     }

     xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(3,xbe_lib,xbe_usr,cct);

     update_rkf45_al(4,h,xbe_lib,xbe_usr,cct,slv,global);

//   RKF45: stage 5
     global.time_given_x = slv.time_present_x + h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (cct.flag_linear_x) {
       solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);
     }

     xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(4,xbe_lib,xbe_usr,cct);

     update_rkf45_al(5,h,xbe_lib,xbe_usr,cct,slv,global);

//   RKF45: stage 5A
     global.time_given_x = slv.time_present_x + slv.rkf45_a5*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (cct.flag_linear_x) {
       solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);
     }

     xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(5,xbe_lib,xbe_usr,cct);
   } else if (flag == global.I_RKF45_SVEC) {
//   RKF45: stage 6
//   this stage computes the updated solvec (using the 4th order
//   part of RKF45) and is required only if the current step
//   is accepted.

     update_rkf45_al(6,h,xbe_lib,xbe_usr,cct,slv,global);

//   final update of non-integrator variables
//   Note that time_present_x has already been updated to (t + h).

     global.time_given_x = slv.time_present_x;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (cct.flag_linear_x) {
       solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);
     }
   } else if (flag == global.I_RKF45_NORM) {
//   compute norm_2 (only for the integrator variables)

     slv.rkf45_norm2 = 0.0;
     update_rkf45_al(7,h,xbe_lib,xbe_usr,cct,slv,global);
     slv.rkf45_norm2 = sqrt(slv.rkf45_norm2);
   }
   return;
} // end of xbe_rkf45_al
// -----------------------------------------------------------------------------
void xbe_bs23(
   const int flag,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   double h;

   h = slv.delt_x;

   if (flag == global.I_BS23_LTE) {
//   bs23: stage 1

     global.time_given_x = slv.time_present_x;
     if (flag_nan(slv.time_present_x)) {
       cout << "xbe_bs23: slv.time_present_x is NAN. Halting..." << endl;
       exit(1);
     }
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(0,xbe_lib,xbe_usr,cct);
     update_bs23(1,h,xbe_lib,xbe_usr,cct,slv,global);

//   bs23: stage 2
     global.time_given_x = slv.time_present_x + slv.bs23_a1*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(1,xbe_lib,xbe_usr,cct);
     update_bs23(2,h,xbe_lib,xbe_usr,cct,slv,global);

//   bs23: stage 3
     global.time_given_x = slv.time_present_x + slv.bs23_a2*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(2,xbe_lib,xbe_usr,cct);
     update_bs23(3,h,xbe_lib,xbe_usr,cct,slv,global);

//   bs23: stage 4
     global.time_given_x = slv.time_present_x + h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(3,xbe_lib,xbe_usr,cct);
   } else if (flag == global.I_BS23_SVEC) {
//   bs23: stage 5
//   this stage computes the updated solvec (using the 2nd order
//   part of bs23) and is required only if the current step
//   is accepted.

     update_bs23(4,h,xbe_lib,xbe_usr,cct,slv,global);

//   final update of non-integrator variables
//   Note that time_present_x has already been updated to (t + h).
     global.time_given_x = slv.time_present_x;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);
   } else if (flag == global.I_BS23_NORM) {
//   compute norm_2 (only for the integrator variables)

     slv.bs23_norm2 = 0.0;
     update_bs23(5,h,xbe_lib,xbe_usr,cct,slv,global);
     slv.bs23_norm2 = sqrt(slv.bs23_norm2);
   }
   return;
} // end of xbe_bs23
// -----------------------------------------------------------------------------
void xbe_bs23_al(
   const int flag,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   SysMat &smat,
   Global &global) {

   double h;

   h = slv.delt_x;

   if (flag == global.I_BS23_LTE) {
//   bs23: stage 1

     global.time_given_x = slv.time_present_x;
     if (flag_nan(slv.time_present_x)) {
       cout << "xbe_bs23_al: slv.time_present_x is NAN. Halting..." << endl;
       exit(1);
     }
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(0,xbe_lib,xbe_usr,cct);
     update_bs23_al(1,h,xbe_lib,xbe_usr,cct,slv,global);

//   bs23: stage 2
     global.time_given_x = slv.time_present_x + slv.bs23_a1*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (cct.flag_linear_x) {
       solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);
     }

     xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(1,xbe_lib,xbe_usr,cct);
     update_bs23_al(2,h,xbe_lib,xbe_usr,cct,slv,global);

//   bs23: stage 3
     global.time_given_x = slv.time_present_x + slv.bs23_a2*h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (cct.flag_linear_x) {
       solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);
     }

     xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(2,xbe_lib,xbe_usr,cct);
     update_bs23_al(3,h,xbe_lib,xbe_usr,cct,slv,global);

//   bs23: stage 4
     global.time_given_x = slv.time_present_x + h;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (cct.flag_linear_x) {
       solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);
     }

     xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

     xbeu_copy_2(3,xbe_lib,xbe_usr,cct);
   } else if (flag == global.I_BS23_SVEC) {
//   bs23: stage 5
//   this stage computes the updated solvec (using the 2nd order
//   part of bs23) and is required only if the current step
//   is accepted.

     update_bs23_al(4,h,xbe_lib,xbe_usr,cct,slv,global);

//   final update of non-integrator variables
     global.time_given_x = slv.time_present_x;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (cct.flag_linear_x) {
       solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);
     }
   } else if (flag == global.I_BS23_NORM) {
//   compute norm_2 (only for the integrator variables)

     slv.bs23_norm2 = 0.0;
     update_bs23_al(5,h,xbe_lib,xbe_usr,cct,slv,global);
     slv.bs23_norm2 = sqrt(slv.bs23_norm2);
   }
   return;
} // end of xbe_bs23_al
// -----------------------------------------------------------------------------
void xbe_meuler(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   double h;

   h = slv.delt_x;

   xbeu_copy_1(xbe_lib,xbe_usr,cct);

// meuler: stage 1

   global.time_given_x = slv.time_present_x;
   if (flag_nan(slv.time_present_x)) {
     cout << "xbe_rk4_meuler: slv.time_present_x is NAN. Halting..." << endl;
     exit(1);
   }
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }
   xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(0,xbe_lib,xbe_usr,cct);
   update_meuler(1,h,xbe_lib,xbe_usr,cct,global);

// meuler: stage 2
   global.time_given_x = slv.time_present_x + h;
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }

   xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(1,xbe_lib,xbe_usr,cct);
   update_meuler(2,(0.5*h),xbe_lib,xbe_usr,cct,global);

// final update of non-integrator variables

   xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

   return;
} // end of xbe_meuler
// -----------------------------------------------------------------------------
void xbe_meuler_al(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   SysMat &smat,
   Global &global) {

   double h;

   h = slv.delt_x;

   xbeu_copy_1(xbe_lib,xbe_usr,cct);

// meuler: stage 1

   global.time_given_x = slv.time_present_x;
   if (flag_nan(slv.time_present_x)) {
     cout << "xbe_meuler_al: slv.time_present_x is NAN. Halting..." << endl;
     exit(1);
   }
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }
   xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(0,xbe_lib,xbe_usr,cct);
   update_meuler_al(1,h,xbe_lib,xbe_usr,cct,global);

// meuler: stage 2
   global.time_given_x = slv.time_present_x + h;
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }

   if (cct.flag_linear_x) {
     solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
   } else {
     solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
       smat,cct,slv,global);
   }

   xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);
   xbeu_copy_2(1,xbe_lib,xbe_usr,cct);
   update_meuler_al(2,(0.5*h),xbe_lib,xbe_usr,cct,global);

// final update of non-integrator variables

   if (cct.flag_linear_x) {
     solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
   } else {
     solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
       smat,cct,slv,global);
   }

   return;
} // end of xbe_meuler_al
// -----------------------------------------------------------------------------
void xbe_heun(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   double h;

   h = slv.delt_x;

   xbeu_copy_1(xbe_lib,xbe_usr,cct);

// heun: stage 1

   global.time_given_x = slv.time_present_x;
   if (flag_nan(slv.time_present_x)) {
     cout << "xbe_heun: slv.time_present_x is NAN. Halting..." << endl;
     exit(1);
   }
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }
   xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(0,xbe_lib,xbe_usr,cct);

   update_heun(1,h,xbe_lib,xbe_usr,cct,slv,global);

// heun: stage 2
   global.time_given_x = slv.time_present_x + (slv.heun_a1*h);
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }

   xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbe_evaluate(global.I_INTEGRATE,xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(1,xbe_lib,xbe_usr,cct);
   update_heun(2,h,xbe_lib,xbe_usr,cct,slv,global);

// final update of non-integrator variables

   global.time_given_x = slv.time_present_x + h;
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }

   xbe_evaluate_2(xbe_lib,xbe_usr,xbe_jac,cct,global);

   return;
} // end of xbe_heun
// -----------------------------------------------------------------------------
void xbe_heun_al(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   SysMat &smat,
   Global &global) {

   double h;

   h = slv.delt_x;
   xbeu_copy_1(xbe_lib,xbe_usr,cct);

// heun: stage 1

   global.time_given_x = slv.time_present_x;
   if (flag_nan(slv.time_present_x)) {
     cout << "xbe_heun_al: slv.time_present_x is NAN. Halting..." << endl;
     exit(1);
   }
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }
   xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(0,xbe_lib,xbe_usr,cct);

   update_heun_al(1,h,xbe_lib,xbe_usr,cct,slv,global);

// heun: stage 2
   global.time_given_x = slv.time_present_x + (slv.heun_a1*h);
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }

   if (cct.flag_linear_x) {
     solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
   } else {
     solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
       smat,cct,slv,global);
   }

   xbe_evaluate_al(xbe_lib,xbe_usr,xbe_jac,cct,global);

   xbeu_copy_2(1,xbe_lib,xbe_usr,cct);
   update_heun_al(2,h,xbe_lib,xbe_usr,cct,slv,global);

// final update of non-integrator variables

   global.time_given_x = slv.time_present_x + h;
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }

   if (cct.flag_linear_x) {
     solve_trns_linear_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
   } else {
     solve_trns_nonlinear_x_al(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
       smat,cct,slv,global);
   }

   return;
} // end of xbe_heun_al
// -----------------------------------------------------------------------------
void xbe_evaluate(
   const int flag,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_trns] = true;
   global.flags[global.i_explicit] = true;

   if (flag == global.I_INTEGRATE) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       if (xbe_lib[i_xbel].flag_integrate) {
         xbe_evaluate_1(flag,i_xbeu,xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
     }
   } else if (flag == global.I_DELAY) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       if (xbe_lib[i_xbel].flag_delay) {
         xbe_evaluate_1(flag,i_xbeu,xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
     }
   } else if (flag == global.I_EVAL_SRC) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       if (xbe_lib[i_xbel].flag_evaluate) {
         if (xbe_lib[i_xbel].flag_source) {
           xbe_evaluate_1(flag,i_xbeu,xbe_lib,xbe_usr,xbe_jac,cct,global);
         }
       }
     }
   } else if (flag == global.I_EVAL_NONSRC) {
     for (int i_pass=0; i_pass < cct.x_n_pass; i_pass++) {
       for (int i=0; i < cct.x_pass_n_beu[i_pass]; i++) {
         i_xbeu = cct.x_pass_beu[i_pass][i];
         xbe_evaluate_1(flag,i_xbeu,xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
     }
   }
   global.flags[global.i_trns] = false;
   global.flags[global.i_explicit] = false;

   return;
} // end of xbe_evaluate
// -----------------------------------------------------------------------------
void xbe_evaluate_1(
   const int flag,
   const int i_xbeu,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   Global &global) {

   int i_xbel;

   i_xbel = xbe_usr[i_xbeu].index_xbel;

   cct_to_xbe_1(i_xbeu,xbe_lib,xbe_usr,cct);

   get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbeu]);

   if (flag != global.I_INTEGRATE) {
     xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
   }
   return;
} // end of xbe_evaluate_1
// -----------------------------------------------------------------------------
void xbe_evaluate_2(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   Global &global) {

   xbe_evaluate(global.I_DELAY      ,xbe_lib,xbe_usr,xbe_jac,cct,global);
   xbe_evaluate(global.I_EVAL_SRC   ,xbe_lib,xbe_usr,xbe_jac,cct,global);
   xbe_evaluate(global.I_EVAL_NONSRC,xbe_lib,xbe_usr,xbe_jac,cct,global);

   return;
} // end of xbe_evaluate_2
// -----------------------------------------------------------------------------
void xbe_evaluate_al(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_trns] = true;
   global.flags[global.i_explicit] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       cct_to_xbe_1(i_xbeu,xbe_lib,xbe_usr,cct);
       get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbeu]);
     }
   }
   global.flags[global.i_trns] = false;
   global.flags[global.i_explicit] = false;

   return;
} // end of xbe_evaluate
// -----------------------------------------------------------------------------
void solve_trns_x_common(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   CctFile &cct_file,
   Global &global) {

   for (int i_file=0;i_file < slv.n_outfile;i_file++) {
     if (slv.flag_out_delt_fixed[i_file]) {
       slv.out_tnext[i_file] = slv.out_tstart[i_file];
     }
   }
   slv.time_present_x = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_x = slv.delt0_x;

   slv.write_iter_n1_x = 0;
   slv.iter_trns_x = -1; 
   global.iter_trns_x = -1;

   if (cct.flag_limit_tstep_x) {
     x_assign_nextbreak_1(xbe_usr,cct,global);
     xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);

     get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
   }
   return;
} // end of solve_trns_x_common
// -----------------------------------------------------------------------------
void update_rk4(
   const int stage,
   const double h1,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;
   int var_flag,var_number;
   int n_f1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_vr[var_number] =
             xbe_usr[i_xbeu].val_vr_0[var_number] + h1*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_vr[var_number] =
             xbe_usr[i_xbeu].val_vr_0[var_number] + h1*xbe_usr[i_xbeu].f1[i_func];
           } if (stage == 3) {
             xbe_usr[i_xbeu].val_vr[var_number] =
             xbe_usr[i_xbeu].val_vr_0[var_number] + h1*xbe_usr[i_xbeu].f2[i_func];
           } if (stage == 4) {
             xbe_usr[i_xbeu].val_vr[var_number] =
             xbe_usr[i_xbeu].val_vr_0[var_number] +
             h1*(xbe_usr[i_xbeu].f0[i_func] +
               2*(xbe_usr[i_xbeu].f1[i_func] + xbe_usr[i_xbeu].f2[i_func]) +
               xbe_usr[i_xbeu].f3[i_func]);
           }
         } else if (var_flag == global.I_XAUX) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_aux[var_number] =
             xbe_usr[i_xbeu].val_aux_0[var_number] + h1*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_aux[var_number] =
             xbe_usr[i_xbeu].val_aux_0[var_number] + h1*xbe_usr[i_xbeu].f1[i_func];
           } if (stage == 3) {
             xbe_usr[i_xbeu].val_aux[var_number] =
             xbe_usr[i_xbeu].val_aux_0[var_number] + h1*xbe_usr[i_xbeu].f2[i_func];
           } if (stage == 4) {
             xbe_usr[i_xbeu].val_aux[var_number] =
             xbe_usr[i_xbeu].val_aux_0[var_number] +
             h1*(xbe_usr[i_xbeu].f0[i_func] +
               2*(xbe_usr[i_xbeu].f1[i_func] + xbe_usr[i_xbeu].f2[i_func]) +
               xbe_usr[i_xbeu].f3[i_func]);
           }
         } else {
           cout << "update_rk4: var_flag=" << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }

   return;
} // end of update_rk4
// -----------------------------------------------------------------------------
void update_rk4_al(
   const int stage,
   const double h1,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;
   int var_flag,var_number;
   int n_f1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_vr[var_number] =
             xbe_usr[i_xbeu].val_vr_0[var_number] + h1*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_vr[var_number] =
             xbe_usr[i_xbeu].val_vr_0[var_number] + h1*xbe_usr[i_xbeu].f1[i_func];
           } if (stage == 3) {
             xbe_usr[i_xbeu].val_vr[var_number] =
             xbe_usr[i_xbeu].val_vr_0[var_number] + h1*xbe_usr[i_xbeu].f2[i_func];
           } if (stage == 4) {
             xbe_usr[i_xbeu].val_vr[var_number] =
             xbe_usr[i_xbeu].val_vr_0[var_number] +
             h1*(xbe_usr[i_xbeu].f0[i_func] +
               2*(xbe_usr[i_xbeu].f1[i_func] + xbe_usr[i_xbeu].f2[i_func]) +
               xbe_usr[i_xbeu].f3[i_func]);
           }
           xbe_usr[i_xbeu].val_vr_u[var_number] =
           xbe_usr[i_xbeu].val_vr  [var_number];
         } else if (var_flag == global.I_XAUX) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_aux[var_number] =
             xbe_usr[i_xbeu].val_aux_0[var_number] + h1*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_aux[var_number] =
             xbe_usr[i_xbeu].val_aux_0[var_number] + h1*xbe_usr[i_xbeu].f1[i_func];
           } if (stage == 3) {
             xbe_usr[i_xbeu].val_aux[var_number] =
             xbe_usr[i_xbeu].val_aux_0[var_number] + h1*xbe_usr[i_xbeu].f2[i_func];
           } if (stage == 4) {
             xbe_usr[i_xbeu].val_aux[var_number] =
             xbe_usr[i_xbeu].val_aux_0[var_number] +
             h1*(xbe_usr[i_xbeu].f0[i_func] +
               2*(xbe_usr[i_xbeu].f1[i_func] + xbe_usr[i_xbeu].f2[i_func]) +
               xbe_usr[i_xbeu].f3[i_func]);
           }
           xbe_usr[i_xbeu].val_aux_u[var_number] =
           xbe_usr[i_xbeu].val_aux  [var_number];
         } else {
           cout << "update_rk4_al: var_flag=" << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }

   return;
} // end of update_rk4_al
// -----------------------------------------------------------------------------
void update_rkf45(
   const int stage,
   const double h,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   double a;
   int i_xbeu,i_xbel;
   int var_flag,var_number;
   int n_f1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         if (stage == 7) {
           a = slv.rkf45_e0*xbe_usr[i_xbeu].f0[i_func]
             + slv.rkf45_e2*xbe_usr[i_xbeu].f2[i_func]
             + slv.rkf45_e3*xbe_usr[i_xbeu].f3[i_func]
             + slv.rkf45_e4*xbe_usr[i_xbeu].f4[i_func]
             + slv.rkf45_e5*xbe_usr[i_xbeu].f5[i_func];
           a= a*h;
           slv.rkf45_norm2 += a*a;
           continue;
         }

         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*(slv.rkf45_b10*xbe_usr[i_xbeu].f0[i_func]);
           } else if (stage == 2) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.rkf45_b20*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b21*xbe_usr[i_xbeu].f1[i_func]);
           } else if (stage == 3) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.rkf45_b30*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b31*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b32*xbe_usr[i_xbeu].f2[i_func]);
           } else if (stage == 4) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.rkf45_b40*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b41*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b42*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_b43*xbe_usr[i_xbeu].f3[i_func]);
           } else if (stage == 5) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.rkf45_b50*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b51*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b52*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_b53*xbe_usr[i_xbeu].f3[i_func]
                 + slv.rkf45_b54*xbe_usr[i_xbeu].f4[i_func]);
           } else if (stage == 6) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.rkf45_4_g0*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_4_g2*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_4_g3*xbe_usr[i_xbeu].f3[i_func]
                 + slv.rkf45_4_g4*xbe_usr[i_xbeu].f4[i_func]);
           }
         } else if (var_flag == global.I_XAUX) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*(slv.rkf45_b10*xbe_usr[i_xbeu].f0[i_func]);
           } else if (stage == 2) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.rkf45_b20*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b21*xbe_usr[i_xbeu].f1[i_func]);
           } else if (stage == 3) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.rkf45_b30*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b31*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b32*xbe_usr[i_xbeu].f2[i_func]);
           } else if (stage == 4) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.rkf45_b40*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b41*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b42*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_b43*xbe_usr[i_xbeu].f3[i_func]);
           } else if (stage == 5) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.rkf45_b50*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b51*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b52*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_b53*xbe_usr[i_xbeu].f3[i_func]
                 + slv.rkf45_b54*xbe_usr[i_xbeu].f4[i_func]);
           } else if (stage == 6) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.rkf45_4_g0*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_4_g2*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_4_g3*xbe_usr[i_xbeu].f3[i_func]
                 + slv.rkf45_4_g4*xbe_usr[i_xbeu].f4[i_func]);
           }
         } else {
           cout << "update_rkf45: var_flag=" << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }
   return;
} // end of update_rkf45
// -----------------------------------------------------------------------------
void update_rkf45_al(
   const int stage,
   const double h,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   double a;
   int i_xbeu,i_xbel;
   int var_flag,var_number;
   int n_f1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         if (stage == 7) {
           a = slv.rkf45_e0*xbe_usr[i_xbeu].f0[i_func]
             + slv.rkf45_e2*xbe_usr[i_xbeu].f2[i_func]
             + slv.rkf45_e3*xbe_usr[i_xbeu].f3[i_func]
             + slv.rkf45_e4*xbe_usr[i_xbeu].f4[i_func]
             + slv.rkf45_e5*xbe_usr[i_xbeu].f5[i_func];
           a= a*h;
           slv.rkf45_norm2 += a*a;
           continue;
         }

         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*(slv.rkf45_b10*xbe_usr[i_xbeu].f0[i_func]);
           } else if (stage == 2) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.rkf45_b20*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b21*xbe_usr[i_xbeu].f1[i_func]);
           } else if (stage == 3) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.rkf45_b30*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b31*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b32*xbe_usr[i_xbeu].f2[i_func]);
           } else if (stage == 4) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.rkf45_b40*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b41*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b42*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_b43*xbe_usr[i_xbeu].f3[i_func]);
           } else if (stage == 5) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.rkf45_b50*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b51*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b52*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_b53*xbe_usr[i_xbeu].f3[i_func]
                 + slv.rkf45_b54*xbe_usr[i_xbeu].f4[i_func]);
           } else if (stage == 6) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.rkf45_4_g0*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_4_g2*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_4_g3*xbe_usr[i_xbeu].f3[i_func]
                 + slv.rkf45_4_g4*xbe_usr[i_xbeu].f4[i_func]);
           }
           xbe_usr[i_xbeu].val_vr_u[var_number] =
           xbe_usr[i_xbeu].val_vr  [var_number];
         } else if (var_flag == global.I_XAUX) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*(slv.rkf45_b10*xbe_usr[i_xbeu].f0[i_func]);
           } else if (stage == 2) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.rkf45_b20*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b21*xbe_usr[i_xbeu].f1[i_func]);
           } else if (stage == 3) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.rkf45_b30*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b31*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b32*xbe_usr[i_xbeu].f2[i_func]);
           } else if (stage == 4) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.rkf45_b40*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b41*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b42*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_b43*xbe_usr[i_xbeu].f3[i_func]);
           } else if (stage == 5) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.rkf45_b50*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_b51*xbe_usr[i_xbeu].f1[i_func]
                 + slv.rkf45_b52*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_b53*xbe_usr[i_xbeu].f3[i_func]
                 + slv.rkf45_b54*xbe_usr[i_xbeu].f4[i_func]);
           } else if (stage == 6) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.rkf45_4_g0*xbe_usr[i_xbeu].f0[i_func]
                 + slv.rkf45_4_g2*xbe_usr[i_xbeu].f2[i_func]
                 + slv.rkf45_4_g3*xbe_usr[i_xbeu].f3[i_func]
                 + slv.rkf45_4_g4*xbe_usr[i_xbeu].f4[i_func]);
           }
           xbe_usr[i_xbeu].val_aux_u[var_number] =
           xbe_usr[i_xbeu].val_aux  [var_number];
         } else {
           cout << "update_rkf45: var_flag=" << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }
   return;
} // end of update_rkf45_al
// -----------------------------------------------------------------------------
void update_bs23(
   const int stage,
   const double h,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   double a;
   int i_xbeu,i_xbel;
   int var_flag,var_number;
   int n_f1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         if (stage == 5) {
           a = slv.bs23_e0*xbe_usr[i_xbeu].f0[i_func]
             + slv.bs23_e1*xbe_usr[i_xbeu].f1[i_func]
             + slv.bs23_e2*xbe_usr[i_xbeu].f2[i_func]
             + slv.bs23_e3*xbe_usr[i_xbeu].f3[i_func];
           slv.bs23_norm2 += a*a;
           continue;
         }
         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*(slv.bs23_b10*xbe_usr[i_xbeu].f0[i_func]);
           } else if (stage == 2) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.bs23_b21*xbe_usr[i_xbeu].f1[i_func]);
           } else if (stage == 3) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.bs23_b30*xbe_usr[i_xbeu].f0[i_func]
                 + slv.bs23_b31*xbe_usr[i_xbeu].f1[i_func]
                 + slv.bs23_b32*xbe_usr[i_xbeu].f2[i_func]);
           } else if (stage == 4) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.bs23_2_g0*xbe_usr[i_xbeu].f0[i_func]
                 + slv.bs23_2_g1*xbe_usr[i_xbeu].f1[i_func]
                 + slv.bs23_2_g2*xbe_usr[i_xbeu].f2[i_func]
                 + slv.bs23_2_g3*xbe_usr[i_xbeu].f3[i_func]);
           }
         } else if (var_flag == global.I_XAUX) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*(slv.bs23_b10*xbe_usr[i_xbeu].f0[i_func]);
           } else if (stage == 2) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.bs23_b21*xbe_usr[i_xbeu].f1[i_func]);
           } else if (stage == 3) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.bs23_b30*xbe_usr[i_xbeu].f0[i_func]
                 + slv.bs23_b31*xbe_usr[i_xbeu].f1[i_func]
                 + slv.bs23_b32*xbe_usr[i_xbeu].f2[i_func]);
           } else if (stage == 4) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.bs23_2_g0*xbe_usr[i_xbeu].f0[i_func]
                 + slv.bs23_2_g1*xbe_usr[i_xbeu].f1[i_func]
                 + slv.bs23_2_g2*xbe_usr[i_xbeu].f2[i_func]
                 + slv.bs23_2_g3*xbe_usr[i_xbeu].f3[i_func]);
           }
         } else {
           cout << "update_bs23: var_flag=" << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }
   return;
} // end of update_bs23
// -----------------------------------------------------------------------------
void update_bs23_al(
   const int stage,
   const double h,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   double a;
   int i_xbeu,i_xbel;
   int var_flag,var_number;
   int n_f1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         if (stage == 5) {
           a = slv.bs23_e0*xbe_usr[i_xbeu].f0[i_func]
             + slv.bs23_e1*xbe_usr[i_xbeu].f1[i_func]
             + slv.bs23_e2*xbe_usr[i_xbeu].f2[i_func]
             + slv.bs23_e3*xbe_usr[i_xbeu].f3[i_func];
           slv.bs23_norm2 += a*a;
           continue;
         }
         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*(slv.bs23_b10*xbe_usr[i_xbeu].f0[i_func]);
           } else if (stage == 2) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.bs23_b21*xbe_usr[i_xbeu].f1[i_func]);
           } else if (stage == 3) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.bs23_b30*xbe_usr[i_xbeu].f0[i_func]
                 + slv.bs23_b31*xbe_usr[i_xbeu].f1[i_func]
                 + slv.bs23_b32*xbe_usr[i_xbeu].f2[i_func]);
           } else if (stage == 4) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.bs23_2_g0*xbe_usr[i_xbeu].f0[i_func]
                 + slv.bs23_2_g1*xbe_usr[i_xbeu].f1[i_func]
                 + slv.bs23_2_g2*xbe_usr[i_xbeu].f2[i_func]
                 + slv.bs23_2_g3*xbe_usr[i_xbeu].f3[i_func]);
           }
           xbe_usr[i_xbeu].val_vr_u[var_number] =
           xbe_usr[i_xbeu].val_vr  [var_number];
         } else if (var_flag == global.I_XAUX) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*(slv.bs23_b10*xbe_usr[i_xbeu].f0[i_func]);
           } else if (stage == 2) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.bs23_b21*xbe_usr[i_xbeu].f1[i_func]);
           } else if (stage == 3) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.bs23_b30*xbe_usr[i_xbeu].f0[i_func]
                 + slv.bs23_b31*xbe_usr[i_xbeu].f1[i_func]
                 + slv.bs23_b32*xbe_usr[i_xbeu].f2[i_func]);
           } else if (stage == 4) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.bs23_2_g0*xbe_usr[i_xbeu].f0[i_func]
                 + slv.bs23_2_g1*xbe_usr[i_xbeu].f1[i_func]
                 + slv.bs23_2_g2*xbe_usr[i_xbeu].f2[i_func]
                 + slv.bs23_2_g3*xbe_usr[i_xbeu].f3[i_func]);
           }
           xbe_usr[i_xbeu].val_aux_u[var_number] =
           xbe_usr[i_xbeu].val_aux  [var_number];
         } else {
           cout << "update_bs23: var_flag=" << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }
   return;
} // end of update_bs23_al
// -----------------------------------------------------------------------------
void update_meuler(
   const int stage,
   const double h1,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;
   int var_flag,var_number;
   int n_f1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h1*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h1*( xbe_usr[i_xbeu].f0[i_func] 
                  + xbe_usr[i_xbeu].f1[i_func]);
           }
         } else if (var_flag == global.I_XAUX) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h1*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h1*( xbe_usr[i_xbeu].f0[i_func] 
                  + xbe_usr[i_xbeu].f1[i_func]);
           }
         } else {
           cout << "update_meuler: var_flag=" << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }
   return;
} // end of update_meuler
// -----------------------------------------------------------------------------
void update_meuler_al(
   const int stage,
   const double h1,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;
   int var_flag,var_number;
   int n_f1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h1*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h1*( xbe_usr[i_xbeu].f0[i_func] 
                  + xbe_usr[i_xbeu].f1[i_func]);
           }
           xbe_usr[i_xbeu].val_vr_u[var_number] =
           xbe_usr[i_xbeu].val_vr  [var_number];
         } else if (var_flag == global.I_XAUX) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h1*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h1*( xbe_usr[i_xbeu].f0[i_func] 
                  + xbe_usr[i_xbeu].f1[i_func]);
           }
           xbe_usr[i_xbeu].val_aux_u[var_number] =
           xbe_usr[i_xbeu].val_aux  [var_number];
         } else {
           cout << "update_meuler: var_flag=" << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }
   return;
} // end of update_meuler_al
// -----------------------------------------------------------------------------
void update_heun(
   const int stage,
   const double h,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_xbeu,i_xbel;
   int var_flag,var_number;
   int n_f1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + slv.heun_b10*h*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.heun_g0*xbe_usr[i_xbeu].f0[i_func] 
                 + slv.heun_g1*xbe_usr[i_xbeu].f1[i_func]);
           }
         } else if (var_flag == global.I_XAUX) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + slv.heun_b10*h*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.heun_g0*xbe_usr[i_xbeu].f0[i_func] 
                 + slv.heun_g1*xbe_usr[i_xbeu].f1[i_func]);
           }
         } else {
           cout << "update_heun: var_flag=" << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }
   return;
} // end of update_heun
// -----------------------------------------------------------------------------
void update_heun_al(
   const int stage,
   const double h,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_xbeu,i_xbel;
   int var_flag,var_number;
   int n_f1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;

       for (int i_func=0; i_func < n_f1; i_func++) {
         var_flag   = xbe_lib[i_xbel].ddt_varflag  [i_func];
         var_number = xbe_lib[i_xbel].ddt_varnumber[i_func];

         if (var_flag == global.I_XVR) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + slv.heun_b10*h*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_vr[var_number]
             = xbe_usr[i_xbeu].val_vr_0[var_number]
             + h*( slv.heun_g0*xbe_usr[i_xbeu].f0[i_func] 
                 + slv.heun_g1*xbe_usr[i_xbeu].f1[i_func]);
           }
           xbe_usr[i_xbeu].val_vr_u[var_number] =
           xbe_usr[i_xbeu].val_vr  [var_number];
         } else if (var_flag == global.I_XAUX) {
           if (stage == 1) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + slv.heun_b10*h*xbe_usr[i_xbeu].f0[i_func];
           } if (stage == 2) {
             xbe_usr[i_xbeu].val_aux[var_number]
             = xbe_usr[i_xbeu].val_aux_0[var_number]
             + h*( slv.heun_g0*xbe_usr[i_xbeu].f0[i_func] 
                 + slv.heun_g1*xbe_usr[i_xbeu].f1[i_func]);
           }
           xbe_usr[i_xbeu].val_aux_u[var_number] =
           xbe_usr[i_xbeu].val_aux  [var_number];
         } else {
           cout << "update_heun: var_flag=" << var_flag
             << " does not make sense. Halting..." << endl;
           exit(1);
         }
       }
       xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
     }
   }
   return;
} // end of update_heun_al
// -----------------------------------------------------------------------------
void solve_dc(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

// Note: With DC, xbe's are not allowed; only ebe's need to be considered.

   init_sol_e(xbe_lib,xbe_usr,ebe_lib,ebe_usr,ebe_jac,slv,cct,smat,global);

   smat.mat_dc_1_e(ebe_lib,ebe_usr,global,cct,cct_file);

   assign_const_1<bool>(global.flags,false);
   global.time_given_e = 0.0;

   if (cct.flag_linear_e) {
     solve_dc_linear_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
   } else {
     solve_dc_nonlinear_e(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,
       smat,cct,slv,global);
     if (!slv.flag_nr_converged) {
       cout << "solve_dc: NR did not converge. Halting..." << endl;
       slv.write_flags_failed();
       exit(1);
     }
   }
   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   find_ebe_cur_stv_dc(ebe_lib,ebe_usr,ebe_jac,smat,cct,global);
   write_dc_startup(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   return;
} // end of solve_dc
// -----------------------------------------------------------------------------
void solve_dc_linear_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   form_jac_rhs_dc_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,global);

   negative_double_1(smat.m_e.n_row,smat.rhs_m_e);

   smat.w_e.allocate_1(smat.m_e.n_nz,smat.m_e.n_row,smat.m_e.n_col);
   smat.mo_e.allocate_1(smat.m_e.n_row);

   solve_jac_1_e(smat,slv,global);

   copy_array_1<double>(smat.m_e.n_row,smat.svec_orig_e,smat.delsvec_e);

   add_arrays_1<double>(smat.m_e.n_row,smat.delsvec_e,smat.svec_e);
   dcmp_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

   return;
} // end of solve_dc_linear_e
// -----------------------------------------------------------------------------
void solve_dc_nonlinear_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_newt;
   bool flag_nan_1;

   check_convergence_count_e(slv);
   slv.get_dmp(cct);

   smat.w_e.allocate_1(smat.m_e.n_nz,smat.m_e.n_row,smat.m_e.n_col);
   smat.mo_e.allocate_1(smat.m_e.n_row);

   for (i_newt=0; i_newt < slv.e_nr_itermax_a; i_newt++) {
     cout << "solve_dc_nonlinear_e: i_newt = " << i_newt << endl;
     slv.iter_newton = i_newt;
     form_jac_rhs_dc_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,global);

     negative_double_1(smat.m_e.n_row,smat.rhs_m_e);

     if (i_newt == 0) {
       solve_jac_1_e(smat,slv,global);
     } else {
       solve_jac_2_e(smat,slv,global);
     }
     copy_array_1<double>(smat.m_e.n_row,smat.svec_orig_e,smat.delsvec_e);

     if ((slv.e_nr_flag_dmp_a) && (i_newt <= slv.e_nr_dmp_itermax_a)) {
       mult_array_1<double>(smat.m_e.n_row,smat.delsvec_e,slv.e_nr_dmp_k_a);
     }
     add_arrays_1<double>(smat.m_e.n_row,smat.delsvec_e,smat.svec_e);
     check_array_for_nan_2(smat.m_e.n_row,smat.svec_e,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_dc_nonlinear_e: svec_e has a NaN" << endl;
       slv.flag_nr_converged = false;
       slv.flag_nr_norm_large = true;
       break;
     } else {
       dcmp_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);
       if (slv.e_nr_flag_check_spice) {
         find_ebe_cur_stv_dc(ebe_lib,ebe_usr,ebe_jac,smat,cct,global);
       }
       check_convergence_e(smat,slv,ebe_lib,ebe_usr,cct);
       if (slv.flag_nr_converged) {
         break;
       }
       if (slv.e_nr_flag_check_spice) {
         copy_array_1<double>(smat.m_e.n_row,smat.svec_e,smat.svec_old_nr_1_e);
         copy_cur_nd_nr_1(ebe_lib,ebe_usr,cct);
       }
     }
   }
   return;
} // end of solve_dc_nonlinear_e
// -----------------------------------------------------------------------------
void solve_startup(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   if (cct.flag_e_only) {
     solve_startup_e(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (cct.flag_x_only) {
     if (slv.x_algo_startup_exp) {
       solve_startup_x_exp(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);
     } else {
       solve_startup_x_imp(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     }
   } else {
     if (cct.flag_exc) {
       solve_startup_exc(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     } else {
       cout << "solve_startup: exs not implemented. Halting..." << endl;
       exit(1);
     }
   }

   return;
} // end of solve_startup
// -----------------------------------------------------------------------------
void solve_startup_e(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   init_sol_e(xbe_lib,xbe_usr,ebe_lib,ebe_usr,ebe_jac,slv,cct,smat,global);

   smat.mat_startup_1_e(ebe_lib,ebe_usr,global,cct,cct_file);
   form_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

   assign_const_1<bool>(global.flags,false);
   global.time_given_e = 0.0;

   if (cct.flag_linear_e) {
     solve_startup_linear_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
   } else {
     solve_startup_nonlinear_e(ebe_lib,ebe_usr,ebe_jac,
       xbe_lib,xbe_usr,smat,cct,slv,global);
   }
   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   find_ebe_cur_stv_startup(ebe_lib,ebe_usr,ebe_jac,smat,cct,global);
   write_dc_startup(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   return;
} // end of solve_startup_e
// -----------------------------------------------------------------------------
void solve_startup_linear_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   form_jac_rhs_startup_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,global);

   negative_double_1(smat.m_e.n_row,smat.rhs_m_e);

   smat.w_e.allocate_1(smat.m_e.n_nz,smat.m_e.n_row,smat.m_e.n_col);
   smat.mo_e.allocate_1(smat.m_e.n_row);

   solve_jac_1_e(smat,slv,global);

   copy_array_1<double>(smat.m_e.n_row,smat.svec_orig_e,smat.delsvec_e);

   add_arrays_1<double>(smat.m_e.n_row,smat.delsvec_e,smat.svec_e);
   dcmp_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

   return;
} // end of solve_startup_linear_e
// -----------------------------------------------------------------------------
void solve_startup_nonlinear_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_newt;
   bool flag_nan_1;

   check_convergence_count_e(slv);
   slv.get_dmp(cct);

   smat.w_e.allocate_1(smat.m_e.n_nz,smat.m_e.n_row,smat.m_e.n_col);
   smat.mo_e.allocate_1(smat.m_e.n_row);

   for (i_newt=0; i_newt < slv.e_nr_itermax_a; i_newt++) {
     cout << "solve_startup_nonlinear_e: i_newt = " << i_newt << endl;
     slv.iter_newton = i_newt;
     form_jac_rhs_startup_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,global);

     negative_double_1(smat.m_e.n_row,smat.rhs_m_e);

     if (i_newt == 0) {
       solve_jac_1_e(smat,slv,global);
     } else {
       solve_jac_2_e(smat,slv,global);
     }
     copy_array_1<double>(smat.m_e.n_row,smat.svec_orig_e,smat.delsvec_e);

     if ((slv.e_nr_flag_dmp_a) && (i_newt <= slv.e_nr_dmp_itermax_a)) {
       mult_array_1<double>(smat.m_e.n_row,smat.delsvec_e,slv.e_nr_dmp_k_a);
     }
     add_arrays_1<double>(smat.m_e.n_row,smat.delsvec_e,smat.svec_e);
     check_array_for_nan_2(smat.m_e.n_row,smat.svec_e,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_startup_nonlinear_e: svec_e has a NaN" << endl;
       slv.flag_nr_converged = false;
       slv.flag_nr_norm_large = true;
       break;
     } else {
       dcmp_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);
       check_convergence_e(smat,slv,ebe_lib,ebe_usr,cct);
       if (slv.flag_nr_converged) {
         break;
       }
     }
     if (slv.e_nr_flag_check_spice) {
       copy_array_1<double>(smat.m_e.n_row,smat.svec_e,smat.svec_old_nr_1_e);
       copy_cur_nd_nr_1(ebe_lib,ebe_usr,cct);
     }
   }
   return;
} // end of solve_startup_nonlinear_e
// -----------------------------------------------------------------------------
void solve_startup_x_exp(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_xbeu,i_xbel;

   cout << "solve_startup_x_exp starting..." << endl;

   assign_const_1<bool>(global.flags,false);
   global.time_given_x = 0.0;

   global.flags[global.i_startup] = true;
   global.flags[global.i_explicit] = true;

// step 1: assign start-up values to integrate and source type elements

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       get_xbe_1(i_xbeu,xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
   }
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_evaluate) {
       if (xbe_lib[i_xbel].flag_source) {
         get_xbe_1(i_xbeu,xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
     }
   }
// step 2: process the remaining elements

   for (int i_pass=0; i_pass < cct.x_n_pass; i_pass++) {
     for (int i=0; i < cct.x_pass_n_beu[i_pass]; i++) {
       i_xbeu = cct.x_pass_beu[i_pass][i];
       get_xbe_1(i_xbeu,xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
   }
   global.flags[global.i_startup] = false;
   global.flags[global.i_explicit] = false;

   write_dc_startup(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   cout << "solve_startup_x_exp over" << endl;

   return;
} // end of solve_startup_x_exp
// -----------------------------------------------------------------------------
void solve_startup_x_imp(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   init_sol_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,slv,cct,smat,global);
   smat.mat_startup_1_x(xbe_lib,xbe_usr,cct,cct_file);
   form_solvec_x(xbe_lib,xbe_usr,smat,cct);

   assign_const_1<bool>(global.flags,false);
   global.time_given_x = 0.0;

   if (cct.flag_linear_x) {
     solve_startup_linear_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
   } else {
     solve_startup_nonlinear_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
       smat,cct,slv,global);
   }
   write_dc_startup(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   return;
} // end of solve_startup_x_imp
// -----------------------------------------------------------------------------
void solve_startup_linear_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   form_jac_rhs_startup_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   form_solvec_x(xbe_lib,xbe_usr,smat,cct);

   negative_double_1(smat.m_x.n_row,smat.rhs_m_x);

   smat.w_x.allocate_1(smat.m_x.n_nz,smat.m_x.n_row,smat.m_x.n_col);
   smat.mo_x.allocate_1(smat.m_x.n_row);

   solve_jac_1_x(smat,slv,global);

   copy_array_1<double>(smat.m_x.n_row,smat.svec_orig_x,smat.delsvec_x);

   add_arrays_1<double>(smat.m_x.n_row,smat.delsvec_x,smat.svec_x);
   dcmp_solvec_x(xbe_lib,xbe_usr,smat,cct);

   return;
} // end of solve_startup_linear_x
// -----------------------------------------------------------------------------
void solve_startup_nonlinear_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_newt;
   bool flag_nan_1;

   check_convergence_count_x(slv);
   slv.get_dmp(cct);

   smat.w_x.allocate_1(smat.m_x.n_nz,smat.m_x.n_row,smat.m_x.n_col);
   smat.mo_x.allocate_1(smat.m_x.n_row);

   for (i_newt=0; i_newt < slv.x_nr_itermax_a; i_newt++) {
     cout << "solve_startup_nonlinear_x: i_newt = " << i_newt << endl;
     slv.iter_newton = i_newt;
     form_jac_rhs_startup_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

     negative_double_1(smat.m_x.n_row,smat.rhs_m_x);

     if (i_newt == 0) {
       solve_jac_1_x(smat,slv,global);
     } else {
       solve_jac_2_x(smat,slv,global);
     }
     copy_array_1<double>(smat.m_x.n_row,smat.svec_orig_x,smat.delsvec_x);

     if ((slv.x_nr_flag_dmp_a) && (i_newt <= slv.x_nr_dmp_itermax_a)) {
       mult_array_1<double>(smat.m_x.n_row,smat.delsvec_x,slv.x_nr_dmp_k_a);
     }
     add_arrays_1<double>(smat.m_x.n_row,smat.delsvec_x,smat.svec_x);
     check_array_for_nan_2(smat.m_x.n_row,smat.svec_x,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_startup_nonlinear_x: svec_x has a NaN" << endl;
       slv.flag_nr_converged = false;
       slv.flag_nr_norm_large = true;
       break;
     } else {
       dcmp_solvec_x(xbe_lib,xbe_usr,smat,cct);
       check_convergence_x(smat,slv);
       if (slv.flag_nr_converged) {
         break;
       }
     }
   }
   return;
} // end of solve_startup_nonlinear_x
// -----------------------------------------------------------------------------
void solve_startup_exc(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   init_sol_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     slv,cct,smat,global);

   smat.mat_startup_1_ex(ebe_lib,ebe_usr,xbe_lib,xbe_usr,global,cct,cct_file);
   form_solvec_ex(ebe_lib,xbe_lib,ebe_usr,xbe_usr,smat,slv,cct);

   assign_const_1<bool>(global.flags,false);
   global.time_given_e = 0.0;
   global.time_given_x = 0.0;

   if (cct.flag_linear_ex) {
     solve_startup_linear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
       smat,cct,slv,global);
   } else {
     solve_startup_nonlinear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
       smat,cct,slv,global);
     if (!slv.flag_nr_converged) {
       cout << "solve_startup_exc: N-R iterations did not converge." << endl;
       slv.write_flags_failed();
       cout << "  Halting..." << endl;
       exit(1);
     }
   }
   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   find_ebe_cur_stv_startup(ebe_lib,ebe_usr,ebe_jac,smat,cct,global);
   write_dc_startup(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   return;
} // end of solve_startup_exc
// -----------------------------------------------------------------------------
void solve_startup_linear_exc(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   form_jac_rhs_startup_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
     smat,cct,global);

   negative_double_1(smat.m_ex.n_row,smat.rhs_m_ex);

   smat.w_ex.allocate_1(smat.m_ex.n_nz,smat.m_ex.n_row,smat.m_ex.n_col);
   smat.mo_ex.allocate_1(smat.m_ex.n_row);

   solve_jac_1_ex(smat,slv,global);

   copy_array_1<double>(smat.m_ex.n_row,smat.svec_orig_ex,smat.delsvec_ex);

   add_arrays_1<double>(smat.m_ex.n_row,smat.delsvec_ex,smat.svec_ex);

   dcmp_solvec_ex(xbe_lib,xbe_usr,ebe_lib,ebe_usr,smat,slv,cct);

   return;
} // end of solve_startup_linear_exc
// -----------------------------------------------------------------------------
void solve_startup_nonlinear_exc(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_newt;
   bool flag_nan_1;

   check_convergence_count_ex(slv);
   slv.get_dmp(cct);

   smat.w_ex.allocate_1(smat.m_ex.n_nz,smat.m_ex.n_row,smat.m_ex.n_col);
   smat.mo_ex.allocate_1(smat.m_ex.n_row);

   for (i_newt=0; i_newt < slv.ex_nr_itermax_a; i_newt++) {
     cout << "solve_startup_nonlinear_exc: i_newt = " << i_newt << endl;
     slv.iter_newton = i_newt;
     form_jac_rhs_startup_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
       smat,cct,global);

     negative_double_1(smat.m_ex.n_row,smat.rhs_m_ex);

     if (i_newt == 0) {
       solve_jac_1_ex(smat,slv,global);
     } else {
       solve_jac_2_ex(smat,slv,global);
     }
     copy_array_1<double>(smat.m_ex.n_row,smat.svec_orig_ex,smat.delsvec_ex);

     if ((slv.ex_nr_flag_dmp_a) && (i_newt <= slv.ex_nr_dmp_itermax_a)) {
       mult_array_1<double>(smat.m_ex.n_row,smat.delsvec_ex,slv.ex_nr_dmp_k_a);
     }
     add_arrays_1<double>(smat.m_ex.n_row,smat.delsvec_ex,smat.svec_ex);
     check_array_for_nan_2(smat.m_ex.n_row,smat.svec_ex,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_startup_nonlinear_exc: svec_ex has a NaN" << endl;
       slv.flag_nr_converged = false;
       slv.flag_nr_norm_large = true;
       break;
     } else {
       dcmp_solvec_ex(xbe_lib,xbe_usr,ebe_lib,ebe_usr,smat,slv,cct);
       check_convergence_ex(smat,slv,ebe_lib,ebe_usr,cct);
       if (slv.flag_nr_converged) {
         break;
       }
     }
     if (slv.e_nr_flag_check_spice) {
       copy_array_1<double>(smat.m_ex.n_row,smat.svec_ex,smat.svec_old_nr_1_ex);
       copy_cur_nd_nr_1(ebe_lib,ebe_usr,cct);
     }
   }
   return;
} // end of solve_startup_nonlinear_exc
// -----------------------------------------------------------------------------
void solve_trns(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   if (cct.flag_e_only) {
     solve_trns_e(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (cct.flag_x_only) {
     if (cct.flag_x_explicit) {
       solve_trns_x_exp(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         slv,cct,cct_file,smat,global);
     } else {
       solve_trns_x_imp(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     }
   } else if (cct.flag_exc) {
     solve_trns_exc(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else {
     cout << "solve_trns: exs option not implemented. Halting..." << endl;
     exit(1);
   }

   return;
} // end of solve_trns
// -----------------------------------------------------------------------------
void solve_trns_e(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   init_sol_e(xbe_lib,xbe_usr,ebe_lib,ebe_usr,ebe_jac,slv,cct,smat,global);
   form_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

   smat.mat_trns_1_e(ebe_lib,ebe_usr,global,cct,cct_file);
   smat.w_e.allocate_1(smat.m_e.n_nz,smat.m_e.n_row,smat.m_e.n_col);
   smat.mo_e.allocate_1(smat.m_e.n_row);

   slv.e_algo_be0 = slv.e_algo_be || slv.e_algo_be_auto || slv.e_algo_be_const;
   slv.e_algo_trz0 = slv.e_algo_trz || slv.e_algo_trz_auto || slv.e_algo_trz_const;
   slv.e_algo_auto = slv.e_algo_be_auto || slv.e_algo_trz_auto;

   slv.e_algo_bdf2 = false;
   if (slv.e_algo_trbdf2) {
     slv.e_algo_trz0 = true;
   }
   for (int i_file=0; i_file < slv.n_outfile; i_file++) {
     if (slv.flag_out_delt_fixed[i_file]) {
       slv.out_tnext[i_file] = slv.out_tstart[i_file];
     }
   }

   if (slv.e_algo_be || slv.e_algo_be_const) {
     solve_trns_e_be(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.e_algo_trz || slv.e_algo_trz_const) {
     solve_trns_e_trz(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.e_algo_be_auto) {
     solve_trns_e_be_auto(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.e_algo_trz_auto) {
     solve_trns_e_trz_auto(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.e_algo_trbdf2) {
     solve_trns_e_trbdf2(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   }

   return;
} // end of solve_trns_e
// -----------------------------------------------------------------------------
void solve_trns_e_be(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached;

   flag_tend_reached = false;

   slv.write_iter_n1_e = 0;
   slv.iter_trns_e = -1;

   slv.time_present_e = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_e = slv.delt0_e;

   if (!slv.flag_const_tstep_e) {
     if (cct.flag_limit_tstep_e) {
       e_assign_nextbreak_1(ebe_usr,cct,global);
       ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);
       get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
     }
   }
   copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

   while (!flag_tend_reached) {
     slv.iter_trns_e++;
     global.iter_trns_e = slv.iter_trns_e;
     write_iter_e(slv);

     slv.time_next_e  = slv.time_present_e + slv.delt_e;
     global.time_given_e = slv.time_next_e;
     if (flag_nan(slv.time_next_e)) {
       cout << "solve_trns_e_be: slv.time_next_e is NAN. Halting..." << endl;
       exit(1);
     }
     slv.trns_constants_2_e();

     form_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

     if (cct.flag_linear_e) {
       solve_trns_linear_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_e(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.flag_write_solution) {
           write_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
         }
         cout << "solve_trns_e_be: N-R iterations did not converge." << endl;
         slv.write_flags_failed();
         cout << "  iter_trns_e =" << slv.iter_trns_e << endl;
         cout << "  time ="
           << scientific << setprecision(6)
           << slv.time_present_e << endl;
         cout << "  Halting..." << endl;
         exit(1);
       }
     }
     slv.time_present_e = slv.time_next_e;
     slv.time_write     = slv.time_next_e;

     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);

     copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
     ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

//   find the new time step if necessary

     slv.delt_e = slv.delt0_e;
     slv.delt_e = max(slv.delt_e,slv.delt_min_e);

     if (!slv.flag_const_tstep_e) {
       if (cct.flag_limit_tstep_e) {
         get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
       }
     }
     time_next_1 = slv.time_present_e + slv.delt_e;
     if (time_next_1 >= global.time_end) {
       slv.delt_e = global.time_end - slv.time_present_e + slv.delt_small;
       slv.delt_e = max(slv.delt_e,slv.delt_min_e);
     }
     if (cct.flag_save_history_e) {
       save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
     }

     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_e > slv.itmax_trns) {
         cout << "solve_trns_e_be: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_e+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   return;
} // end of solve_trns_e_be
// -----------------------------------------------------------------------------
void solve_trns_e_trz(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached;

   flag_tend_reached = false;

   slv.write_iter_n1_e = 0;
   slv.iter_trns_e = -1;

   slv.time_present_e = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_e = slv.delt0_e;

   if (!slv.flag_const_tstep_e) {
     if (cct.flag_limit_tstep_e) {
       e_assign_nextbreak_1(ebe_usr,cct,global);
       ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);

       get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
     }
   }
   copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

   slv.time_next_e = slv.time_present_e;
   global.time_given_e = slv.time_next_e;
   if (flag_nan(slv.time_next_e)) {
     cout << "solve_trns_e_trz: slv.time_next_e is NAN. Halting..." << endl;
     exit(1);
   }
   slv.trns_constants_2_e();
   find_functions_trns_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);

   while (!flag_tend_reached) {
     slv.iter_trns_e++;
     global.iter_trns_e = slv.iter_trns_e;
     write_iter_e(slv);

     slv.time_next_e  = slv.time_present_e + slv.delt_e;
     global.time_given_e = slv.time_next_e;

     form_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

     if (cct.flag_linear_e) {
       find_functions_trns_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
     }
     copy_func_to_old_e(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

     slv.trns_constants_2_e();

     if (cct.flag_linear_e) {
       solve_trns_linear_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_e(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.flag_write_solution) {
           write_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
         }
         cout << "solve_trns_e_trz: N-R iterations did not converge." << endl;
         slv.write_flags_failed();
         cout << "  iter_trns_e =" << slv.iter_trns_e << endl;
         cout << "  time ="
           << scientific << setprecision(6)
           << slv.time_present_e << endl;
         cout << "  Halting..." << endl;
         exit(1);
       }
     }
     slv.time_present_e = slv.time_next_e;
     slv.time_write     = slv.time_next_e;

     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);

     copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
     ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

//   find the new time step if necessary

     slv.delt_e = slv.delt0_e;
     slv.delt_e = max(slv.delt_e,slv.delt_min_e);

     if (!slv.flag_const_tstep_e) {
       if (cct.flag_limit_tstep_e) {
         get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
       }
     }
     time_next_1 = slv.time_present_e + slv.delt_e;
     if (time_next_1 >= global.time_end) {
       slv.delt_e = global.time_end - slv.time_present_e + slv.delt_small;
       slv.delt_e = max(slv.delt_e,slv.delt_min_e);
     }
     if (cct.flag_save_history_e) {
       save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
     }

     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_e > slv.itmax_trns) {
         cout << "solve_trns_e_trz: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_e+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   return;
} // end of solve_trns_e_trz
// -----------------------------------------------------------------------------
void solve_trns_e_be_auto(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached,flag_repeat_step;
   int iter_stepred;

   flag_tend_reached = false;
   flag_repeat_step = false;

   slv.write_iter_n1_e = 0;
   slv.iter_trns_e = -1;

   slv.time_present_e = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_e = slv.delt0_e;

   if (cct.flag_limit_tstep_e) {
     e_assign_nextbreak_1(ebe_usr,cct,global);
     ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);

     get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
   }
   copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

   while (!flag_tend_reached) {
     if (!flag_repeat_step) {
       slv.iter_trns_e++;
       global.iter_trns_e = slv.iter_trns_e;
       write_iter_e(slv);

       iter_stepred = 0;
     }
     flag_repeat_step = false;

     slv.time_next_e  = slv.time_present_e + slv.delt_e;
     global.time_given_e = slv.time_next_e;
     if (flag_nan(slv.time_next_e)) {
       cout << "solve_trns_e_be_auto: slv.time_next_e is NAN. Halting..." << endl;
       exit(1);
     }

     slv.trns_constants_2_e();

     form_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

     if (cct.flag_linear_e) {
       solve_trns_linear_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_e(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.delt_e == slv.delt_min_e) {
           cout << "solve_trns_e_be_auto: no convergence even with" << endl;
           cout << "  the smallest time step." << endl;
           slv.write_flags_failed();
           cout << "iter_trns_e=" << slv.iter_trns_e
                << ", time =" << slv.time_present_e << endl;
           cout << "  Halting..." << endl;
           exit(1);
         } else {
           iter_stepred++;
           if (iter_stepred > slv.itmax_stepred) {
             cout << "solve_trns_e_be_auto: iter_stepred has exceeded" << endl;
             cout << "  itmax_stepred." << endl;
             cout << "  iter_stepred=" << iter_stepred << endl;
             cout << "  itmax_stepred=" << slv.itmax_stepred << endl;
             cout << "  Halting..." << endl;
             exit(1);
           }
           slv.delt_e = slv.factor_stepdec*slv.delt_e;
           slv.delt_e = max(slv.delt_e,slv.delt_min_e);

           copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
           ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

           flag_repeat_step = true;
         }
       } else {
         if (slv.delt_e != slv.delt_max_e) {
           slv.delt_e = slv.factor_stepinc*slv.delt_e;
           slv.delt_e = min(slv.delt_e,slv.delt_max_e);
         }
         flag_repeat_step = false;
       }
     }
     if (!flag_repeat_step) {
       slv.time_present_e = slv.time_next_e;
       slv.time_write     = slv.time_next_e;

       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);

       copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
       ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

       slv.delt_e = max(slv.delt_e,slv.delt_min_e);

       if (cct.flag_limit_tstep_e) {
         get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
       }
       time_next_1 = slv.time_present_e + slv.delt_e;
       if (time_next_1 >= global.time_end) {
         slv.delt_e = global.time_end - slv.time_present_e + slv.delt_small;
         slv.delt_e = max(slv.delt_e,slv.delt_min_e);
       }
       if (cct.flag_save_history_e) {
         save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
       }
       if (slv.flag_limit_iter_trns) {
         if (slv.iter_trns_e > slv.itmax_trns) {
           cout << "solve_trns_e_be_auto: itmax_trns exceeded." << endl;
           break;
         }
       }
       if ((slv.time_present_e+slv.delt_small) >= global.time_end) {
         flag_tend_reached = true;
       }
     }
   }
   return;
} // end of solve_trns_e_be_auto
// -----------------------------------------------------------------------------
void solve_trns_e_trz_auto(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached,flag_repeat_step;
   int iter_stepred;

   flag_tend_reached = false;
   flag_repeat_step = false;

   slv.write_iter_n1_e = 0;
   slv.iter_trns_e = -1;

   slv.time_present_e = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_e = slv.delt0_e;

   if (cct.flag_limit_tstep_e) {
     e_assign_nextbreak_1(ebe_usr,cct,global);
     ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);

     get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
   }
   copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

   slv.time_next_e = slv.time_present_e;
   global.time_given_e = slv.time_next_e;
   if (flag_nan(slv.time_next_e)) {
     cout << "solve_trns_e_trz_auto: slv.time_next_e is NAN. Halting..." << endl;
     exit(1);
   }
   slv.trns_constants_2_e();
   find_functions_trns_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);

   while (!flag_tend_reached) {
     if (!flag_repeat_step) {
       slv.iter_trns_e++;
       global.iter_trns_e = slv.iter_trns_e;
       write_iter_e(slv);

       iter_stepred = 0;
     }
     flag_repeat_step = false;

     slv.time_next_e  = slv.time_present_e + slv.delt_e;
     global.time_given_e = slv.time_next_e;

     form_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

     if (cct.flag_linear_e) {
       find_functions_trns_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
     }
     copy_func_to_old_e(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);
     slv.trns_constants_2_e();

     if (cct.flag_linear_e) {
       solve_trns_linear_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_e(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.delt_e == slv.delt_min_e) {
           cout << "solve_trns_e_trzauto: no convergence even with" << endl;
           cout << "  the smallest time step." << endl;
           slv.write_flags_failed();
           cout << "iter_trns_e=" << slv.iter_trns_e
                << ", time =" << slv.time_present_e << endl;
           cout << "  Halting..." << endl;
           exit(1);
         } else {
           iter_stepred++;
           if (iter_stepred > slv.itmax_stepred) {
             cout << "solve_trns_e_trzauto: iter_stepred has exceeded" << endl;
             cout << "  itmax_stepred." << endl;
             cout << "  iter_stepred=" << iter_stepred << endl;
             cout << "  itmax_stepred=" << slv.itmax_stepred << endl;
             cout << "  Halting..." << endl;
             exit(1);
           }
           slv.delt_e = slv.factor_stepdec*slv.delt_e;
           slv.delt_e = max(slv.delt_e,slv.delt_min_e);

           copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
           ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

           flag_repeat_step = true;
         }
       } else {
         if (slv.delt_e != slv.delt_max_e) {
           slv.delt_e = slv.factor_stepinc*slv.delt_e;
           slv.delt_e = min(slv.delt_e,slv.delt_max_e);
         }
         flag_repeat_step = false;
       }
     }
     if (!flag_repeat_step) {
       slv.time_present_e = slv.time_next_e;
       slv.time_write     = slv.time_next_e;

       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);

       copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
       ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

       slv.delt_e = max(slv.delt_e,slv.delt_min_e);

       if (cct.flag_limit_tstep_e) {
         get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
       }
       time_next_1 = slv.time_present_e + slv.delt_e;
       if (time_next_1 >= global.time_end) {
         slv.delt_e = global.time_end - slv.time_present_e + slv.delt_small;
         slv.delt_e = max(slv.delt_e,slv.delt_min_e);
       }
       if (cct.flag_save_history_e) {
         save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
       }
       if (slv.flag_limit_iter_trns) {
         if (slv.iter_trns_e > slv.itmax_trns) {
           cout << "solve_trns_e_trz_auto: itmax_trns exceeded." << endl;
           break;
         }
       }
       if ((slv.time_present_e+slv.delt_small) >= global.time_end) {
         flag_tend_reached = true;
       }
     }
   }
   return;
} // end of solve_trns_e_trz_auto
// -----------------------------------------------------------------------------
void solve_trns_e_trbdf2(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double delt_tmp,t_present_tmp,time_next_1;
   bool flag_tend_reached,flag_repeat_step;
   int iter_trbdf2,iter_trz,iter_bdf2,n_reject;

   slv.e_algo_trz0 = true;

   flag_tend_reached = false;
   flag_repeat_step = false;

   slv.write_iter_n1_e = 0;
   slv.iter_trns_e = -1;

   slv.time_present_e = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_e = slv.delt0_e;

   if (cct.flag_limit_tstep_e) {
     e_assign_nextbreak_1(ebe_usr,cct,global);
     ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);

     get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
   }
   copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

   slv.trns_constants_2_e();
   find_functions_trns_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);

   while (!flag_tend_reached) {
     if (!flag_repeat_step) {
       slv.iter_trns_e++;
       global.iter_trns_e = slv.iter_trns_e;
       write_iter_e(slv);

       iter_trbdf2 = 0;
       iter_trz = 0;
       iter_bdf2 = 0;
       n_reject = 0;
     }
     flag_repeat_step = false;

     iter_trbdf2++;
     if (iter_trbdf2 > slv.itmax_trbdf2) {
       cout << "solve_trns_e_trbdf2: iter_trbdf2 has exceeded itmax_trbdf2" << endl;
       cout << "  iter_trbdf2=" << iter_trbdf2 << endl;
       cout << "  itmax_trbdf2=" << slv.itmax_trbdf2 << endl;
       cout << "  n_reject=" << n_reject << endl;
       cout << "  time_given=" << global.time_given_e << endl;
       cout << "  Halting..." << endl; exit(1);
     }
     delt_tmp = slv.delt_e;
     t_present_tmp = slv.time_present_e;

     slv.delt_e = slv.bank_gamma*delt_tmp;
     slv.time_next_e  = slv.time_present_e + slv.delt_e;
     global.time_given_e = slv.time_next_e;
     if (flag_nan(slv.time_next_e)) {
       cout << "solve_trns_e_trbdf2: slv.time_next_e is NAN. Halting..." << endl;
       exit(1);
     }

//   solve trz part:

     slv.e_algo_trz0 = true;
     slv.e_algo_bdf2 = false;
     iter_trz++;

     copy_func_to_old_e(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);
     slv.trns_constants_2_e();

     form_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

     if (cct.flag_linear_e) {
       solve_trns_linear_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
       find_functions_trns_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_e(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.delt_e == slv.delt_min_e) {
           cout << "solve_trns_e_trbdf2: no convergence even with" << endl;
           cout << "  the smallest time step." << endl;
           slv.write_flags_failed();
           cout << "  iter_trz=" << iter_trz << endl;
           cout << "  iter_trbdf2=" << iter_trbdf2 << endl;
           cout << "  n_reject=" << n_reject << endl;
           cout << "  iter_trns_e="   << slv.iter_trns_e << endl;
           cout << "  Halting..." << endl;
           exit(1);
         }
         slv.delt_e = slv.factor_stepdec*delt_tmp;
         slv.delt_e = max(slv.delt_e,slv.delt_min_e);

         copy_array_1<double>(smat.n_solvec_e,smat.svec_old_1_e,smat.svec_e);
         ebeu_copy_stv_1(global.I_COPY_1_TO_0,ebe_lib,ebe_usr,cct,global);

         flag_repeat_step = true;
       }
     }

     if (!flag_repeat_step) {
       copy_array_1<double>(smat.n_solvec_e,smat.svec_old_1_e,smat.svec_old_2_e);
       copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);

       ebeu_copy_stv_1(global.I_COPY_1_TO_2,ebe_lib,ebe_usr,cct,global);
       ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

       copy_func_to_old_e(global.I_COPY_1_TO_2,ebe_lib,ebe_usr,cct,global);
       copy_func_to_old_e(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

//     solve bdf2 part:

       slv.e_algo_trz0 = false;
       slv.e_algo_bdf2 = true;
       iter_bdf2++;

       slv.time_next_e  = t_present_tmp + delt_tmp;
       global.time_given_e = slv.time_next_e;
       slv.delt_e = delt_tmp;
       slv.trns_constants_2_e();

       form_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

       if (cct.flag_linear_e) {
         solve_trns_linear_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
         find_functions_trns_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
       } else {
         solve_trns_nonlinear_e(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,
           smat,cct,slv,global);

         if (!slv.flag_nr_converged) {
           if (slv.delt_e == slv.delt_min_e) {
             cout << "solve_trns_e_trbdf2: no convergence even with" << endl;
             cout << "  the smallest time step." << endl;
             slv.write_flags_failed();
             cout << "  iter_trz=" << iter_trz << endl;
             cout << "  iter_trbdf2=" << iter_trbdf2 << endl;
             cout << "  n_reject=" << n_reject << endl;
             cout << "  iter_trns_e=" << slv.iter_trns_e << endl;
             cout << "  Halting..." << endl;
             exit(1);
           }
           slv.delt_e = slv.factor_stepdec*delt_tmp;
           slv.delt_e = max(slv.delt_e,slv.delt_min_e);

           copy_array_1<double>(smat.n_solvec_e,smat.svec_old_2_e,smat.svec_old_1_e);
           copy_array_1<double>(smat.n_solvec_e,smat.svec_old_2_e,smat.svec_e);

           ebeu_copy_stv_1(global.I_COPY_2_TO_1,ebe_lib,ebe_usr,cct,global);
           ebeu_copy_stv_1(global.I_COPY_2_TO_0,ebe_lib,ebe_usr,cct,global);

           flag_repeat_step = true;
         }
       }
     }
     if (!flag_repeat_step) {
//     check whether the solution should be accepted
       trzbdf2_1_e(ebe_lib,ebe_usr,smat,cct,slv);

       slv.delt_e = slv.delt_new_e;
       slv.delt_e = max(slv.delt_e,slv.delt_min_e);
       slv.delt_e = min(slv.delt_e,slv.delt_max_e);

       if (!slv.flag_accept_sol) {
         n_reject++;

         copy_array_1<double>(smat.n_solvec_e,smat.svec_old_2_e,smat.svec_old_1_e);
         copy_array_1<double>(smat.n_solvec_e,smat.svec_old_2_e,smat.svec_e);

         ebeu_copy_stv_1(global.I_COPY_2_TO_1,ebe_lib,ebe_usr,cct,global);
         ebeu_copy_stv_1(global.I_COPY_2_TO_0,ebe_lib,ebe_usr,cct,global);

         copy_func_to_old_e(global.I_COPY_2_TO_0,ebe_lib,ebe_usr,cct,global);
         dcmp_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

         flag_repeat_step = true;
       }
     }
     if (!flag_repeat_step) {
       slv.time_present_e = slv.time_next_e;
       slv.time_write     = slv.time_next_e;

       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);

       copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
       ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

       slv.delt_e = max(slv.delt_e,slv.delt_min_e);

       if (cct.flag_limit_tstep_e) {
         get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
       }
       time_next_1 = slv.time_present_e + slv.delt_e;
       if (time_next_1 >= global.time_end) {
         slv.delt_e = global.time_end - slv.time_present_e + slv.delt_small;
         slv.delt_e = max(slv.delt_e,slv.delt_min_e);
       }
       if (cct.flag_save_history_e) {
         save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
       }
       if (slv.flag_limit_iter_trns) {
         if (slv.iter_trns_e > slv.itmax_trns) {
           cout << "solve_trns_e_trbdf2: itmax_trns exceeded." << endl;
           break;
         }
       }
       if ((slv.time_present_e+slv.delt_small) >= global.time_end) {
         flag_tend_reached = true;
       }
     }
   }
   return;
} // end of solve_trns_e_trbdf2
// -----------------------------------------------------------------------------
void solve_trns_linear_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   bool flag_write;

   form_jac_rhs_trns_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);

   add_trns_terms_e(ebe_usr,smat,slv);

   negative_double_1(smat.m_e.n_row,smat.rhs_m_e);

   if ((slv.iter_trns_e == 0) && (!slv.e_algo_bdf2)) {
     solve_jac_1_e(smat,slv,global);
   } else {
     solve_jac_2_e(smat,slv,global);
   }

   copy_array_1<double>(smat.m_e.n_row,smat.svec_orig_e,smat.delsvec_e);

   add_arrays_1<double>(smat.m_e.n_row,smat.delsvec_e,smat.svec_e);
   dcmp_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   find_ebe_cur_trns_e(true,ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);

   return;
} // end of solve_trns_linear_e
// -----------------------------------------------------------------------------
void solve_trns_nonlinear_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   bool flag_nan_1,flag_write;
   int i_newt;

   check_convergence_count_e(slv);
   slv.get_dmp(cct);

   for (i_newt=0; i_newt < slv.e_nr_itermax_a; i_newt++) {
     slv.iter_newton = i_newt;

     form_jac_rhs_trns_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
     add_trns_terms_e(ebe_usr,smat,slv);

     negative_double_1(smat.m_e.n_row,smat.rhs_m_e);

     if (slv.iter_newton == 0) {
       if ((slv.iter_trns_e == 0) && (!slv.e_algo_bdf2)) {
         solve_jac_1_e(smat,slv,global);
       } else {
         solve_jac_2_e(smat,slv,global);
       }
     } else {
       solve_jac_2_e(smat,slv,global);
     }

     copy_array_1<double>(smat.m_e.n_row,smat.svec_orig_e,smat.delsvec_e);
     if ((slv.e_nr_flag_dmp_a) && (i_newt <= slv.e_nr_dmp_itermax_a)) {
       mult_array_1<double>(smat.m_e.n_row,smat.delsvec_e,slv.e_nr_dmp_k_a);
     }

     add_arrays_1<double>(smat.m_e.n_row,smat.delsvec_e,smat.svec_e);

     check_array_for_nan_2(smat.m_e.n_row,smat.svec_e,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_trns_nonlinear_e: svec_e has a NaN" << endl;
       slv.flag_nr_converged = false;
       slv.flag_nr_norm_large = true;
       break;
     } else {
       dcmp_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);
       if (slv.e_nr_flag_check_spice) {
         find_ebe_cur_trns_e(true,ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
       }
       check_convergence_e(smat,slv,ebe_lib,ebe_usr,cct);
       if (slv.flag_nr_converged) {
         break;
       }
       if (slv.e_nr_flag_check_spice) {
         copy_array_1<double>(smat.m_e.n_row,smat.svec_e,smat.svec_old_nr_1_e);
         copy_cur_nd_nr_1(ebe_lib,ebe_usr,cct);
       }
     }
   }
   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   find_ebe_cur_trns_e(true,ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);

   return;
} // end of solve_trns_nonlinear_e
// -----------------------------------------------------------------------------
void solve_trns_x_imp(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   init_sol_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,slv,cct,smat,global);

   form_solvec_x(xbe_lib,xbe_usr,smat,cct);
   form_map_xbeuvr_1(smat,cct);

   smat.mat_trns_1_x(xbe_lib,xbe_usr,cct,global,cct_file);
   smat.w_x.allocate_1(smat.m_x.n_nz,smat.m_x.n_row,smat.m_x.n_col);
   smat.mo_x.allocate_1(smat.m_x.n_row);

   slv.x_algo_bdf2 = false;

   slv.x_algo_be0 = slv.x_algo_be || slv.x_algo_be_auto || slv.x_algo_be_const;
   slv.x_algo_trz0 = slv.x_algo_trz || slv.x_algo_trz_auto || slv.x_algo_trz_const;
   slv.x_algo_auto = slv.x_algo_be_auto || slv.x_algo_trz_auto;

   for (int i_file=0; i_file < slv.n_outfile; i_file++) {
     if (slv.flag_out_delt_fixed[i_file]) {
       slv.out_tnext[i_file] = slv.out_tstart[i_file];
     }
   }

   if (slv.x_algo_be || slv.x_algo_be_const) {
     solve_trns_x_be(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.x_algo_trz || slv.x_algo_trz_const) {
     solve_trns_x_trz(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.x_algo_be_auto) {
     solve_trns_x_be_auto(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.x_algo_trz_auto) {
     solve_trns_x_trz_auto(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.x_algo_trbdf2) {
     solve_trns_x_trbdf2(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   }

   return;
} // end of solve_trns_x_imp
// -----------------------------------------------------------------------------
void solve_trns_x_be(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached;

   flag_tend_reached = false;

   slv.write_iter_n1_x = 0;
   slv.iter_trns_x = -1;

   slv.time_present_x = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_x = slv.delt0_x;

   if (!slv.flag_const_tstep_x) {
     if (cct.flag_limit_tstep_x) {
       x_assign_nextbreak_1(xbe_usr,cct,global);
       xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);

       get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
     }
   }
   copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);

   while (!flag_tend_reached) {
     slv.iter_trns_x++;
     global.iter_trns_x = slv.iter_trns_x;
     write_iter_x(slv);

     if (slv.iter_trns_x != 0) {
       if (cct.flag_modulo_x) {
         xbe_modulo_implicit(cct,xbe_lib,xbe_usr,smat);
       }
     }
     slv.time_next_x  = slv.time_present_x + slv.delt_x;
     global.time_given_x = slv.time_next_x;
     if (flag_nan(slv.time_next_x)) {
       cout << "solve_trns_x_be: slv.time_next_x is NAN. Halting..." << endl;
       exit(1);
     }
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     slv.trns_constants_2_x();

     form_solvec_x(xbe_lib,xbe_usr,smat,cct);

     if (cct.flag_linear_x) {
       solve_trns_linear_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);

     } else {
       solve_trns_nonlinear_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.flag_write_solution) {
           write_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
         }
         cout << "solve_trns_x_be: N-R iterations did not converge." << endl;
         slv.write_flags_failed();
         cout << "  iter_trns_x =" << slv.iter_trns_x << endl;
         cout << "  time ="
           << scientific << setprecision(6)
           << slv.time_present_x << endl;
         cout << "  Halting..." << endl;
         exit(1);
       }
     }
     slv.time_present_x = slv.time_next_x;
     slv.time_write     = slv.time_next_x;

     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);

     slv.delt_x = slv.delt0_x;
     slv.delt_x = max(slv.delt_x,slv.delt_min_x);

     if (!slv.flag_const_tstep_x) {
       if (cct.flag_limit_tstep_x) {
         get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
       }
     }
     time_next_1 = slv.time_present_x + slv.delt_x;
     if (time_next_1 >= global.time_end) {
       slv.delt_x = global.time_end - slv.time_present_x + slv.delt_small;
       slv.delt_x = max(slv.delt_x,slv.delt_min_x);
     }
     if (cct.flag_reset_x) {
       xbe_reset_1(true,xbe_lib,xbe_usr,xbe_jac,cct,smat,global);
     }
     copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);

     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_x > slv.itmax_trns) {
         cout << "solve_trns_x_be: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_x+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   return;
} // end of solve_trns_x_be
// -----------------------------------------------------------------------------
void solve_trns_x_trz(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached;

   flag_tend_reached = false;

   slv.write_iter_n1_x = 0;
   slv.iter_trns_x = -1;

   slv.time_present_x = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_x = slv.delt0_x;

   if (!slv.flag_const_tstep_x) {
     if (cct.flag_limit_tstep_x) {
       x_assign_nextbreak_1(xbe_usr,cct,global);
       xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);
       get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
     }
   }
   copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);

   slv.time_next_x = slv.time_present_x;
   global.time_given_x = slv.time_next_x;
   if (flag_nan(slv.time_next_x)) {
     cout << "solve_trns_x_trz: slv.time_next_x is NAN. Halting..." << endl;
     exit(1);
   }
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }
   slv.trns_constants_2_x();
   find_functions_trns_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   while (!flag_tend_reached) {
     slv.iter_trns_x++;
     global.iter_trns_x = slv.iter_trns_x;
     write_iter_x(slv);

     if (slv.iter_trns_x != 0) {
       if (cct.flag_modulo_x) {
         xbe_modulo_implicit(cct,xbe_lib,xbe_usr,smat);
       }
     }
     slv.time_next_x  = slv.time_present_x + slv.delt_x;
     global.time_given_x = slv.time_next_x;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     form_solvec_x(xbe_lib,xbe_usr,smat,cct);

     if (cct.flag_linear_x) {
       find_functions_trns_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);
     }
     copy_func_to_old_x(global.I_COPY_0_TO_1,xbe_lib,xbe_usr,cct,global);
     slv.trns_constants_2_x();

     if (cct.flag_linear_x) {
       solve_trns_linear_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.flag_write_solution) {
           write_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
         }
         cout << "solve_trns_x_trz: N-R iterations did not converge." << endl;
         slv.write_flags_failed();
         cout << "  iter_trns_x =" << slv.iter_trns_x << endl;
         cout << "  time ="
           << scientific << setprecision(6)
           << slv.time_present_x << endl;
         cout << "  Halting..." << endl;
         exit(1);
       }
     }
     slv.time_present_x = slv.time_next_x;
     slv.time_write     = slv.time_next_x;

     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);

//   find the new time step if necessary

     slv.delt_x = slv.delt0_x;
     slv.delt_x = max(slv.delt_x,slv.delt_min_x);

     if (!slv.flag_const_tstep_x) {
       if (cct.flag_limit_tstep_x) {
         get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
       }
     }
     time_next_1 = slv.time_present_x + slv.delt_x;
     if (time_next_1 >= global.time_end) {
       slv.delt_x = global.time_end - slv.time_present_x + slv.delt_small;
       slv.delt_x = max(slv.delt_x,slv.delt_min_x);
     }
     if (cct.flag_reset_x) {
       xbe_reset_1(true,xbe_lib,xbe_usr,xbe_jac,cct,smat,global);
     }
     copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);
     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_x > slv.itmax_trns) {
         cout << "solve_trns_e_trz: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_x+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   return;
} // end of solve_trns_x_trz
// -----------------------------------------------------------------------------
void solve_trns_x_be_auto(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached,flag_repeat_step;
   int iter_stepred;

   flag_tend_reached = false;
   flag_repeat_step = false;

   slv.write_iter_n1_x = 0;
   slv.iter_trns_x = -1;

   slv.time_present_x = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_x = slv.delt0_x;

   if (cct.flag_limit_tstep_x) {
     x_assign_nextbreak_1(xbe_usr,cct,global);
     xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);

     get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
   }
   copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);

   while (!flag_tend_reached) {
     if (!flag_repeat_step) {
       slv.iter_trns_x++;
       global.iter_trns_x = slv.iter_trns_x;
       write_iter_x(slv);

       if (slv.iter_trns_x != 0) {
         if (cct.flag_modulo_x) {
           xbe_modulo_implicit(cct,xbe_lib,xbe_usr,smat);
         }
       }
       iter_stepred = 0;
     }
     flag_repeat_step = false;

     slv.time_next_x  = slv.time_present_x + slv.delt_x;
     global.time_given_x = slv.time_next_x;
     if (flag_nan(slv.time_next_x)) {
       cout << "solve_trns_x_be_auto: slv.time_next_x is NAN. Halting..." << endl;
       exit(1);
     }
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     slv.trns_constants_2_x();

     form_solvec_x(xbe_lib,xbe_usr,smat,cct);

     if (cct.flag_linear_x) {
       solve_trns_linear_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.delt_x == slv.delt_min_x) {
           cout << "solve_trns_x_be_auto: no convergence even with" << endl;
           cout << "  the smallest time step." << endl;
           slv.write_flags_failed();
           cout << "iter_trns_x=" << slv.iter_trns_x
                << ", time =" << slv.time_present_x << endl;
           cout << "  Halting..." << endl;
           exit(1);
         } else {
           iter_stepred++;
           if (iter_stepred > slv.itmax_stepred) {
             cout << "solve_trns_x_be_auto: iter_stepred has exceeded" << endl;
             cout << "  itmax_stepred." << endl;
             cout << "  iter_stepred=" << iter_stepred << endl;
             cout << "  itmax_stepred=" << slv.itmax_stepred << endl;
             cout << "  Halting..." << endl;
             exit(1);
           }
           slv.delt_x = slv.factor_stepdec*slv.delt_x;
           slv.delt_x = max(slv.delt_x,slv.delt_min_x);

           copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);
           flag_repeat_step = true;
         }
       } else {
         if (slv.delt_x != slv.delt_max_x) {
           slv.delt_x = slv.factor_stepinc*slv.delt_x;
           slv.delt_x = min(slv.delt_x,slv.delt_max_x);
         }
         flag_repeat_step = false;
       }
     }
     if (!flag_repeat_step) {
       slv.time_present_x = slv.time_next_x;
       slv.time_write     = slv.time_next_x;

       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);

       slv.delt_x = max(slv.delt_x,slv.delt_min_x);

       if (cct.flag_limit_tstep_x) {
         get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
       }
       time_next_1 = slv.time_present_x + slv.delt_x;
       if (time_next_1 >= global.time_end) {
         slv.delt_x = global.time_end - slv.time_present_x + slv.delt_small;
         slv.delt_x = max(slv.delt_x,slv.delt_min_x);
       }
       if (cct.flag_reset_x) {
         xbe_reset_1(true,xbe_lib,xbe_usr,xbe_jac,cct,smat,global);
       }
       copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);
       if (cct.flag_save_history_x) {
         save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
       if (slv.flag_limit_iter_trns) {
         if (slv.iter_trns_x > slv.itmax_trns) {
           cout << "solve_trns_e_be_auto: itmax_trns exceeded." << endl;
           break;
         }
       }
       if ((slv.time_present_x+slv.delt_small) >= global.time_end) {
         flag_tend_reached = true;
       }
     }
   }
   return;
} // end of solve_trns_x_be_auto
// -----------------------------------------------------------------------------
void solve_trns_x_trz_auto(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

// todo check from here for cout (include skip/end skip)
   double time_next_1;
   bool flag_tend_reached,flag_repeat_step;
   int iter_stepred;

   flag_tend_reached = false;
   flag_repeat_step = false;

   slv.write_iter_n1_x = 0;
   slv.iter_trns_x = -1;

   slv.time_present_x = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_x = slv.delt0_x;

   if (cct.flag_limit_tstep_x) {
     x_assign_nextbreak_1(xbe_usr,cct,global);
     xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);

     get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
   }
   copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);

   slv.time_next_x = slv.time_present_x;
   global.time_given_x = slv.time_next_x;
   if (flag_nan(slv.time_next_x)) {
     cout << "solve_trns_x_trz_auto: slv.time_next_x is NAN. Halting..." << endl;
     exit(1);
   }
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }
   slv.trns_constants_2_x();
   find_functions_trns_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   while (!flag_tend_reached) {
     if (!flag_repeat_step) {
       slv.iter_trns_x++;
       global.iter_trns_x = slv.iter_trns_x;
       write_iter_x(slv);

       if (slv.iter_trns_x != 0) {
         if (cct.flag_modulo_x) {
           xbe_modulo_implicit(cct,xbe_lib,xbe_usr,smat);
         }
       }
       iter_stepred = 0;
     }
     flag_repeat_step = false;

     slv.time_next_x  = slv.time_present_x + slv.delt_x;
     global.time_given_x = slv.time_next_x;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     form_solvec_x(xbe_lib,xbe_usr,smat,cct);

     if (cct.flag_linear_x) {
       find_functions_trns_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);
     }
     copy_func_to_old_x(global.I_COPY_0_TO_1,xbe_lib,xbe_usr,cct,global);
     slv.trns_constants_2_x();

     if (cct.flag_linear_x) {
       solve_trns_linear_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.delt_x == slv.delt_min_x) {
           cout << "solve_trns_x_trzauto: no convergence even with" << endl;
           cout << "  the smallest time step." << endl;
           slv.write_flags_failed();
           cout << "iter_trns_x=" << slv.iter_trns_x
                << ", time =" << slv.time_present_x << endl;
           cout << "  Halting..." << endl;
           exit(1);
         } else {
           iter_stepred++;
           if (iter_stepred > slv.itmax_stepred) {
             cout << "solve_trns_x_trzauto: iter_stepred has exceeded" << endl;
             cout << "  itmax_stepred." << endl;
             cout << "  iter_stepred=" << iter_stepred << endl;
             cout << "  itmax_stepred=" << slv.itmax_stepred << endl;
             cout << "  Halting..." << endl;
             exit(1);
           }
           slv.delt_x = slv.factor_stepdec*slv.delt_x;
           slv.delt_x = max(slv.delt_x,slv.delt_min_x);

           copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);

           flag_repeat_step = true;
         }
       } else {
         if (slv.delt_x != slv.delt_max_x) {
           slv.delt_x = slv.factor_stepinc*slv.delt_x;
           slv.delt_x = min(slv.delt_x,slv.delt_max_x);
         }
         flag_repeat_step = false;
       }
     }
     if (!flag_repeat_step) {
       slv.time_present_x = slv.time_next_x;
       slv.time_write     = slv.time_next_x;

       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);

       slv.delt_x = max(slv.delt_x,slv.delt_min_x);

       if (cct.flag_limit_tstep_x) {
         get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
       }
       time_next_1 = slv.time_present_x + slv.delt_x;
       if (time_next_1 >= global.time_end) {
         slv.delt_x = global.time_end - slv.time_present_x + slv.delt_small;
         slv.delt_x = max(slv.delt_x,slv.delt_min_x);
       }
       if (cct.flag_reset_x) {
         xbe_reset_1(true,xbe_lib,xbe_usr,xbe_jac,cct,smat,global);
       }
       copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);
       if (cct.flag_save_history_x) {
         save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
       if (slv.flag_limit_iter_trns) {
         if (slv.iter_trns_x > slv.itmax_trns) {
           cout << "solve_trns_e_trz_auto: itmax_trns exceeded." << endl;
           break;
         }
       }
       if ((slv.time_present_x+slv.delt_small) >= global.time_end) {
         flag_tend_reached = true;
       }
     }
   }
   return;
} // end of solve_trns_x_trz_auto
// -----------------------------------------------------------------------------
void solve_trns_x_trbdf2(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double delt_tmp,t_present_tmp,time_next_1;
   bool flag_tend_reached,flag_repeat_step;
   int iter_trbdf2,iter_trz,iter_bdf2,n_reject;

   slv.x_algo_trz0 = true;

   flag_tend_reached = false;
   flag_repeat_step = false;

   slv.write_iter_n1_x = 0;
   slv.iter_trns_x = -1;

   slv.time_present_x = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_x = slv.delt0_x;

   if (cct.flag_limit_tstep_x) {
     x_assign_nextbreak_1(xbe_usr,cct,global);
     xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);

     get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
   }
   copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);

   slv.trns_constants_2_x();
   find_functions_trns_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   while (!flag_tend_reached) {
     if (!flag_repeat_step) {
       slv.iter_trns_x++;
       global.iter_trns_x = slv.iter_trns_x;
       write_iter_x(slv);

       if (slv.iter_trns_x != 0) {
         if (cct.flag_modulo_x) {
           xbe_modulo_implicit(cct,xbe_lib,xbe_usr,smat);
         }
       }

       iter_trbdf2 = 0;
       iter_trz = 0;
       iter_bdf2 = 0;
       n_reject = 0;
     }
     flag_repeat_step = false;

     iter_trbdf2++;
     if (iter_trbdf2 > slv.itmax_trbdf2) {
       cout << "solve_trns_x_trbdf2: iter_trbdf2 has exceeded itmax_trbdf2" << endl;
       cout << "  iter_trbdf2=" << iter_trbdf2 << endl;
       cout << "  itmax_trbdf2=" << slv.itmax_trbdf2 << endl;
       cout << "  n_reject=" << n_reject << endl;
       cout << "  time_given=" << global.time_given_x << endl;
       cout << "  Halting..." << endl; exit(1);
     }
     delt_tmp = slv.delt_x;
     t_present_tmp = slv.time_present_x;

     slv.delt_x = slv.bank_gamma*delt_tmp;
     slv.time_next_x  = slv.time_present_x + slv.delt_x;
     global.time_given_x = slv.time_next_x;
     if (flag_nan(slv.time_next_x)) {
       cout << "solve_trns_x_trbdf2: slv.time_next_x is NAN. Halting..." << endl;
       exit(1);
     }
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

//   solve trz part:

     slv.x_algo_trz0 = true;
     slv.x_algo_bdf2 = false;
     iter_trz++;

     copy_func_to_old_x(global.I_COPY_0_TO_1,xbe_lib,xbe_usr,cct,global);
     slv.trns_constants_2_x();

     form_solvec_x(xbe_lib,xbe_usr,smat,cct);

     if (cct.flag_linear_x) {

       solve_trns_linear_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);

       find_functions_trns_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);
     } else {
       solve_trns_nonlinear_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.delt_x == slv.delt_min_x) {
           cout << "solve_trns_x_trbdf2: no convergence even with" << endl;
           cout << "  the smallest time step." << endl;
           slv.write_flags_failed();
           cout << "  iter_trz=" << iter_trz << endl;
           cout << "  iter_trbdf2=" << iter_trbdf2 << endl;
           cout << "  n_reject=" << n_reject << endl;
           cout << "  iter_trns_x="   << slv.iter_trns_x << endl;
           cout << "  Halting..." << endl;
           exit(1);
         }

         slv.delt_x = slv.factor_stepdec*delt_tmp;

         slv.delt_x = max(slv.delt_x,slv.delt_min_x);

         copy_array_1<double>(smat.n_solvec_x,smat.svec_old_1_x,smat.svec_x);
         flag_repeat_step = true;
       }
     }

     if (!flag_repeat_step) {
       copy_array_1<double>(smat.n_solvec_x,smat.svec_old_1_x,smat.svec_old_2_x);
       copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);

       copy_func_to_old_x(global.I_COPY_1_TO_2,xbe_lib,xbe_usr,cct,global);
       copy_func_to_old_x(global.I_COPY_0_TO_1,xbe_lib,xbe_usr,cct,global);

//     solve bdf2 part:

       slv.x_algo_trz0 = false;
       slv.x_algo_bdf2 = true;
       iter_bdf2++;

       slv.time_next_x  = t_present_tmp + delt_tmp;
       global.time_given_x = slv.time_next_x;
       if (cct.flag_time_parms) {
         xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
       slv.delt_x = delt_tmp;
       slv.trns_constants_2_x();

       form_solvec_x(xbe_lib,xbe_usr,smat,cct);

       if (cct.flag_linear_x) {

         solve_trns_linear_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,slv,global);

         find_functions_trns_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);
       } else {
         solve_trns_nonlinear_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,
           smat,cct,slv,global);

         if (!slv.flag_nr_converged) {
           if (slv.delt_x == slv.delt_min_x) {
             cout << "solve_trns_x_trbdf2: no convergence even with" << endl;
             cout << "  the smallest time step." << endl;
             slv.write_flags_failed();
             cout << "  iter_trz=" << iter_trz << endl;
             cout << "  iter_trbdf2=" << iter_trbdf2 << endl;
             cout << "  n_reject=" << n_reject << endl;
             cout << "  iter_trns_x=" << slv.iter_trns_x << endl;
             cout << "  Halting..." << endl;
             exit(1);
           }
           slv.delt_x = slv.factor_stepdec*delt_tmp;
           slv.delt_x = max(slv.delt_x,slv.delt_min_x);

           copy_array_1<double>(smat.n_solvec_x,smat.svec_old_2_x,smat.svec_old_1_x);
           copy_array_1<double>(smat.n_solvec_x,smat.svec_old_2_x,smat.svec_x);

           flag_repeat_step = true;
         }
       }
     }
     if (!flag_repeat_step) {
//     check whether the solution should be accepted

       trzbdf2_1_x(xbe_usr,smat,slv);

       slv.delt_x = slv.delt_new_x;

       slv.delt_x = max(slv.delt_x,slv.delt_min_x);
       slv.delt_x = min(slv.delt_x,slv.delt_max_x);

       if (!slv.flag_accept_sol) {
         n_reject++;

         copy_array_1<double>(smat.n_solvec_x,smat.svec_old_2_x,smat.svec_old_1_x);
         copy_array_1<double>(smat.n_solvec_x,smat.svec_old_2_x,smat.svec_x);

         copy_func_to_old_x(global.I_COPY_2_TO_0,xbe_lib,xbe_usr,cct,global);
         dcmp_solvec_x(xbe_lib,xbe_usr,smat,cct);

         flag_repeat_step = true;
       }
     }
     if (!flag_repeat_step) {
       slv.time_present_x = slv.time_next_x;
       slv.time_write     = slv.time_next_x;

       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);

//     copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);

       slv.delt_x = max(slv.delt_x,slv.delt_min_x);

       if (cct.flag_limit_tstep_x) {

         get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);

       }
       time_next_1 = slv.time_present_x + slv.delt_x;
       if (time_next_1 >= global.time_end) {
         slv.delt_x = global.time_end - slv.time_present_x + slv.delt_small;
         slv.delt_x = max(slv.delt_x,slv.delt_min_x);
       }
       if (cct.flag_reset_x) {
         xbe_reset_1(true,xbe_lib,xbe_usr,xbe_jac,cct,smat,global);
       }
       copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);
       if (cct.flag_save_history_x) {
         save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
       if (slv.flag_limit_iter_trns) {
         if (slv.iter_trns_x > slv.itmax_trns) {
           cout << "solve_trns_x_trbdf2: itmax_trns exceeded." << endl;
           break;
         }
       }
       if ((slv.time_present_x+slv.delt_small) >= global.time_end) {
         flag_tend_reached = true;
       }
     }
   }
   return;
} // end of solve_trns_x_trbdf2
// -----------------------------------------------------------------------------
void solve_trns_linear_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   bool flag_write;

   form_jac_rhs_trns_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);
   add_trns_terms_x(xbe_usr,smat,slv);

   negative_double_1(smat.m_x.n_row,smat.rhs_m_x);

   if ((slv.iter_trns_x == 0) && (!slv.x_algo_bdf2)) {
     solve_jac_1_x(smat,slv,global);
   } else {
     solve_jac_2_x(smat,slv,global);
   }

   copy_array_1<double>(smat.m_x.n_row,smat.svec_orig_x,smat.delsvec_x);

   add_arrays_1<double>(smat.m_x.n_row,smat.delsvec_x,smat.svec_x);
   dcmp_solvec_x(xbe_lib,xbe_usr,smat,cct);

   return;
} // end of solve_trns_linear_x
// -----------------------------------------------------------------------------
void solve_trns_nonlinear_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   bool flag_nan_1,flag_write;
   int i_newt;

   check_convergence_count_x(slv);
   slv.get_dmp(cct);

   for (i_newt=0; i_newt < slv.x_nr_itermax_a; i_newt++) {

     slv.iter_newton = i_newt;

     form_jac_rhs_trns_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

     add_trns_terms_x(xbe_usr,smat,slv);

     negative_double_1(smat.m_x.n_row,smat.rhs_m_x);

     if (slv.iter_newton == 0) {
       if ((slv.iter_trns_x == 0) && (!slv.x_algo_bdf2)) {

         solve_jac_1_x(smat,slv,global);
       } else {

         solve_jac_2_x(smat,slv,global);
       }
     } else {

       solve_jac_2_x(smat,slv,global);
     }

     copy_array_1<double>(smat.m_x.n_row,smat.svec_orig_x,smat.delsvec_x);
     if ((slv.x_nr_flag_dmp_a) && (i_newt <= slv.x_nr_dmp_itermax_a)) {
       mult_array_1<double>(smat.m_x.n_row,smat.delsvec_x,slv.x_nr_dmp_k_a);
     }
     add_arrays_1<double>(smat.m_x.n_row,smat.delsvec_x,smat.svec_x);
     check_array_for_nan_2(smat.m_x.n_row,smat.svec_x,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_trns_nonlinear_x: svec_x has a NaN" << endl;
       slv.flag_nr_converged = false;
       slv.flag_nr_norm_large = true;
       break;
     } else {
       dcmp_solvec_x(xbe_lib,xbe_usr,smat,cct);
       check_convergence_x(smat,slv);
       if (slv.flag_nr_converged) {
         break;
       }
     }
   }

   return;
} // end of solve_trns_nonlinear_x
// -----------------------------------------------------------------------------
void solve_trns_linear_x_al(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   bool flag_write;

// solve algebraic loop equations in only xbe's/explicit trns case.

   form_jac_rhs_trns_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   form_solvec_x(xbe_lib,xbe_usr,smat,cct);

   negative_double_1(smat.m_x.n_row,smat.rhs_m_x);

   if (slv.iter_trns_x == 0) {
     solve_jac_1_x(smat,slv,global);
   } else {
     solve_jac_2_x(smat,slv,global);
   }

   copy_array_1<double>(smat.m_x.n_row,smat.svec_orig_x,smat.delsvec_x);

   add_arrays_1<double>(smat.m_x.n_row,smat.delsvec_x,smat.svec_x);
   dcmp_solvec_x(xbe_lib,xbe_usr,smat,cct);

   return;
} // end of solve_trns_linear_x_al
// -----------------------------------------------------------------------------
void solve_trns_nonlinear_x_al(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

// solve algebraic loop equations in only xbe's/explicit trns case.

   bool flag_nan_1,flag_write;
   int i_newt;

   check_convergence_count_x(slv);
   slv.get_dmp(cct);

   for (i_newt=0; i_newt < slv.x_nr_itermax_a; i_newt++) {
     slv.iter_newton = i_newt;

     form_jac_rhs_trns_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

     negative_double_1(smat.m_x.n_row,smat.rhs_m_x);

     if (slv.iter_newton == 0) {
       if (slv.iter_trns_x == 0) {
         solve_jac_1_x(smat,slv,global);
       } else {
         solve_jac_2_x(smat,slv,global);
       }
     } else {
       solve_jac_2_x(smat,slv,global);
     }

     copy_array_1<double>(smat.m_x.n_row,smat.svec_orig_x,smat.delsvec_x);
     if ((slv.x_nr_flag_dmp_a) && (i_newt <= slv.x_nr_dmp_itermax_a)) {
       mult_array_1<double>(smat.m_x.n_row,smat.delsvec_x,slv.x_nr_dmp_k_a);
     }
     add_arrays_1<double>(smat.m_x.n_row,smat.delsvec_x,smat.svec_x);
     check_array_for_nan_2(smat.m_x.n_row,smat.svec_x,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_trns_nonlinear_x_al: svec_x has a NaN" << endl;
       slv.flag_nr_converged = false;
       slv.flag_nr_norm_large = true;
       break;
     } else {
       dcmp_solvec_x(xbe_lib,xbe_usr,smat,cct);
       check_convergence_x(smat,slv);
       if (slv.flag_nr_converged) {
         break;
       }
     }
   }
   if (!slv.flag_nr_converged) {
     cout << "solve_trns_nonlinear_x_al: NR did not converge. Halting..." << endl;
     slv.write_flags_failed();
     exit(1);
   }

   return;
} // end of solve_trns_nonlinear_x_al
// -----------------------------------------------------------------------------
void solve_trns_exc(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   form_map_xbeuvr_1(smat,cct);

   init_sol_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     slv,cct,smat,global);
   form_solvec_ex(ebe_lib,xbe_lib,ebe_usr,xbe_usr,smat,slv,cct);

   smat.mat_trns_1_ex(ebe_lib,ebe_usr,xbe_lib,xbe_usr,global,cct,cct_file);

   slv.ex_algo_be0 = slv.ex_algo_be || slv.ex_algo_be_auto || slv.ex_algo_be_const;
   slv.ex_algo_trz0 = slv.ex_algo_trz || slv.ex_algo_trz_auto || slv.ex_algo_trz_const;
   slv.ex_algo_auto = slv.ex_algo_be_auto || slv.ex_algo_trz_auto;

   slv.ex_algo_bdf2 = false;
   if (slv.ex_algo_trbdf2) {
     slv.ex_algo_trz0 = true;
   }
   for (int i_file=0; i_file < slv.n_outfile; i_file++) {
     if (slv.flag_out_delt_fixed[i_file]) {
       slv.out_tnext[i_file] = slv.out_tstart[i_file];
     }
   }

   if (slv.ex_algo_be  || slv.ex_algo_be_const) {
     solve_trns_exc_be(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.ex_algo_trz || slv.ex_algo_trz_const) {
     solve_trns_exc_trz(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.ex_algo_be_auto) {
     solve_trns_exc_be_auto(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.ex_algo_trz_auto) {
     solve_trns_exc_trz_auto(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (slv.ex_algo_trbdf2) {
     solve_trns_exc_trbdf2(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   }

   cout << "solve_trns_exc completed." << endl;
   return;
} // end of solve_trns_exc
// -----------------------------------------------------------------------------
void solve_trns_exc_be(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached;
   bool flag_nan_1;

   flag_tend_reached = false;

   slv.write_iter_n1_e = 0;
   slv.iter_trns_e = -1;
   slv.iter_trns_x = -1;

   slv.time_present_e = global.time_begin;
   slv.time_present_x = global.time_begin;

   slv.time_write = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_e = slv.delt0_ex;
   slv.delt_x = slv.delt0_ex;

   if (!slv.flag_const_tstep_ex) {
     if (cct.flag_limit_tstep) {
       e_assign_nextbreak_1(ebe_usr,cct,global);
       x_assign_nextbreak_1(xbe_usr,cct,global);

       ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);
       xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);

       get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         slv,cct,global);
     }
   }
   copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

   smat.w_ex.allocate_1(smat.m_ex.n_nz,smat.m_ex.n_row,smat.m_ex.n_col);
   smat.mo_ex.allocate_1(smat.m_ex.n_row);

   while (!flag_tend_reached) {
     check_array_for_nan_2(smat.n_solvec_ex,smat.svec_ex,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_trns_exc_be: svec_ex has a NAN. Halting..." << endl;
       exit(1);
     }
     slv.iter_trns_e++;
     slv.iter_trns_x++;
     global.iter_trns_e = slv.iter_trns_e;
     global.iter_trns_x = slv.iter_trns_x;
     write_iter_e(slv);

     slv.time_next_e  = slv.time_present_e + slv.delt_e;
     slv.time_next_x  = slv.time_present_x + slv.delt_x;
     global.time_given_e = slv.time_next_e;
     global.time_given_x = slv.time_next_x;
     if (flag_nan(slv.time_next_e)) {
       cout << "solve_trns_exc_be: slv.time_next_e is NAN. Halting..." << endl;
       exit(1);
     }
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     if (slv.delt_e < slv.delt_min_e) {
       cout << "solve_trns_exc_be: check slv.delt_e. Halting..." << endl;
       exit(1);
     }

     slv.trns_constants_2_ex();

     form_solvec_ex(ebe_lib,xbe_lib,ebe_usr,xbe_usr,smat,slv,cct);

     if (cct.flag_linear_ex) {
       solve_trns_linear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.flag_write_solution) {
           write_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
         }
         cout << "solve_trns_exc_be: N-R iterations did not converge." << endl;
         slv.write_flags_failed();
         cout << "  iter_trns_e =" << slv.iter_trns_e << endl;
         cout << "  time ="
           << scientific << setprecision(6)
           << slv.time_present_e << endl;
         cout << "  Halting..." << endl;
         exit(1);
       }
     }
     slv.time_present_e = slv.time_next_e;
     slv.time_present_x = slv.time_next_x;
     slv.time_write     = slv.time_next_e;

     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);

//   find the new time step if necessary

     slv.delt_e = slv.delt0_ex;
     slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
     slv.delt_x = slv.delt_e;

     if (!slv.flag_const_tstep_ex) {
       if (cct.flag_limit_tstep) {
         get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
           slv,cct,global);
       }
     }
     time_next_1 = slv.time_present_e + slv.delt_e;
     if (time_next_1 >= global.time_end) {
       slv.delt_e = global.time_end - slv.time_present_e + slv.delt_small;
       slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
       slv.delt_x = slv.delt_e;
     }

     if (cct.flag_reset_x) {
       xbe_reset_1_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,slv,cct,smat,global);
     }
     copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
     ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

     if (cct.flag_save_history_e) {
       save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
     }
     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_e > slv.itmax_trns) {
         cout << "solve_trns_exc_be: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_e+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   return;
} // end of solve_trns_exc_be
// -----------------------------------------------------------------------------
void solve_trns_exc_trz(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached;
   bool flag_nan_1;

   flag_tend_reached = false;

   slv.write_iter_n1_e = 0;
   slv.write_iter_n1_x = 0;
   slv.iter_trns_e = -1;

   slv.time_present_e = global.time_begin;
   slv.time_present_x = global.time_begin;

   slv.time_write = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_e = slv.delt0_ex;
   slv.delt_x = slv.delt0_ex;

   if (!slv.flag_const_tstep_ex) {
     if (cct.flag_limit_tstep) {
       e_assign_nextbreak_1(ebe_usr,cct,global);
       x_assign_nextbreak_1(xbe_usr,cct,global);

       ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);
       xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);

       get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         slv,cct,global);
     }
   }
   copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

   slv.time_next_e = slv.time_present_e;
   slv.time_next_x = slv.time_present_x;
   global.time_given_e = slv.time_next_e;
   global.time_given_x = slv.time_next_x;
   if (flag_nan(slv.time_next_e)) {
     cout << "solve_trns_exc_trz: slv.time_next_e is NAN. Halting..." << endl;
     exit(1);
   }
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }
   slv.trns_constants_2_ex();
   find_functions_trns_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
     smat,cct,slv,global);

   smat.w_ex.allocate_1(smat.m_ex.n_nz,smat.m_ex.n_row,smat.m_ex.n_col);
   smat.mo_ex.allocate_1(smat.m_ex.n_row);

   while (!flag_tend_reached) {
     check_array_for_nan_2(smat.n_solvec_ex,smat.svec_ex,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_trns_exc_trz: svec_ex has a NAN. Halting..." << endl;
       exit(1);
     }
     slv.iter_trns_e++;
     slv.iter_trns_x++;
     global.iter_trns_e = slv.iter_trns_e;
     global.iter_trns_x = slv.iter_trns_x;
     write_iter_e(slv);

     slv.time_next_e  = slv.time_present_e + slv.delt_e;
     slv.time_next_x  = slv.time_present_x + slv.delt_x;
     global.time_given_e = slv.time_next_e;
     global.time_given_x = slv.time_next_x;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     form_solvec_ex(ebe_lib,xbe_lib,ebe_usr,xbe_usr,smat,slv,cct);

     if (cct.flag_linear_ex) {
       find_functions_trns_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);
     }
     copy_func_to_old_ex(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,xbe_lib,xbe_usr,
       cct,global);
     slv.trns_constants_2_ex();

     if (cct.flag_linear_ex) {
       solve_trns_linear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.flag_write_solution) {
           write_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
         }
         cout << "solve_trns_exc_trz: N-R iterations did not converge." << endl;
         slv.write_flags_failed();
         cout << "  iter_trns_e =" << slv.iter_trns_e << endl;
         cout << "  time ="
           << scientific << setprecision(6)
           << slv.time_present_e << endl;
         cout << "  Halting..." << endl;
         exit(1);
       }
     }
     slv.time_present_e = slv.time_next_e;
     slv.time_present_x = slv.time_next_x;
     slv.time_write     = slv.time_next_e;

     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);

     slv.delt_e = slv.delt0_ex;
     slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
     slv.delt_x = slv.delt_e;

     if (!slv.flag_const_tstep_ex) {
       if (cct.flag_limit_tstep) {
         get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
           slv,cct,global);
       }
     }
     time_next_1 = slv.time_present_e + slv.delt_e;
     if (time_next_1 >= global.time_end) {
       slv.delt_e = global.time_end - slv.time_present_e + slv.delt_small;
       slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
       slv.delt_x = slv.delt_e;
     }
     if (cct.flag_reset_x) {
       xbe_reset_1_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,slv,cct,smat,global);
     }
     copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
     ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

     if (cct.flag_save_history_e) {
       save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
     }
     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
     if (slv.flag_limit_iter_trns) {
       if (slv.iter_trns_e > slv.itmax_trns) {
         cout << "solve_trns_exc_trz: itmax_trns exceeded." << endl;
         break;
       }
     }
     if ((slv.time_present_e+slv.delt_small) >= global.time_end) {
       flag_tend_reached = true;
     }
   }
   return;
} // end of solve_trns_exc_trz
// -----------------------------------------------------------------------------
void solve_trns_exc_be_auto(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached,flag_repeat_step;
   bool flag_nan_1;
   int iter_stepred;

   flag_tend_reached = false;
   flag_repeat_step = false;

   slv.write_iter_n1_e = 0;
   slv.iter_trns_e = -1;
   slv.iter_trns_x = -1;

   slv.time_present_e = global.time_begin;
   slv.time_present_x = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_e = slv.delt0_e;
   slv.delt_x = slv.delt0_x;

   if (cct.flag_limit_tstep) {
     e_assign_nextbreak_1(ebe_usr,cct,global);
     x_assign_nextbreak_1(xbe_usr,cct,global);

     ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);
     xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);

     get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
       slv,cct,global);
   }
   copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

   smat.w_ex.allocate_1(smat.m_ex.n_nz,smat.m_ex.n_row,smat.m_ex.n_col);
   smat.mo_ex.allocate_1(smat.m_ex.n_row);

   while (!flag_tend_reached) {
     check_array_for_nan_2(smat.n_solvec_ex,smat.svec_ex,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_trns_exc_be_auto: svec_ex has a NAN. Halting..." << endl;
       exit(1);
     }
     if (!flag_repeat_step) {
       slv.iter_trns_e++;
       slv.iter_trns_x++;
       global.iter_trns_e = slv.iter_trns_e;
       global.iter_trns_x = slv.iter_trns_x;
       write_iter_e(slv);

       iter_stepred = 0;
     }
     flag_repeat_step = false;

     slv.time_next_e  = slv.time_present_e + slv.delt_e;
     slv.time_next_x  = slv.time_present_x + slv.delt_x;
     global.time_given_e = slv.time_next_e;
     global.time_given_x = slv.time_next_x;
     if (flag_nan(slv.time_next_e)) {
       cout << "solve_trns_exc_be_auto: slv.time_next_e is NAN. Halting..." << endl;
       exit(1);
     }
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     slv.trns_constants_2_ex();

     form_solvec_ex(ebe_lib,xbe_lib,ebe_usr,xbe_usr,smat,slv,cct);

     if (cct.flag_linear_ex) {
       solve_trns_linear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.delt_e == slv.delt_min_ex) {
           cout << "solve_trns_exc_be_auto: no convergence even with" << endl;
           cout << "  the smallest time step." << endl;
           slv.write_flags_failed();
           cout << "iter_trns_e=" << slv.iter_trns_e
                << ", time =" << slv.time_present_e << endl;
           cout << "  Halting..." << endl;
           exit(1);
         } else {
           iter_stepred++;
           if (iter_stepred > slv.itmax_stepred) {
             cout << "solve_trns_exc_be_auto: iter_stepred has exceeded" << endl;
             cout << "  itmax_stepred." << endl;
             cout << "  iter_stepred=" << iter_stepred << endl;
             cout << "  itmax_stepred=" << slv.itmax_stepred << endl;
             cout << "  Halting..." << endl;
             exit(1);
           }
           slv.delt_e = slv.factor_stepdec*slv.delt_e;
           slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
           slv.delt_x = slv.delt_e;

           copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
           ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

           flag_repeat_step = true;
         }
       } else {
         if (slv.delt_e != slv.delt_max_ex) {
           slv.delt_e = slv.factor_stepinc*slv.delt_e;
           slv.delt_e = min(slv.delt_e,slv.delt_max_ex);
           slv.delt_x = slv.delt_e;
         }
         flag_repeat_step = false;
       }
     }
     if (!flag_repeat_step) {
       slv.time_present_e = slv.time_next_e;
       slv.time_present_x = slv.time_next_x;
       slv.time_write     = slv.time_next_e;

       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);

       slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
       slv.delt_x = slv.delt_e;

       if (cct.flag_limit_tstep) {
         get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
           slv,cct,global);
       }
       time_next_1 = slv.time_present_e + slv.delt_e;
       if (time_next_1 >= global.time_end) {
         slv.delt_e = global.time_end - slv.time_present_e + slv.delt_small;
         slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
         slv.delt_x = slv.delt_e;
       }
       if (cct.flag_reset_x) {
         xbe_reset_1_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,slv,cct,smat,global);
       }
       copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
       ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

       if (cct.flag_save_history_e) {
         save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
       }
       if (cct.flag_save_history_x) {
         save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
       if (slv.flag_limit_iter_trns) {
         if (slv.iter_trns_e > slv.itmax_trns) {
           cout << "solve_trns_exc_be_auto: itmax_trns exceeded." << endl;
           break;
         }
       }
       if ((slv.time_present_e+slv.delt_small) >= global.time_end) {
         flag_tend_reached = true;
       }
     }
   }
   return;
} // end of solve_trns_exc_be_auto
// -----------------------------------------------------------------------------
void solve_trns_exc_trz_auto(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double time_next_1;
   bool flag_tend_reached,flag_repeat_step;
   bool flag_nan_1;
   int iter_stepred;

   flag_tend_reached = false;
   flag_repeat_step = false;

   slv.write_iter_n1_e = 0;
   slv.iter_trns_e = -1;
   slv.iter_trns_x = -1;

   slv.time_present_e = global.time_begin;
   slv.time_present_x = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_e = slv.delt0_ex;
   slv.delt_x = slv.delt0_ex;

   if (cct.flag_limit_tstep) {
     e_assign_nextbreak_1(ebe_usr,cct,global);
     x_assign_nextbreak_1(xbe_usr,cct,global);

     ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);
     xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);

     get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
       slv,cct,global);
   }
   copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

   slv.time_next_e = slv.time_present_e;
   slv.time_next_x = slv.time_present_x;
   global.time_given_e = slv.time_next_e;
   global.time_given_x = slv.time_next_x;
   if (flag_nan(slv.time_next_e)) {
     cout << "solve_trns_exc_trz_auto: slv.time_next_e is NAN. Halting..." << endl;
     exit(1);
   }
   if (cct.flag_time_parms) {
     xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
   }
   slv.trns_constants_2_ex();
   find_functions_trns_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
     smat,cct,slv,global);

   smat.w_ex.allocate_1(smat.m_ex.n_nz,smat.m_ex.n_row,smat.m_ex.n_col);
   smat.mo_ex.allocate_1(smat.m_ex.n_row);

   while (!flag_tend_reached) {
     check_array_for_nan_2(smat.n_solvec_ex,smat.svec_ex,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_trns_exc_trz_auto: svec_ex has a NAN. Halting..." << endl;
       exit(1);
     }
     if (!flag_repeat_step) {
       slv.iter_trns_e++;
       slv.iter_trns_x++;
       global.iter_trns_e = slv.iter_trns_e;
       global.iter_trns_x = slv.iter_trns_x;
       write_iter_e(slv);

       iter_stepred = 0;
     }
     flag_repeat_step = false;

     slv.time_next_e  = slv.time_present_e + slv.delt_e;
     slv.time_next_x  = slv.time_present_x + slv.delt_x;
     global.time_given_e = slv.time_next_e;
     global.time_given_x = slv.time_next_x;
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

     form_solvec_ex(ebe_lib,xbe_lib,ebe_usr,xbe_usr,smat,slv,cct);

     if (cct.flag_linear_ex) {
       find_functions_trns_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);
     }
     copy_func_to_old_ex(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,xbe_lib,xbe_usr,
       cct,global);
     slv.trns_constants_2_ex();

     if (cct.flag_linear_ex) {
       solve_trns_linear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.delt_e == slv.delt_min_ex) {
           cout << "solve_trns_exc_trzauto: no convergence even with" << endl;
           cout << "  the smallest time step." << endl;
           slv.write_flags_failed();
           cout << "iter_trns_e=" << slv.iter_trns_e
                << ", time =" << slv.time_present_e << endl;
           cout << "  Halting..." << endl;
           exit(1);
         } else {
           iter_stepred++;
           if (iter_stepred > slv.itmax_stepred) {
             cout << "solve_trns_exc_trzauto: iter_stepred has exceeded" << endl;
             cout << "  itmax_stepred." << endl;
             cout << "  iter_stepred=" << iter_stepred << endl;
             cout << "  itmax_stepred=" << slv.itmax_stepred << endl;
             cout << "  Halting..." << endl;
             exit(1);
           }
           slv.delt_e = slv.factor_stepdec*slv.delt_e;
           slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
           slv.delt_x = slv.delt_e;

           copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
           ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

           flag_repeat_step = true;
         }
       } else {
         if (slv.delt_e != slv.delt_max_ex) {
           slv.delt_e = slv.factor_stepinc*slv.delt_e;
           slv.delt_e = min(slv.delt_e,slv.delt_max_ex);
           slv.delt_x = slv.delt_e;
         }
         flag_repeat_step = false;
       }
     }
     if (!flag_repeat_step) {
       slv.time_present_e = slv.time_next_e;
       slv.time_present_x = slv.time_next_x;
       slv.time_write     = slv.time_next_e;

       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);

       slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
       slv.delt_x = slv.delt_e;

       if (cct.flag_limit_tstep) {
         get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
           slv,cct,global);
       }
       time_next_1 = slv.time_present_e + slv.delt_e;
       if (time_next_1 >= global.time_end) {
         slv.delt_e = global.time_end - slv.time_present_e + slv.delt_small;
         slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
         slv.delt_x = slv.delt_e;
       }
       if (cct.flag_reset_x) {
         xbe_reset_1_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,slv,cct,smat,global);
       }
       copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
       ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

       if (cct.flag_save_history_e) {
         save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
       }
       if (cct.flag_save_history_x) {
         save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
       if (slv.flag_limit_iter_trns) {
         if (slv.iter_trns_e > slv.itmax_trns) {
           cout << "solve_trns_exc_trz_auto: itmax_trns exceeded." << endl;
           break;
         }
       }
       if ((slv.time_present_e+slv.delt_small) >= global.time_end) {
         flag_tend_reached = true;
       }
     }
   }
   return;
} // end of solve_trns_exc_trz_auto
// -----------------------------------------------------------------------------
void solve_trns_exc_trbdf2(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   double delt_tmp,t_present_tmp,time_next_1;
   bool flag_tend_reached,flag_repeat_step;
   bool flag_nan_1;
   int iter_trbdf2,iter_trz,iter_bdf2,n_reject;

   slv.ex_algo_trz0 = true;

   flag_tend_reached = false;
   flag_repeat_step = false;

   slv.write_iter_n1_e = 0;
   slv.iter_trns_e = -1;
   slv.iter_trns_x = -1;

   slv.time_present_e = global.time_begin;
   slv.time_present_x = global.time_begin;
   slv.time_write     = global.time_begin;

   write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     cct,slv,cct_file,global);

   slv.delt_e = slv.delt0_ex;
   slv.delt_x = slv.delt0_ex;

   if (cct.flag_limit_tstep) {
     e_assign_nextbreak_1(ebe_usr,cct,global);
     x_assign_nextbreak_1(xbe_usr,cct,global);

     ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);
     xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);

     get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
       slv,cct,global);
   }
   copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

   slv.trns_constants_2_ex();
   find_functions_trns_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
     smat,cct,slv,global);

   smat.w_ex.allocate_1(smat.m_ex.n_nz,smat.m_ex.n_row,smat.m_ex.n_col);
   smat.mo_ex.allocate_1(smat.m_ex.n_row);

   while (!flag_tend_reached) {
     check_array_for_nan_2(smat.n_solvec_ex,smat.svec_ex,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_trns_exc_trbdf2: svec_ex has a NAN. Halting..." << endl;
       exit(1);
     }
     if (!flag_repeat_step) {
       slv.iter_trns_e++;
       slv.iter_trns_x++;
       global.iter_trns_e = slv.iter_trns_e;
       global.iter_trns_x = slv.iter_trns_x;
       write_iter_e(slv);

       iter_trbdf2 = 0;
       iter_trz = 0;
       iter_bdf2 = 0;
       n_reject = 0;
     }
     flag_repeat_step = false;

     iter_trbdf2++;
     if (iter_trbdf2 > slv.itmax_trbdf2) {
       cout << "solve_trns_exc_trbdf2: iter_trbdf2 has exceeded itmax_trbdf2" << endl;
       cout << "  iter_trbdf2=" << iter_trbdf2 << endl;
       cout << "  itmax_trbdf2=" << slv.itmax_trbdf2 << endl;
       cout << "  n_reject=" << n_reject << endl;
       cout << "  time_given=" << global.time_given_e << endl;
       cout << "  Halting..." << endl; exit(1);
     }
     delt_tmp = slv.delt_e;
     t_present_tmp = slv.time_present_e;

     slv.delt_e = slv.bank_gamma*delt_tmp;
     slv.delt_x = slv.delt_e;
     slv.time_next_e  = slv.time_present_e + slv.delt_e;
     slv.time_next_x  = slv.time_present_x + slv.delt_x;
     global.time_given_e = slv.time_next_e;
     global.time_given_x = slv.time_next_x;
     if (flag_nan(slv.time_next_e)) {
       cout << "solve_trns_exc_trbdf2: slv.time_next_e is NAN. Halting..." << endl;
       exit(1);
     }
     if (cct.flag_time_parms) {
       xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }

//   solve trz part:

     slv.ex_algo_trz0 = true;
     slv.ex_algo_bdf2 = false;
     iter_trz++;

     copy_func_to_old_ex(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,xbe_lib,xbe_usr,
       cct,global);
     slv.trns_constants_2_ex();

     form_solvec_ex(ebe_lib,xbe_lib,ebe_usr,xbe_usr,smat,slv,cct);

     if (cct.flag_linear_ex) {
       solve_trns_linear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);
       find_functions_trns_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);
     } else {
       solve_trns_nonlinear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,global);

       if (!slv.flag_nr_converged) {
         if (slv.delt_e == slv.delt_min_ex) {
           cout << "solve_trns_exc_trbdf2: no convergence even with" << endl;
           cout << "  the smallest time step." << endl;
           slv.write_flags_failed();
           cout << "  iter_trz=" << iter_trz << endl;
           cout << "  iter_trbdf2=" << iter_trbdf2 << endl;
           cout << "  n_reject=" << n_reject << endl;
           cout << "  iter_trns_e="   << slv.iter_trns_e << endl;
           cout << "  Halting..." << endl;
           exit(1);
         }
         slv.delt_e = slv.factor_stepdec*delt_tmp;
         slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
         slv.delt_x = slv.delt_e;

         copy_array_1<double>(smat.n_solvec_ex,smat.svec_old_1_ex,smat.svec_ex);
         ebeu_copy_stv_1(global.I_COPY_1_TO_0,ebe_lib,ebe_usr,cct,global);

         flag_repeat_step = true;
       }
     }

     if (!flag_repeat_step) {
       copy_array_1<double>(smat.n_solvec_ex,smat.svec_old_1_ex,smat.svec_old_2_ex);
       copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);

       ebeu_copy_stv_1(global.I_COPY_1_TO_2,ebe_lib,ebe_usr,cct,global);
       ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

       copy_func_to_old_ex(global.I_COPY_1_TO_2,ebe_lib,ebe_usr,xbe_lib,xbe_usr,
         cct,global);
       copy_func_to_old_ex(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,xbe_lib,xbe_usr,
         cct,global);

//     solve bdf2 part:

       slv.ex_algo_trz0 = false;
       slv.ex_algo_bdf2 = true;
       iter_bdf2++;

       slv.time_next_e  = t_present_tmp + delt_tmp;
       slv.time_next_x  = t_present_tmp + delt_tmp;
       global.time_given_e = slv.time_next_e;
       global.time_given_x = slv.time_next_x;
       if (cct.flag_time_parms) {
         xbe_time_parms(xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
       slv.delt_e = delt_tmp;
       slv.delt_x = delt_tmp;
       slv.trns_constants_2_ex();

       form_solvec_ex(ebe_lib,xbe_lib,ebe_usr,xbe_usr,smat,slv,cct);

       if (cct.flag_linear_ex) {
         solve_trns_linear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
           smat,cct,slv,global);
         find_functions_trns_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
           smat,cct,slv,global);
       } else {
         solve_trns_nonlinear_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
           smat,cct,slv,global);

         if (!slv.flag_nr_converged) {
           if (slv.delt_e == slv.delt_min_ex) {
             cout << "solve_trns_exc_trbdf2: no convergence even with" << endl;
             cout << "  the smallest time step." << endl;
             slv.write_flags_failed();
             cout << "  iter_trz=" << iter_trz << endl;
             cout << "  iter_trbdf2=" << iter_trbdf2 << endl;
             cout << "  n_reject=" << n_reject << endl;
             cout << "  iter_trns_e=" << slv.iter_trns_e << endl;
             cout << "  Halting..." << endl;
             exit(1);
           }
           slv.delt_e = slv.factor_stepdec*delt_tmp;
           slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
           slv.delt_x = slv.delt_e;

           copy_array_1<double>
             (smat.n_solvec_ex,smat.svec_old_2_ex,smat.svec_old_1_ex);
           copy_array_1<double>
             (smat.n_solvec_ex,smat.svec_old_2_ex,smat.svec_ex);

           ebeu_copy_stv_1(global.I_COPY_2_TO_1,ebe_lib,ebe_usr,cct,global);
           ebeu_copy_stv_1(global.I_COPY_2_TO_0,ebe_lib,ebe_usr,cct,global);

           flag_repeat_step = true;
         }
       }
     }
     if (!flag_repeat_step) {
//     check whether the solution should be accepted
       trzbdf2_1_ex(ebe_lib,ebe_usr,xbe_usr,smat,cct,slv);

       slv.delt_e = slv.delt_new_e;
       slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
       slv.delt_e = min(slv.delt_e,slv.delt_max_ex);
       slv.delt_x = slv.delt_e;

       if (!slv.flag_accept_sol) {
         n_reject++;

         copy_array_1<double>
           (smat.n_solvec_ex,smat.svec_old_2_ex,smat.svec_old_1_ex);
         copy_array_1<double>
           (smat.n_solvec_ex,smat.svec_old_2_ex,smat.svec_ex);

         ebeu_copy_stv_1(global.I_COPY_2_TO_1,ebe_lib,ebe_usr,cct,global);
         ebeu_copy_stv_1(global.I_COPY_2_TO_0,ebe_lib,ebe_usr,cct,global);

         copy_func_to_old_ex(global.I_COPY_2_TO_0,ebe_lib,ebe_usr,
           xbe_lib,xbe_usr,cct,global);
         dcmp_solvec_ex(xbe_lib,xbe_usr,ebe_lib,ebe_usr,smat,slv,cct);

         flag_repeat_step = true;
       }
     }
     if (!flag_repeat_step) {
       slv.time_present_e = slv.time_next_e;
       slv.time_present_x = slv.time_next_x;
       slv.time_write     = slv.time_next_e;

       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);

       slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
       slv.delt_x = slv.delt_e;

       if (cct.flag_limit_tstep) {
         get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
           slv,cct,global);
       }
       time_next_1 = slv.time_present_e + slv.delt_e;
       if (time_next_1 >= global.time_end) {
         slv.delt_e = global.time_end - slv.time_present_e + slv.delt_small;
         slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
         slv.delt_x = slv.delt_e;
       }
       if (cct.flag_reset_x) {
         xbe_reset_1_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,slv,cct,smat,global);
       }
       copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
       ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

       if (cct.flag_save_history_e) {
         save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
       }
       if (cct.flag_save_history_x) {
         save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
       if (slv.flag_limit_iter_trns) {
         if (slv.iter_trns_e > slv.itmax_trns) {
           cout << "solve_trns_exc_trbdf2: itmax_trns exceeded." << endl;
           break;
         }
       }
       if ((slv.time_present_e+slv.delt_small) >= global.time_end) {
         flag_tend_reached = true;
       }
     }
   }
   return;
} // end of solve_trns_exc_trbdf2
// -----------------------------------------------------------------------------
void solve_trns_linear_exc(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   bool flag_write;

   form_jac_rhs_trns_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
     smat,cct,slv,global);

   add_trns_terms_exc_e(ebe_usr,smat,slv);
   add_trns_terms_exc_x(xbe_usr,smat,slv);

   negative_double_1(smat.m_ex.n_row,smat.rhs_m_ex);

   if ((slv.iter_trns_e == 0) && (!slv.ex_algo_bdf2)) {
     solve_jac_1_ex(smat,slv,global);
   } else {
     solve_jac_2_ex(smat,slv,global);
   }

   copy_array_1<double>(smat.m_ex.n_row,smat.svec_orig_ex,smat.delsvec_ex);

   add_arrays_1<double>(smat.m_ex.n_row,smat.delsvec_ex,smat.svec_ex);
   dcmp_solvec_ex(xbe_lib,xbe_usr,ebe_lib,ebe_usr,smat,slv,cct);

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   find_ebe_cur_trns_ex(true,ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);

   return;
} // end of solve_trns_linear_exc
// -----------------------------------------------------------------------------
void solve_trns_nonlinear_exc(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   bool flag_nan_1,flag_write;
   int i_newt;

   check_convergence_count_ex(slv);

   slv.get_dmp(cct);

   for (i_newt=0; i_newt < slv.ex_nr_itermax_a; i_newt++) {
     slv.iter_newton = i_newt;

     form_jac_rhs_trns_exc(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
       smat,cct,slv,global);
     add_trns_terms_exc_e(ebe_usr,smat,slv);
     add_trns_terms_exc_x(xbe_usr,smat,slv);

     negative_double_1(smat.m_ex.n_row,smat.rhs_m_ex);

     if (slv.iter_newton == 0) {
       if ((slv.iter_trns_e == 0) && (!slv.ex_algo_bdf2)) {
         solve_jac_1_ex(smat,slv,global);
       } else {
         solve_jac_2_ex(smat,slv,global);
       }
     } else {
       solve_jac_2_ex(smat,slv,global);
     }

     copy_array_1<double>(smat.m_ex.n_row,smat.svec_orig_ex,smat.delsvec_ex);
     if ((slv.ex_nr_flag_dmp_a) && (i_newt <= slv.ex_nr_dmp_itermax_a)) {
       mult_array_1<double>(smat.m_ex.n_row,smat.delsvec_ex,slv.ex_nr_dmp_k_a);
     }
     add_arrays_1<double>(smat.m_ex.n_row,smat.delsvec_ex,smat.svec_ex);
     check_array_for_nan_2(smat.m_ex.n_row,smat.svec_ex,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_trns_nonlinear_exc: svec_ex has a NaN" << endl;
       slv.flag_nr_converged = false;
       slv.flag_nr_norm_large = true;
       break;
     } else {
       dcmp_solvec_ex(xbe_lib,xbe_usr,ebe_lib,ebe_usr,smat,slv,cct);
       if (slv.e_nr_flag_check_spice) {
         find_ebe_cur_trns_ex(true,ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
       }
       check_convergence_ex(smat,slv,ebe_lib,ebe_usr,cct);

       if (slv.flag_nr_converged) {
         break;
       }
       if (slv.e_nr_flag_check_spice) {
         copy_array_1<double>(smat.m_ex.n_row,smat.svec_ex,smat.svec_old_nr_1_ex);
         copy_cur_nd_nr_1(ebe_lib,ebe_usr,cct);
       }
     }
   }

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   find_ebe_cur_trns_ex(true,ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);

   return;
} // end of solve_trns_nonlinear_exc
// -----------------------------------------------------------------------------
void solve_jac_1_e(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_NO_RHS_MTOW,
     global.I_MAT_MTOW,
     global.I_GAUSS1A,
     global.I_NO_GAUSS2,
     global.I_NO_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_e,smat.w_e,smat.mo_e,smat.rhs_m_e,
     smat.rhs_w_e,smat.svec_w_e,smat.svec_orig_e,global);
   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_e,smat.w_e,smat.mo_e,smat.rhs_m_e,
     smat.rhs_w_e,smat.svec_w_e,smat.svec_orig_e,global);

   return;
} // end of solve_jac_1_e
// -----------------------------------------------------------------------------
void solve_jac_2_e(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_e,smat.w_e,smat.mo_e,smat.rhs_m_e,
     smat.rhs_w_e,smat.svec_w_e,smat.svec_orig_e,global);

   return;
} // end of solve_jac_2_e
// -----------------------------------------------------------------------------
void solve_jac_1_x(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_NO_RHS_MTOW,
     global.I_MAT_MTOW,
     global.I_GAUSS1A,
     global.I_NO_GAUSS2,
     global.I_NO_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_x,smat.w_x,smat.mo_x,smat.rhs_m_x,
     smat.rhs_w_x,smat.svec_w_x,smat.svec_orig_x,global);
   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_x,smat.w_x,smat.mo_x,smat.rhs_m_x,
     smat.rhs_w_x,smat.svec_w_x,smat.svec_orig_x,global);

   return;
} // end of solve_jac_1_x
// -----------------------------------------------------------------------------
void solve_jac_2_x(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_x,smat.w_x,smat.mo_x,smat.rhs_m_x,
     smat.rhs_w_x,smat.svec_w_x,smat.svec_orig_x,global);

   return;
} // end of solve_jac_2_x
// -----------------------------------------------------------------------------
void solve_jac_1_ex(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_NO_RHS_MTOW,
     global.I_MAT_MTOW,
     global.I_GAUSS1A,
     global.I_NO_GAUSS2,
     global.I_NO_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ex,smat.w_ex,smat.mo_ex,smat.rhs_m_ex,
     smat.rhs_w_ex,smat.svec_w_ex,smat.svec_orig_ex,global);
   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ex,smat.w_ex,smat.mo_ex,smat.rhs_m_ex,
     smat.rhs_w_ex,smat.svec_w_ex,smat.svec_orig_ex,global);

   return;
} // end of solve_jac_1_ex
// -----------------------------------------------------------------------------
void solve_jac_2_ex(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ex,smat.w_ex,smat.mo_ex,smat.rhs_m_ex,
     smat.rhs_w_ex,smat.svec_w_ex,smat.svec_orig_ex,global);

   return;
} // end of solve_jac_2_ex
// -----------------------------------------------------------------------------
void solve_jac_1_ssw_e(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_NO_RHS_MTOW,
     global.I_MAT_MTOW,
     global.I_GAUSS1A,
     global.I_NO_GAUSS2,
     global.I_NO_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_e,smat.svec_orig_e,global);
   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_e,smat.svec_orig_e,global);

   return;
} // end of solve_jac_1_ssw_e
// -----------------------------------------------------------------------------
void solve_jac_2_ssw_e(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_e,smat.svec_orig_e,global);

   return;
} // end of solve_jac_2_ssw_e
// -----------------------------------------------------------------------------
void solve_jac_3_ssw_e(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_e,smat.svec_orig_e,global);

   return;
} // end of solve_jac_3_ssw_e
// -----------------------------------------------------------------------------
void solve_jac_1_ssw_ex(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_NO_RHS_MTOW,
     global.I_MAT_MTOW,
     global.I_GAUSS1A,
     global.I_NO_GAUSS2,
     global.I_NO_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_ex,smat.svec_orig_ex,global);
   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_ex,smat.svec_orig_ex,global);

   return;
} // end of solve_jac_1_ssw_ex
// -----------------------------------------------------------------------------
void solve_jac_2_ssw_ex(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_ex,smat.svec_orig_ex,global);

   return;
} // end of solve_jac_2_ssw_ex
// -----------------------------------------------------------------------------
void solve_jac_3_ssw_ex(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_ex,smat.svec_orig_ex,global);

   return;
} // end of solve_jac_3_ssw_ex
// -----------------------------------------------------------------------------
void solve_jac_1_ssw_x(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_NO_RHS_MTOW,
     global.I_MAT_MTOW,
     global.I_GAUSS1A,
     global.I_NO_GAUSS2,
     global.I_NO_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_x,smat.svec_orig_x,global);
   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_x,smat.svec_orig_x,global);

   return;
} // end of solve_jac_1_ssw_x
// -----------------------------------------------------------------------------
void solve_jac_2_ssw_x(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_LU_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_x,smat.svec_orig_x,global);

   return;
} // end of solve_jac_2_ssw_x
// -----------------------------------------------------------------------------
void solve_jac_3_ssw_x(
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   solve_jac(
     global.I_RHS_MTOW,
     global.I_NO_MAT_MTOW,
     global.I_NO_GAUSS1,
     global.I_GAUSS2,
     global.I_BSUB,
     slv.flag_debug_gauss1,slv.flag_debug_gauss2,
     slv.gauss_epsln,slv.zero_piv,
     smat.m_ssw,smat.w_ssw,smat.mo_ssw,smat.rhs_m_ssw,
     smat.rhs_w_ssw,smat.svec_w_x,smat.svec_orig_x,global);

   return;
} // end of solve_jac_3_ssw_x
// -----------------------------------------------------------------------------
void solve_jac(
   const int flag_copy_rhs,
   const int flag_copy_m,
   const int flag_call_gauss1,
   const int flag_call_gauss2,
   const int flag_gauss2,
   const bool flag_debug_gauss1,
   const bool flag_debug_gauss2,
   const double gauss_epsln,
   const double zero_piv,
   KnuthMat &m,
   KnuthMat &w,
   MatOp &mo,
   double *rhs_m,
   double *rhs_w,
   double *svec,
   double *svec_orig,
   Global &global) {

   bool flag_error,flag_error_1;

   if (flag_copy_rhs == global.I_RHS_MTOW) {
     copy_array_1<double>(m.n_row,rhs_m,rhs_w);
   }
   if (flag_copy_m == global.I_MAT_MTOW) {
     knuth_copy_1(m,w);
   }
   if (flag_call_gauss1 == global.I_GAUSS1) {
     gauss_1(flag_debug_gauss1,m,w,mo);
   } else if (flag_call_gauss1 == global.I_GAUSS1A) {
     gauss_1a(flag_debug_gauss1,gauss_epsln,m,w,mo);
   }

   if (flag_call_gauss2 == global.I_GAUSS2) {
     gauss_2(flag_gauss2,flag_debug_gauss2,zero_piv,flag_error,global,
       m,w,mo,rhs_w,svec,svec_orig);
     if (flag_error) {
       cout << "solve_jac: error in gauss_2." << endl;
       cout << "  Calling gauss_1a (to reorder)..." << endl;

       if (w.flag_delete) w.delete_1();
       if (mo.flag_delete) mo.delete_1(m.n_row);

       w.allocate_1(m.n_nz,m.n_row,m.n_col);
       mo.allocate_1(m.n_row);

       knuth_copy_1(m,w);

       gauss_1a(flag_debug_gauss1,gauss_epsln,m,w,mo);
       copy_array_1<double>(m.n_row,rhs_m,rhs_w);
       gauss_2(global.I_LU_BSUB,flag_debug_gauss2,zero_piv,flag_error_1,global,
         m,w,mo,rhs_w,svec,svec_orig);

       if (flag_error_1) {
         cout << "solve_jac: error in gauss_2 again!" << endl;
         cout << "  (even after reordering)" << endl;
         cout << "  Halting..." << endl;
         exit(1);
       }
     }
   }
   return;
} //end of solve_jac
// -----------------------------------------------------------------------------
void form_jac_rhs_dc_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   mat_dc_2_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,global);

   return;
} // end of form_jac_rhs_dc_e
// -----------------------------------------------------------------------------
void form_jac_rhs_startup_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   mat_startup_2_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,global);

   return;
} // end of form_jac_rhs_startup_e
// -----------------------------------------------------------------------------
void form_jac_rhs_startup_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   cct_to_xbe_all(xbe_lib,xbe_usr,cct);
   mat_startup_2_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   return;
} // end of form_jac_rhs_startup_x
// -----------------------------------------------------------------------------
void form_jac_rhs_startup_exc(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   cct_to_ebe_xvr_all(ebe_lib,ebe_usr,cct);
   cct_to_xbe_all(xbe_lib,xbe_usr,cct);

   mat_startup_2_exc_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,global);
   mat_startup_2_exc_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   return;
} // end of form_jac_rhs_startup_exc
// -----------------------------------------------------------------------------
void form_jac_rhs_trns_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   mat_trns_2_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);

   return;
} // end of form_jac_rhs_trns_e
// -----------------------------------------------------------------------------
void form_jac_rhs_trns_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   cct_to_xbe_all(xbe_lib,xbe_usr,cct);
   mat_trns_2_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   return;
} // end of form_jac_rhs_trns_x
// -----------------------------------------------------------------------------
void form_jac_rhs_trns_x_al(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   cct_to_xbe_all(xbe_lib,xbe_usr,cct);
   mat_trns_2_x_al(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   return;
} // end of form_jac_rhs_trns_x_al
// -----------------------------------------------------------------------------
void form_jac_rhs_trns_exc(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   cct_to_ebe_xvr_all(ebe_lib,ebe_usr,cct);
   cct_to_xbe_all(xbe_lib,xbe_usr,cct);

   mat_trns_2_exc_e(ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
   mat_trns_2_exc_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   return;
} // end of form_jac_rhs_trns_exc
// -----------------------------------------------------------------------------
void find_functions_trns_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   find_ebe_cur_trns_e(false,ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);

   return;
} // end of find_functions_trns_e
// -----------------------------------------------------------------------------
void find_functions_trns_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   cct_to_xbe_all(xbe_lib,xbe_usr,cct);
   mat_trns_2a_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   return;
} // end of find_functions_trns_x
// -----------------------------------------------------------------------------
void find_functions_trns_exc(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   cct_to_ebe_xvr_all(ebe_lib,ebe_usr,cct);
   find_ebe_cur_trns_ex(false,ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);

   cct_to_xbe_all(xbe_lib,xbe_usr,cct);
   mat_trns_2a_exc_x(xbe_lib,xbe_usr,xbe_jac,smat,cct,global);

   return;
} // end of find_functions_trns_exc
// -----------------------------------------------------------------------------
void find_ebe_cur_trns_e(
   bool flag_ebe_cur,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_trns] = true;
   global.flags[global.i_function] = true;

   ebe_init_func_trns_e(ebe_lib,ebe_usr,cct,smat);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);

     if (flag_ebe_cur) {
       mat_trns_3b_e(i_ebeu,i_ebel,slv,ebe_lib,ebe_usr);
     }
   }
   global.flags[global.i_trns] = false;
   global.flags[global.i_function] = false;

   return;
} // end of find_ebe_cur_trns_e
// -----------------------------------------------------------------------------
void find_ebe_cur_trns_ex(
   bool flag_ebe_cur,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_trns] = true;
   global.flags[global.i_function] = true;

   ebe_init_func_trns_ex(ebe_lib,ebe_usr,cct,smat);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);

     if (flag_ebe_cur) {
       mat_trns_3b_exc(i_ebeu,i_ebel,slv,ebe_lib,ebe_usr);
     }
   }
   global.flags[global.i_trns] = false;
   global.flags[global.i_function] = false;

   return;
} // end of find_ebe_cur_trns_ex
// -----------------------------------------------------------------------------
void find_ebe_cur_ssw_trns_e(
   bool flag_ebe_cur,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_trns] = true;
   global.flags[global.i_function] = true;

   ebe_init_func_ssw_trns_e(ebe_lib,ebe_usr,cct,smat);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);

     if (flag_ebe_cur) {
       mat_trns_3b_e(i_ebeu,i_ebel,slv,ebe_lib,ebe_usr);
     }
   }
   global.flags[global.i_trns] = false;
   global.flags[global.i_function] = false;

   return;
} // end of find_ebe_cur_ssw_trns_e
// -----------------------------------------------------------------------------
void mat_dc_2_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_dc] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   ebe_init_func_jac_dc_e(ebe_lib,ebe_usr,cct,smat);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);

     mat_dc_3_e(i_ebeu,i_ebel,ebe_lib,ebe_usr,ebe_jac,smat,global);

   }

   global.flags[global.i_dc] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of mat_dc_2_e
// -----------------------------------------------------------------------------
void mat_dc_3_e(
   const int i_ebeu,
   const int i_ebel,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Global &global) {

   int i_f,n_f1,n_nd1,n_fvar1,k,row0;
   int var_flag,var_number;
   double val;

   n_f1 = ebe_lib[i_ebel].n_f;
   n_nd1 = ebe_lib[i_ebel].n_nd;

// KCL equations:
   for (i_f=0; i_f < n_nd1; i_f++) {
     n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
     for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
       k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
       if (k != -1) {
         var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
         var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

         if (var_flag == global.I_NV) {
           val = ebe_jac[i_ebel].dfdv[i_f][var_number];
         } else if (var_flag == global.I_EAUX) {
           val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
         } else {
           cout << "mat_dc_3_e: incorrect value of var_flag in f." << endl;
           cout << "  i_f = " << i_f << ", i_favr = " << i_fvar << endl;
           cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
           exit(1);
         }
         smat.m_e.val[k] += val;
       }
     }
     row0 = smat.ebe_f_to_row[i_ebeu][i_f];
     if (row0 != -1) {
       smat.rhs_m_e[row0] += ebe_usr[i_ebeu].f[i_f];
     }
   }
// non-KCL equations:
   for (i_f=n_nd1; i_f < n_f1; i_f++) {
     n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
     for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
       k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
       if (k != -1) {
         var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
         var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

         if (var_flag == global.I_NV) {
           val = ebe_jac[i_ebel].dfdv[i_f][var_number];
         } else if (var_flag == global.I_EAUX) {
           val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
         } else {
           cout << "mat_dc_3_e: incorrect value of var_flag in f." << endl;
           cout << "  i_f = " << i_f << ", i_favr = " << i_fvar << endl;
           cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
           exit(1);
         }
         smat.m_e.val[k] += val;
       }
     }
     row0 = smat.ebe_f_to_row[i_ebeu][i_f];
     smat.rhs_m_e[row0] = ebe_usr[i_ebeu].f[i_f];
   }
   return;
} // end of mat_dc_3_e
// -----------------------------------------------------------------------------
void mat_dc_3b_e(
   const int i_ebeu,
   const int i_ebel,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr) {

   int i_f,i_g,n_nd1,n_g1;

   n_nd1 = ebe_lib[i_ebel].n_nd;

// get currents:
   for (i_f=0; i_f < n_nd1; i_f++) {
     ebe_usr[i_ebeu].cur_nd[i_f] = ebe_usr[i_ebeu].f[i_f];
   }

   n_g1 = ebe_lib[i_ebel].n_g;
   for (i_g=0; i_g < n_g1; i_g++) {
     ebe_usr[i_ebeu].val_stv[i_g] = ebe_usr[i_ebeu].g[i_g];
   }

   return;
} // end of mat_dc_3b_e
// -----------------------------------------------------------------------------
void mat_startup_2_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_startup] = true;
   global.flags[global.i_implicit] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   ebe_init_func_jac_startup_e(ebe_lib,ebe_usr,cct,smat);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);

     mat_startup_3_e(i_ebeu,i_ebel,ebe_lib,ebe_usr,ebe_jac,smat,global);
   }
   global.flags[global.i_startup] = false;
   global.flags[global.i_implicit] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of mat_startup_2_e
// -----------------------------------------------------------------------------
void mat_startup_2_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_startup] = true;
   global.flags[global.i_implicit] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   xbe_init_jac_startup_x(xbe_lib,xbe_usr,cct,smat);

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);

     mat_startup_3_x(i_xbeu,i_xbel,xbe_lib,xbe_usr,xbe_jac,smat,global);
   }
   global.flags[global.i_startup] = false;
   global.flags[global.i_implicit] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of mat_startup_2_x
// -----------------------------------------------------------------------------
void mat_startup_2_exc_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_startup] = true;
   global.flags[global.i_implicit] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   ebe_init_func_jac_startup_exc_e(ebe_lib,ebe_usr,cct,smat);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);

     mat_startup_3_exc_e(i_ebeu,i_ebel,ebe_lib,ebe_usr,ebe_jac,smat,global);
   }
   global.flags[global.i_startup] = false;
   global.flags[global.i_implicit] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of mat_startup_2_exc_e
// -----------------------------------------------------------------------------
void mat_startup_2_exc_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_startup] = true;
   global.flags[global.i_implicit] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   xbe_init_jac_startup_exc_x(xbe_lib,xbe_usr,cct,smat);

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);

     mat_startup_3_exc_x(i_xbeu,i_xbel,xbe_lib,xbe_usr,xbe_jac,smat,global);
   }
   global.flags[global.i_startup] = false;
   global.flags[global.i_implicit] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of mat_startup_2_exc_x
// -----------------------------------------------------------------------------
void mat_startup_3_e(
   const int i_ebeu,
   const int i_ebel,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Global &global) {

   int i_h,n_h1,n_nd1,n_hvar1,k,row0;
   int var_flag,var_number;
   double val;

   n_h1 = ebe_lib[i_ebel].n_h;
   n_nd1 = ebe_lib[i_ebel].n_nd;

// KCL equations:
   for (i_h=0; i_h < n_nd1; i_h++) {
     n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
     for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
       k = smat.map_hvar_to_ebe[i_ebeu][i_h][i_hvar];
       if (k != -1) {
         var_flag   = ebe_lib[i_ebel].hvar_flag [i_h][i_hvar];
         var_number = ebe_lib[i_ebel].hvar_index[i_h][i_hvar];

         if (var_flag == global.I_NV) {
           val = ebe_jac[i_ebel].dhdv[i_h][var_number];
         } else if (var_flag == global.I_EAUXS) {
           val = ebe_jac[i_ebel].dhdauxs[i_h][var_number];
         } else if (var_flag == global.I_XVR) {
           val = ebe_jac[i_ebel].dhdxvr[i_h][var_number];
         } else {
           cout << "mat_startup_3_e: incorrect value of var_flag in h." << endl;
           cout << "  i_h = " << i_h << ", i_havr = " << i_hvar << endl;
           cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
           exit(1);
         }
         smat.m_e.val[k] += val;
       }
     }
     row0 = smat.ebe_h_to_row[i_ebeu][i_h];
     if (row0 != -1) {
       smat.rhs_m_e[row0] += ebe_usr[i_ebeu].h[i_h];
     }
   }
// non-KCL equations:
   for (i_h=n_nd1; i_h < n_h1; i_h++) {
     n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
     for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
       k = smat.map_hvar_to_ebe[i_ebeu][i_h][i_hvar];
       if (k != -1) {
         var_flag   = ebe_lib[i_ebel].hvar_flag [i_h][i_hvar];
         var_number = ebe_lib[i_ebel].hvar_index[i_h][i_hvar];

         if (var_flag == global.I_NV) {
           val = ebe_jac[i_ebel].dhdv[i_h][var_number];
         } else if (var_flag == global.I_EAUXS) {
           val = ebe_jac[i_ebel].dhdauxs[i_h][var_number];
         } else if (var_flag == global.I_XVR) {
           val = ebe_jac[i_ebel].dhdxvr[i_h][var_number];
         } else {
           cout << "mat_startup_3_e: incorrect value of var_flag in h." << endl;
           cout << "  i_h = " << i_h << ", i_havr = " << i_hvar << endl;
           cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
           exit(1);
         }
         smat.m_e.val[k] += val;
       }
     }
     row0 = smat.ebe_h_to_row[i_ebeu][i_h];
     smat.rhs_m_e[row0] = ebe_usr[i_ebeu].h[i_h];
   }
   return;
} // end of mat_startup_3_e
// -----------------------------------------------------------------------------
void mat_startup_3_x(
   const int i_xbeu,
   const int i_xbel,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Global &global) {

   int i_f,i_g,n_f1,n_g1,n_gvar1,k,row0;
   int var_flag,var_number;
   double val;

   if (xbe_lib[i_xbel].flag_integrate) {
     n_f1 = xbe_lib[i_xbel].n_f;

     for (i_f=0; i_f < n_f1; i_f++) {
       row0 = smat.xbe_f_to_row[i_xbeu][i_f];
       if (row0 == -1) {
         cout << "mat_startup_3_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_x[row0] = xbe_usr[i_xbeu].h[i_f];
       }
     }
   } else {
     n_g1 = xbe_lib[i_xbel].n_g;

     for (i_g=0; i_g < n_g1; i_g++) {
       n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
       for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
         k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
         if (k == -1) {
           cout << "mat_startup_3_x: k = -1 is not expected. Halting.." << endl;
           exit(1);
         } else {
           var_flag   = xbe_lib[i_xbel].gvar_flag [i_g][i_gvar];
           var_number = xbe_lib[i_xbel].gvar_index[i_g][i_gvar];

           if (var_flag == global.I_XVR) {
             val = xbe_jac[i_xbel].dgdvr[i_g][var_number];
           } else if (var_flag == global.I_XAUX) {
             val = xbe_jac[i_xbel].dgdaux[i_g][var_number];
           } else {
             cout << "mat_startup_3_x: incorrect value of var_flag." << endl;
             cout << "  i_g = " << i_g << ", i_gavr = " << i_gvar << endl;
             cout << "  xbe_lib is " << xbe_lib[i_xbel].name << ". Halting.." << endl;
             exit(1);
           }
           smat.m_x.val[k] = val;
         }
       }
       row0 = smat.xbe_g_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_startup_3_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_x[row0] = xbe_usr[i_xbeu].g[i_g];
       }
     }
   }

   return;
} // end of mat_startup_3_x
// -----------------------------------------------------------------------------
void mat_startup_3_exc_e(
   const int i_ebeu,
   const int i_ebel,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Global &global) {

   int i_h,n_h1,n_nd1,n_hvar1,k,row0;
   int var_flag,var_number;
   double val;

   n_h1 = ebe_lib[i_ebel].n_h;
   n_nd1 = ebe_lib[i_ebel].n_nd;

// KCL equations:
   for (i_h=0; i_h < n_nd1; i_h++) {
     n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
     for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
       k = smat.map_hvar_to_ebe[i_ebeu][i_h][i_hvar];
       if (k != -1) {
         var_flag   = ebe_lib[i_ebel].hvar_flag [i_h][i_hvar];
         var_number = ebe_lib[i_ebel].hvar_index[i_h][i_hvar];

         if (var_flag == global.I_NV) {
           val = ebe_jac[i_ebel].dhdv[i_h][var_number];
         } else if (var_flag == global.I_EAUXS) {
           val = ebe_jac[i_ebel].dhdauxs[i_h][var_number];
         } else if (var_flag == global.I_XVR) {
           val = ebe_jac[i_ebel].dhdxvr[i_h][var_number];
         } else {
           cout << "mat_startup_3_exc_e: incorrect value of var_flag in h." << endl;
           cout << "  i_h = " << i_h << ", i_havr = " << i_hvar << endl;
           cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
           exit(1);
         }
         smat.m_ex.val[k] += val;
       }
     }
     row0 = smat.ebe_h_to_row[i_ebeu][i_h];
     if (row0 != -1) {
       smat.rhs_m_ex[row0] += ebe_usr[i_ebeu].h[i_h];
     }
   }
// non-KCL equations:
   for (i_h=n_nd1; i_h < n_h1; i_h++) {
     n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
     for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
       k = smat.map_hvar_to_ebe[i_ebeu][i_h][i_hvar];
       if (k != -1) {
         var_flag   = ebe_lib[i_ebel].hvar_flag [i_h][i_hvar];
         var_number = ebe_lib[i_ebel].hvar_index[i_h][i_hvar];

         if (var_flag == global.I_NV) {
           val = ebe_jac[i_ebel].dhdv[i_h][var_number];
         } else if (var_flag == global.I_EAUXS) {
           val = ebe_jac[i_ebel].dhdauxs[i_h][var_number];
         } else if (var_flag == global.I_XVR) {
           val = ebe_jac[i_ebel].dhdxvr[i_h][var_number];
         } else {
           cout << "mat_startup_3_e: incorrect value of var_flag in h." << endl;
           cout << "  i_h = " << i_h << ", i_havr = " << i_hvar << endl;
           cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
           exit(1);
         }
         smat.m_ex.val[k] += val;
       }
     }
     row0 = smat.ebe_h_to_row[i_ebeu][i_h];
     smat.rhs_m_ex[row0] = ebe_usr[i_ebeu].h[i_h];
   }
   return;
} // end of mat_startup_3_ex
// -----------------------------------------------------------------------------
void mat_startup_3_exc_x(
   const int i_xbeu,
   const int i_xbel,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Global &global) {

   int i_f,i_g,n_f1,n_g1,n_gvar1,k,row0;
   int var_flag,var_number;
   double val;
   int k0,r0;

   k0 = smat.m_e.n_nz;
   r0 = smat.m_e.n_row;

   if (xbe_lib[i_xbel].flag_integrate) {
//   assign only rhs
     n_f1 = xbe_lib[i_xbel].n_f;

     for (i_f=0; i_f < n_f1; i_f++) {
       row0 = smat.xbe_f_to_row[i_xbeu][i_f];
       if (row0 == -1) {
         cout << "mat_startup_3_exc_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_ex[row0+r0] = xbe_usr[i_xbeu].h[i_f];
       }
     }
   } else {
//   assign rhs and jac
     n_g1 = xbe_lib[i_xbel].n_g;

     for (i_g=0; i_g < n_g1; i_g++) {
       n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
       for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
         k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
         if (k == -1) {
           cout << "mat_startup_3_exc_x: k = -1 is not expected. Halting.." << endl;
           exit(1);
         } else {
           var_flag   = xbe_lib[i_xbel].gvar_flag [i_g][i_gvar];
           var_number = xbe_lib[i_xbel].gvar_index[i_g][i_gvar];

           if (var_flag == global.I_XVR) {
             val = xbe_jac[i_xbel].dgdvr[i_g][var_number];
           } else if (var_flag == global.I_XAUX) {
             val = xbe_jac[i_xbel].dgdaux[i_g][var_number];
           } else {
             cout << "mat_startup_3_exc_x: incorrect value of var_flag." << endl;
             cout << "  i_g = " << i_g << ", i_gavr = " << i_gvar << endl;
             cout << "  xbe_lib is " << xbe_lib[i_xbel].name << ". Halting.." << endl;
             exit(1);
           }
           smat.m_ex.val[k+k0] = val;
         }
       }
       row0 = smat.xbe_g_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_startup_3_exc_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_ex[row0+r0] = xbe_usr[i_xbeu].g[i_g];
       }
     }
   }
   return;
} // end of mat_startup_3_exc_x
// -----------------------------------------------------------------------------
void mat_startup_3b_e(
   const int i_ebeu,
   const int i_ebel,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr) {

   int i_h,n_nd1;

   n_nd1 = ebe_lib[i_ebel].n_nd;

// get currents:
   for (i_h=0; i_h < n_nd1; i_h++) {
     ebe_usr[i_ebeu].cur_nd[i_h] = ebe_usr[i_ebeu].h[i_h];
   }

   return;
} // end of mat_startup_3b_e
// -----------------------------------------------------------------------------
void mat_trns_2_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_trns] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   ebe_init_func_jac_trns_e(ebe_lib,ebe_usr,cct,smat);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
     mat_trns_3_e(i_ebeu,i_ebel,ebe_lib,ebe_usr,ebe_jac,smat,slv,global);
   }

   global.flags[global.i_trns] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of mat_trns_2_e
// -----------------------------------------------------------------------------
void mat_trns_2_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   xbe_init_jac_trns_x(xbe_lib,xbe_usr,cct,smat);

   global.flags[global.i_trns] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;
   global.flags[global.i_implicit] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);
     mat_trns_3_x(i_xbeu,i_xbel,xbe_lib,xbe_usr,xbe_jac,smat,global);
   }
   global.flags[global.i_trns] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;
   global.flags[global.i_implicit] = false;

   return;
} // end of mat_trns_2_x
// -----------------------------------------------------------------------------
void mat_trns_2a_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_trns] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_implicit] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);
     mat_trns_3a_x(i_xbeu,i_xbel,xbe_lib,xbe_usr,smat);
   }
   global.flags[global.i_trns] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_implicit] = false;

   return;
} // end of mat_trns_2a_x
// -----------------------------------------------------------------------------
void mat_trns_2a_exc_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_trns] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_implicit] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);
     mat_trns_3a_exc_x(i_xbeu,i_xbel,xbe_lib,xbe_usr,smat);
   }
   global.flags[global.i_trns] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_implicit] = false;

   return;
} // end of mat_trns_2a_exc_x
// -----------------------------------------------------------------------------
void mat_trns_2_x_al(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_trns] = true;

   xbe_init_jac_startup_x(xbe_lib,xbe_usr,cct,smat);

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if (xbe_lib[i_xbel].flag_integrate) {
       global.flags[global.i_explicit] = true;
       global.flags[global.i_alg_loop] = true;
     } else {
       global.flags[global.i_implicit] = true;
       global.flags[global.i_function] = true;
       global.flags[global.i_jacobian] = true;
     }

     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);

     if (xbe_lib[i_xbel].flag_integrate) {
       global.flags[global.i_explicit] = false;
       global.flags[global.i_alg_loop] = false;
     } else {
       global.flags[global.i_implicit] = false;
       global.flags[global.i_function] = false;
       global.flags[global.i_jacobian] = false;
     }

     mat_startup_3_x(i_xbeu,i_xbel,xbe_lib,xbe_usr,xbe_jac,smat,global);
   }
   global.flags[global.i_trns] = false;

   return;
} // end of mat_trns_2_x_al
// -----------------------------------------------------------------------------
void mat_trns_2_exc_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_trns] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   ebe_init_func_jac_trns_exc_e(ebe_lib,ebe_usr,cct,smat);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
     mat_trns_3_exc_e(i_ebeu,i_ebel,ebe_lib,ebe_usr,ebe_jac,smat,slv,global);
   }

   global.flags[global.i_trns] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of mat_trns_2_exc_e
// -----------------------------------------------------------------------------
void mat_trns_2_exc_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   xbe_init_jac_trns_exc_x(xbe_lib,xbe_usr,cct,smat);

   global.flags[global.i_trns] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;
   global.flags[global.i_implicit] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);
     mat_trns_3_exc_x(i_xbeu,i_xbel,xbe_lib,xbe_usr,xbe_jac,smat,global);
   }
   global.flags[global.i_trns] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;
   global.flags[global.i_implicit] = false;

   return;
} // end of mat_trns_2_exc_x
// -----------------------------------------------------------------------------
void mat_trns_3_e(
   const int i_ebeu,
   const int i_ebel,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   int i_f,n_f1,n_nd1,n_fvar1,row0;
   bool flag_ddt,flag_ddt_1,flag_ddt_2;
   double x_0,x_1,x_2,f_1;
   int var_flag,var_number;
   int var_flag_stv,var_number_stv;
   double val;
   int i_stv,i_g,n_gvar1,k,kg;

   n_f1 = ebe_lib[i_ebel].n_f;
   n_nd1 = ebe_lib[i_ebel].n_nd;

// KCL equations:
   for (i_f=0; i_f < n_nd1; i_f++) {
     row0 = smat.ebe_f_to_row[i_ebeu][i_f];
     if (row0 != -1) {
       smat.rhs_m_e[row0] += ebe_usr[i_ebeu].f[i_f];

       flag_ddt = ebe_lib[i_ebel].f_ddt[i_f];

       if (flag_ddt) {
         i_stv = ebe_lib[i_ebel].f_ddt_stv_index[i_f];

         x_0 = ebe_usr[i_ebeu].val_stv  [i_stv];
         x_1 = ebe_usr[i_ebeu].val_stv_1[i_stv];

         if (slv.e_algo_be0) {
           smat.rhs_m_e[row0] +=
             slv.beuler_1_e*(x_0-x_1);
         } else if (slv.e_algo_trz0) {
           f_1 = ebe_usr[i_ebeu].cur_nd_1[i_f] - ebe_usr[i_ebeu].f_old_1[i_f];
           smat.rhs_m_e[row0] +=
             slv.trz_1_e*(x_0-x_1) - f_1;
         } else if (slv.e_algo_bdf2) {
           x_2 = ebe_usr[i_ebeu].val_stv_2[i_stv];
           smat.rhs_m_e[row0] +=
             slv.bdf2_1_e*x_0 - slv.bdf2_2_e*x_1 + slv.bdf2_3_e*x_2;
         }
       }
//     Jacobian entries:
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         flag_ddt_1 = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];

         if (flag_ddt_1) {
           i_g = ebe_lib[i_ebel].f_ddt_stv_eqn[i_f];
           n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];

           for (int i_gvar=1; i_gvar < n_gvar1; i_gvar++) {
             kg = smat.map_gvar_to_ebe[i_ebeu][i_g][i_gvar];
             if (kg != -1) {
               var_flag_stv = ebe_lib[i_ebel].gvar_flag[i_g][i_gvar];
               var_number_stv = ebe_lib[i_ebel].gvar_index[i_g][i_gvar];

               if (var_flag_stv == global.I_NV) {
                 val = ebe_jac[i_ebel].dgdv[i_g][var_number_stv];
               } else if (var_flag_stv == global.I_EAUX) {
                 val = ebe_jac[i_ebel].dgdaux[i_g][var_number_stv];
               }

               if (slv.e_algo_be0) {
                 smat.m_e.val[kg] += slv.beuler_1_e*val;
               } else if (slv.e_algo_trz0) {
                 smat.m_e.val[kg] += slv.trz_1_e*val;
               } else if (slv.e_algo_bdf2) {
                 smat.m_e.val[kg] += slv.bdf2_1_e*val;
               }
             }
           }
         } else {
           k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
           if (k != -1) {
             var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
             var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

             if (var_flag == global.I_NV) {
               val = ebe_jac[i_ebel].dfdv[i_f][var_number];
             } else if (var_flag == global.I_EAUX) {
               val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
             }
             smat.m_e.val[k] += val;
           }
         }
       }
     } else {
//     this equation should be ignored since it is adding to KCL at the ref node
       continue;
     }
   }
   for (i_f=n_nd1; i_f < n_f1; i_f++) {
     row0 = smat.ebe_f_to_row[i_ebeu][i_f];
     smat.rhs_m_e[row0] = ebe_usr[i_ebeu].f[i_f];
     n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
     for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
       k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
       if (k != -1) {
         var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
         var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

         if (var_flag == global.I_NV) {
           val = ebe_jac[i_ebel].dfdv[i_f][var_number];
         } else if (var_flag == global.I_EAUX) {
           val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
         }
         flag_ddt_2 = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];
         if (!flag_ddt_2) {
           smat.m_e.val[k] += val;
         }
       }
     }
   }
   return;
} // end of mat_trns_3_e
// -----------------------------------------------------------------------------
void mat_trns_3_x(
   const int i_xbeu,
   const int i_xbel,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Global &global) {

   int i_g,n_g1,n_gvar1,k,row0;
   int var_flag,var_number;
   double val;

   n_g1 = xbe_lib[i_xbel].n_g;

   for (i_g=0; i_g < n_g1; i_g++) {
     n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
     for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
       k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
       if (k == -1) {
         cout << "mat_trns_3_x: k = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         var_flag   = xbe_lib[i_xbel].gvar_flag [i_g][i_gvar];
         var_number = xbe_lib[i_xbel].gvar_index[i_g][i_gvar];

         if (var_flag == global.I_XVR) {
           val = xbe_jac[i_xbel].dgdvr[i_g][var_number];
         } else if (var_flag == global.I_XAUX) {
           val = xbe_jac[i_xbel].dgdaux[i_g][var_number];
         } else {
           cout << "mat_trns_3_x: incorrect value of var_flag." << endl;
           cout << "  i_g = " << i_g << ", i_gavr = " << i_gvar << endl;
           cout << "  xbe_lib is " << xbe_lib[i_xbel].name << ". Halting.." << endl;
           exit(1);
         }
         smat.m_x.val[k] = val;
       }
     }
     if (xbe_lib[i_xbel].flag_integrate) {
       row0 = smat.xbe_f_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_trns_3_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_x[row0] = xbe_usr[i_xbeu].g[i_g];
       }
     } else {
       row0 = smat.xbe_g_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_trns_3_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_x[row0] = xbe_usr[i_xbeu].g[i_g];
       }
     }
   }
   return;
} // end of mat_trns_3_x
// -----------------------------------------------------------------------------
void mat_trns_3_exc_e(
   const int i_ebeu,
   const int i_ebel,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   SolveBlocks &slv,
   Global &global) {

   int i_f,n_f1,n_nd1,n_fvar1,row0;
   bool flag_ddt,flag_ddt_1,flag_ddt_2;
   double x_0,x_1,x_2,f_1;
   int var_flag,var_number;
   int var_flag_stv,var_number_stv;
   double val;
   int i_stv,i_g,n_gvar1,k,kg;

   n_f1 = ebe_lib[i_ebel].n_f;
   n_nd1 = ebe_lib[i_ebel].n_nd;

// KCL equations:
   for (i_f=0; i_f < n_nd1; i_f++) {
     row0 = smat.ebe_f_to_row[i_ebeu][i_f];
     if (row0 != -1) {
       smat.rhs_m_ex[row0] += ebe_usr[i_ebeu].f[i_f];

       flag_ddt = ebe_lib[i_ebel].f_ddt[i_f];

       if (flag_ddt) {
         i_stv = ebe_lib[i_ebel].f_ddt_stv_index[i_f];

         x_0 = ebe_usr[i_ebeu].val_stv  [i_stv];
         x_1 = ebe_usr[i_ebeu].val_stv_1[i_stv];

         if (slv.ex_algo_be0) {
           smat.rhs_m_ex[row0] +=
             slv.beuler_1_e*(x_0-x_1);
         } else if (slv.ex_algo_trz0) {
           f_1 = ebe_usr[i_ebeu].cur_nd_1[i_f] - ebe_usr[i_ebeu].f_old_1[i_f];
           smat.rhs_m_ex[row0] +=
             slv.trz_1_e*(x_0-x_1) - f_1;
         } else if (slv.ex_algo_bdf2) {
           x_2 = ebe_usr[i_ebeu].val_stv_2[i_stv];
           smat.rhs_m_ex[row0] +=
             slv.bdf2_1_e*x_0 - slv.bdf2_2_e*x_1 + slv.bdf2_3_e*x_2;
         }
       }
//     Jacobian entries:
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         flag_ddt_1 = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];

         if (flag_ddt_1) {
           i_g = ebe_lib[i_ebel].f_ddt_stv_eqn[i_f];
           n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];

           for (int i_gvar=1; i_gvar < n_gvar1; i_gvar++) {
             kg = smat.map_gvar_to_ebe[i_ebeu][i_g][i_gvar];
             if (kg != -1) {
               var_flag_stv = ebe_lib[i_ebel].gvar_flag[i_g][i_gvar];
               var_number_stv = ebe_lib[i_ebel].gvar_index[i_g][i_gvar];

               if (var_flag_stv == global.I_NV) {
                 val = ebe_jac[i_ebel].dgdv[i_g][var_number_stv];
               } else if (var_flag_stv == global.I_EAUX) {
                 val = ebe_jac[i_ebel].dgdaux[i_g][var_number_stv];
               }

               if (slv.ex_algo_be0) {
                 smat.m_ex.val[kg] += slv.beuler_1_e*val;
               } else if (slv.ex_algo_trz0) {
                 smat.m_ex.val[kg] += slv.trz_1_e*val;
               } else if (slv.ex_algo_bdf2) {
                 smat.m_ex.val[kg] += slv.bdf2_1_e*val;
               }
             }
           }
         } else {
           k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
           if (k != -1) {
             var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
             var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

             if (var_flag == global.I_NV) {
               val = ebe_jac[i_ebel].dfdv[i_f][var_number];
             } else if (var_flag == global.I_EAUX) {
               val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
             } else if (var_flag == global.I_XVR) {
               val = ebe_jac[i_ebel].dfdxvr[i_f][var_number];
             }
             smat.m_ex.val[k] += val;
           }
         }
       }
     } else {
//     this equation should be ignored since it is adding to KCL at the ref node
       continue;
     }
   }
   for (i_f=n_nd1; i_f < n_f1; i_f++) {
     row0 = smat.ebe_f_to_row[i_ebeu][i_f];
     smat.rhs_m_ex[row0] = ebe_usr[i_ebeu].f[i_f];
     n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
     for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
       k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
       if (k != -1) {
         var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
         var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

         if (var_flag == global.I_NV) {
           val = ebe_jac[i_ebel].dfdv[i_f][var_number];
         } else if (var_flag == global.I_EAUX) {
           val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
         } else if (var_flag == global.I_XVR) {
           val = ebe_jac[i_ebel].dfdxvr[i_f][var_number];
         }
         flag_ddt_2 = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];
         if (!flag_ddt_2) {
           smat.m_ex.val[k] += val;
         }
       }
     }
   }
   return;
} // end of mat_trns_3_exc_e
// -----------------------------------------------------------------------------
void mat_trns_3_exc_x(
   const int i_xbeu,
   const int i_xbel,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Global &global) {

   int i_g,n_g1,n_gvar1,k,row0;
   int var_flag,var_number;
   double val;
   int k0,r0;

   k0 = smat.m_e.n_nz;
   r0 = smat.m_e.n_row;

   n_g1 = xbe_lib[i_xbel].n_g;

   for (i_g=0; i_g < n_g1; i_g++) {
     n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
     for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
       k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
       if (k == -1) {
         cout << "mat_trns_3_exc_x: k = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         var_flag   = xbe_lib[i_xbel].gvar_flag [i_g][i_gvar];
         var_number = xbe_lib[i_xbel].gvar_index[i_g][i_gvar];

         if (var_flag == global.I_XVR) {
           val = xbe_jac[i_xbel].dgdvr[i_g][var_number];
         } else if (var_flag == global.I_XAUX) {
           val = xbe_jac[i_xbel].dgdaux[i_g][var_number];
         } else {
           cout << "mat_trns_3_exc_x: incorrect value of var_flag." << endl;
           cout << "  i_g = " << i_g << ", i_gavr = " << i_gvar << endl;
           cout << "  xbe_lib is " << xbe_lib[i_xbel].name << ". Halting.." << endl;
           exit(1);
         }
         smat.m_ex.val[k+k0] = val;
       }
     }
     if (xbe_lib[i_xbel].flag_integrate) {
       row0 = smat.xbe_f_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_trns_3_exc_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_ex[row0+r0] = xbe_usr[i_xbeu].g[i_g];
       }
     } else {
       row0 = smat.xbe_g_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_trns_3_exc_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_ex[row0+r0] = xbe_usr[i_xbeu].g[i_g];
       }
     }
   }
   return;
} // end of mat_trns_3_exc_x
// -----------------------------------------------------------------------------
void mat_trns_3a_x(
   const int i_xbeu,
   const int i_xbel,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   SysMat &smat) {

   int i_g,n_g1,row0;

   n_g1 = xbe_lib[i_xbel].n_g;

   for (i_g=0; i_g < n_g1; i_g++) {
     if (xbe_lib[i_xbel].flag_integrate) {
       row0 = smat.xbe_f_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_trns_3a_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_x[row0] = xbe_usr[i_xbeu].g[i_g];
       }
     } else {
       row0 = smat.xbe_g_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_trns_3a_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_x[row0] = xbe_usr[i_xbeu].g[i_g];
       }
     }
   }
   return;
} // end of mat_trns_3a_x
// -----------------------------------------------------------------------------
void mat_trns_3a_exc_x(
   const int i_xbeu,
   const int i_xbel,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   SysMat &smat) {

   int i_g,n_g1,row0;
   int r0;

   r0 = smat.m_e.n_row;
   n_g1 = xbe_lib[i_xbel].n_g;

   for (i_g=0; i_g < n_g1; i_g++) {
     if (xbe_lib[i_xbel].flag_integrate) {
       row0 = smat.xbe_f_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_trns_3a_exc_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_ex[row0+r0] = xbe_usr[i_xbeu].g[i_g];
       }
     } else {
       row0 = smat.xbe_g_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_trns_3a_exc_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_ex[row0+r0] = xbe_usr[i_xbeu].g[i_g];
       }
     }
   }
   return;
} // end of mat_trns_3a_exc_x
// -----------------------------------------------------------------------------
void mat_trns_3b_e(
   const int i_ebeu,
   const int i_ebel,
   SolveBlocks &slv,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr) {

   int i_f,i_stv,n_nd1;
   bool flag_ddt;
   double x_0,x_1,x_2,f_1;

   n_nd1 = ebe_lib[i_ebel].n_nd;

   for (i_f=0; i_f < n_nd1; i_f++) {

     flag_ddt = ebe_lib[i_ebel].f_ddt[i_f];

     if (flag_ddt) {
       i_stv = ebe_lib[i_ebel].f_ddt_stv_index[i_f];

       x_0 = ebe_usr[i_ebeu].val_stv  [i_stv];
       x_1 = ebe_usr[i_ebeu].val_stv_1[i_stv];

       if (slv.e_algo_be0) {
         ebe_usr[i_ebeu].cur_nd[i_f] = ebe_usr[i_ebeu].f[i_f]
           + slv.beuler_1_e*(x_0-x_1);
       } else if (slv.e_algo_trz0) {
         f_1 = ebe_usr[i_ebeu].cur_nd_1[i_f] - ebe_usr[i_ebeu].f_old_1[i_f];
         ebe_usr[i_ebeu].cur_nd[i_f] = ebe_usr[i_ebeu].f[i_f]
           + slv.trz_1_e*(x_0-x_1) - f_1;
       } else if (slv.e_algo_bdf2) {
         x_2 = ebe_usr[i_ebeu].val_stv_2[i_stv];
         ebe_usr[i_ebeu].cur_nd[i_f] = ebe_usr[i_ebeu].f[i_f]
           + slv.bdf2_1_e*x_0 - slv.bdf2_2_e*x_1 + slv.bdf2_3_e*x_2;
       }
     } else {
       ebe_usr[i_ebeu].cur_nd[i_f] = ebe_usr[i_ebeu].f[i_f];
     }
   }

   return;
} // end of mat_trns_3b_e
// -----------------------------------------------------------------------------
void mat_trns_3b_exc(
   const int i_ebeu,
   const int i_ebel,
   SolveBlocks &slv,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr) {

   int i_f,i_stv,n_nd1;
   bool flag_ddt;
   double x_0,x_1,x_2,f_1;

   n_nd1 = ebe_lib[i_ebel].n_nd;

   for (i_f=0; i_f < n_nd1; i_f++) {

     flag_ddt = ebe_lib[i_ebel].f_ddt[i_f];

     if (flag_ddt) {
       i_stv = ebe_lib[i_ebel].f_ddt_stv_index[i_f];

       x_0 = ebe_usr[i_ebeu].val_stv  [i_stv];
       x_1 = ebe_usr[i_ebeu].val_stv_1[i_stv];

       if (slv.ex_algo_be0) {
         ebe_usr[i_ebeu].cur_nd[i_f] = ebe_usr[i_ebeu].f[i_f]
           + slv.beuler_1_e*(x_0-x_1);
       } else if (slv.ex_algo_trz0) {
         f_1 = ebe_usr[i_ebeu].cur_nd_1[i_f] - ebe_usr[i_ebeu].f_old_1[i_f];
         ebe_usr[i_ebeu].cur_nd[i_f] = ebe_usr[i_ebeu].f[i_f]
           + slv.trz_1_e*(x_0-x_1) - f_1;
       } else if (slv.ex_algo_bdf2) {
         x_2 = ebe_usr[i_ebeu].val_stv_2[i_stv];
         ebe_usr[i_ebeu].cur_nd[i_f] = ebe_usr[i_ebeu].f[i_f]
           + slv.bdf2_1_e*x_0 - slv.bdf2_2_e*x_1 + slv.bdf2_3_e*x_2;
       }
     } else {
       ebe_usr[i_ebeu].cur_nd[i_f] = ebe_usr[i_ebeu].f[i_f];
     }
   }

   return;
} // end of mat_trns_3b_exc
// -----------------------------------------------------------------------------
void add_trns_terms_e(
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   SolveBlocks &slv) {

   int i_ebeu,i_f;
   double x_0,x_1,x_2,f_1;
   int i_rhs,i_var,pntr;

   if (slv.e_algo_be0) {
     for (i_rhs=0; i_rhs < smat.m_e.n_row; i_rhs++) {
       if (smat.ebe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.ebe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.ebe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_e      [i_var];
         x_1 = smat.svec_old_1_e[i_var];
         smat.rhs_m_e[i_rhs] += -slv.beuler_1_e*(x_0-x_1);
         smat.m_e.val[pntr] += -slv.beuler_1_e;
       }
     }
   } else if (slv.e_algo_trz0) {
     for (i_rhs=0; i_rhs < smat.m_e.n_row; i_rhs++) {
       if (smat.ebe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.ebe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.ebe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_e      [i_var];
         x_1 = smat.svec_old_1_e[i_var];

         i_ebeu = smat.ebe_rhs_ddt_i_ebeu[i_rhs];
         i_f = smat.ebe_rhs_ddt_i_f[i_rhs];
         f_1 = ebe_usr[i_ebeu].f_old_1[i_f];

         smat.rhs_m_e[i_rhs] += -slv.trz_1_e*(x_0-x_1) + f_1;
         smat.m_e.val[pntr] += -slv.trz_1_e;
       }
     }
   } else if (slv.e_algo_bdf2) {
     for (i_rhs=0; i_rhs < smat.m_e.n_row; i_rhs++) {
       if (smat.ebe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.ebe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.ebe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_e      [i_var];
         x_1 = smat.svec_old_1_e[i_var];
         x_2 = smat.svec_old_2_e[i_var];

         smat.rhs_m_e[i_rhs] +=
           -(slv.bdf2_1_e*x_0 - slv.bdf2_2_e*x_1 + slv.bdf2_3_e*x_2);
         smat.m_e.val[pntr] += -slv.bdf2_1_e;
       }
     }
   }

   return;
} // end of add_trns_terms_e
// -----------------------------------------------------------------------------
void add_trns_terms_x(
   vector<XbeUsr> &xbe_usr,
   SysMat &smat,
   SolveBlocks &slv) {

   int i_xbeu,i_f;
   double x_0,x_1,x_2,f_1;
   int i_rhs,i_var,pntr;

   if (slv.x_algo_be0) {
     for (i_rhs=0; i_rhs < smat.m_x.n_row; i_rhs++) {
       if (smat.xbe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.xbe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.xbe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_x      [i_var];
         x_1 = smat.svec_old_1_x[i_var];
         smat.rhs_m_x[i_rhs] +=
           -slv.beuler_1_x*(x_0-x_1);

         smat.m_x.val[pntr] += -slv.beuler_1_x;

       }
     }
   } else if (slv.x_algo_trz0) {
     for (i_rhs=0; i_rhs < smat.m_x.n_row; i_rhs++) {
       if (smat.xbe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.xbe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.xbe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_x      [i_var];
         x_1 = smat.svec_old_1_x[i_var];

         i_xbeu = smat.xbe_rhs_ddt_i_xbeu[i_rhs];
         i_f = smat.xbe_rhs_ddt_i_f[i_rhs];
         f_1 = xbe_usr[i_xbeu].g_old_1[i_f];

         smat.rhs_m_x[i_rhs] +=
           -slv.trz_1_x*(x_0-x_1) + f_1;
         smat.m_x.val[pntr] += -slv.trz_1_x;
       }
     }
   } else if (slv.x_algo_bdf2) {
     for (i_rhs=0; i_rhs < smat.m_x.n_row; i_rhs++) {
       if (smat.xbe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.xbe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.xbe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_x      [i_var];
         x_1 = smat.svec_old_1_x[i_var];
         x_2 = smat.svec_old_2_x[i_var];

         smat.rhs_m_x[i_rhs] +=
           -(slv.bdf2_1_x*x_0 - slv.bdf2_2_x*x_1 + slv.bdf2_3_x*x_2);
         smat.m_x.val[pntr] += -slv.bdf2_1_x;
       }
     }
   }
// cout << "add_trns_terms_x ending" << endl;

   return;
} // end of add_trns_terms_x
// -----------------------------------------------------------------------------
void add_trns_terms_exc_e(
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   SolveBlocks &slv) {

   int i_ebeu,i_f;
   double x_0,x_1,x_2,f_1;
   int i_rhs,i_var,pntr;

   if (slv.ex_algo_be0) {
     for (i_rhs=0; i_rhs < smat.m_e.n_row; i_rhs++) {
       if (smat.ebe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.ebe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.ebe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_ex      [i_var];
         x_1 = smat.svec_old_1_ex[i_var];

         smat.rhs_m_ex[i_rhs] +=
           -slv.beuler_1_e*(x_0-x_1);
         smat.m_ex.val[pntr] += -slv.beuler_1_e;
       }
     }
   } else if (slv.ex_algo_trz0) {
     for (i_rhs=0; i_rhs < smat.m_e.n_row; i_rhs++) {
       if (smat.ebe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.ebe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.ebe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_ex      [i_var];
         x_1 = smat.svec_old_1_ex[i_var];

         i_ebeu = smat.ebe_rhs_ddt_i_ebeu[i_rhs];
         i_f = smat.ebe_rhs_ddt_i_f[i_rhs];
         f_1 = ebe_usr[i_ebeu].f_old_1[i_f];

         smat.rhs_m_ex[i_rhs] +=
           -slv.trz_1_e*(x_0-x_1) + f_1;
         smat.m_ex.val[pntr] += -slv.trz_1_e;
       }
     }
   } else if (slv.ex_algo_bdf2) {
     for (i_rhs=0; i_rhs < smat.m_e.n_row; i_rhs++) {
       if (smat.ebe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.ebe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.ebe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_ex      [i_var];
         x_1 = smat.svec_old_1_ex[i_var];
         x_2 = smat.svec_old_2_ex[i_var];

         smat.rhs_m_ex[i_rhs] +=
           -(slv.bdf2_1_e*x_0 - slv.bdf2_2_e*x_1 + slv.bdf2_3_e*x_2);
         smat.m_ex.val[pntr] += -slv.bdf2_1_e;
       }
     }
   }

   return;
} // end of add_trns_terms_exc_e
// -----------------------------------------------------------------------------
void add_trns_terms_exc_x(
   vector<XbeUsr> &xbe_usr,
   SysMat &smat,
   SolveBlocks &slv) {

   int i_xbeu,i_f;
   double x_0,x_1,x_2,f_1;
   int i_rhs,i_var,pntr;
   int k0,r0;

   k0 = smat.m_e.n_nz;
   r0 = smat.m_e.n_row;

   if (slv.ex_algo_be0) {
     for (i_rhs=0; i_rhs < smat.m_x.n_row; i_rhs++) {
       if (smat.xbe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.xbe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.xbe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_ex      [i_var];
         x_1 = smat.svec_old_1_ex[i_var];
         smat.rhs_m_ex[i_rhs+r0] +=
           -slv.beuler_1_x*(x_0-x_1);
         smat.m_ex.val[pntr+k0] += -slv.beuler_1_x;
       }
     }
   } else if (slv.ex_algo_trz0) {
     for (i_rhs=0; i_rhs < smat.m_x.n_row; i_rhs++) {
       if (smat.xbe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.xbe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.xbe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_ex      [i_var];
         x_1 = smat.svec_old_1_ex[i_var];

         i_xbeu = smat.xbe_rhs_ddt_i_xbeu[i_rhs];
         i_f = smat.xbe_rhs_ddt_i_f[i_rhs];
         f_1 = xbe_usr[i_xbeu].g_old_1[i_f];

         smat.rhs_m_ex[i_rhs+r0] +=
           -slv.trz_1_x*(x_0-x_1) + f_1;
         smat.m_ex.val[pntr+k0] += -slv.trz_1_x;
       }
     }
   } else if (slv.ex_algo_bdf2) {
     for (i_rhs=0; i_rhs < smat.m_x.n_row; i_rhs++) {
       if (smat.xbe_rhs_ddt_flag[i_rhs]) {
         i_var = smat.xbe_rhs_ddt_varnumber[i_rhs];
         pntr = smat.xbe_rhs_ddt_pntr[i_rhs];
         x_0 = smat.svec_ex      [i_var];
         x_1 = smat.svec_old_1_ex[i_var];
         x_2 = smat.svec_old_2_ex[i_var];

         smat.rhs_m_ex[i_rhs+r0] +=
           -(slv.bdf2_1_x*x_0 - slv.bdf2_2_x*x_1 + slv.bdf2_3_x*x_2);
         smat.m_ex.val[pntr+k0] += -slv.bdf2_1_x;
       }
     }
   }

   return;
} // end of add_trns_terms_exc_x
// -----------------------------------------------------------------------------
void find_ebe_cur_stv_dc(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_dc] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   ebe_init_func_jac_dc_e(ebe_lib,ebe_usr,cct,smat);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
     mat_dc_3b_e(i_ebeu,i_ebel,ebe_lib,ebe_usr);
   }

   global.flags[global.i_dc] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of find_ebe_cur_stv_dc
// -----------------------------------------------------------------------------
void find_ebe_cur_stv_startup(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_startup] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   ebe_init_func_jac_startup_e(ebe_lib,ebe_usr,cct,smat);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
     mat_startup_3b_e(i_ebeu,i_ebel,ebe_lib,ebe_usr);
   }

   global.flags[global.i_startup] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of find_ebe_cur_stv_startup
// -----------------------------------------------------------------------------
void ebe_init_func_jac_dc_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_ebeu,i_ebel,i_f,n_f1,n_nd1,n_fvar1;
   int k,row0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;

     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
     for (i_f=0; i_f < n_nd1; i_f++) {
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
         if (k != -1) {
           smat.m_e.val[k] = 0.0;
         }
       }
       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       if (row0 != -1) {
         smat.rhs_m_e[row0] = 0.0;
       }
     }
//   non-KCL equations:
     for (i_f=n_nd1; i_f < n_f1; i_f++) {
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
         if (k != -1) {
           smat.m_e.val[k] = 0.0;
         }
       }
     }
   }
   return;
} // end of ebe_init_func_jac_dc_e
// -----------------------------------------------------------------------------
void ebe_init_func_jac_startup_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_ebeu,i_ebel,i_h,n_h1,n_nd1,n_hvar1;
   int k,row0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;

     n_h1 = ebe_lib[i_ebel].n_h;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
     for (i_h=0; i_h < n_nd1; i_h++) {
       n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
       for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
         k = smat.map_hvar_to_ebe[i_ebeu][i_h][i_hvar];
         if (k != -1) {
           smat.m_e.val[k] = 0.0;
         }
       }
       row0 = smat.ebe_h_to_row[i_ebeu][i_h];
       if (row0 != -1) {
         smat.rhs_m_e[row0] = 0.0;
       }
     }
//   non-KCL equations:
     for (i_h=n_nd1; i_h < n_h1; i_h++) {
       n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
       for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
         k = smat.map_hvar_to_ebe[i_ebeu][i_h][i_hvar];
         if (k != -1) {
           smat.m_e.val[k] = 0.0;
         }
       }
     }
   }

   return;
} // end of ebe_init_func_jac_startup_e
// -----------------------------------------------------------------------------
void ebe_init_func_jac_startup_exc_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_ebeu,i_ebel,i_h,n_h1,n_nd1,n_hvar1;
   int k,row0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;

     n_h1 = ebe_lib[i_ebel].n_h;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
     for (i_h=0; i_h < n_nd1; i_h++) {
       n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
       for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
         k = smat.map_hvar_to_ebe[i_ebeu][i_h][i_hvar];
         if (k != -1) {
           smat.m_ex.val[k] = 0.0;
         }
       }
       row0 = smat.ebe_h_to_row[i_ebeu][i_h];
       if (row0 != -1) {
         smat.rhs_m_ex[row0] = 0.0;
       }
     }
//   non-KCL equations:
     for (i_h=n_nd1; i_h < n_h1; i_h++) {
       n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
       for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
         k = smat.map_hvar_to_ebe[i_ebeu][i_h][i_hvar];
         if (k != -1) {
           smat.m_ex.val[k] = 0.0;
         }
       }
     }
   }
   return;
} // end of ebe_init_func_jac_startup_exc_e
// -----------------------------------------------------------------------------
void xbe_init_jac_startup_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_xbeu,i_xbel,k,i_g,n_g1,n_gvar1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if (!xbe_lib[i_xbel].flag_integrate) {
       n_g1 = xbe_lib[i_xbel].n_g;
       for (i_g=0; i_g < n_g1; i_g++) {
         n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
         for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
           k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
           smat.m_x.val[k] = 0.0;
         }
       }
     }
   }
   return;
} // end of xbe_init_jac_startup_x
// -----------------------------------------------------------------------------
void xbe_init_jac_startup_exc_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_xbeu,i_xbel,k,i_g,n_g1,n_gvar1,k0;

   k0 = smat.m_e.n_nz;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if (!xbe_lib[i_xbel].flag_integrate) {
       n_g1 = xbe_lib[i_xbel].n_g;
       for (i_g=0; i_g < n_g1; i_g++) {
         n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
         for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
           k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
           smat.m_ex.val[k+k0] = 0.0;
         }
       }
     }
   }
   return;
} // end of xbe_init_jac_startup_exc_x
// -----------------------------------------------------------------------------
void ebe_init_func_jac_trns_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_ebeu,i_ebel,i_f,n_f1,n_nd1,n_fvar1;
   int k,row0;
   int i_g,n_gvar1,kg;
   bool flag_ddt;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;

     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
     for (i_f=0; i_f < n_nd1; i_f++) {
       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       if (row0 != -1) {
         smat.rhs_m_e[row0] = 0.0;
         n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
         for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
           flag_ddt = ebe_lib[i_ebel].f_ddt[i_f];
           if (flag_ddt) {
             i_g = ebe_lib[i_ebel].f_ddt_stv_eqn[i_f];
             n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];

             for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
               kg = smat.map_gvar_to_ebe[i_ebeu][i_g][i_gvar];
               if (kg != -1) {
                 smat.m_e.val[kg] = 0.0;
               }
             }
           } else {
             k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
             if (k != -1) {
               smat.m_e.val[k] = 0.0;
             }
           }
         }
       } else {
         continue;
       }
     }
//   non-KCL equations:
     for (i_f=n_nd1; i_f < n_f1; i_f++) {
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
         if (k != -1) {
           smat.m_e.val[k] = 0.0;
         }
       }
     }
   }
   return;
} // end of ebe_init_func_jac_trns_e
// -----------------------------------------------------------------------------
void ebe_init_func_jac_trns_exc_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_ebeu,i_ebel,i_f,n_f1,n_nd1,n_fvar1;
   int k,row0;
   int i_g,n_gvar1,kg;
   bool flag_ddt;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;

     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
     for (i_f=0; i_f < n_nd1; i_f++) {
       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       if (row0 != -1) {
         smat.rhs_m_ex[row0] = 0.0;
         n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
         for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
           flag_ddt = ebe_lib[i_ebel].f_ddt[i_f];
           if (flag_ddt) {
             i_g = ebe_lib[i_ebel].f_ddt_stv_eqn[i_f];
             n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];

             for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
               kg = smat.map_gvar_to_ebe[i_ebeu][i_g][i_gvar];
               if (kg != -1) {
                 smat.m_ex.val[kg] = 0.0;
               }
             }
           } else {
             k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
             if (k != -1) {
               smat.m_ex.val[k] = 0.0;
             }
           }
         }
       } else {
         continue;
       }
     }
//   non-KCL equations:
     for (i_f=n_nd1; i_f < n_f1; i_f++) {
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
         if (k != -1) {
           smat.m_ex.val[k] = 0.0;
         }
       }
     }
   }
   return;
} // end of ebe_init_func_jac_trns_exc_e
// -----------------------------------------------------------------------------
void xbe_init_jac_trns_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_xbeu,i_xbel,i_f,row0,k,i_g,n_f1,n_g1,n_gvar1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;
       for (i_f=0; i_f < n_f1; i_f++) {
         row0 = smat.xbe_f_to_row[i_xbeu][i_f];
         k = smat.xbe_rhs_ddt_pntr[row0];
         smat.m_x.val[k] = 0.0;
       }
     }
     n_g1 = xbe_lib[i_xbel].n_g;
     for (i_g=0; i_g < n_g1; i_g++) {
       n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
       for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
         k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
         smat.m_x.val[k] = 0.0;
       }
     }
   }
   return;
} // end of xbe_init_jac_trns_x
// -----------------------------------------------------------------------------
void xbe_init_jac_trns_exc_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_xbeu,i_xbel,i_f,row0,k,i_g,n_f1,n_g1,n_gvar1,k0;

   k0 = smat.m_e.n_nz;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;
       for (i_f=0; i_f < n_f1; i_f++) {
         row0 = smat.xbe_f_to_row[i_xbeu][i_f];
         k = smat.xbe_rhs_ddt_pntr[row0];
         smat.m_ex.val[k+k0] = 0.0;
       }
     }
     n_g1 = xbe_lib[i_xbel].n_g;
     for (i_g=0; i_g < n_g1; i_g++) {
       n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
       for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
         k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
         smat.m_ex.val[k+k0] = 0.0;
       }
     }
   }
   return;
} // end of xbe_init_jac_trns_exc_x
// -----------------------------------------------------------------------------
void xbe_init_jac_ssw_trns_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_xbeu,i_xbel,i_f,row0,k,i_g,n_f1,n_g1,n_gvar1,k0;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;
       for (i_f=0; i_f < n_f1; i_f++) {
         row0 = smat.xbe_f_to_row[i_xbeu][i_f];
         k = smat.xbe_rhs_ddt_pntr[row0];
         smat.m_ssw.val[k] = 0.0;
       }
     }
     n_g1 = xbe_lib[i_xbel].n_g;
     for (i_g=0; i_g < n_g1; i_g++) {
       n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
       for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
         k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
         smat.m_ssw.val[k] = 0.0;
       }
     }
   }
   return;
} // end of xbe_init_jac_ssw_trns_x
// -----------------------------------------------------------------------------
void xbe_init_jac_ssw_trns_ex_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_xbeu,i_xbel,i_f,row0,k,i_g,n_f1,n_g1,n_gvar1,k0;

   k0 = smat.m_kcl.n_nz + smat.m_e.n_nz;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;
       for (i_f=0; i_f < n_f1; i_f++) {
         row0 = smat.xbe_f_to_row[i_xbeu][i_f];
         k = smat.xbe_rhs_ddt_pntr[row0];
         smat.m_ssw.val[k+k0] = 0.0;
       }
     }
     n_g1 = xbe_lib[i_xbel].n_g;
     for (i_g=0; i_g < n_g1; i_g++) {
       n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
       for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
         k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
         smat.m_ssw.val[k+k0] = 0.0;
       }
     }
   }
   return;
} // end of xbe_init_jac_ssw_trns_ex_x
// -----------------------------------------------------------------------------
void ebe_init_func_trns_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_ebeu,i_ebel,i_f,n_nd1;
   int row0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
     for (i_f=0; i_f < n_nd1; i_f++) {
       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       if (row0 != -1) {
         smat.rhs_m_e[row0] = 0.0;
       }
     }
   }
   return;
} // end of ebe_init_func_trns_e
// -----------------------------------------------------------------------------
void ebe_init_func_trns_ex(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_ebeu,i_ebel,i_f,n_nd1;
   int row0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
     for (i_f=0; i_f < n_nd1; i_f++) {
       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       if (row0 != -1) {
         smat.rhs_m_ex[row0] = 0.0;
       }
     }
   }
   return;
} // end of ebe_init_func_trns_ex
// -----------------------------------------------------------------------------
void ebe_init_func_ssw_trns_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_ebeu,i_ebel,i_f,n_nd1;
   int row0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
     for (i_f=0; i_f < n_nd1; i_f++) {
       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       if (row0 != -1) {
         smat.rhs_m_ssw[row0] = 0.0;
       }
     }
   }
   return;
} // end of ebe_init_func_ssw_trns_e
// -----------------------------------------------------------------------------
void solve_ssw(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   if (smat.n_statevar == 0) {
     cout << "solve_ssw: n_statevar = 0?!" << endl;
     cout << "   Halting..." << endl; exit(1);
   }

   if (cct.flag_e_only) {
     solve_ssw_e(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (cct.flag_x_only) {
     solve_ssw_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   } else if (cct.flag_exc) {
     solve_ssw_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   }

   return;
} // end of solve_ssw
// -----------------------------------------------------------------------------
void solve_ssw_e(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int k,k1;
   double rhs_ssw_norm,norm0;
   bool flag_rhs2_converged,flag_spice_converged,flag_net_converged;
   int i_ebeu,i_ebel,n_nd1;

   init_sol_e(xbe_lib,xbe_usr,ebe_lib,ebe_usr,ebe_jac,slv,cct,smat,global);

   smat.mat_ssw_1_e(ebe_lib,ebe_usr,global,cct,cct_file);

   smat.offs_ssw[0] = 0;
   for (int i_statevar=1; i_statevar < smat.n_statevar; i_statevar++) {
     smat.offs_ssw[i_statevar] = smat.offs_ssw[i_statevar-1] + smat.n_solvec_e;
   }

   if (slv.solve_type_previous != global.I_SSW) {
     for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
       k = smat.ssw_indx2[i_statevar];
       smat.svec_ssw_2[i_statevar] = smat.svec_e[k];
     }
   }

   for (int i_newton=0; i_newton < slv.ssw_nr_itermax; i_newton++) {
     slv.ssw_iter_newton = i_newton;

     copy_array_1<double>(smat.n_statevar,smat.svec_ssw_2,smat.svec_ssw_2_old);

     if (slv.ssw_iter_newton > -1) {
       for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
         k = smat.ssw_indx2[i_statevar];
         smat.svec_e[k] = smat.svec_ssw_2[i_statevar];
       }
     }

     for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
       k  = smat.ssw_indx2[i_statevar];
       k1 = smat.offs_ssw [i_statevar];

       set_vector_1(&(smat.svec_ssw_1[k1]),smat.n_solvec_e,k);
     }

     solve_ssw_1_e(false,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);

     for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
       k = smat.ssw_indx2[i_statevar];
       smat.svec_ssw_2[i_statevar] = smat.svec_e[k];
     }

     diff_arrays_1<double>(smat.n_statevar,smat.svec_ssw_2_old,smat.svec_ssw_2,
       smat.rhs_ssw);

     rhs_ssw_norm = norm_2(smat.n_statevar,smat.rhs_ssw);
     cout << "solve_ssw_e: ssw_iter_newton=" << slv.ssw_iter_newton
       << ", rhs_ssw_norm=" << rhs_ssw_norm << endl;

     if (slv.ssw_nr_flag_check_rhs2) {
       flag_rhs2_converged = (rhs_ssw_norm < slv.ssw_nr_eps_rhs);
     }
     flag_net_converged = true;
     if (slv.ssw_nr_flag_check_rhs2) {
       if (!flag_rhs2_converged) {
         flag_net_converged = false;
       }
     }
     if (flag_net_converged) goto jump1;

     mat_solve(smat.n_statevar,smat.ssw_mat,smat.ssw_mat_1,
       smat.rhs_ssw,smat.delsvec_ssw_2,
       smat.indxc_ssw,smat.indxr_ssw,smat.ipiv_ssw);

     if (slv.ssw_nr_flag_dmp) {
       if (slv.ssw_iter_newton <= slv.ssw_nr_dmp_itermax) {
         mult_array_1<double> (smat.n_statevar,smat.delsvec_ssw_2,
           slv.ssw_nr_dmp_k);
       }
     }
     add_arrays_2<double>(smat.n_statevar,smat.svec_ssw_2_old,
       smat.delsvec_ssw_2,smat.svec_ssw_2);
   }

   cout << "solve_ssw_e: convergence not reached." << endl;
   cout << "  This is a warning only." << endl;

   jump1: ;

// Do one more trns step for output.

   cout << "solve_ssw_e: calling solve_ssw_1_e for one more trns step" << endl;
   slv.flag_ssw_final_trns = true;

   solve_ssw_1_e(true,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     smat,cct,slv,cct_file,global);

   cout << "solve_ssw_1_e over (after trns step for output)" << endl;
   cout << "solve_ssw_e ends, slv.ssw_iter_newton="
     << slv.ssw_iter_newton << endl;

   return;
} // end of solve_ssw_e
// -----------------------------------------------------------------------------
void solve_ssw_1_e(
   const bool l_write_1,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i0,iter_stepred,indx_1;
   double time_next_1;
   double time_epsilon=1.0e-13;

   assign_const_1<bool>(global.flags,false);

   slv.e_algo_be0 = slv.e_algo_be || slv.e_algo_be_auto || slv.e_algo_be_const;
   slv.e_algo_trz0 = slv.e_algo_trz || slv.e_algo_trz_auto || slv.e_algo_trz_const;
   slv.e_algo_auto = slv.e_algo_be_auto || slv.e_algo_trz_auto;

   if (l_write_1) {
     for (int i_file=0; i_file < slv.n_outfile; i_file++) {
       if (slv.flag_out_delt_fixed[i_file]) {
         slv.out_tnext[i_file] = slv.out_tstart[i_file];
       }
     }
   }
   dcmp_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);
   ebe_form_arrays_ssw_e(ebe_lib,ebe_usr,smat,cct);

   slv.time_present_e = global.time_begin;
   global.time_end = global.time_begin + slv.ssw_period_1;

   global.time_given_e = slv.time_present_e;

   if (l_write_1) {
     slv.time_write = global.time_begin;
     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);
   }

   slv.delt_e = slv.delt0_e;

   if (!slv.flag_const_tstep_e) {
     if (cct.flag_limit_tstep_e) {
       e_assign_nextbreak_1(ebe_usr,cct,global);
       ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);
       get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
     }
   }

   copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);
   if (slv.e_algo_trz0) {
     slv.time_next_e = slv.time_present_e;
     global.time_given_e = slv.time_next_e;
     slv.trns_constants_2_e();

     find_functions_ssw_trns_e(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   }

   slv.write_iter_n1_e = 0;

// todo: get rid of goto's: visit later

   for (i0=0; i0 < slv.itmax_trns; i0++) {
     slv.iter_trns_e = i0;
     global.iter_trns_e = i0;

     write_iter_e(slv);
     iter_stepred = 0;

     jump_ssw1: ;

     slv.time_next_e  = slv.time_present_e + slv.delt_e;

     if (slv.time_next_e >= (global.time_end - slv.delt_min_e)) {
       slv.time_next_e = global.time_end;
     }
     global.time_given_e = slv.time_next_e;

     if ((slv.time_next_e + time_epsilon) >= global.time_end) {
       slv.ssw_flag_laststep = true;
     } else {
       slv.ssw_flag_laststep = false;
     }
     if (slv.e_algo_trz0) {
       slv.trns_constants_2_e();
       copy_func_to_old_e(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

       find_ssw_trz_1_e(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     }
     slv.trns_constants_2_e();

     if (cct.flag_linear_e) {
       solve_ssw_trns_linear_e(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     } else {
       solve_ssw_trns_newton_e(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
       if (!slv.flag_nr_converged) {
         if (slv.e_algo_auto) {
           if (slv.delt_e == slv.delt_min_e) {
             cout << "solve_ssw_1_e: no convergence even with" << endl;
             cout << "  the smallest time step." << endl;
             slv.write_flags_failed();
             cout << "iter_trns_e=" << slv.iter_trns_e
                  << ", time =" << slv.time_present_e << endl;
             cout << "  Halting..." << endl;
           } else {
             iter_stepred++;
             if (iter_stepred > slv.itmax_stepred) {
               cout << "solve_ssw_1_e: iter_stepred has exceeded" << endl;
               cout << "  itmax_stepred." << endl;
               cout << "  iter_stepred=" << iter_stepred << endl;
               cout << "  itmax_stepred=" << slv.itmax_stepred << endl;
               cout << "  Halting..." << endl;
               exit(1);
             }
             slv.delt_e = slv.factor_stepdec*slv.delt_e;
             slv.delt_e = max(slv.delt_e,slv.delt_min_e);

             copy_array_1<double>(smat.n_solvec_e,smat.svec_old_1_e,smat.svec_e);
             ebeu_copy_stv_1(global.I_COPY_1_TO_0,ebe_lib,ebe_usr,cct,global);

             goto jump_ssw1;
           }
         } else {
           if (slv.flag_write_solution) {
             write_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
           }
           cout << "solve_ssw_1_e: N-R iterations did not converge." << endl;
           slv.write_flags_failed();
           cout << "  iter_trns_e =" << slv.iter_trns_e << endl;
           cout << "  time =" << scientific << setprecision(6)
             << slv.time_present_e << endl;
           cout << "  Halting..." << endl; exit(1);
         }
       } else {
         if (slv.e_algo_auto) {
           if (slv.delt_e != slv.delt_max_e) {
             slv.delt_e = slv.factor_stepinc*slv.delt_e;
             slv.delt_e = min(slv.delt_e,slv.delt_max_e);
           }
         }
       }
     }
     if (!l_write_1) {
       for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
//       solve_linear_ssw in Sequel
         solve_linear_ssw_e(i_statevar,smat,cct,slv,global);
       }
     }
     slv.time_present_e = slv.time_next_e;

     if (l_write_1) {
       slv.time_write = slv.time_next_e;
       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);
       if ((slv.time_present_e + time_epsilon) >= global.time_end) {
         goto jump_ssw2;
       }
     }
     if ((slv.time_present_e + time_epsilon) >= global.time_end) {
       goto jump_ssw3;
     }
     copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_old_1_e);
     ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

     if (slv.e_algo_trz || slv.e_algo_be || slv.e_algo_trz_const || slv.e_algo_be_const) {
       slv.delt_e = slv.delt0_e;
     }
     slv.delt_e = max(slv.delt_e,slv.delt_min_e);

     if (!slv.flag_const_tstep_e) {
       if (cct.flag_limit_tstep_e) {
         get_tnext_e(ebe_lib,ebe_usr,ebe_jac,slv,cct,global);
       }
     }
     time_next_1 = slv.time_present_e + slv.delt_e;
     if (time_next_1 >= global.time_end) {
       slv.delt_e = global.time_end - slv.time_present_e + time_epsilon;
       slv.delt_e = max(slv.delt_e,slv.delt_min_e);
     }
     if (cct.flag_save_history_e) {
       save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
     }
   }
   cout << "solve_ssw_1_e: time_end not reached! Halting..." << endl;
   exit(1);

   jump_ssw3: ;

   for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
     for (int j_statevar=0; j_statevar < smat.n_statevar; j_statevar++) {
       indx_1 = smat.offs_ssw[j_statevar] + smat.ssw_indx2[i_statevar];
       if (i_statevar == j_statevar) {
         smat.ssw_mat[i_statevar][j_statevar] = 1.0 - smat.svec_ssw_1[indx_1];
       } else {
         smat.ssw_mat[i_statevar][j_statevar] = - smat.svec_ssw_1[indx_1];
       }
     }
   }
   jump_ssw2: ;

   assign_const_1<bool>(global.flags,false);

   return;
} // end of solve_ssw_1_e
// -----------------------------------------------------------------------------
void solve_ssw_1_ex(
   const bool l_write_1,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i0,iter_stepred,indx_1;
   double time_next_1;
   double time_epsilon=1.0e-13;

   assign_const_1<bool>(global.flags,false);

   slv.ex_algo_be0 = slv.ex_algo_be || slv.ex_algo_be_auto;
   slv.ex_algo_trz0 = slv.ex_algo_trz || slv.ex_algo_trz_auto;
   slv.ex_algo_auto = slv.ex_algo_be_auto || slv.ex_algo_trz_auto;

   if (l_write_1) {
     for (int i_file=0; i_file < slv.n_outfile; i_file++) {
       if (slv.flag_out_delt_fixed[i_file]) {
         slv.out_tnext[i_file] = slv.out_tstart[i_file];
       }
     }
   }
   dcmp_solvec_ex(xbe_lib,xbe_usr,ebe_lib,ebe_usr,smat,slv,cct);
   ebe_form_arrays_ssw_ex(ebe_lib,ebe_usr,smat,cct);

   slv.time_present_e = global.time_begin;
   slv.time_present_x = global.time_begin;
   global.time_end = global.time_begin + slv.ssw_period_1;

   global.time_given_e = slv.time_present_e;
   global.time_given_x = slv.time_present_x;

   if (l_write_1) {
     slv.time_write = global.time_begin;
     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);
   }
   slv.delt_e = slv.delt0_ex;
   slv.delt_x = slv.delt0_ex;

   if (!slv.flag_const_tstep_ex) {
     if (cct.flag_limit_tstep) {
       e_assign_nextbreak_1(ebe_usr,cct,global);
       x_assign_nextbreak_1(xbe_usr,cct,global);

       ebe_find_nextbreak(ebe_lib,ebe_usr,ebe_jac,cct,global);
       xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);

       get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
         slv,cct,global);
     }
   }

   copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
   ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

   if (slv.ex_algo_trz0) {
     slv.time_next_e = slv.time_present_e;
     slv.time_next_x = slv.time_present_x;
     global.time_given_e = slv.time_next_e;
     global.time_given_x = slv.time_next_x;

     slv.trns_constants_2_ex();

     find_functions_ssw_trns_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   }

   slv.write_iter_n1_e = 0;

   for (i0=0; i0 < slv.itmax_trns; i0++) {
     slv.iter_trns_e = i0;
     slv.iter_trns_x = i0;
     global.iter_trns_e = i0;
     global.iter_trns_x = i0;

     write_iter_e(slv);
     iter_stepred = 0;

     if (slv.iter_trns_x != 0) {
       if (cct.flag_modulo_x) {
         xbe_modulo(cct,xbe_usr);
       }
     }
     jump_ssw1: ;

     slv.time_next_e = slv.time_present_e + slv.delt_e;
     slv.time_next_x = slv.time_present_x + slv.delt_x;

     if (slv.time_next_e >= (global.time_end - slv.delt_min_e)) {
       slv.time_next_e = global.time_end;
       slv.time_next_x = global.time_end;
     }
     global.time_given_e = slv.time_next_e;
     global.time_given_x = slv.time_next_x;

     if ((slv.time_next_e + time_epsilon) >= global.time_end) {
       slv.ssw_flag_laststep = true;
     } else {
       slv.ssw_flag_laststep = false;
     }
     if (slv.ex_algo_trz0) {
       slv.trns_constants_2_ex();
       copy_func_to_old_ex(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,xbe_lib,xbe_usr,
         cct,global);

       find_ssw_trz_1_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     }
     slv.trns_constants_2_ex();

     if (cct.flag_linear_ex) {
       solve_ssw_trns_linear_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     } else {
       solve_ssw_trns_newton_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
       if (!slv.flag_nr_converged) {
         if (slv.ex_algo_auto) {
           if (slv.delt_e == slv.delt_min_e) {
             cout << "solve_ssw_1_ex: no convergence even with" << endl;
             cout << "  the smallest time step." << endl;
             slv.write_flags_failed();
             cout << "iter_trns_e=" << slv.iter_trns_e
                  << ", time =" << slv.time_present_e << endl;
             cout << "  Halting..." << endl;
           } else {
             iter_stepred++;
             if (iter_stepred > slv.itmax_stepred) {
               cout << "solve_ssw_1_ex: iter_stepred has exceeded" << endl;
               cout << "  itmax_stepred." << endl;
               cout << "  iter_stepred=" << iter_stepred << endl;
               cout << "  itmax_stepred=" << slv.itmax_stepred << endl;
               cout << "  Halting..." << endl;
               exit(1);
             }
             slv.delt_e = slv.factor_stepdec*slv.delt_e;
             slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
             slv.delt_x = slv.delt_e;

             copy_array_1<double>(smat.n_solvec_ex,smat.svec_old_1_ex,smat.svec_ex);
             ebeu_copy_stv_1(global.I_COPY_1_TO_0,ebe_lib,ebe_usr,cct,global);

             goto jump_ssw1;
           }
         } else {
           if (slv.flag_write_solution) {
             write_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
           }
           cout << "solve_ssw_1_ex: N-R iterations did not converge." << endl;
           slv.write_flags_failed();
           cout << "  iter_trns_e =" << slv.iter_trns_e << endl;
           cout << "  time =" << scientific << setprecision(6)
             << slv.time_present_e << endl;
           cout << "  Halting..." << endl; exit(1);
         }
       } else {
         if (slv.ex_algo_auto) {
           if (slv.delt_e != slv.delt_max_ex) {
             slv.delt_e = slv.factor_stepinc*slv.delt_e;
             slv.delt_e = min(slv.delt_e,slv.delt_max_ex);
             slv.delt_x = slv.delt_e;
           }
         }
       }
     }
     if (!l_write_1) {
       for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
//       solve_linear_ssw in Sequel
         solve_linear_ssw_ex(i_statevar,smat,cct,slv,global);
       }
     }
     slv.time_present_e = slv.time_next_e;
     slv.time_present_x = slv.time_next_x;

     if (l_write_1) {
       slv.time_write = slv.time_next_e;
       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);
       if ((slv.time_present_e + time_epsilon) >= global.time_end) {
         goto jump_ssw2;
       }
     }
     if ((slv.time_present_e + time_epsilon) >= global.time_end) {
       goto jump_ssw3;
     }
     copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_old_1_ex);
     ebeu_copy_stv_1(global.I_COPY_0_TO_1,ebe_lib,ebe_usr,cct,global);

     if (slv.ex_algo_trz || slv.ex_algo_be) {
       slv.delt_e = slv.delt0_ex;
       slv.delt_x = slv.delt0_ex;
     }
     slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
     slv.delt_x = slv.delt_e;

     if (!slv.flag_const_tstep_ex) {
       if (cct.flag_limit_tstep) {
         get_tnext_ex(ebe_lib,ebe_usr,ebe_jac,xbe_lib,xbe_usr,xbe_jac,
           slv,cct,global);
       }
     }
     time_next_1 = slv.time_present_e + slv.delt_e;
     if (time_next_1 >= global.time_end) {
       slv.delt_e = global.time_end - slv.time_present_e + time_epsilon;
       slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
       slv.delt_x = slv.delt_e;
     }
     if (cct.flag_save_history_e) {
       save_history_e(ebe_lib,ebe_usr,ebe_jac,cct,global);
     }
     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
   }
   cout << "solve_ssw_1_ex: time_end not reached! Halting..." << endl;
   exit(1);

   jump_ssw3: ;

   for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
     for (int j_statevar=0; j_statevar < smat.n_statevar; j_statevar++) {
       indx_1 = smat.offs_ssw[j_statevar] + smat.ssw_indx2[i_statevar];
       if (i_statevar == j_statevar) {
         smat.ssw_mat[i_statevar][j_statevar] = 1.0 - smat.svec_ssw_1[indx_1];
       } else {
         smat.ssw_mat[i_statevar][j_statevar] = - smat.svec_ssw_1[indx_1];
       }
     }
   }
   jump_ssw2: ;

   assign_const_1<bool>(global.flags,false);

   return;
} // end of solve_ssw_1_ex
// -----------------------------------------------------------------------------
void solve_ssw_1_x(
   const bool l_write_1,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i0,iter_stepred,indx_1;
   double time_next_1;
   double time_epsilon=1.0e-13;

   cout << "solve_ssw_1_x starts..." << endl;
   assign_const_1<bool>(global.flags,false);

   slv.x_algo_be0 = slv.x_algo_be || slv.x_algo_be_auto;
   slv.x_algo_trz0 = slv.x_algo_trz || slv.x_algo_trz_auto;
   slv.x_algo_auto = slv.x_algo_be_auto || slv.x_algo_trz_auto;

   if (l_write_1) {
     for (int i_file=0; i_file < slv.n_outfile; i_file++) {
       if (slv.flag_out_delt_fixed[i_file]) {
         slv.out_tnext[i_file] = slv.out_tstart[i_file];
       }
     }
   }
   dcmp_solvec_x(xbe_lib,xbe_usr,smat,cct);

   slv.time_present_x = global.time_begin;
   global.time_end = global.time_begin + slv.ssw_period_1;

   global.time_given_x = slv.time_present_x;

   if (l_write_1) {
     slv.time_write = global.time_begin;
     write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       cct,slv,cct_file,global);
   }
   slv.delt_x = slv.delt0_x;

   if (!slv.flag_const_tstep_x) {
     if (cct.flag_limit_tstep) {
       x_assign_nextbreak_1(xbe_usr,cct,global);
       xbe_find_nextbreak(xbe_lib,xbe_usr,xbe_jac,cct,global);
       get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
     }
   }

   copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);

   if (slv.x_algo_trz0) {
     slv.time_next_x = slv.time_present_x;
     global.time_given_x = slv.time_next_x;

     slv.trns_constants_2_x();

     find_functions_ssw_trns_x(xbe_lib,xbe_usr,xbe_jac,
       smat,cct,slv,cct_file,global);
   }

   slv.write_iter_n1_x = 0;

   for (i0=0; i0 < slv.itmax_trns; i0++) {
     slv.iter_trns_x = i0;
     global.iter_trns_x = i0;

     write_iter_x(slv);
     iter_stepred = 0;

     if (slv.iter_trns_x != 0) {
       if (cct.flag_modulo_x) {
         xbe_modulo(cct,xbe_usr);
       }
     }
     jump_ssw1: ;

     slv.time_next_x = slv.time_present_x + slv.delt_x;

     if (slv.time_next_x >= (global.time_end - slv.delt_min_x)) {
       slv.time_next_x = global.time_end;
     }
     global.time_given_x = slv.time_next_x;

     if ((slv.time_next_x + time_epsilon) >= global.time_end) {
       slv.ssw_flag_laststep = true;
     } else {
       slv.ssw_flag_laststep = false;
     }
     if (slv.x_algo_trz0) {
       slv.trns_constants_2_x();
       copy_func_to_old_x(global.I_COPY_0_TO_1,xbe_lib,xbe_usr,cct,global);

       find_ssw_trz_1_x(xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,cct_file,global);
     }
     slv.trns_constants_2_x();

     if (cct.flag_linear_x) {
       solve_ssw_trns_linear_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     } else {
       solve_ssw_trns_newton_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
       if (!slv.flag_nr_converged) {
         if (slv.x_algo_auto) {
           if (slv.delt_x == slv.delt_min_x) {
             cout << "solve_ssw_1_x: no convergence even with" << endl;
             cout << "  the smallest time step." << endl;
             slv.write_flags_failed();
             cout << "iter_trns_x=" << slv.iter_trns_x
                  << ", time =" << slv.time_present_x << endl;
             cout << "  Halting..." << endl;
           } else {
             iter_stepred++;
             if (iter_stepred > slv.itmax_stepred) {
               cout << "solve_ssw_1_x: iter_stepred has exceeded" << endl;
               cout << "  itmax_stepred." << endl;
               cout << "  iter_stepred=" << iter_stepred << endl;
               cout << "  itmax_stepred=" << slv.itmax_stepred << endl;
               cout << "  Halting..." << endl;
               exit(1);
             }
             slv.delt_x = slv.factor_stepdec*slv.delt_x;
             slv.delt_x = max(slv.delt_x,slv.delt_min_x);

             copy_array_1<double>(smat.n_solvec_x,smat.svec_old_1_x,smat.svec_x);
             goto jump_ssw1;
           }
         } else {
           if (slv.flag_write_solution) {
             write_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
           }
           cout << "solve_ssw_1_x: N-R iterations did not converge." << endl;
           slv.write_flags_failed();
           cout << "  iter_trns_x =" << slv.iter_trns_x << endl;
           cout << "  time =" << scientific << setprecision(6)
             << slv.time_present_x << endl;
           cout << "  Halting..." << endl; exit(1);
         }
       } else {
         if (slv.x_algo_auto) {
           if (slv.delt_x != slv.delt_max_x) {
             slv.delt_x = slv.factor_stepinc*slv.delt_x;
             slv.delt_x = min(slv.delt_x,slv.delt_max_x);
           }
         }
       }
     }
     if (!l_write_1) {
       for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
//       solve_linear_ssw in Sequel
         solve_linear_ssw_x(i_statevar,smat,cct,slv,global);
       }
     }
     slv.time_present_x = slv.time_next_x;

     if (l_write_1) {
       slv.time_write = slv.time_next_x;
       write_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         cct,slv,cct_file,global);
       if ((slv.time_present_x + time_epsilon) >= global.time_end) {
         goto jump_ssw2;
       }
     }
     if ((slv.time_present_x + time_epsilon) >= global.time_end) {
       goto jump_ssw3;
     }
     copy_array_1<double>(smat.n_solvec_x,smat.svec_x,smat.svec_old_1_x);

     if (slv.x_algo_trz || slv.x_algo_be) {
       slv.delt_x = slv.delt0_x;
     }
     slv.delt_x = max(slv.delt_x,slv.delt_min_x);

     if (!slv.flag_const_tstep_x) {
       if (cct.flag_limit_tstep_x) {
         get_tnext_x(xbe_lib,xbe_usr,xbe_jac,slv,cct,global);
       }
     }
     time_next_1 = slv.time_present_x + slv.delt_x;
     if (time_next_1 >= global.time_end) {
       slv.delt_x = global.time_end - slv.time_present_x + time_epsilon;
       slv.delt_x = max(slv.delt_x,slv.delt_min_x);
     }
     if (cct.flag_save_history_x) {
       save_history_x(xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
   }
   cout << "solve_ssw_1_x: time_end not reached! Halting..." << endl;
   exit(1);

   jump_ssw3: ;

   for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
     for (int j_statevar=0; j_statevar < smat.n_statevar; j_statevar++) {
       indx_1 = smat.offs_ssw[j_statevar] + smat.ssw_indx2[i_statevar];
       if (i_statevar == j_statevar) {
         smat.ssw_mat[i_statevar][j_statevar] = 1.0 - smat.svec_ssw_1[indx_1];
       } else {
         smat.ssw_mat[i_statevar][j_statevar] = - smat.svec_ssw_1[indx_1];
       }
     }
   }
   jump_ssw2: ;

   assign_const_1<bool>(global.flags,false);

   return;
} // end of solve_ssw_1_x
// -----------------------------------------------------------------------------
void solve_linear_ssw_e(
   const int i_statevar,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i1;

   assign_all_double_1(smat.rhs_m_ssw,smat.m_ssw.n_row,0.0);

// Sequel: add_ssw_terms(i_statevar,slv,mat);
   add_ssw_terms_e(i_statevar,smat,cct,slv,global);

   solve_jac_3_ssw_e(smat,slv,global);

   copy_array_1<double>(smat.m_ssw.n_row,smat.svec_orig_e,smat.delsvec_e);

   i1 = smat.offs_ssw[i_statevar];
   copy_array_1<double>(smat.m_ssw.n_row,smat.delsvec_e,&(smat.svec_ssw_1[i1]));

   return;
} // end of solve_linear_ssw_e
// -----------------------------------------------------------------------------
void solve_linear_ssw_ex(
   const int i_statevar,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i1;

   assign_all_double_1(smat.rhs_m_ssw,smat.m_ssw.n_row,0.0);

// Sequel: add_ssw_terms(i_statevar,slv,mat);
   add_ssw_terms_ex(i_statevar,smat,cct,slv,global);

   solve_jac_3_ssw_ex(smat,slv,global);

   copy_array_1<double>(smat.m_ssw.n_row,smat.svec_orig_ex,smat.delsvec_ex);

   i1 = smat.offs_ssw[i_statevar];
   copy_array_1<double>(smat.m_ssw.n_row,smat.delsvec_ex,&(smat.svec_ssw_1[i1]));

   return;
} // end of solve_linear_ssw_ex
// -----------------------------------------------------------------------------
void solve_linear_ssw_x(
   const int i_statevar,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i1;

   assign_all_double_1(smat.rhs_m_ssw,smat.m_ssw.n_row,0.0);

// Sequel: add_ssw_terms(i_statevar,slv,mat);
   add_ssw_terms_x(i_statevar,smat,cct,slv,global);

   solve_jac_3_ssw_x(smat,slv,global);

   copy_array_1<double>(smat.m_ssw.n_row,smat.svec_orig_x,smat.delsvec_x);

   i1 = smat.offs_ssw[i_statevar];
   copy_array_1<double>(smat.m_ssw.n_row,smat.delsvec_x,&(smat.svec_ssw_1[i1]));

   return;
} // end of solve_linear_ssw_x
// -----------------------------------------------------------------------------
void add_ssw_terms_e(
   const int i_statevar,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   bool flag_1;
   int r0,i_eberhs;
   int i_sysvar,i_sysvar_1,j_statevar;
   double x_0,ssw_trz_2;

   r0 = smat.m_kcl.n_row;

   for (int i_sysrhs=r0; i_sysrhs < smat.m_ssw.n_row; i_sysrhs++) {
     i_eberhs = i_sysrhs - r0;
     flag_1 = smat.ebe_rhs_ddt_flag[i_eberhs] || smat.flag_ebe_stv[i_eberhs];
     if (flag_1) {
       i_sysvar = smat.ebe_rhs_ddt_varnumber[i_eberhs];
       i_sysvar_1 = i_sysvar + smat.offs_ssw[i_statevar];
       x_0 = smat.svec_ssw_1[i_sysvar_1];

       if (slv.e_algo_be0) {
         smat.rhs_m_ssw[i_sysrhs] = -slv.beuler_1_e*x_0;
       } else if (slv.e_algo_trz0) {
         j_statevar = smat.ssw_indx3[i_sysvar];
         ssw_trz_2 = smat.ssw_trz_1[j_statevar][i_statevar];
         smat.rhs_m_ssw[i_sysrhs] = -slv.trz_1_e*x_0 - ssw_trz_2;
       } else {
         cout << "add_ssw_terms_e: check solution method. Halting..." << endl;
         exit(1);
       }
     }
   }
   return;
} // end of add_ssw_terms_e
// -----------------------------------------------------------------------------
void add_ssw_terms_ex(
   const int i_statevar,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   bool flag_1;
   int r_begin_e,r_end_e;
   int r_begin_x,r_end_x;
   int i_eberhs,i_xberhs;
   int i_sysvar,i_sysvar_1,j_statevar;
   double x_0,ssw_trz_2;

   r_begin_e = smat.m_kcl.n_row;
   r_end_e = r_begin_e + smat.m_e.n_row;
   r_begin_x = r_end_e;
   r_end_x = r_begin_x + smat.m_x.n_row;

   for (int i_sysrhs=r_begin_e; i_sysrhs < r_end_e; i_sysrhs++) {
     i_eberhs = i_sysrhs - r_begin_e;
     flag_1 = smat.ebe_rhs_ddt_flag[i_eberhs] || smat.flag_ebe_stv[i_eberhs];
     if (flag_1) {
       i_sysvar = smat.ebe_rhs_ddt_varnumber[i_eberhs];
       i_sysvar_1 = i_sysvar + smat.offs_ssw[i_statevar];
       x_0 = smat.svec_ssw_1[i_sysvar_1];

       if (slv.ex_algo_be0) {
         smat.rhs_m_ssw[i_sysrhs] = -slv.beuler_1_e*x_0;
       } else if (slv.ex_algo_trz0) {
         j_statevar = smat.ssw_indx3[i_sysvar];
         ssw_trz_2 = smat.ssw_trz_1[j_statevar][i_statevar];
         smat.rhs_m_ssw[i_sysrhs] = -slv.trz_1_e*x_0 - ssw_trz_2;
       } else {
         cout << "add_ssw_terms_ex: check solution method. Halting..." << endl;
         exit(1);
       }
     }
   }
   for (int i_sysrhs=r_begin_x; i_sysrhs < r_end_x; i_sysrhs++) {
     i_xberhs = i_sysrhs - r_begin_x;
     if (smat.xbe_rhs_ddt_flag[i_xberhs]) {
       i_sysvar = smat.xbe_rhs_ddt_varnumber[i_xberhs];
       i_sysvar_1 = i_sysvar + smat.offs_ssw[i_statevar];
       x_0 = smat.svec_ssw_1[i_sysvar_1];

       if (slv.ex_algo_be0) {
         smat.rhs_m_ssw[i_sysrhs] = -slv.beuler_1_x*x_0;
       } else if (slv.ex_algo_trz0) {
         j_statevar = smat.ssw_indx3[i_sysvar];
         ssw_trz_2 = smat.ssw_trz_1[j_statevar][i_statevar];
         smat.rhs_m_ssw[i_sysrhs] = -slv.trz_1_x*x_0 - ssw_trz_2;
       } else {
         cout << "add_ssw_terms_ex: check solution method. Halting..." << endl;
         exit(1);
       }
     }
   }
   return;
} // end of add_ssw_terms_ex
// -----------------------------------------------------------------------------
void add_ssw_terms_x(
   const int i_statevar,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   bool flag_1;
   int r_begin_x,r_end_x;
   int i_xberhs;
   int i_sysvar,i_sysvar_1,j_statevar;
   double x_0,ssw_trz_2;

   r_begin_x = 0;
   r_end_x = r_begin_x + smat.m_x.n_row;

   for (int i_sysrhs=r_begin_x; i_sysrhs < r_end_x; i_sysrhs++) {
     i_xberhs = i_sysrhs;
     if (smat.xbe_rhs_ddt_flag[i_xberhs]) {
       i_sysvar = smat.xbe_rhs_ddt_varnumber[i_xberhs];
       i_sysvar_1 = i_sysvar + smat.offs_ssw[i_statevar];
       x_0 = smat.svec_ssw_1[i_sysvar_1];

       if (slv.x_algo_be0) {
         smat.rhs_m_ssw[i_sysrhs] = -slv.beuler_1_x*x_0;
       } else if (slv.x_algo_trz0) {
         j_statevar = smat.ssw_indx3[i_sysvar];
         ssw_trz_2 = smat.ssw_trz_1[j_statevar][i_statevar];
         smat.rhs_m_ssw[i_sysrhs] = -slv.trz_1_x*x_0 - ssw_trz_2;
       } else {
         cout << "add_ssw_terms_x: check solution method. Halting..." << endl;
         exit(1);
       }
     }
   }
   return;
} // end of add_ssw_terms_x
// -----------------------------------------------------------------------------
void find_functions_ssw_trns_e(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

// find_functions_ssw_trns in Sequel
// todo remove unused arguments in the call

   int i_ebeu,i_ebel;

   ebe_form_arrays_ssw_e(ebe_lib,ebe_usr,smat,cct);

   global.flags[global.i_trns    ] = true;
   global.flags[global.i_ssw     ] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = false;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
   }

   global.flags[global.i_trns    ] = false;
   global.flags[global.i_ssw     ] = false;
   global.flags[global.i_function] = false;

   return;
} // end of find_functions_ssw_trns_e
// -----------------------------------------------------------------------------
void find_functions_ssw_trns_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_ebeu,i_ebel;
   int i_xbeu,i_xbel;

   ebe_form_arrays_ssw_ex(ebe_lib,ebe_usr,smat,cct);

   global.flags[global.i_trns    ] = true;
   global.flags[global.i_ssw     ] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = false;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
   }

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);
   }

   global.flags[global.i_trns    ] = false;
   global.flags[global.i_ssw     ] = false;
   global.flags[global.i_function] = false;

   return;
} // end of find_functions_ssw_trns_ex
// -----------------------------------------------------------------------------
void find_functions_ssw_trns_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_trns    ] = true;
   global.flags[global.i_ssw     ] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = false;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);
   }

   global.flags[global.i_trns    ] = false;
   global.flags[global.i_ssw     ] = false;
   global.flags[global.i_function] = false;

   return;
} // end of find_functions_ssw_trns_x
// -----------------------------------------------------------------------------
void find_ssw_trz_1_e(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

// find_ssw_trz_1 in Sequel
// todo remove unused arguments in the call

   int i_ebeu,i_ebel;

   for (int i=0; i < smat.n_statevar; i++) {
     for (int j=0; j < smat.n_statevar; j++) {
       smat.ssw_trz_1[i][j] = 0.0;
     }
   }

   global.flags[global.i_trns    ] = true;
   global.flags[global.i_ssw     ] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);

     mat_ssw_3_e(i_ebeu,i_ebel,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   }
   global.flags[global.i_trns    ] = false;
   global.flags[global.i_ssw     ] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of find_ssw_trz_1_e
// -----------------------------------------------------------------------------
void find_ssw_trz_1_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

// find_ssw_trz_1 in Sequel
// todo remove unused arguments in the call

   int i_ebeu,i_ebel;
   int i_xbeu,i_xbel;

   for (int i=0; i < smat.n_statevar; i++) {
     for (int j=0; j < smat.n_statevar; j++) {
       smat.ssw_trz_1[i][j] = 0.0;
     }
   }

   global.flags[global.i_trns    ] = true;
   global.flags[global.i_ssw     ] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);

     mat_ssw_3_ex_e(i_ebeu,i_ebel,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   }

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);

       mat_ssw_3_ex_x(i_xbeu,i_xbel,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,cct_file,global);
     }
   }
   global.flags[global.i_trns    ] = false;
   global.flags[global.i_ssw     ] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of find_ssw_trz_1_ex
// -----------------------------------------------------------------------------
void find_ssw_trz_1_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_xbeu,i_xbel;

   for (int i=0; i < smat.n_statevar; i++) {
     for (int j=0; j < smat.n_statevar; j++) {
       smat.ssw_trz_1[i][j] = 0.0;
     }
   }

   global.flags[global.i_trns    ] = true;
   global.flags[global.i_ssw     ] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);

       mat_ssw_3_ex_x(i_xbeu,i_xbel,xbe_lib,xbe_usr,xbe_jac,
         smat,cct,slv,cct_file,global);
     }
   }
   global.flags[global.i_trns    ] = false;
   global.flags[global.i_ssw     ] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of find_ssw_trz_1_x
// -----------------------------------------------------------------------------
void mat_ssw_3_e(
   const int i_ebeu,
   const int i_ebel,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

// ebce_mat_3_ssw in Sequel
// todo remove unused arguments in the call

   int i_f,i_stv_l,i_stv_u,k;
   int n_f1,n_nd1,n_fvar1;
   int var_flag,var_number;
   int var_number_1;
   int var_number_l,var_number_u;
   int i_sysvar,i_statevar;
   bool l_ddt_stv,flag_ddt;
   int col_given,offs_ssw_1;
   double val;

   n_f1 = ebe_lib[i_ebel].n_f;
   n_nd1 = ebe_lib[i_ebel].n_nd;

   for (i_f=0; i_f < n_f1; i_f++) {
     if (i_f < n_nd1) {
//     KCL equations:
       flag_ddt = ebe_lib[i_ebel].f_ddt[i_f];

       if (flag_ddt) {
         i_stv_l = ebe_lib[i_ebel].f_ddt_stv_index[i_f];
         i_stv_u = ebe_usr[i_ebeu].stv[i_stv_l];

         i_sysvar = smat.offs_estv + i_stv_u;
         i_statevar = smat.ssw_indx3[i_sysvar];

         n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
         for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
           l_ddt_stv = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];

           if (!l_ddt_stv) {
             k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
             if (k != -1) {
               var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
               var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

               if (var_flag == global.I_NV) {
                 val = ebe_jac[i_ebel].dfdv[i_f][var_number];
               } else if (var_flag == global.I_EAUX) {
                 val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
               } else if (var_flag == global.I_XVR) {
                 val = ebe_jac[i_ebel].dfdxvr[i_f][var_number];
               } else {
                 cout << "mat_ssw_3_e: incorrect value of var_flag in f." << endl;
                 cout << "  i_f = " << i_f << ", i_favr = " << i_fvar << endl;
                 cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
                 exit(1);
               }
               var_number_1 = ebe_usr[i_ebeu].fvar[i_f][i_fvar];

               if (var_flag == global.I_NV) {
                 if (var_number_1 < cct.ref_nd) {
                   col_given = smat.offs[var_flag] + var_number_1;
                 } else {
                   col_given = smat.offs[var_flag] + var_number_1 - 1;
                 }
               } else {
                 col_given = smat.offs[var_flag] + var_number_1;
               }
               for (int j_statevar=0; j_statevar < smat.n_statevar; j_statevar++) {
                 offs_ssw_1 = smat.offs_ssw[j_statevar];

                 smat.ssw_trz_1[i_statevar][j_statevar] +=
                 (-val)*smat.svec_ssw_1[offs_ssw_1+col_given];
               }
             }
           }
         }

         var_number_1 = cct.ebeu_nd_start[i_ebeu] + i_f;
         col_given = smat.offs[global.I_NDCUR] + var_number_1;

         for (int j_statevar=0; j_statevar < smat.n_statevar; j_statevar++) {
           offs_ssw_1 = smat.offs_ssw[j_statevar];

           smat.ssw_trz_1[i_statevar][j_statevar] +=
           smat.svec_ssw_1[offs_ssw_1+col_given];
         }
       }
     } else {
//     non-KCL equation
       if (ebe_lib[i_ebel].f_ddt[i_f]) {

         var_number_l = ebe_lib[i_ebel].f_ddt_var_index[i_f];
         var_flag = ebe_lib[i_ebel].f_ddt_var_flag[i_f];

         if (var_flag == global.I_EAUX) {
           var_number_u = ebe_usr[i_ebeu].aux[var_number_l];
           i_sysvar = smat.offs_eaux + var_number_u;
         } else {
           cout << "mat_ssw_3_e: incorrect value of var_flag in f." << endl;
           cout << "  i_f = " << i_f << endl;
           cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
           exit(1);
         }
         i_statevar = smat.ssw_indx3[i_sysvar];

         n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
         for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
           k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
           if (k != -1) {
             var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
             var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

             if (var_flag == global.I_NV) {
               val = ebe_jac[i_ebel].dfdv[i_f][var_number];
             } else if (var_flag == global.I_EAUX) {
               val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
             } else {
               cout << "mat_ssw_3_e: incorrect value of var_flag in f." << endl;
               cout << "  i_f = " << i_f << ", i_favr = " << i_fvar << endl;
               cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
               exit(1);
             }
             var_number_1 = ebe_usr[i_ebeu].fvar[i_f][i_fvar];
             if (!ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar]) {
               col_given = smat.offs[var_flag] + var_number_1;
               if (var_flag == global.I_NV) {
                 if (var_number_1 < cct.ref_nd) {
                   col_given = smat.offs[var_flag] + var_number_1;
                 } else {
                   col_given = smat.offs[var_flag] + var_number_1 - 1;
                 }
               } else {
                 col_given = smat.offs[var_flag] + var_number_1;
               }
               for (int j_statevar=0; j_statevar < smat.n_statevar; j_statevar++) {
                 offs_ssw_1 = smat.offs_ssw[j_statevar];

                 smat.ssw_trz_1[i_statevar][j_statevar] +=
                 val*smat.svec_ssw_1[offs_ssw_1+col_given];
               }
             }
           }
         }
       }
     }
   }
   return;
} // end of mat_ssw_3_e
// -----------------------------------------------------------------------------
void mat_ssw_3_ex_e(
   const int i_ebeu,
   const int i_ebel,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_f,i_stv_l,i_stv_u,k;
   int n_f1,n_nd1,n_fvar1;
   int var_flag,var_number;
   int var_number_1;
   int var_number_l,var_number_u;
   int i_sysvar,i_statevar;
   bool l_ddt_stv,flag_ddt;
   int col_given,offs_ssw_1;
   double val;

   n_f1 = ebe_lib[i_ebel].n_f;
   n_nd1 = ebe_lib[i_ebel].n_nd;

   for (i_f=0; i_f < n_f1; i_f++) {
     if (i_f < n_nd1) {
//     KCL equations:
       flag_ddt = ebe_lib[i_ebel].f_ddt[i_f];

       if (flag_ddt) {
         i_stv_l = ebe_lib[i_ebel].f_ddt_stv_index[i_f];
         i_stv_u = ebe_usr[i_ebeu].stv[i_stv_l];

         i_sysvar = smat.offs_estv + i_stv_u;
         i_statevar = smat.ssw_indx3[i_sysvar];

         n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
         for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
           l_ddt_stv = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];

           if (!l_ddt_stv) {
             k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
             if (k != -1) {
               var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
               var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

               if (var_flag == global.I_NV) {
                 val = ebe_jac[i_ebel].dfdv[i_f][var_number];
               } else if (var_flag == global.I_EAUX) {
                 val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
               } else if (var_flag == global.I_XVR) {
                 val = ebe_jac[i_ebel].dfdxvr[i_f][var_number];
               } else {
                 cout << "mat_ssw_3_ex_e: incorrect value of var_flag in f." << endl;
                 cout << "  i_f = " << i_f << ", i_favr = " << i_fvar << endl;
                 cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
                 exit(1);
               }
               var_number_1 = ebe_usr[i_ebeu].fvar[i_f][i_fvar];

               if (var_flag == global.I_NV) {
                 if (var_number_1 < cct.ref_nd) {
                   col_given = smat.offs[var_flag] + var_number_1;
                 } else {
                   col_given = smat.offs[var_flag] + var_number_1 - 1;
                 }
               } else {
                 col_given = smat.offs[var_flag] + var_number_1;
               }
               for (int j_statevar=0; j_statevar < smat.n_statevar; j_statevar++) {
                 offs_ssw_1 = smat.offs_ssw[j_statevar];

                 smat.ssw_trz_1[i_statevar][j_statevar] +=
                 (-val)*smat.svec_ssw_1[offs_ssw_1+col_given];
               }
             }
           }
         }
         var_number_1 = cct.ebeu_nd_start[i_ebeu] + i_f;
         col_given = smat.offs[global.I_NDCUR] + var_number_1;

         for (int j_statevar=0; j_statevar < smat.n_statevar; j_statevar++) {
           offs_ssw_1 = smat.offs_ssw[j_statevar];

           smat.ssw_trz_1[i_statevar][j_statevar] +=
           smat.svec_ssw_1[offs_ssw_1+col_given];
         }
       }
     } else {
//     non-KCL equation
       if (ebe_lib[i_ebel].f_ddt[i_f]) {
         var_number_l = ebe_lib[i_ebel].f_ddt_var_index[i_f];
         var_flag = ebe_lib[i_ebel].f_ddt_var_flag[i_f];

         if (var_flag == global.I_EAUX) {
           var_number_u = ebe_usr[i_ebeu].aux[var_number_l];
           i_sysvar = smat.offs_eaux + var_number_u;
         } else {
           cout << "mat_ssw_3_ex_e: incorrect value of var_flag in f." << endl;
           cout << "  i_f = " << i_f << endl;
           cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
           exit(1);
         }
         i_statevar = smat.ssw_indx3[i_sysvar];

         n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
         for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
           k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
           if (k != -1) {
             var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
             var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

             if (var_flag == global.I_NV) {
               val = ebe_jac[i_ebel].dfdv[i_f][var_number];
             } else if (var_flag == global.I_EAUX) {
               val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
             } else if (var_flag == global.I_XVR) {
               val = ebe_jac[i_ebel].dfdxvr[i_f][var_number];
             } else {
               cout << "mat_ssw_3_ex_e: incorrect value of var_flag in f." << endl;
               cout << "  i_f = " << i_f << ", i_favr = " << i_fvar << endl;
               cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
               exit(1);
             }
             var_number_1 = ebe_usr[i_ebeu].fvar[i_f][i_fvar];
             if (!ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar]) {
               col_given = smat.offs[var_flag] + var_number_1;
               if (var_flag == global.I_NV) {
                 if (var_number_1 < cct.ref_nd) {
                   col_given = smat.offs[var_flag] + var_number_1;
                 } else {
                   col_given = smat.offs[var_flag] + var_number_1 - 1;
                 }
               } else {
                 col_given = smat.offs[var_flag] + var_number_1;
               }
               for (int j_statevar=0; j_statevar < smat.n_statevar; j_statevar++) {
                 offs_ssw_1 = smat.offs_ssw[j_statevar];

                 smat.ssw_trz_1[i_statevar][j_statevar] +=
                 val*smat.svec_ssw_1[offs_ssw_1+col_given];
               }
             }
           }
         }
       }
     }
   }
   return;
} // end of mat_ssw_3_ex_e
// -----------------------------------------------------------------------------
void mat_ssw_3_ex_x(
   const int i_xbeu,
   const int i_xbel,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_g,n_g1,n_gvar1,col_given;
   int var_flag_f,var_number_f;
   int var_flag_g,var_number_g;
   int i_sysvar,i_statevar;
   double val;
   int offs_ssw_1;

   n_g1 = xbe_lib[i_xbel].n_g;

   for (i_g=0; i_g < n_g1; i_g++) {
     var_flag_f   = xbe_lib[i_xbel].ddt_varflag  [i_g];
     var_number_f = xbe_lib[i_xbel].ddt_varnumber[i_g];
     if (var_flag_f == global.I_XVR) {
       i_sysvar = smat.offs_xvr + xbe_usr[i_xbeu].vr[var_number_f];
     } else if (var_flag_f == global.I_XAUX) {
       i_sysvar = smat.offs_xaux + xbe_usr[i_xbeu].aux[var_number_f];
     } else {
       cout << "mat_ssw_3_ex_x: var_flag_f = " << var_flag_f
         << " does not make sense. Halting..." << endl;
       exit(1);
     }
     i_statevar = smat.ssw_indx3[i_sysvar];

     n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
     for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
       var_flag_g   = xbe_lib[i_xbel].gvar_flag [i_g][i_gvar];
       var_number_g = xbe_lib[i_xbel].gvar_index[i_g][i_gvar];

       if (var_flag_g == global.I_XVR) {
         val = xbe_jac[i_xbel].dgdvr[i_g][var_number_g];
       } else if (var_flag_g == global.I_XAUX) {
         val = xbe_jac[i_xbel].dgdaux[i_g][var_number_g];
       } else {
         cout << "mat_ssw_3_ex_x: incorrect value of var_flag." << endl;
         cout << "  i_g = " << i_g << ", i_gavr = " << i_gvar << endl;
         cout << "  xbe_lib is " << xbe_lib[i_xbel].name << ". Halting.." << endl;
         exit(1);
       }
       col_given = smat.offs[var_flag_g] + var_number_g;
       for (int j_statevar=0; j_statevar < smat.n_statevar; j_statevar++) {
         offs_ssw_1 = smat.offs_ssw[j_statevar];
         smat.ssw_trz_1[i_statevar][j_statevar] +=
           val*smat.svec_ssw_1[offs_ssw_1+col_given];
       }
     }
   }
   return;
} // end of mat_ssw_3_ex_x
// -----------------------------------------------------------------------------
void mat_ssw_trns_3_e(
   const int i_ebeu,
   const int i_ebel,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_f,i_g,i_stv_l,i_stv_u,k,k0,k1,kg;
   int n_f1,n_nd1,n_fvar1,n_gvar1;
   double f_1,x_0,x_1;
   double val;
   bool flag_ddt,l_ddt_stv;
   int i_sysvar,i_sysvar_ndcur,pntr_nd;
   int var_flag,var_number;
   int var_flag_stv,var_number_stv;
   int r0,row0,row1;

   n_f1 = ebe_lib[i_ebel].n_f;
   n_nd1 = ebe_lib[i_ebel].n_nd;

   k0 = smat.m_kcl.n_nz;
   r0 = smat.m_kcl.n_row;

   for (i_f=0; i_f < n_f1; i_f++) {
     if (i_f < n_nd1) {
//     KCL equation:

       pntr_nd = cct.ebeu_nd_start[i_ebeu] + i_f;
       i_sysvar_ndcur = smat.offs_ndcur + pntr_nd;

       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       smat.rhs_m_ssw[row0+r0] = smat.svec_e[i_sysvar_ndcur] - ebe_usr[i_ebeu].f[i_f];

       flag_ddt = ebe_lib[i_ebel].f_ddt[i_f];

       if (flag_ddt) {
         i_stv_l = ebe_lib[i_ebel].f_ddt_stv_index[i_f];
         i_stv_u = ebe_usr[i_ebeu].stv[i_stv_l];
         i_sysvar = smat.offs_estv + i_stv_u;

         x_0 = smat.svec_e[i_sysvar];
         x_1 = smat.svec_old_1_e[i_sysvar];

         if (slv.e_algo_be0) {
           smat.rhs_m_ssw[row0+r0] += (-slv.beuler_1_e*(x_0-x_1));
         } else if (slv.e_algo_trz0) {
           f_1 = smat.svec_old_1_e[i_sysvar_ndcur] - ebe_usr[i_ebeu].f_old_1[i_f];
           smat.rhs_m_ssw[row0+r0] += (-(slv.trz_1_e*(x_0-x_1) - f_1));
         }
       }
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         l_ddt_stv = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];

         if (!l_ddt_stv) {
           k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
           if (k != -1) {
             var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
             var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

             if (var_flag == global.I_NV) {
               val = ebe_jac[i_ebel].dfdv[i_f][var_number];
             } else if (var_flag == global.I_EAUX) {
               val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
             } else if (var_flag == global.I_XVR) {
               val = ebe_jac[i_ebel].dfdxvr[i_f][var_number];
             } else {
               cout << "mat_ssw_trns_3_e: incorrect value of var_flag in f." << endl;
               cout << "  i_f = " << i_f << ", i_favr = " << i_fvar << endl;
               cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
               exit(1);
             }
             smat.m_ssw.val[k+k0] = - val;
           }
         } else {
           k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];

           if (slv.e_algo_be0) {
             smat.m_ssw.val[k+k0] = - slv.beuler_1_e;
           } else if (slv.e_algo_trz0) {
             smat.m_ssw.val[k+k0] = - slv.trz_1_e;
           }
           x_0 = smat.svec_e[i_sysvar];
           i_g = ebe_lib[i_ebel].f_ddt_stv_eqn[i_f];

           row1 = smat.ebe_g_to_row[i_ebeu][i_g];
           smat.rhs_m_ssw[row1+r0] = ebe_usr[i_ebeu].g[i_g] - x_0;

           k1 = smat.map_stv_to_ebe[i_ebeu][i_f][i_fvar];
           smat.m_ssw.val[k1+k0] = -1.0;

           n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];
           for (int i_gvar=1; i_gvar < n_gvar1; i_gvar++) {
             kg = smat.map_gvar_to_ebe[i_ebeu][i_g][i_gvar];
             if (kg != -1) {
               var_flag_stv = ebe_lib[i_ebel].gvar_flag[i_g][i_gvar];
               var_number_stv = ebe_lib[i_ebel].gvar_index[i_g][i_gvar];

               if (var_flag_stv == global.I_NV) {
                 val = ebe_jac[i_ebel].dgdv[i_g][var_number_stv];
               } else if (var_flag_stv == global.I_EAUX) {
                 val = ebe_jac[i_ebel].dgdaux[i_g][var_number_stv];
               }
               smat.m_ssw.val[kg+k0] = val;
             }
           }
         }
       }
     } else {
//     non-KCL equation
       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       smat.rhs_m_ssw[row0+r0] = ebe_usr[i_ebeu].f[i_f];

       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
         if (k != -1) {
           var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
           var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

           if (var_flag == global.I_NV) {
             val = ebe_jac[i_ebel].dfdv[i_f][var_number];
           } else if (var_flag == global.I_EAUX) {
             val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
           } else if (var_flag == global.I_XVR) {
             val = ebe_jac[i_ebel].dfdxvr[i_f][var_number];
           } else {
             cout << "mat_ssw_trns_3_e: incorrect value of var_flag in f." << endl;
             cout << "  i_f = " << i_f << ", i_favr = " << i_fvar << endl;
             cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
             exit(1);
           }
           smat.m_ssw.val[k+k0] = val;
         }
       }
     }
   }
   return;
} // end of mat_ssw_trns_3_e
// -----------------------------------------------------------------------------
void mat_ssw_trns_3_ex_e(
   const int i_ebeu,
   const int i_ebel,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   Global &global) {

   int i_f,i_g,i_stv_l,i_stv_u,k,k0,k1,kg;
   int n_f1,n_nd1,n_fvar1,n_gvar1;
   double f_1,x_0,x_1;
   double val;
   bool flag_ddt,l_ddt_stv;
   int i_sysvar,i_sysvar_ndcur,pntr_nd;
   int var_flag,var_number;
   int var_flag_stv,var_number_stv;
   int r0,row0,row1;

   n_f1 = ebe_lib[i_ebel].n_f;
   n_nd1 = ebe_lib[i_ebel].n_nd;

   k0 = smat.m_kcl.n_nz;
   r0 = smat.m_kcl.n_row;

   for (i_f=0; i_f < n_f1; i_f++) {
     if (i_f < n_nd1) {
//     KCL equation:

       pntr_nd = cct.ebeu_nd_start[i_ebeu] + i_f;
       i_sysvar_ndcur = smat.offs_ndcur + pntr_nd;

       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       smat.rhs_m_ssw[row0+r0] = smat.svec_ex[i_sysvar_ndcur] - ebe_usr[i_ebeu].f[i_f];

       flag_ddt = ebe_lib[i_ebel].f_ddt[i_f];

       if (flag_ddt) {
         i_stv_l = ebe_lib[i_ebel].f_ddt_stv_index[i_f];
         i_stv_u = ebe_usr[i_ebeu].stv[i_stv_l];
         i_sysvar = smat.offs_estv + i_stv_u;

         x_0 = smat.svec_ex[i_sysvar];
         x_1 = smat.svec_old_1_ex[i_sysvar];

         if (slv.ex_algo_be0) {
           smat.rhs_m_ssw[row0+r0] += (-slv.beuler_1_e*(x_0-x_1));
         } else if (slv.ex_algo_trz0) {
           f_1 = smat.svec_old_1_ex[i_sysvar_ndcur] - ebe_usr[i_ebeu].f_old_1[i_f];
           smat.rhs_m_ssw[row0+r0] += (-(slv.trz_1_e*(x_0-x_1) - f_1));
         }
       }
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         l_ddt_stv = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];

         if (!l_ddt_stv) {
           k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
           if (k != -1) {
             var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
             var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

             if (var_flag == global.I_NV) {
               val = ebe_jac[i_ebel].dfdv[i_f][var_number];
             } else if (var_flag == global.I_EAUX) {
               val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
             } else if (var_flag == global.I_XVR) {
               val = ebe_jac[i_ebel].dfdxvr[i_f][var_number];
             } else {
               cout << "mat_ssw_trns_3_ex_e: incorrect value of var_flag in f." << endl;
               cout << "  i_f = " << i_f << ", i_favr = " << i_fvar << endl;
               cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
               exit(1);
             }
             smat.m_ssw.val[k+k0] = - val;
           }
         } else {
           k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];

           if (slv.ex_algo_be0) {
             smat.m_ssw.val[k+k0] = - slv.beuler_1_e;
           } else if (slv.ex_algo_trz0) {
             smat.m_ssw.val[k+k0] = - slv.trz_1_e;
           }
           x_0 = smat.svec_ex[i_sysvar];
           i_g = ebe_lib[i_ebel].f_ddt_stv_eqn[i_f];

           row1 = smat.ebe_g_to_row[i_ebeu][i_g];
           smat.rhs_m_ssw[row1+r0] = ebe_usr[i_ebeu].g[i_g] - x_0;

           k1 = smat.map_stv_to_ebe[i_ebeu][i_f][i_fvar];
           smat.m_ssw.val[k1+k0] = -1.0;

           n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];
           for (int i_gvar=1; i_gvar < n_gvar1; i_gvar++) {
             kg = smat.map_gvar_to_ebe[i_ebeu][i_g][i_gvar];
             if (kg != -1) {
               var_flag_stv = ebe_lib[i_ebel].gvar_flag[i_g][i_gvar];
               var_number_stv = ebe_lib[i_ebel].gvar_index[i_g][i_gvar];

               if (var_flag_stv == global.I_NV) {
                 val = ebe_jac[i_ebel].dgdv[i_g][var_number_stv];
               } else if (var_flag_stv == global.I_EAUX) {
                 val = ebe_jac[i_ebel].dgdaux[i_g][var_number_stv];
               } else if (var_flag_stv == global.I_XVR) {
                 val = ebe_jac[i_ebel].dgdxvr[i_g][var_number_stv];
               }
               smat.m_ssw.val[kg+k0] = val;
             }
           }
         }
       }
     } else {
//     non-KCL equation
       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       smat.rhs_m_ssw[row0+r0] = ebe_usr[i_ebeu].f[i_f];

       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
         if (k != -1) {
           var_flag   = ebe_lib[i_ebel].fvar_flag [i_f][i_fvar];
           var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];

           if (var_flag == global.I_NV) {
             val = ebe_jac[i_ebel].dfdv[i_f][var_number];
           } else if (var_flag == global.I_EAUX) {
             val = ebe_jac[i_ebel].dfdaux[i_f][var_number];
           } else if (var_flag == global.I_XVR) {
             val = ebe_jac[i_ebel].dfdxvr[i_f][var_number];
           } else {
             cout << "mat_ssw_trns_3_ex_e: incorrect value of var_flag in f." << endl;
             cout << "  i_f = " << i_f << ", i_favr = " << i_fvar << endl;
             cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
             exit(1);
           }
           smat.m_ssw.val[k+k0] = val;
         }
       }
     }
   }
   return;
} // end of mat_ssw_trns_3_ex_e
// -----------------------------------------------------------------------------
void mat_ssw_trns_3_ex_x(
   const int i_xbeu,
   const int i_xbel,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Global &global) {

   int i_g,n_g1,n_gvar1,k,row0;
   int var_flag,var_number;
   double val;
   int k0,r0;

   k0 = smat.m_kcl.n_nz + smat.m_e.n_nz;
   r0 = smat.m_kcl.n_row + smat.m_e.n_row;

   n_g1 = xbe_lib[i_xbel].n_g;

   for (i_g=0; i_g < n_g1; i_g++) {
     n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
     for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
       k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
       if (k == -1) {
         cout << "mat_ssw_trns_3_ex_x: k = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         var_flag   = xbe_lib[i_xbel].gvar_flag [i_g][i_gvar];
         var_number = xbe_lib[i_xbel].gvar_index[i_g][i_gvar];

         if (var_flag == global.I_XVR) {
           val = xbe_jac[i_xbel].dgdvr[i_g][var_number];
         } else if (var_flag == global.I_XAUX) {
           val = xbe_jac[i_xbel].dgdaux[i_g][var_number];
         } else {
           cout << "mat_ssw_trns_3_ex_x: incorrect value of var_flag." << endl;
           cout << "  i_g = " << i_g << ", i_gavr = " << i_gvar << endl;
           cout << "  xbe_lib is " << xbe_lib[i_xbel].name << ". Halting.." << endl;
           exit(1);
         }
         smat.m_ssw.val[k+k0] = val;
       }
     }
     if (xbe_lib[i_xbel].flag_integrate) {
       row0 = smat.xbe_f_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_ssw_trns_3_ex_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_ssw[row0+r0] = xbe_usr[i_xbeu].g[i_g];
       }
     } else {
       row0 = smat.xbe_g_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_ssw_trns_3_ex_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_ssw[row0+r0] = xbe_usr[i_xbeu].g[i_g];
       }
     }
   }
   return;
} // end of mat_ssw_trns_3_ex_x
// -----------------------------------------------------------------------------
void mat_ssw_trns_3_x(
   const int i_xbeu,
   const int i_xbel,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Global &global) {

   int i_g,n_g1,n_gvar1,k,row0;
   int var_flag,var_number;
   double val;

   n_g1 = xbe_lib[i_xbel].n_g;

   for (i_g=0; i_g < n_g1; i_g++) {
     n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
     for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
       k = smat.map_gvar_to_xbe[i_xbeu][i_g][i_gvar];
       if (k == -1) {
         cout << "mat_ssw_trns_3_x: k = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         var_flag   = xbe_lib[i_xbel].gvar_flag [i_g][i_gvar];
         var_number = xbe_lib[i_xbel].gvar_index[i_g][i_gvar];

         if (var_flag == global.I_XVR) {
           val = xbe_jac[i_xbel].dgdvr[i_g][var_number];
         } else if (var_flag == global.I_XAUX) {
           val = xbe_jac[i_xbel].dgdaux[i_g][var_number];
         } else {
           cout << "mat_ssw_trns_3_x: incorrect value of var_flag." << endl;
           cout << "  i_g = " << i_g << ", i_gavr = " << i_gvar << endl;
           cout << "  xbe_lib is " << xbe_lib[i_xbel].name << ". Halting.." << endl;
           exit(1);
         }
         smat.m_ssw.val[k] = val;
       }
     }
     if (xbe_lib[i_xbel].flag_integrate) {
       row0 = smat.xbe_f_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_ssw_trns_3_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_ssw[row0] = xbe_usr[i_xbeu].g[i_g];
       }
     } else {
       row0 = smat.xbe_g_to_row[i_xbeu][i_g];
       if (row0 == -1) {
         cout << "mat_ssw_trns_3_x: row0 = -1 is not expected. Halting.." << endl;
         exit(1);
       } else {
         smat.rhs_m_ssw[row0] = xbe_usr[i_xbeu].g[i_g];
       }
     }
   }
   return;
} // end of mat_ssw_trns_3_x
// -----------------------------------------------------------------------------
void solve_ssw_trns_linear_e(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

// todo
// remove unused arguments from the call

   bool flag_write;

   form_jac_rhs_ssw_trns_e(ebe_lib,ebe_usr,ebe_jac,
     smat,cct,slv,cct_file,global);

// add_ssw_trns_terms in Sequel
   add_ssw_trns_terms_e(ebe_lib,ebe_usr,ebe_jac,
     smat,cct,slv,cct_file,global);

   negative_double_1(smat.m_ssw.n_row,smat.rhs_m_ssw);

   if (slv.iter_trns_e == 0) {
     solve_jac_1_ssw_e(smat,slv,global);
   } else {
     solve_jac_2_ssw_e(smat,slv,global);
   }
   copy_array_1<double>(smat.m_ssw.n_row,smat.svec_orig_e,smat.delsvec_e);

   add_arrays_1<double>(smat.m_ssw.n_row,smat.delsvec_e,smat.svec_e);
   dcmp_solvec_ssw_e(ebe_lib,ebe_usr,smat,slv,cct);

   return;
} // end of solve_ssw_trns_linear_e
// -----------------------------------------------------------------------------
void solve_ssw_trns_linear_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   bool flag_write;

   form_jac_rhs_ssw_trns_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     smat,cct,slv,cct_file,global);

// add_ssw_trns_terms in Sequel
   add_ssw_trns_terms_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     smat,cct,slv,cct_file,global);

   negative_double_1(smat.m_ssw.n_row,smat.rhs_m_ssw);

   if (slv.iter_trns_e == 0) {
     solve_jac_1_ssw_ex(smat,slv,global);
   } else {
     solve_jac_2_ssw_ex(smat,slv,global);
   }
   copy_array_1<double>(smat.m_ssw.n_row,smat.svec_orig_ex,smat.delsvec_ex);

   add_arrays_1<double>(smat.m_ssw.n_row,smat.delsvec_ex,smat.svec_ex);
   dcmp_solvec_ssw_ex(xbe_lib,xbe_usr,ebe_lib,ebe_usr,smat,slv,cct);

   return;
} // end of solve_ssw_trns_linear_ex
// -----------------------------------------------------------------------------
void solve_ssw_trns_linear_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   bool flag_write;

   form_jac_rhs_ssw_trns_x(xbe_lib,xbe_usr,xbe_jac,
     smat,cct,slv,cct_file,global);

// add_ssw_trns_terms in Sequel
   add_ssw_trns_terms_x(xbe_lib,xbe_usr,xbe_jac,
     smat,cct,slv,cct_file,global);

   negative_double_1(smat.m_ssw.n_row,smat.rhs_m_ssw);

   if (slv.iter_trns_x == 0) {
     solve_jac_1_ssw_x(smat,slv,global);
   } else {
     solve_jac_2_ssw_x(smat,slv,global);
   }
   copy_array_1<double>(smat.m_ssw.n_row,smat.svec_orig_x,smat.delsvec_x);

   add_arrays_1<double>(smat.m_ssw.n_row,smat.delsvec_x,smat.svec_x);

   dcmp_solvec_x(xbe_lib,xbe_usr,smat,cct);

   return;
} // end of solve_ssw_trns_linear_x
// -----------------------------------------------------------------------------
void solve_ssw_trns_newton_e(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

// todo
// remove unused arguments from the call

   bool flag_write,flag_nan_1;
   int i_newt;

   slv.get_dmp(cct);

   for (i_newt=0; i_newt < slv.e_nr_itermax_a; i_newt++) {
     slv.iter_newton = i_newt;

     form_jac_rhs_ssw_trns_e(ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);

//   add_ssw_trns_terms in Sequel
     add_ssw_trns_terms_e(ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);

     negative_double_1(smat.m_ssw.n_row,smat.rhs_m_ssw);
     if (slv.ssw_iter_newton == 0) {
       if (slv.iter_newton == 0) {
         if (slv.iter_trns_e == 0) {
           solve_jac_1_ssw_e(smat,slv,global);
         } else {
           solve_jac_2_ssw_e(smat,slv,global);
         }
       } else {
         solve_jac_2_ssw_e(smat,slv,global);
       }
     } else {
       solve_jac_2_ssw_e(smat,slv,global);
     }
     copy_array_1<double>(smat.m_ssw.n_row,smat.svec_orig_e,smat.delsvec_e);

     if ((slv.e_nr_flag_dmp_a) && (i_newt <= slv.e_nr_dmp_itermax_a)) {
       mult_array_1<double>(smat.m_ssw.n_row,smat.delsvec_e,slv.e_nr_dmp_k_a);
     }

     add_arrays_1<double>(smat.m_ssw.n_row,smat.delsvec_e,smat.svec_e);
     check_array_for_nan_2(smat.n_solvec_e,smat.svec_e,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_ssw_trns_newton_e: svec_e has a NAN. Halting..." << endl;
       exit(1);
     }
     dcmp_solvec_ssw_e(ebe_lib,ebe_usr,smat,slv,cct);
     if (slv.e_nr_flag_check_spice) {
       find_ebe_cur_ssw_trns_e(true,ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
     }
     check_convergence_e(smat,slv,ebe_lib,ebe_usr,cct);
     if (slv.flag_nr_norm_large) {
       cout << "solve_ssw_trns_newton_e: norm too large." << endl;
       break;
     }
     if (slv.flag_nr_converged) goto jump1;
     if (slv.e_nr_flag_check_spice) {
       copy_array_1<double>(smat.m_ssw.n_row,smat.svec_e,smat.svec_old_nr_1_e);
       copy_cur_nd_nr_1(ebe_lib,ebe_usr,cct);
     }
   }
// no convergence in slv.e_nr_itermax_a interations
   slv.flag_nr_converged = false;
   jump1: ;

   ebe_form_arrays_ssw_e(ebe_lib,ebe_usr,smat,cct);

   return;
} // end of solve_ssw_trns_newton_e
// -----------------------------------------------------------------------------
void solve_ssw_trns_newton_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   bool flag_write,flag_nan_1;
   int i_newt;

   slv.get_dmp(cct);

   for (i_newt=0; i_newt < slv.ex_nr_itermax_a; i_newt++) {
     slv.iter_newton = i_newt;

     form_jac_rhs_ssw_trns_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);

//   add_ssw_trns_terms in Sequel
     add_ssw_trns_terms_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);

     negative_double_1(smat.m_ssw.n_row,smat.rhs_m_ssw);
     if (slv.ssw_iter_newton == 0) {
       if (slv.iter_newton == 0) {
         if (slv.iter_trns_e == 0) {
           solve_jac_1_ssw_ex(smat,slv,global);
         } else {
           solve_jac_2_ssw_ex(smat,slv,global);
         }
       } else {
         solve_jac_2_ssw_ex(smat,slv,global);
       }
     } else {
       solve_jac_2_ssw_ex(smat,slv,global);
     }
     copy_array_1<double>(smat.m_ssw.n_row,smat.svec_orig_ex,smat.delsvec_ex);

     if ((slv.ex_nr_flag_dmp_a) && (i_newt <= slv.ex_nr_dmp_itermax_a)) {
       mult_array_1<double>(smat.m_ssw.n_row,smat.delsvec_ex,slv.ex_nr_dmp_k_a);
     }
     add_arrays_1<double>(smat.m_ssw.n_row,smat.delsvec_ex,smat.svec_ex);
     check_array_for_nan_2(smat.n_solvec_ex,smat.svec_ex,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_ssw_trns_newton_ex: svec_ex has a NAN. Halting..." << endl;
       exit(1);
     }
     dcmp_solvec_ssw_ex(xbe_lib,xbe_usr,ebe_lib,ebe_usr,smat,slv,cct);
     if (slv.e_nr_flag_check_spice) {
       find_ebe_cur_ssw_trns_e(true,ebe_lib,ebe_usr,ebe_jac,smat,cct,slv,global);
     }
     check_convergence_ex(smat,slv,ebe_lib,ebe_usr,cct);
     if (slv.flag_nr_norm_large) {
       cout << "solve_ssw_trns_newton_ex: norm too large." << endl;
       break;
     }
     if (slv.flag_nr_converged) goto jump1;
     if (slv.e_nr_flag_check_spice) {
       copy_array_1<double>(smat.m_ssw.n_row,smat.svec_ex,smat.svec_old_nr_1_ex);
       copy_cur_nd_nr_1(ebe_lib,ebe_usr,cct);
     }
   }
// no convergence in slv.e_nr_itermax_a interations
   slv.flag_nr_converged = false;
   jump1: ;

   ebe_form_arrays_ssw_ex(ebe_lib,ebe_usr,smat,cct);

   return;
} // end of solve_ssw_trns_newton_ex
// -----------------------------------------------------------------------------
void solve_ssw_trns_newton_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   bool flag_write,flag_nan_1;
   int i_newt;

   slv.get_dmp(cct);

   for (i_newt=0; i_newt < slv.ex_nr_itermax_a; i_newt++) {
     slv.iter_newton = i_newt;

     form_jac_rhs_ssw_trns_x(xbe_lib,xbe_usr,xbe_jac,
       smat,cct,slv,cct_file,global);

//   add_ssw_trns_terms in Sequel
     add_ssw_trns_terms_x(xbe_lib,xbe_usr,xbe_jac,
       smat,cct,slv,cct_file,global);

     negative_double_1(smat.m_ssw.n_row,smat.rhs_m_ssw);
     if (slv.ssw_iter_newton == 0) {
       if (slv.iter_newton == 0) {
         if (slv.iter_trns_x == 0) {
           solve_jac_1_ssw_x(smat,slv,global);
         } else {
           solve_jac_2_ssw_x(smat,slv,global);
         }
       } else {
         solve_jac_2_ssw_x(smat,slv,global);
       }
     } else {
       solve_jac_2_ssw_x(smat,slv,global);
     }
     copy_array_1<double>(smat.m_ssw.n_row,smat.svec_orig_x,smat.delsvec_x);

     if ((slv.x_nr_flag_dmp_a) && (i_newt <= slv.x_nr_dmp_itermax_a)) {
       mult_array_1<double>(smat.m_ssw.n_row,smat.delsvec_x,slv.x_nr_dmp_k_a);
     }
     add_arrays_1<double>(smat.m_ssw.n_row,smat.delsvec_x,smat.svec_x);
     check_array_for_nan_2(smat.n_solvec_x,smat.svec_x,flag_nan_1);
     if (flag_nan_1) {
       cout << "solve_ssw_trns_newton_x: svec_x has a NAN. Halting..." << endl;
       exit(1);
     }
     dcmp_solvec_x(xbe_lib,xbe_usr,smat,cct);
     check_convergence_x(smat,slv);
     if (slv.flag_nr_norm_large) {
       cout << "solve_ssw_trns_newton_x: norm too large." << endl;
       break;
     }
     if (slv.flag_nr_converged) goto jump1;
   }
// no convergence in slv.x_nr_itermax_a interations
   slv.flag_nr_converged = false;
   jump1: ;

   return;
} // end of solve_ssw_trns_newton_x
// -----------------------------------------------------------------------------
void form_jac_rhs_ssw_trns_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_ebeu,i_ebel;

   ebe_form_arrays_ssw_e(ebe_lib,ebe_usr,smat,cct);

   global.flags[global.i_trns    ] = true;
   global.flags[global.i_ssw     ] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;

   ebe_init_func_jac_ssw_trns_e(ebe_lib,ebe_usr,ebe_jac,
     smat,cct,slv,cct_file,global);

   assign_kcl_ssw_trns_e(ebe_lib,ebe_usr,smat,cct);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);

     mat_ssw_trns_3_e(i_ebeu,i_ebel,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);
   }
   global.flags[global.i_trns    ] = false;
   global.flags[global.i_ssw     ] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;

   return;
} // end of form_jac_rhs_ssw_trns_e
// -----------------------------------------------------------------------------
void form_jac_rhs_ssw_trns_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_ebeu,i_ebel;
   int i_xbeu,i_xbel;

   ebe_form_arrays_ssw_ex(ebe_lib,ebe_usr,smat,cct);

   global.flags[global.i_trns    ] = true;
   global.flags[global.i_ssw     ] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;
   global.flags[global.i_implicit] = true;

   ebe_init_func_jac_ssw_trns_e(ebe_lib,ebe_usr,ebe_jac,
     smat,cct,slv,cct_file,global);

   assign_kcl_ssw_trns_ex(ebe_lib,ebe_usr,smat,cct);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);

     mat_ssw_trns_3_ex_e(i_ebeu,i_ebel,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,global);
   }

   xbe_init_jac_ssw_trns_ex_x(xbe_lib,xbe_usr,cct,smat);

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);

     mat_ssw_trns_3_ex_x(i_xbeu,i_xbel,xbe_lib,xbe_usr,xbe_jac,
       smat,global);
   }
   global.flags[global.i_trns    ] = false;
   global.flags[global.i_ssw     ] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;
   global.flags[global.i_implicit] = false;

   return;
} // end of form_jac_rhs_ssw_trns_ex
// -----------------------------------------------------------------------------
void form_jac_rhs_ssw_trns_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_trns    ] = true;
   global.flags[global.i_ssw     ] = true;
   global.flags[global.i_function] = true;
   global.flags[global.i_jacobian] = true;
   global.flags[global.i_implicit] = true;

   xbe_init_jac_ssw_trns_x(xbe_lib,xbe_usr,cct,smat);

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);

     mat_ssw_trns_3_x(i_xbeu,i_xbel,xbe_lib,xbe_usr,xbe_jac,
       smat,global);
   }
   global.flags[global.i_trns    ] = false;
   global.flags[global.i_ssw     ] = false;
   global.flags[global.i_function] = false;
   global.flags[global.i_jacobian] = false;
   global.flags[global.i_implicit] = false;

   return;
} // end of form_jac_rhs_ssw_trns_x
// -----------------------------------------------------------------------------
void ebe_init_func_jac_ssw_trns_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_ebeu,i_ebel;
   int n_f1,n_nd1,n_fvar1,k,k0;

// todo
// remove unused arguments from the call

   k0 = smat.m_kcl.n_nz;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;

     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;

     for (int i_f=0; i_f < n_nd1; i_f++) {
       if (i_f < n_nd1) {
//       KCL equations:
         n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
         for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
           k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
           if (k != -1) {
             smat.m_ssw.val[k+k0] = 0.0;
           }
           if (!ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar]) {
             k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
             if (k != -1) {
               smat.m_ssw.val[k+k0] = 0.0;
             }
           } else {
             k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
             smat.m_ssw.val[k+k0] = 0.0;
           }
         }
       } else {
//       non-KCL equations:
         n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
         for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
           k = smat.map_fvar_to_ebe[i_ebeu][i_f][i_fvar];
           if (k != -1) {
             smat.m_ssw.val[k+k0] = 0.0;
           }
         }
       }
     }
   }

   return;
} // end of ebe_init_func_jac_ssw_trns_e
// -----------------------------------------------------------------------------
void assign_kcl_ssw_trns_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   Circuit &cct) {

   int i_ebeu,i_ebel;
   int n_nd1,i_ebeu_nd,pntr,row0,i_sysvar;

   for (int i=0; i < (cct.n_ebeu_nd-1); i++) {
     smat.rhs_m_ssw[i] = 0.0;
   }
   pntr = 0;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;

     for (int i=0; i < n_nd1; i++) {
       i_ebeu_nd = ebe_usr[i_ebeu].nd[i];
       if (i_ebeu_nd != cct.ref_nd) {
         if (i_ebeu_nd < cct.ref_nd) {
           row0 = i_ebeu_nd;
         } else {
           row0 = i_ebeu_nd - 1;
         }

         if (pntr == -1) {
           cout << "assign_kcl_ssw_trns_e: pntr = -1? Halting" << endl;
           exit(1);
         }
         i_sysvar = smat.offs_ndcur + pntr;

         smat.rhs_m_ssw[row0] += smat.svec_e[i_sysvar];
       }
       pntr++;
     }
   }
   return;
} // end of assign_kcl_ssw_trns_e
// -----------------------------------------------------------------------------
void assign_kcl_ssw_trns_ex(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   Circuit &cct) {

   int i_ebeu,i_ebel;
   int n_nd1,i_ebeu_nd,pntr,row0,i_sysvar;

   for (int i=0; i < (cct.n_ebeu_nd-1); i++) {
     smat.rhs_m_ssw[i] = 0.0;
   }
   pntr = 0;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;

     for (int i=0; i < n_nd1; i++) {
       i_ebeu_nd = ebe_usr[i_ebeu].nd[i];
       if (i_ebeu_nd != cct.ref_nd) {
         if (i_ebeu_nd < cct.ref_nd) {
           row0 = i_ebeu_nd;
         } else {
           row0 = i_ebeu_nd - 1;
         }
         if (pntr == -1) {
           cout << "assign_kcl_ssw_trns_ex: pntr = -1? Halting" << endl;
           exit(1);
         }
         i_sysvar = smat.offs_ndcur + pntr;
         smat.rhs_m_ssw[row0] += smat.svec_ex[i_sysvar];
       }
       pntr++;
     }
   }
   return;
} // end of assign_kcl_ssw_trns_ex
// -----------------------------------------------------------------------------
void add_ssw_trns_terms_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_sysrhs,i_eberhs,i_sysvar,i_statevar,pntr_m,k0,r0,rhs_flag;
   int i_ebeu,i_f;
   bool flag_1;
   double x_0,x_1,f_1;
   double ssw_maxf1;

   r0 = smat.m_kcl.n_row;
   k0 = smat.m_kcl.n_nz;

   if (slv.e_algo_be0) {
     slv.ssw_maxf = 0.0;

     for (int i_sysrhs=r0; i_sysrhs < smat.m_ssw.n_row; i_sysrhs++) {

       i_eberhs = i_sysrhs - r0;
       flag_1 = smat.ebe_rhs_ddt_flag[i_eberhs] || smat.flag_ebe_stv[i_eberhs];
       if (flag_1) {

         i_sysvar = smat.ebe_rhs_ddt_varnumber[i_eberhs];
         x_0 = smat.svec_e[i_sysvar];
         x_1 = smat.svec_old_1_e[i_sysvar];

         if (slv.ssw_flag_laststep) {
           i_statevar = smat.ssw_indx3[i_sysvar];

           smat.ssw_rhs[i_statevar] = smat.rhs_m_ssw[i_sysrhs];

           ssw_maxf1 = fabs(smat.rhs_m_ssw[i_sysrhs]);

           if (ssw_maxf1 > slv.ssw_maxf) {
             slv.ssw_maxf = ssw_maxf1;
             slv.ssw_index_k = i_statevar;
           }
         }

         if (smat.ebe_rhs_ddt_flag[i_eberhs]) {
           smat.rhs_m_ssw[i_sysrhs] += - slv.beuler_1_e*(x_0-x_1);

           pntr_m = smat.ebe_rhs_ddt_pntr[i_eberhs];
           rhs_flag = smat.ssw_ebe_rhs_flag[i_eberhs];

           if (rhs_flag == global.I_EBE_DDT_NONKCL) {
             smat.m_ssw.val[pntr_m+k0] += -slv.beuler_1_e;
           } else {
             cout << "add_ssw_trns_terms_e: rhs_flag is not correct." << endl;
             cout << "  rhs_flag=" << rhs_flag << endl;
             cout << "  Halting..." << endl; exit(1); 
           }
         }
       }
     }
   } else if (slv.e_algo_trz0) {
     slv.ssw_maxf = 0.0;
     for (int i_sysrhs=r0; i_sysrhs < smat.m_ssw.n_row; i_sysrhs++) {
       i_eberhs = i_sysrhs - r0;
       flag_1 = smat.ebe_rhs_ddt_flag[i_eberhs] || smat.flag_ebe_stv[i_eberhs];

       if (flag_1) {
         i_sysvar = smat.ebe_rhs_ddt_varnumber[i_eberhs];
         x_0 = smat.svec_e[i_sysvar];
         x_1 = smat.svec_old_1_e[i_sysvar];

         if (slv.ssw_flag_laststep) {
           i_statevar = smat.ssw_indx3[i_sysvar];

           smat.ssw_rhs[i_statevar] = smat.rhs_m_ssw[i_sysrhs];
           ssw_maxf1 = fabs(smat.rhs_m_ssw[i_sysrhs]);

           if (ssw_maxf1 > slv.ssw_maxf) {
             slv.ssw_maxf = ssw_maxf1;
             slv.ssw_index_k = i_statevar;
           }
         }
         if (smat.ebe_rhs_ddt_flag[i_eberhs]) {
           smat.rhs_m_ssw[i_sysrhs] += - slv.trz_1_e*(x_0-x_1);

           pntr_m = smat.ebe_rhs_ddt_pntr[i_eberhs];
           rhs_flag = smat.ssw_ebe_rhs_flag[i_eberhs];

           if (rhs_flag == global.I_EBE_DDT_NONKCL) {
             smat.m_ssw.val[pntr_m+k0] += -slv.trz_1_e;

//           i_eqn = mat->ebce_rhs_to_func1[i_sysrhs];
//           f_1 = ebeu->f_old_1[i_eqn];

             i_ebeu = smat.ebe_rhs_ddt_i_ebeu[i_eberhs];
             i_f = smat.ebe_rhs_ddt_i_f[i_eberhs];
             f_1 = ebe_usr[i_ebeu].f_old_1[i_f];
           } else {
             cout << "add_ssw_trns_terms_e: rhs_flag is not correct." << endl;
             cout << "  rhs_flag=" << rhs_flag << endl;
             cout << "  Halting..." << endl; exit(1); 
           }
//         mat->rhs_m[i_sysrhs] = mat->rhs_m[i_sysrhs] + f_1;
           smat.rhs_m_ssw[i_sysrhs] += f_1;
         }
       }
     }
   }
   return;
} // end of add_ssw_trns_terms_e
// -----------------------------------------------------------------------------
void add_ssw_trns_terms_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_sysrhs,i_eberhs,i_xberhs,i_sysvar,i_statevar,pntr_m,rhs_flag;
   int r_begin_e,r_end_e;
   int r_begin_x,r_end_x;
   int k_begin_e,k_end_e;
   int k_begin_x,k_end_x;
   int i_ebeu,i_xbeu,i_f;
   bool flag_1;
   double x_0,x_1,f_1;
   double ssw_maxf1;

   r_begin_e = smat.m_kcl.n_row;
   r_end_e = r_begin_e + smat.m_e.n_row;
   r_begin_x = r_end_e;
   r_end_x = r_begin_x + smat.m_x.n_row;

   k_begin_e = smat.m_kcl.n_nz;
   k_end_e = k_begin_e + smat.m_e.n_nz;
   k_begin_x = k_end_e;
   k_end_x = k_begin_x + smat.m_x.n_nz;

   if (slv.ex_algo_be0) {
     slv.ssw_maxf = 0.0;

     for (int i_sysrhs=r_begin_e; i_sysrhs < r_end_e; i_sysrhs++) {
       i_eberhs = i_sysrhs - r_begin_e;
       flag_1 = smat.ebe_rhs_ddt_flag[i_eberhs] || smat.flag_ebe_stv[i_eberhs];
       if (flag_1) {
         i_sysvar = smat.ebe_rhs_ddt_varnumber[i_eberhs];
         x_0 = smat.svec_ex[i_sysvar];
         x_1 = smat.svec_old_1_ex[i_sysvar];

         if (slv.ssw_flag_laststep) {
           i_statevar = smat.ssw_indx3[i_sysvar];

           smat.ssw_rhs[i_statevar] = smat.rhs_m_ssw[i_sysrhs];

           ssw_maxf1 = fabs(smat.rhs_m_ssw[i_sysrhs]);

           if (ssw_maxf1 > slv.ssw_maxf) {
             slv.ssw_maxf = ssw_maxf1;
             slv.ssw_index_k = i_statevar;
           }
         }
         if (smat.ebe_rhs_ddt_flag[i_eberhs]) {
           smat.rhs_m_ssw[i_sysrhs] += - slv.beuler_1_e*(x_0-x_1);

           pntr_m = smat.ebe_rhs_ddt_pntr[i_eberhs];
           rhs_flag = smat.ssw_ebe_rhs_flag[i_eberhs];

           if (rhs_flag == global.I_EBE_DDT_NONKCL) {
             smat.m_ssw.val[pntr_m+k_begin_e] += -slv.beuler_1_e;
           } else {
             cout << "add_ssw_trns_terms_ex: rhs_flag is not correct." << endl;
             cout << "  rhs_flag=" << rhs_flag << endl;
             cout << "  Halting..." << endl; exit(1); 
           }
         }
       }
     }
     for (int i_sysrhs=r_begin_x; i_sysrhs < r_end_x; i_sysrhs++) {
       i_xberhs = i_sysrhs - r_begin_x;
       if (smat.xbe_rhs_ddt_flag[i_xberhs]) {
         i_sysvar = smat.xbe_rhs_ddt_varnumber[i_xberhs];
         x_0 = smat.svec_ex[i_sysvar];
         x_1 = smat.svec_old_1_ex[i_sysvar];
         smat.rhs_m_ssw[i_sysrhs] += - slv.beuler_1_x*(x_0-x_1);
         pntr_m = smat.xbe_rhs_ddt_pntr[i_xberhs];
         smat.m_ssw.val[pntr_m+k_begin_x] += -slv.beuler_1_x;
       }
     }
   } else if (slv.ex_algo_trz0) {
     slv.ssw_maxf = 0.0;
     for (int i_sysrhs=r_begin_e; i_sysrhs < r_end_e; i_sysrhs++) {
       i_eberhs = i_sysrhs - r_begin_e;
       flag_1 = smat.ebe_rhs_ddt_flag[i_eberhs] || smat.flag_ebe_stv[i_eberhs];

       if (flag_1) {
         i_sysvar = smat.ebe_rhs_ddt_varnumber[i_eberhs];
         x_0 = smat.svec_ex[i_sysvar];
         x_1 = smat.svec_old_1_ex[i_sysvar];

         if (slv.ssw_flag_laststep) {
           i_statevar = smat.ssw_indx3[i_sysvar];

           smat.ssw_rhs[i_statevar] = smat.rhs_m_ssw[i_sysrhs];
           ssw_maxf1 = fabs(smat.rhs_m_ssw[i_sysrhs]);

           if (ssw_maxf1 > slv.ssw_maxf) {
             slv.ssw_maxf = ssw_maxf1;
             slv.ssw_index_k = i_statevar;
           }
         }
         if (smat.ebe_rhs_ddt_flag[i_eberhs]) {
           smat.rhs_m_ssw[i_sysrhs] += - slv.trz_1_e*(x_0-x_1);

           pntr_m = smat.ebe_rhs_ddt_pntr[i_eberhs];
           rhs_flag = smat.ssw_ebe_rhs_flag[i_eberhs];

           if (rhs_flag == global.I_EBE_DDT_NONKCL) {
             smat.m_ssw.val[pntr_m+k_begin_e] += -slv.trz_1_e;

             i_ebeu = smat.ebe_rhs_ddt_i_ebeu[i_eberhs];
             i_f = smat.ebe_rhs_ddt_i_f[i_eberhs];
             f_1 = ebe_usr[i_ebeu].f_old_1[i_f];
           } else {
             cout << "add_ssw_trns_terms_ex: rhs_flag is not correct." << endl;
             cout << "  rhs_flag=" << rhs_flag << endl;
             cout << "  Halting..." << endl; exit(1); 
           }
           smat.rhs_m_ssw[i_sysrhs] += f_1;
         }
       }
     }
     for (int i_sysrhs=r_begin_x; i_sysrhs < r_end_x; i_sysrhs++) {
       i_xberhs = i_sysrhs - r_begin_x;
       if (smat.xbe_rhs_ddt_flag[i_xberhs]) {
         i_sysvar = smat.xbe_rhs_ddt_varnumber[i_xberhs];
         x_0 = smat.svec_ex[i_sysvar];
         x_1 = smat.svec_old_1_ex[i_sysvar];
         smat.rhs_m_ssw[i_sysrhs] += - slv.trz_1_x*(x_0-x_1);
         pntr_m = smat.xbe_rhs_ddt_pntr[i_xberhs];
         smat.m_ssw.val[pntr_m+k_begin_x] += -slv.trz_1_x;

         i_xbeu = smat.xbe_rhs_ddt_i_xbeu[i_xberhs];
         i_f = smat.xbe_rhs_ddt_i_f[i_xberhs];
         f_1 = xbe_usr[i_xbeu].g_old_1[i_f];
         smat.rhs_m_ssw[i_sysrhs] += f_1;
       }
     }
   }
   return;
} // end of add_ssw_trns_terms_ex
// -----------------------------------------------------------------------------
void add_ssw_trns_terms_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_sysrhs,i_eberhs,i_xberhs,i_sysvar,i_statevar,pntr_m,rhs_flag;
   int r_begin_x,r_end_x;
   int k_begin_x,k_end_x;
   int i_xbeu,i_f;
   bool flag_1;
   double x_0,x_1,f_1;
   double ssw_maxf1;

   r_begin_x = 0;
   r_end_x = r_begin_x + smat.m_x.n_row;

   if (slv.x_algo_be0) {
     slv.ssw_maxf = 0.0;

     for (int i_sysrhs=r_begin_x; i_sysrhs < r_end_x; i_sysrhs++) {
       i_xberhs = i_sysrhs;
       if (smat.xbe_rhs_ddt_flag[i_xberhs]) {
         i_sysvar = smat.xbe_rhs_ddt_varnumber[i_xberhs];
         x_0 = smat.svec_x[i_sysvar];
         x_1 = smat.svec_old_1_x[i_sysvar];
         smat.rhs_m_ssw[i_sysrhs] += - slv.beuler_1_x*(x_0-x_1);
         pntr_m = smat.xbe_rhs_ddt_pntr[i_xberhs];
         smat.m_ssw.val[pntr_m] += -slv.beuler_1_x;

       }
     }
   } else if (slv.x_algo_trz0) {
     slv.ssw_maxf = 0.0;
     for (int i_sysrhs=r_begin_x; i_sysrhs < r_end_x; i_sysrhs++) {
       i_xberhs = i_sysrhs;
       if (smat.xbe_rhs_ddt_flag[i_xberhs]) {
         i_sysvar = smat.xbe_rhs_ddt_varnumber[i_xberhs];
         x_0 = smat.svec_x[i_sysvar];
         x_1 = smat.svec_old_1_x[i_sysvar];
         smat.rhs_m_ssw[i_sysrhs] += - slv.trz_1_x*(x_0-x_1);
         pntr_m = smat.xbe_rhs_ddt_pntr[i_xberhs];
         smat.m_ssw.val[pntr_m] += -slv.trz_1_x;

         i_xbeu = smat.xbe_rhs_ddt_i_xbeu[i_xberhs];
         i_f = smat.xbe_rhs_ddt_i_f[i_xberhs];
         f_1 = xbe_usr[i_xbeu].g_old_1[i_f];
         smat.rhs_m_ssw[i_sysrhs] += f_1;
       }
     }
   }
   return;
} // end of add_ssw_trns_terms_x
// -----------------------------------------------------------------------------
void solve_ssw_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int k,k1;
   double rhs_ssw_norm,norm0;
   bool flag_rhs2_converged,flag_spice_converged,flag_net_converged;
   int i_ebeu,i_ebel,n_nd1;

   init_sol_ex(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     slv,cct,smat,global);

   smat.mat_ssw_1_ex(xbe_lib,xbe_usr,ebe_lib,ebe_usr,global,cct,cct_file);

   smat.offs_ssw[0] = 0;
   for (int i_statevar=1; i_statevar < smat.n_statevar; i_statevar++) {
     smat.offs_ssw[i_statevar] = smat.offs_ssw[i_statevar-1] + smat.n_solvec_ex;
   }
   if (slv.solve_type_previous != global.I_SSW) {
     for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
       k = smat.ssw_indx2[i_statevar];
       smat.svec_ssw_2[i_statevar] = smat.svec_ex[k];
     }
   }

   for (int i_newton=0; i_newton < slv.ssw_nr_itermax; i_newton++) {
     slv.ssw_iter_newton = i_newton;
     copy_array_1<double>(smat.n_statevar,smat.svec_ssw_2,smat.svec_ssw_2_old);

     if (slv.ssw_iter_newton > -1) {
       for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
         k = smat.ssw_indx2[i_statevar];
         smat.svec_ex[k] = smat.svec_ssw_2[i_statevar];
       }
     }
     for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
       k  = smat.ssw_indx2[i_statevar];
       k1 = smat.offs_ssw [i_statevar];

       set_vector_1(&(smat.svec_ssw_1[k1]),smat.n_solvec_ex,k);
     }

     solve_ssw_1_ex(false,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);

     for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
       k = smat.ssw_indx2[i_statevar];
       smat.svec_ssw_2[i_statevar] = smat.svec_ex[k];
     }
     diff_arrays_1<double>(smat.n_statevar,smat.svec_ssw_2_old,smat.svec_ssw_2,
       smat.rhs_ssw);
     rhs_ssw_norm = norm_2(smat.n_statevar,smat.rhs_ssw);
     cout << "solve_ssw_ex: ssw_iter_newton=" << slv.ssw_iter_newton
       << ", rhs_ssw_norm=" << rhs_ssw_norm << endl;

     if (slv.ssw_nr_flag_check_rhs2) {
       flag_rhs2_converged = (rhs_ssw_norm < slv.ssw_nr_eps_rhs);
     }
     flag_net_converged = true;
     if (slv.ssw_nr_flag_check_rhs2) {
       if (!flag_rhs2_converged) {
         flag_net_converged = false;
       }
     }
     if (flag_net_converged) goto jump1;

     mat_solve(smat.n_statevar,smat.ssw_mat,smat.ssw_mat_1,
       smat.rhs_ssw,smat.delsvec_ssw_2,
       smat.indxc_ssw,smat.indxr_ssw,smat.ipiv_ssw);

     if (slv.ssw_nr_flag_dmp) {
       if (slv.ssw_iter_newton <= slv.ssw_nr_dmp_itermax) {
         mult_array_1<double> (smat.n_statevar,smat.delsvec_ssw_2,
           slv.ssw_nr_dmp_k);
       }
     }
     add_arrays_2<double>(smat.n_statevar,smat.svec_ssw_2_old,
       smat.delsvec_ssw_2,smat.svec_ssw_2);
   }
   cout << "solve_ssw_ex: convergence not reached." << endl;
   cout << "  This is a warning only." << endl;

   jump1: ;

   cout << "solve_ssw_ex: calling solve_ssw_1_ex for one more trns step" << endl;
   slv.flag_ssw_final_trns = true;

   solve_ssw_1_ex(true,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     smat,cct,slv,cct_file,global);

   cout << "solve_ssw_1_ex over (after trns step for output)" << endl;
   cout << "solve_ssw_ex ends, slv.ssw_iter_newton="
     << slv.ssw_iter_newton << endl;

   return;
} // end of solve_ssw_ex
// -----------------------------------------------------------------------------
void solve_ssw_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int k,k1;
   double rhs_ssw_norm,norm0;
   bool flag_rhs2_converged,flag_spice_converged,flag_net_converged;

   cout << "solve_ssw_x starts..." << endl;

   init_sol_x(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,slv,cct,smat,global);

   smat.mat_ssw_1_x(xbe_lib,xbe_usr,global,cct,cct_file);

   smat.offs_ssw[0] = 0;
   for (int i_statevar=1; i_statevar < smat.n_statevar; i_statevar++) {
     smat.offs_ssw[i_statevar] = smat.offs_ssw[i_statevar-1] + smat.n_solvec_x;
   }
   if (slv.solve_type_previous != global.I_SSW) {
     for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
       k = smat.ssw_indx2[i_statevar];
       smat.svec_ssw_2[i_statevar] = smat.svec_x[k];
     }
   }

   for (int i_newton=0; i_newton < slv.ssw_nr_itermax; i_newton++) {
     slv.ssw_iter_newton = i_newton;
     copy_array_1<double>(smat.n_statevar,smat.svec_ssw_2,smat.svec_ssw_2_old);

     if (slv.ssw_iter_newton > -1) {
       for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
         k = smat.ssw_indx2[i_statevar];
         smat.svec_x[k] = smat.svec_ssw_2[i_statevar];
       }
     }
     for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
       k  = smat.ssw_indx2[i_statevar];
       k1 = smat.offs_ssw [i_statevar];

       set_vector_1(&(smat.svec_ssw_1[k1]),smat.n_solvec_x,k);
     }

     solve_ssw_1_x(false,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
       smat,cct,slv,cct_file,global);

     for (int i_statevar=0; i_statevar < smat.n_statevar; i_statevar++) {
       k = smat.ssw_indx2[i_statevar];
       smat.svec_ssw_2[i_statevar] = smat.svec_x[k];
     }
     diff_arrays_1<double>(smat.n_statevar,smat.svec_ssw_2_old,smat.svec_ssw_2,
       smat.rhs_ssw);
     rhs_ssw_norm = norm_2(smat.n_statevar,smat.rhs_ssw);
     cout << "solve_ssw_x: ssw_iter_newton=" << slv.ssw_iter_newton
       << ", rhs_ssw_norm=" << rhs_ssw_norm << endl;

     if (slv.ssw_nr_flag_check_rhs2) {
       flag_rhs2_converged = (rhs_ssw_norm < slv.ssw_nr_eps_rhs);
     }
     flag_net_converged = true;
     if (slv.ssw_nr_flag_check_rhs2) {
       if (!flag_rhs2_converged) {
         flag_net_converged = false;
       }
     }
     if (flag_net_converged) goto jump1;

     mat_solve(smat.n_statevar,smat.ssw_mat,smat.ssw_mat_1,
       smat.rhs_ssw,smat.delsvec_ssw_2,
       smat.indxc_ssw,smat.indxr_ssw,smat.ipiv_ssw);

     if (slv.ssw_nr_flag_dmp) {
       if (slv.ssw_iter_newton <= slv.ssw_nr_dmp_itermax) {
         mult_array_1<double> (smat.n_statevar,smat.delsvec_ssw_2,
           slv.ssw_nr_dmp_k);
       }
     }
     add_arrays_2<double>(smat.n_statevar,smat.svec_ssw_2_old,
       smat.delsvec_ssw_2,smat.svec_ssw_2);
   }
   cout << "solve_ssw_x: convergence not reached." << endl;
   cout << "  This is a warning only." << endl;

   jump1: ;

   cout << "solve_ssw_x: calling solve_ssw_1_x for one more trns step" << endl;
   slv.flag_ssw_final_trns = true;

   solve_ssw_1_x(true,xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
     smat,cct,slv,cct_file,global);

   cout << "solve_ssw_1_x over (after trns step for output)" << endl;
   cout << "solve_ssw_x ends, slv.ssw_iter_newton="
     << slv.ssw_iter_newton << endl;

   return;
} // end of solve_ssw_x
// -----------------------------------------------------------------------------
void trzbdf2_1_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv) {

   double norm2,norm,r,hstar,step1,step2,x,c1,c2,c3;
   double f_0,f_1,f_2;
   int i_ebeu,i_ebel;
   int i_f,i_rhs,row0,n_nd1;

   norm2 = 0.0;
   c1 = 1.0/slv.bank_gamma;
   c3 = 1.0/(1.0-slv.bank_gamma);
   c2 = c1*c3;

// KCL equations:
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;

     for (i_f=0; i_f < n_nd1; i_f++) {
       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       if (row0 != -1) {
         f_0 = ebe_usr[i_ebeu].cur_nd  [i_f] - ebe_usr[i_ebeu].f      [i_f];
         f_1 = ebe_usr[i_ebeu].cur_nd_1[i_f] - ebe_usr[i_ebeu].f_old_1[i_f];
         f_2 = ebe_usr[i_ebeu].cur_nd_2[i_f] - ebe_usr[i_ebeu].f_old_2[i_f];

         x = c1*f_2 - c2*f_1 + c3*f_0;
         norm2 = norm2 + x*x;
       }
     }
   }
   for (i_rhs=0; i_rhs < smat.m_e.n_row; i_rhs++) {
     if (smat.ebe_rhs_ddt_flag[i_rhs]) {
       i_f = smat.ebe_rhs_ddt_i_f[i_rhs];

       f_0 = ebe_usr[i_ebeu].f      [i_f];
       f_1 = ebe_usr[i_ebeu].f_old_1[i_f];
       f_2 = ebe_usr[i_ebeu].f_old_2[i_f];

       x = c1*f_2 - c2*f_1 + c3*f_0;
       norm2 = norm2 + x*x;
     }
   }
   norm = 2.0*fabs(slv.bank_c)*slv.delt_e*sqrt(norm2);
   r = norm/slv.bank_tolr;
   hstar = slv.delt_e*pow(r,-0.333333333);

   if (r >= 2.0) {
     slv.flag_accept_sol = false;
     slv.delt_new_e = 0.9*hstar;
   } else {
     slv.flag_accept_sol = true;
     step1 = slv.bank_theta1*hstar;
     step2 = 2.0*slv.delt_e;
     slv.delt_new_e = min(step1,step2);
   }

   return;
} // end of trzbdf2_1_e
// -----------------------------------------------------------------------------
void trzbdf2_1_x(
   vector<XbeUsr> &xbe_usr,
   SysMat &smat,
   SolveBlocks &slv) {

   double norm2,norm,r,hstar,step1,step2,x,c1,c2,c3;
   double g_0,g_1,g_2;
   int i_xbeu;
   int i_f,i_rhs;

   norm2 = 0.0;
   c1 = 1.0/slv.bank_gamma;
   c3 = 1.0/(1.0-slv.bank_gamma);
   c2 = c1*c3;

   for (i_rhs=0; i_rhs < smat.m_x.n_row; i_rhs++) {
     if (smat.xbe_rhs_ddt_flag[i_rhs]) {
       i_xbeu = smat.xbe_rhs_ddt_i_xbeu[i_rhs];
       i_f = smat.xbe_rhs_ddt_i_f[i_rhs];

       g_0 = xbe_usr[i_xbeu].g      [i_f];
       g_1 = xbe_usr[i_xbeu].g_old_1[i_f];
       g_2 = xbe_usr[i_xbeu].g_old_2[i_f];

       x = c1*g_2 - c2*g_1 + c3*g_0;

       norm2 = norm2 + x*x;
       if (flag_nan(norm2)) {
         cout << "trzbdf2_1_x: norm2 is NAN. Halting..." << endl; exit(1);
       }
     }
   }

   norm = 2.0*fabs(slv.bank_c)*slv.delt_x*sqrt(norm2);

   r = norm/slv.bank_tolr;

   hstar = slv.delt_x*pow(r,-0.333333333);

   if (r >= 2.0) {
     slv.flag_accept_sol = false;
     slv.delt_new_x = 0.9*hstar;
   } else {
     slv.flag_accept_sol = true;
     step1 = slv.bank_theta1*hstar;
     step2 = 2.0*slv.delt_x;
     slv.delt_new_x = min(step1,step2);
   }

   return;
} // end of trzbdf2_1_x
// -----------------------------------------------------------------------------
void trzbdf2_1_ex(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeUsr> &xbe_usr,
   SysMat &smat,
   Circuit &cct,
   SolveBlocks &slv) {

   double norm2,norm,r,hstar,step1,step2,x,c1,c2,c3;
   double f_0,f_1,f_2;
   double g_0,g_1,g_2;
   int i_ebeu,i_ebel;
   int i_xbeu;
   int i_f,i_rhs,row0,n_nd1;

   norm2 = 0.0;
   c1 = 1.0/slv.bank_gamma;
   c3 = 1.0/(1.0-slv.bank_gamma);
   c2 = c1*c3;

// KCL equations:
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;

     for (i_f=0; i_f < n_nd1; i_f++) {
       row0 = smat.ebe_f_to_row[i_ebeu][i_f];
       if (row0 != -1) {
         f_0 = ebe_usr[i_ebeu].cur_nd  [i_f] - ebe_usr[i_ebeu].f      [i_f];
         f_1 = ebe_usr[i_ebeu].cur_nd_1[i_f] - ebe_usr[i_ebeu].f_old_1[i_f];
         f_2 = ebe_usr[i_ebeu].cur_nd_2[i_f] - ebe_usr[i_ebeu].f_old_2[i_f];

         x = c1*f_2 - c2*f_1 + c3*f_0;
         norm2 = norm2 + x*x;
       }
     }
   }
   for (i_rhs=0; i_rhs < smat.m_e.n_row; i_rhs++) {
     if (smat.ebe_rhs_ddt_flag[i_rhs]) {
       i_f = smat.ebe_rhs_ddt_i_f[i_rhs];

       f_0 = ebe_usr[i_ebeu].f      [i_f];
       f_1 = ebe_usr[i_ebeu].f_old_1[i_f];
       f_2 = ebe_usr[i_ebeu].f_old_2[i_f];

       x = c1*f_2 - c2*f_1 + c3*f_0;
       norm2 = norm2 + x*x;
     }
   }
   for (i_rhs=0; i_rhs < smat.m_x.n_row; i_rhs++) {
     if (smat.xbe_rhs_ddt_flag[i_rhs]) {
       i_xbeu = smat.xbe_rhs_ddt_i_xbeu[i_rhs];
       i_f = smat.xbe_rhs_ddt_i_f[i_rhs];

       g_0 = xbe_usr[i_xbeu].g      [i_f];
       g_1 = xbe_usr[i_xbeu].g_old_1[i_f];
       g_2 = xbe_usr[i_xbeu].g_old_2[i_f];

       x = c1*g_2 - c2*g_1 + c3*g_0;
       norm2 = norm2 + x*x;
     }
   }
   norm = 2.0*fabs(slv.bank_c)*slv.delt_e*sqrt(norm2);
   r = norm/slv.bank_tolr;
   hstar = slv.delt_e*pow(r,-0.333333333);

   if (r >= 2.0) {
     slv.flag_accept_sol = false;
     slv.delt_new_e = 0.9*hstar;
   } else {
     slv.flag_accept_sol = true;
     step1 = slv.bank_theta1*hstar;
     step2 = 2.0*slv.delt_e;
     slv.delt_new_e = min(step1,step2);
   }
   slv.delt_new_x = slv.delt_new_e;

   return;
} // end of trzbdf2_1_ex
