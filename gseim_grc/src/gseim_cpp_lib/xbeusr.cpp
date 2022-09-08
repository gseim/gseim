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

#include "xbeusr.h"

XbeUsr::XbeUsr(){}

void XbeUsr::set_values_1(
   const int i_xbeu,
   const vector<std::string> v1,
   const vector<std::string> &xbeu_vr_name,
   const int xbeu_aux0,
   const vector<XbeLib> &xbe_lib,
   Global &global,
   CctFile &cct_file) {

   int i_xbel;
   int n_vr1,n_aux1;
   int pos,pos1,pos2,pos3;
   vector<bool> tick;
   std::string s1,s2,s3,s4;
   bool flag_1,flag_2;
   int n_f1,n_g1,n_gvar1;
   int var_number,var_flag;

// assign default name to xbeu: $$0, $$1, ...
   name = "$$" + to_string(i_xbeu);

   tick.resize(v1.size());
   assign_const_1<bool>(tick,false);
   tick[0] = true;

   s1 = assign_string_3(v1,1,2,pos,"type");
   tick[pos] = true; tick[pos+1] = true;
   i_xbel = find_name<XbeLib>(xbe_lib,s1);
   index_xbel = i_xbel;
   xbel_name = xbe_lib[i_xbel].name;
   n_vr1 = xbe_lib[i_xbel].n_vr;
   vr.resize(n_vr1);

   for (int i=0; i < n_vr1; i++) {
     s2 = xbe_lib[i_xbel].vr_name[i];
     s3 = assign_string_3(v1,1,2,pos,s2);
     tick[pos] = true; tick[pos+1] = true;
     find_word_2(xbeu_vr_name,s3,pos1);
     vr[i] = pos1;
   }

   n_aux1 = xbe_lib[i_xbel].n_aux;
   aux.resize(n_aux1);

   for (int i=0; i < n_aux1; i++) {
     aux[i] = i + xbeu_aux0;
   }

   outprm.resize(xbe_lib[i_xbel].n_outprm);

// copy xbel parameters to xbeu:
   copy_vec_1<int>(xbe_lib[i_xbel].iprm,iprm);
   copy_vec_1<std::string>(xbe_lib[i_xbel].sprm,sprm);
   copy_vec_1<double>(xbe_lib[i_xbel].rprm,rprm);
   copy_vec_1<double>(xbe_lib[i_xbel].stprm,stprm);
   copy_vec_1<double>(xbe_lib[i_xbel].igprm,igprm);

// assign parameters from the input line (of the circuit file):
   assign_parms_int_2(v1,xbe_lib[i_xbel].iprm_name,iprm,tick);
   assign_parms_string_2(v1,xbe_lib[i_xbel].sprm_name,sprm,tick);
   assign_parms_double_2(v1,xbe_lib[i_xbel].rprm_name,rprm,tick);
   assign_parms_double_2(v1,xbe_lib[i_xbel].stprm_name,stprm,tick);
   assign_parms_double_2(v1,xbe_lib[i_xbel].igprm_name,igprm,tick);

   find_word_3(v1,"name",1,2,pos2,flag_1);
   if (flag_1) {
     tick[pos2] = true; tick[pos2+1] = true;
     s4 = v1[pos2+1];
     name = s4;

     for (int i=0; i < cct_file.n_ov; i++) {
       if (cct_file.ovr_name[i] == s4) {
         find_word_1(xbe_lib[i_xbel].outprm_name,
           cct_file.ovl_name[i],pos3,flag_2);
         if (flag_2) {
           cct_file.ov1[i] = i_xbeu;
           cct_file.ov2[i] = pos3;
           cct_file.ov_flag[i] = global.I_OV_XBE;
         }
       }
     }
   }

// check if all words of the input lines have been ticked:
   if (!check_vec_const_1<bool>(tick,true)) {
     cout << "XbeUsr::set_values_1: all words not ticked." << endl;
     cout << "  Input line:" << endl;
     print_vec_2<std::string>(v1);
     cout << "  Halting..." << endl; exit(1);
   }

   next_break = 0.0;

   val_vr.resize(n_vr1);
   val_vr_new.resize(n_vr1);
   val_vr_0.resize(n_vr1);
   val_vr_u.resize(n_vr1);

   n_aux1 = xbe_lib[i_xbel].n_aux;

   val_aux.resize(n_aux1);
   val_aux_new.resize(n_aux1);
   val_aux_0.resize(n_aux1);
   val_aux_u.resize(n_aux1);

// function vectors:

   n_f1 = xbe_lib[i_xbel].n_f;
   n_g1 = xbe_lib[i_xbel].n_g;

   f.resize(n_f1);
   g.resize(n_g1);
   h.resize(n_f1);

   g_old_1.resize(n_g1);
   g_old_2.resize(n_g1);

   f0.resize(n_f1);
   f1.resize(n_f1);
   f2.resize(n_f1);
   f3.resize(n_f1);
   f4.resize(n_f1);
   f5.resize(n_f1);

// assign fvar[]
// there are two possibilities: fvar is an XVR or XAUX

   fvar.resize(n_f1);

   for (int i_f=0; i_f < n_f1; i_f++) {
     var_number = xbe_lib[i_xbel].ddt_varnumber[i_f];
     var_flag = xbe_lib[i_xbel].ddt_varflag[i_f];

     if (var_flag == global.I_XVR) {
       fvar[i_f] = vr[var_number];
     } else if (var_flag == global.I_XAUX) {
       fvar[i_f] = aux[var_number];
     }
   }

// assign gvar[][]
// there are two possibilities: gvar is an XVR or XAUX

   gvar.resize(n_g1);
   for (int i=0; i < n_g1; i++) {
     gvar[i].resize(xbe_lib[i_xbel].n_gvar[i]);
   }

   for (int i_g=0; i_g < n_g1; i_g++) {
     n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
     for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
       var_number = xbe_lib[i_xbel].gvar_index[i_g][i_gvar];
       var_flag = xbe_lib[i_xbel].gvar_flag[i_g][i_gvar];

       if (var_flag == global.I_XVR) {
         gvar[i_g][i_gvar] = vr[var_number];
       } else if (var_flag == global.I_XAUX) {
         gvar[i_g][i_gvar] = aux[var_number];
       }
     }
   }

   return;
}
