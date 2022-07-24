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

#ifndef EBE_USR_H
#define EBE_USR_H

#include "utils.h"
#include "cctfile.h"
#include "ebelib.h"

using namespace std;

class EbeUsr {

  public:
   int index_ebel;
   std::string name;
   std::string ebel_name;

   vector<int> iprm;
   vector<double> rprm;
   vector<double> stprm;
   vector<double> igprm;
   vector<double> outprm;
   vector<std::string> sprm;

   vector<int> nd;
   vector<int> xvr;
   vector<int> aux;
   vector<int> auxs;
   vector<int> stv;

   vector<double> cur_nd;
   vector<double> cur_nd_1;
   vector<double> cur_nd_2;

   vector<double> cur_nd_old_nr_1;
   vector<double> norm_spice_cur_nd;
   vector<double> tol_spice_cur_nd;

   vector<double> f;
   vector<double> f_old_1;
   vector<double> f_old_2;

   vector<double> g;
   vector<double> g_old_1;
   vector<double> g_old_2;

   vector<double> h;

   vector<double> val_nd;
   vector<double> val_nd_new;
   vector<double> dval_nd;
   vector<double> val_aux;
   vector<double> val_aux_new;
   vector<double> val_auxs;
   vector<double> val_auxs_new;
   vector<double> val_stv,val_stv_1,val_stv_2;
   vector<double> val_xvr;

   vector< vector<int> > fvar;
   vector< vector<int> > gvar;
   vector< vector<int> > hvar;

   double next_break;

  public:

  EbeUsr();

  void set_values_1(
   const int i_ebeu,
   const vector<std::string> v1,
   const vector<std::string> &ebeu_nd_name,
   const vector<EbeLib> &ebe_lib,
   const vector<std::string> &xbeu_vr_name,
   const int ebeu_aux0,
   const int ebeu_auxs0,
   const int ebeu_stv0,
   Global &global,
   CctFile &cct_file);

};
#endif
