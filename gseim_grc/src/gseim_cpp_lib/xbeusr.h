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

#ifndef XBE_USR_H
#define XBE_USR_H

#include "utils.h"
#include "cctfile.h"
#include "xbelib.h"

using namespace std;

class XbeUsr {

  public:
   int index_xbel;
   std::string name;
   std::string xbel_name;

   vector<int> iprm;
   vector<double> rprm;
   vector<double> stprm;
   vector<double> igprm;
   vector<double> outprm;
   vector<std::string> sprm;

   vector<int> vr;
   vector<int> aux;

   vector<int> fvar;
   vector< vector<int> > gvar;

   vector<double> val_vr;
   vector<double> val_aux;

// use these for handling algebraic loops
// (the updated values of variables of integrator-type elements)
   vector<double> val_vr_u;
   vector<double> val_aux_u;

   vector<double> val_vr_new;
   vector<double> val_aux_new;

   vector<double> val_vr_0;
   vector<double> val_aux_0;

   vector<double> f;
   vector<double> g;
   vector<double> h;

   vector<double> g_old_1;
   vector<double> g_old_2;

   vector<double> f0;
   vector<double> f1;
   vector<double> f2;
   vector<double> f3;
   vector<double> f4;
   vector<double> f5;

   double next_break;

   int edge_delay_nmax;
   int edge_delay_nloc;

   vector<double> edge_delay_tchange;

   vector<int> edge_delay_flag;

   int edge_delay_nextloc;

   int edge_delay_top;

   vector<double> vec1,vec2;
   vector<vector<double> > vec2d_1;
   vector<vector<vector<double> > > vec3d_1;

   queue<double> que1,que2;
  public:
   XbeUsr();

  void set_values_1(
   const int i_xbeu,
   const vector<std::string> v1,
   const vector<std::string> &xbeu_vr_name,
   const int xbeu_aux0,
   const vector<XbeLib> &xbe_lib,
   Global &global,
   CctFile &cct_file);

};
#endif
