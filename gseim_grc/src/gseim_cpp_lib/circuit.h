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

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <math.h>

#include "global.h"
#include "cctfile.h"
#include "utils.h"
#include "xbelib.h"
#include "xbeusr.h"
#include "ebelib.h"
#include "ebeusr.h"

using namespace std; 

class Circuit {

public:
   int n_xbeu,n_xbeu_vr;
   int x_n_ttlaux;
   int x_n_ttlg;

   vector<int> xbeu_aux_start;
   vector<string> xbeu_vr_name;

   int nttl_xbeu_modulo;
   vector<int> xbeu_modulo_map;
   vector<int> map_xbeuvr_to_svec;

   vector<int> n_map_vr;
   vector< vector<int> > map_vr_1;
   vector< vector<int> > map_vr_2;

   vector<double> val_xvr;
   vector<double> val_xvr_new;

   int x_n_opvr_ttlfanout;
   int x_n_pass;

   vector<bool> x_vr_visited;
   vector<bool> x_beu_visited;

   vector<int> x_pass;
   vector<int> x_pass_n_beu;

   vector< vector<int> > x_pass_beu;

   int n_xvr_ebe_in;
   vector<bool> flag_xvr_ebe_in;
   vector<int> map1_xvr_ebe_in;
   vector<int> map2_xvr_ebe_in;

   int n_xvr_ebe_out;
   vector<bool> flag_xvr_ebe_out;
   vector<int> map1_xvr_ebe_out;
   vector<int> map2_xvr_ebe_out;

   int x_n_intgrtr,x_n_delay,x_n_eval_src,x_n_eval_nonsrc;
   bool flag_alg_loop;

   int n_ebeu,n_ebeu_nd;
   int e_n_ttlaux,e_n_ttlauxs;
   int e_n_ttlnd,e_n_ttlstv;
   vector<string> ebeu_nd_name;
   vector<int> ebeu_aux_start;
   vector<int> ebeu_auxs_start;
   vector<int> ebeu_stv_start;
   vector<int> ebeu_nd_start;

// circuit node voltage values:
   vector<double> val_nd;
   vector<double> val_nd_new;

   int ref_nd;

   bool flag_save_history;
   bool flag_save_history_x;
   bool flag_save_history_e;

   bool flag_reset_x;
   bool flag_modulo_x;

   bool flag_limit_tstep  ;
   bool flag_limit_tstep_x;
   bool flag_limit_tstep_e;

   bool flag_limit_newton_x;
   bool flag_limit_newton_e;

   bool flag_x,flag_x_only;
   bool flag_x_explicit;
   bool flag_x_matrix;
   bool flag_e,flag_x_e,flag_e_only;
   bool flag_exc,flag_exs,flag_exs_fex,flag_exs_fe,flag_exs_fx;
   bool flag_time_parms;

   bool flag_linear_x;
   bool flag_linear_e,flag_linear_ex;

   int n_samplers;

public:
  Circuit();

  Circuit(
   const std::string &filename,
   const vector<XbeLib> &xbe_lib,
   const vector<EbeLib> &ebe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   CctFile &cct_file);

  void check_save_history(
   const vector<XbeLib> &xbe_lib,
   const vector<EbeLib> &ebe_lib,
   const vector<XbeUsr> &xbe_usr,
   const vector<EbeUsr> &ebe_usr);

  void check_reset_x(
   const vector<XbeLib> &xbe_lib,
   const vector<XbeUsr> &xbe_usr);

  void check_modulo_x(
   const vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr);

  void check_limit_tstep(
   const vector<XbeLib> &xbe_lib,
   const vector<EbeLib> &ebe_lib,
   const vector<XbeUsr> &xbe_usr,
   const vector<EbeUsr> &ebe_usr);

  void check_limit_newton(
   const vector<XbeLib> &xbe_lib,
   const vector<EbeLib> &ebe_lib,
   const vector<XbeUsr> &xbe_usr,
   const vector<EbeUsr> &ebe_usr);

  void check_time_parms(
   const vector<XbeLib> &xbe_lib,
   const vector<XbeUsr> &xbe_usr);

  void check_sampler_index(
   const vector<XbeLib> &xbe_lib,
   const vector<XbeUsr> &xbe_usr,
   Global &global);

  void xbe_map_vr(
   const vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr);

  void process_xbeu_1(
   const vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr);

  void assign_flag_linear_x(
   const vector<XbeLib> &xbe_lib,
   const vector<XbeUsr> &xbe_usr);

  void assign_flag_linear_e(
   const vector<EbeLib> &ebe_lib,
   const vector<EbeUsr> &ebe_usr);

  void assign_xvr_ebe_1(
   const vector<EbeLib> &ebe_lib,
   const vector<EbeUsr> &ebe_usr);

  void set_flags_default();

};
#endif
