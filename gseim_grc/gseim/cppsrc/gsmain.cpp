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
#include <cstring>
#include <fstream>
#include <sys/time.h>
#include <sys/resource.h>

#include "global.h"
#include "cctfile.h"
#include "utils.h"
#include "xbelib.h"
#include "xbeusr.h"
#include "circuit.h"
#include "solveblocks.h"
#include "routines1.h"

using namespace std;

int main(int argc, char** argv) {

   std::string file_xbe_list;
   std::string file_ebe_list;
   std::string file_circuit;
   int n_xbel;
   int n_ebel;
   int i_ebeu,i_ebel,i_xbeu,i_xbel;

   std::string homedir;

   Global global;
   CctFile cct_file;
   Circuit cct;
   SolveBlocks slv;
   SysMat smat;

   vector<XbeLib> xbe_lib;
   vector<XbeUsr> xbe_usr;
   vector<EbeLib> ebe_lib;
   vector<EbeUsr> ebe_usr;

   std::ofstream outf;

   homedir = "."; 
   file_xbe_list = homedir + "/gseim_grc/gseim/cppsrc/xbe.aux";
   file_ebe_list = homedir + "/gseim_grc/gseim/cppsrc/ebe.aux";

   if (argc == 1) {
     file_circuit = "rrtest.in";
   } else {
     file_circuit = argv[1];
   }

   cct_file = CctFile(file_circuit);

   xbe_lib = get_lib_elements<XbeLib>(global,file_xbe_list);
   ebe_lib = get_lib_elements<EbeLib>(global,file_ebe_list);

   n_xbel = xbe_lib.size();
   n_ebel = ebe_lib.size();

   vector<XbeJac> xbe_jac;
   xbe_jac.resize(n_xbel);
   for (int i=0; i < n_xbel; i++) {
     xbe_jac[i].initialise(xbe_lib,i);
   }

   vector<EbeJac> ebe_jac;
   ebe_jac.resize(n_ebel);
   for (int i=0; i < n_ebel; i++) {
     ebe_jac[i].initialise(ebe_lib,i);
   }

   cct = Circuit(file_circuit,xbe_lib,ebe_lib,xbe_usr,ebe_usr,global,cct_file);

// compute one-time parameters:

   if (cct.flag_x) one_time_parms_x(xbe_usr,xbe_jac,cct,global);
   if (cct.flag_e) one_time_parms_e(ebe_usr,ebe_jac,cct,global);

   slv.solve_type_previous = global.I_CLEAR;
   slv.flag_prev_solution_exists = false;

   cct.xbe_map_vr(xbe_lib,xbe_usr);

   cct.check_save_history(xbe_lib,ebe_lib,xbe_usr,ebe_usr);

   cct.check_reset_x(xbe_lib,xbe_usr);
   cct.check_modulo_x(xbe_lib,xbe_usr);
   cct.check_time_parms(xbe_lib,xbe_usr);
   cct.check_sampler_index(xbe_lib,xbe_usr,global);

   cct.check_limit_tstep(xbe_lib,ebe_lib,xbe_usr,ebe_usr);
   cct.check_limit_newton(xbe_lib,ebe_lib,xbe_usr,ebe_usr);

   cct.assign_flag_linear_x(xbe_lib,xbe_usr);
   cct.assign_flag_linear_e(ebe_lib,ebe_usr);

   cct.flag_linear_ex = cct.flag_linear_e && cct.flag_linear_x;

// fixed size allocation (not dependent on solve blocks):
   smat.allocate_1(global,ebe_lib,xbe_lib,ebe_usr,xbe_usr,cct);

   for (int i_solve=0; i_solve < cct_file.n_solve; i_solve++) {
     slv.index_solve = i_solve;
     assign_const_1<bool>(global.flags,false);
     cout << "main: i_solve = " << i_solve << endl;
     slv.set_values_1(file_circuit,cct,global,cct_file);

     if (slv.flag_ssw) {
       for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
         i_ebel = ebe_usr[i_ebeu].index_ebel;
         if (!ebe_lib[i_ebel].flag_allow_ssw) {
           cout << "main: ebe " << ebe_lib[i_ebel].name << " does not allow ssw." << endl;
           cout << "   Halting..." << endl; exit(1);
         }
       }
       for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
         i_xbel = xbe_usr[i_xbeu].index_xbel;
         if (!xbe_lib[i_xbel].flag_allow_ssw) {
           cout << "main: xbe " << xbe_lib[i_xbel].name << " does not allow ssw." << endl;
           cout << "   Halting..." << endl; exit(1);
         }
       }
     }
     if (slv.flag_ssw) {
       smat.ssw_allocate_1(ebe_lib,xbe_lib,ebe_usr,xbe_usr,cct);
     }

     slv.open_output_files();

//   computation of offs, n_solvec:
     smat.set_values_1(global,slv,cct);

     if (slv.flag_ssw) {
       smat.ssw_allocate_2(ebe_lib,xbe_lib,ebe_usr,xbe_usr,cct);
     }

     if (slv.flag_dc) {
//     Note: solve_dc is allowed for circuits with ebe's only, but we
//       have checked that separately. No need to check here.
       cout << "main: calling solve_dc" << endl;
       solve_dc(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     }
     if (slv.flag_startup) {
       cout << "main: calling solve_startup" << endl;
       solve_startup(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     }
     if (slv.flag_trns) {
       cout << "main: calling solve_trns" << endl;
       solve_trns(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     }
     if (slv.flag_ssw) {
       cout << "main: calling solve_trns" << endl;
       solve_ssw(xbe_lib,xbe_usr,xbe_jac,ebe_lib,ebe_usr,ebe_jac,
         smat,cct,slv,cct_file,global);
     }
     if (slv.flag_write_solution) {
       write_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
     }
     slv.close_output_files();

     slv.flag_prev_solution_exists = true;
     slv.solve_type_previous = slv.solve_type;
     slv.flag_sync_x_e_previous = slv.flag_sync_x_e;

     smat.n_solvec_x_previous = smat.n_solvec_x;
     smat.n_solvec_ex_previous = smat.n_solvec_ex;
     smat.n_solvec_e_previous = smat.n_solvec_e;

     if (cct.flag_x_only) {
       form_solvec_x(xbe_lib,xbe_usr,smat,cct);
     }
     smat.copy_svec_to_previous();

     smat.offs_xvr_previous   = smat.offs_xvr;
     smat.offs_xaux_previous  = smat.offs_xaux;
     smat.offs_nv_previous    = smat.offs_nv;
     smat.offs_eaux_previous  = smat.offs_eaux;
     smat.offs_eauxs_previous = smat.offs_eauxs;
     smat.offs_estv_previous  = smat.offs_estv;
     smat.offs_ndcur_previous = smat.offs_ndcur;

     smat.delete_1(slv);
     if (slv.flag_ssw) {
       smat.ssw_delete_1();
     }
   }

   cout << "GSEIM: Program completed." << endl;

   return 0;
}
