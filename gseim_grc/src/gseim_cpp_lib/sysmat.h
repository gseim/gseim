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

#ifndef SYSMAT_H
#define SYSMAT_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <iomanip>
#include <vector>
#include "utils.h"
#include "solveblocks.h"
#include "xbeusr.h"
#include "ebeusr.h"
#include "knuthmat.h"
#include "matgnrl1.h"
#include "matop.h"

using namespace std;

class SysMat {

  public:

   int n_solvec_x;
   int n_solvec_x0;
   int n_rhs_x0;

   int n_solvec_ex,n_solvec_e;
   int n_solvec_e0,n_solvec_e0s;
   int n_rhs_e0;

   int n_solvec_x_previous,n_solvec_ex_previous,n_solvec_e_previous;

   int offs_xvr,offs_xaux;
   int offs_nv,offs_eaux,offs_eauxs;
   int offs_estv,offs_ndcur;

   int offs_xvr_previous,offs_xaux_previous;
   int offs_nv_previous,offs_eaux_previous,offs_eauxs_previous;
   int offs_estv_previous,offs_ndcur_previous;

   int offs_xvr_in,offs_xvr_out;
   int offs_row_xvr_in;

   int n_xvr,n_xaux;
   int n_nv,n_eaux,n_eauxs;

   int nrow_ebce_1;

   vector<int> offs;
   vector<int> svec_flag_x;
   vector<int> svec_flag_ex,svec_flag_e;

   KnuthMat m_x,w_x;
   KnuthMat m_e,w_e;
   KnuthMat m_ex,w_ex;
   KnuthMat m_ssw,w_ssw;

// m_kcl is used for ssw:
   KnuthMat m_kcl;

   MatOp mo_x;
   MatOp mo_e;
   MatOp mo_ex;
   MatOp mo_ssw;

   bool flag_alloc,flag_delete;

   double *delsvec_x,*svec_x,*svec_w_x,*svec_orig_x;
   double *delsvec_e,*svec_e,*svec_w_e,*svec_orig_e;
   double *delsvec_ex,*svec_ex,*svec_w_ex,*svec_orig_ex;

   double *svec_x_previous;
   double *svec_e_previous;
   double *svec_ex_previous;

   double *svec_old_1_x,*svec_old_2_x;
   double *svec_old_1_e,*svec_old_2_e;
   double *svec_old_1_ex,*svec_old_2_ex;

   double *svec_old_nr_1_e,*svec_old_nr_2_e;
   double *svec_old_nr_1_ex,*svec_old_nr_2_ex;

   double *rhs_m_x,*rhs_w_x;
   double *rhs_m_e,*rhs_w_e;
   double *rhs_m_ex,*rhs_w_ex;
   double *rhs_m_ssw,*rhs_w_ssw;

   double* tol_spice_e;
   double* tol_spice_ex;
   double* norm_spice_e;
   double* norm_spice_ex;

   int ***map_gvar_to_xbe;
   int ***map_fvar_to_ebe;
   int ***map_gvar_to_ebe;
   int ***map_hvar_to_ebe;

   int *map_jac_ebe_to_m;

   int ***map_stv_to_ebe;
   int *ebe_stv_index;

// index 1: i_ebeu, 2: i_function
   int **eqn_ebe_be_index;
   int **eqn_ebe_be_eqn;
   int **ebe_f_to_row;
   int **ebe_g_to_row;
   int **ebe_h_to_row;
   int **ebe_f_stv_index;

   int *ddt_ebe_pntr;
   bool *flag_ebe_stv;

   int *ebe_rhs_to_f;
   int *ebe_rhs_to_f1;

   bool *ebe_rhs_ddt_flag;

   int *ebe_rhs_ddt_varnumber;
   int *ebe_rhs_ddt_pntr;
   int *ebe_rhs_ddt_varflag;
   int *ssw_ebe_rhs_flag;
   int *ebe_rhs_ddt_i_ebeu;
   int *ebe_rhs_ddt_i_f;
   int *ebe_rhs_ddt_i_g;

   bool *flag_ebe_ddt;

   int *ebe_nd_to_row;

   int **xbe_f_to_row;
   int **xbe_g_to_row;

   bool *xbe_rhs_ddt_flag;
   int *xbe_rhs_ddt_varnumber;
   int *xbe_rhs_ddt_pntr;
   int *xbe_rhs_ddt_varflag;
   int *xbe_rhs_ddt_i_xbeu;
   int *xbe_rhs_ddt_i_f;

   int *map_jac_xbe_to_m;

   int *eqn_xbe_be_index;
   int *eqn_xbe_be_eqn;
   int *xbe_ddt_varnumber;

   double** ssw_mat;
   double** ssw_mat_1;
   double** ssw_trz_1;

   double* ssw_mat_pntr;
   double* ssw_mat_pntr_1;
   double* ssw_trz_1_pntr;

   int* indxc_ssw;
   int* indxr_ssw;
   int* ipiv_ssw;

   double* svec_full_ssw_old_e;
   double* svec_full_ssw_old_ex;

   double* svec_ssw_1;
   double* svec_ssw_2;
   double* svec_ssw_2_old;
   double* delsvec_ssw_2;
   double* rhs_ssw;
   double* ssw_rhs;

   int* offs_ssw;
   int n_statevar;
   int* ssw_flag1;
   int* ssw_indx1;
   int* ssw_indx2;
   int* ssw_indx3;

  public:

  SysMat();

  void allocate_1(
   Global &global,
   vector<EbeLib> &ebe_lib,
   vector<XbeLib> &xbe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct);

  void set_values_1(
   Global &global,
   SolveBlocks &slv,
   Circuit &cct);

  void mat_dc_1_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file);

  void mat_startup_1_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   CctFile &cct_file);

  void mat_startup_1_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file);

  void mat_startup_1_ex(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file);

  void mat_trns_1_x0(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global,
   CctFile &cct_file);

  void mat_trns_1_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global,
   CctFile &cct_file);

  void mat_trns_1_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file);

  void mat_trns_1_ex(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file);

  void mat_ssw_1_e0(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file);

  void mat_ssw_1_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file);

  void mat_ssw_1_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file);

  void mat_ssw_1_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file);

  void get_offs_new(
   const int var_number,
   int &offs_new,
   int &var_number_new,
   Circuit &cct);

  void ssw_allocate_1(
   vector<EbeLib> &ebe_lib,
   vector<XbeLib> &xbe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct);

  void ssw_allocate_2(
   vector<EbeLib> &ebe_lib,
   vector<XbeLib> &xbe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct);

  void kcl_allocate_1(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct);

  void kcl_mat_2(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file);

  void delete_1(
   SolveBlocks &slv);

  void ssw_delete_1();

  void copy_svec_to_previous();

};
#endif
