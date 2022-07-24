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

#ifndef EBE_LIB_H
#define EBE_LIB_H

#include "utils.h"

using namespace std;

class EbeLib {

  public:
   int n_iprm;
   int n_sprm;
   int n_rprm;
   int n_stprm;
   int n_igprm;
   int n_outprm;
   int n_nd;
   int n_aux;
   int n_auxs;
   int n_stv;
   int n_xvr;

   bool flag_lmttstep;
   bool flag_lmtnewt;
   bool flag_savehist;
   bool flag_jac_const;
   bool flag_gmin_step;
   bool flag_x_inputs;
   bool flag_x_outputs;
   bool flag_allow_ssw;

   std::string name;
   bool flag_ground;

   vector<std::string> iprm_name;
   vector<std::string> sprm_name;
   vector<std::string> rprm_name;
   vector<std::string> stprm_name;
   vector<std::string> igprm_name;
   vector<std::string> outprm_name;

   vector<std::string> nd_name;
   vector<std::string> aux_name;
   vector<std::string> auxs_name;
   vector<std::string> xvr_name;
   vector<std::string> stv_name;

   vector<int> iprm;
   vector<std::string> sprm;
   vector<double> rprm;
   vector<double> stprm;
   vector<double> igprm;

   int n_f,n_g,n_h;

   vector<int> n_fvar;
   vector<bool> f_ddt;
   vector<int> f_ddt_var_index;
   vector<int> f_ddt_var_flag;
   vector<int> f_ddt_stv_eqn;
   vector<int> f_ddt_stv_index;
   vector< vector<int> > fvar_index;
   vector< vector<int> > fvar_flag;
   vector< vector<bool> > fvar_ddt;

   vector<int> n_gvar;
   vector<int> gvar_stv_index;
   vector< vector<int> > gvar_index;
   vector< vector<int> > gvar_flag;

   vector<int> n_hvar;
   vector< vector<int> > hvar_index;
   vector< vector<int> > hvar_flag;

  public:
   EbeLib();

   EbeLib(
     const std::string &filename,
     Global &global);

   void write_element(
     std::ofstream &outf);

};
#endif
