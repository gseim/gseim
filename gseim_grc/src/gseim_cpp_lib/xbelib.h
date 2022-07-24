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

#ifndef XBE_LIB_H
#define XBE_LIB_H

#include "utils.h"

using namespace std;

class XbeLib {

  public:
   int n_vr;
   int n_ipvr;
   int n_opvr;
   int n_iprm;
   int n_sprm;
   int n_rprm;
   int n_igprm;
   int n_aux;
   int n_stprm;
   int n_outprm;

   bool flag_lmttstep;
   bool flag_lmtnewt;
   bool flag_savehist;
   bool flag_modulo;
   bool flag_setrparm;
   bool flag_getrparm;
   bool flag_evaluate;
   bool flag_integrate;
   bool flag_source;
   bool flag_reset;
   bool flag_delay;
   bool flag_jac_const;
   bool flag_allow_ssw;
   bool flag_time_parms;

   std::string name;

   vector<std::string> iprm_name;
   vector<std::string> sprm_name;
   vector<std::string> rprm_name;
   vector<std::string> igprm_name;
   vector<std::string> stprm_name;
   vector<std::string> outprm_name;

   vector<std::string> vr_name;
   vector<std::string> aux_name;

   vector<int> iprm;
   vector<std::string> sprm;
   vector<double> rprm;
   vector<double> igprm;
   vector<double> stprm;

   int n_f;
   int n_g;
   int n_h;

   vector<int> ddt_varflag;
   vector<int> ddt_varnumber;

   vector<int> n_gvar;
   vector< vector<int> > gvar_index;
   vector< vector<int> > gvar_flag;
   vector< vector<int> > gvar_count;

  public:
   XbeLib();

   XbeLib(
     const std::string &filename,
     Global &global);

   void write_element(
     std::ofstream &outf);

};
#endif
