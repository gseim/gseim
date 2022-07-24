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

#include "ebeusr.h"

EbeUsr::EbeUsr(){}

void EbeUsr::set_values_1(
   const int i_ebeu,
   const vector<std::string> v1,
   const vector<std::string> &ebeu_nd_name,
   const vector<EbeLib> &ebe_lib,
   const vector<std::string> &xbeu_vr_name,
   const int ebeu_aux0,
   const int ebeu_auxs0,
   const int ebeu_stv0,
   Global &global,
   CctFile &cct_file) {

   int i_ebel;
   std::string s1,s2,s3,s4;
   int pos,pos1,pos2,pos3;
   vector<bool> tick;
   int n_nd1,n_aux1,n_auxs1,n_stv1,n_xvr1;
   int n_f1,n_g1,n_h1;
   int var_number,var_flag,n_fvar1,n_gvar1,n_hvar1;
   bool flag_1,flag_2;

// assign default name to ebeu: $$0, $$1, ...
   name = "$$" + to_string(i_ebeu);

   tick.resize(v1.size());
   assign_const_1<bool>(tick,false);
   tick[0] = true;

   s1 = assign_string_3(v1,1,2,pos,"type");
   tick[pos] = true; tick[pos+1] = true;
   i_ebel = find_name<EbeLib>(ebe_lib,s1);
   index_ebel = i_ebel;

   ebel_name = ebe_lib[i_ebel].name;

// assign nd[xx]
   n_nd1 = ebe_lib[i_ebel].n_nd;
   nd.resize(n_nd1);

   for (int i=0; i < n_nd1; i++) {
     s2 = ebe_lib[i_ebel].nd_name[i];
     s3 = assign_string_3(v1,1,2,pos,s2);
     tick[pos] = true; tick[pos+1] = true;
     find_word_2(ebeu_nd_name,s3,pos1);
     nd[i] = pos1;
   }

// assign xvr[xx]
   n_xvr1 = ebe_lib[i_ebel].n_xvr;
   xvr.resize(n_xvr1);

   for (int i=0; i < n_xvr1; i++) {
     s2 = ebe_lib[i_ebel].xvr_name[i];
     s3 = assign_string_3(v1,1,2,pos,s2);
     tick[pos] = true; tick[pos+1] = true;
     find_word_2(xbeu_vr_name,s3,pos1);
     xvr[i] = pos1;
   }

   outprm.resize(ebe_lib[i_ebel].n_outprm);

// copy ebel parameters to ebeu:
   copy_vec_1<int>(ebe_lib[i_ebel].iprm,iprm);
   copy_vec_1<std::string>(ebe_lib[i_ebel].sprm,sprm);
   copy_vec_1<double>(ebe_lib[i_ebel].rprm,rprm);
   copy_vec_1<double>(ebe_lib[i_ebel].stprm,stprm);
   copy_vec_1<double>(ebe_lib[i_ebel].igprm,igprm);

// assign parameters from the input line (of the circuit file):
   assign_parms_int_2(v1,ebe_lib[i_ebel].iprm_name,iprm,tick);
   assign_parms_string_2(v1,ebe_lib[i_ebel].sprm_name,sprm,tick);
   assign_parms_double_2(v1,ebe_lib[i_ebel].rprm_name,rprm,tick);
   assign_parms_double_2(v1,ebe_lib[i_ebel].stprm_name,stprm,tick);
   assign_parms_double_2(v1,ebe_lib[i_ebel].igprm_name,igprm,tick);

   find_word_3(v1,"name",1,2,pos2,flag_1);
   if (flag_1) {
     tick[pos2] = true; tick[pos2+1] = true;
     s4 = v1[pos2+1];
     name = s4;

     for (int i=0; i < cct_file.n_ov; i++) {
       if (cct_file.ovr_name[i] == s4) {
         find_word_1(ebe_lib[i_ebel].outprm_name,
           cct_file.ovl_name[i],pos3,flag_2);
         if (flag_2) {
           cct_file.ov1[i] = i_ebeu;
           cct_file.ov2[i] = pos3;
           cct_file.ov_flag[i] = global.I_OV_EBE;
         }
       }
     }
   }

// check if all words of the input lines have been ticked:
   if (!check_vec_const_1<bool>(tick,true)) {
     cout << "EbeUsr::set_values_1: all words not ticked." << endl;
     cout << "  Input line:" << endl;
     print_vec_2<std::string>(v1);
     cout << "  Halting..." << endl; exit(1);
   }

   next_break = 0.0;

   val_nd.resize(n_nd1);
   val_nd_new.resize(n_nd1);
   dval_nd.resize(n_nd1);

   cur_nd.resize(n_nd1);
   cur_nd_1.resize(n_nd1);
   cur_nd_2.resize(n_nd1);
   cur_nd_old_nr_1.resize(n_nd1);
   norm_spice_cur_nd.resize(n_nd1);
   tol_spice_cur_nd.resize(n_nd1);

   n_aux1 = ebe_lib[i_ebel].n_aux;
   val_aux.resize(n_aux1);
   val_aux_new.resize(n_aux1);

   n_auxs1 = ebe_lib[i_ebel].n_auxs;
   val_auxs.resize(n_auxs1);
   val_auxs_new.resize(n_auxs1);

   n_stv1 = ebe_lib[i_ebel].n_stv;
   val_stv.resize(n_stv1);
   val_stv_1.resize(n_stv1);
   val_stv_2.resize(n_stv1);

   n_xvr1 = ebe_lib[i_ebel].n_xvr;
   val_xvr.resize(n_xvr1);

// function vectors:
// resize fvar,gvar,hvar

   n_f1 = ebe_lib[i_ebel].n_f;
   f.resize(n_f1);
   f_old_1.resize(n_f1);
   f_old_2.resize(n_f1);

   fvar.resize(n_f1);
   for (int i=0; i < n_f1; i++) {
     fvar[i].resize(ebe_lib[i_ebel].n_fvar[i]);
   }

   n_g1 = ebe_lib[i_ebel].n_g;
   g.resize(n_g1);
   g_old_1.resize(n_g1);
   g_old_2.resize(n_g1);

   gvar.resize(n_g1);
   for (int i=0; i < n_g1; i++) {
     gvar[i].resize(ebe_lib[i_ebel].n_gvar[i]);
   }

   n_h1 = ebe_lib[i_ebel].n_h;
   h.resize(n_h1);
   hvar.resize(n_h1);
   for (int i=0; i < n_h1; i++) {
     hvar[i].resize(ebe_lib[i_ebel].n_hvar[i]);
   }

// assign aux[i_aux]
   aux.resize(n_aux1);
   for (int i=0; i < n_aux1; i++) {
     aux[i] = i + ebeu_aux0;
   }
   auxs.resize(n_auxs1);
   for (int i=0; i < n_auxs1; i++) {
     auxs[i] = i + ebeu_auxs0;
   }
   stv.resize(n_stv1);
   for (int i=0; i < n_stv1; i++) {
     stv[i] = i + ebeu_stv0;
   }

// dc/trns equations:
   for (int i_f=0; i_f < n_f1; i_f++) {
     n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];

     for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {

       var_number = ebe_lib[i_ebel].fvar_index[i_f][i_fvar];
       var_flag = ebe_lib[i_ebel].fvar_flag[i_f][i_fvar];

       if (var_flag == global.I_NV) {
         fvar[i_f][i_fvar] = nd[var_number];
       } else if (var_flag == global.I_EAUX) {
         fvar[i_f][i_fvar] = aux[var_number];
       } else if (var_flag == global.I_XVR) {
         fvar[i_f][i_fvar] = xvr[var_number];
       } else if (var_flag == global.I_ESTV) {
         fvar[i_f][i_fvar] = stv[var_number];
       } else {
         cout << "EbeUsr: incorrect value of var_flag in f." << endl;
         cout << "  i_f = " << i_f << ", i_favr = " << i_fvar << endl;
         cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
         exit(1);
       }
     }
   }
// stv equations:
   for (int i_g=0; i_g < n_g1; i_g++) {
     n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];

     for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
       var_number = ebe_lib[i_ebel].gvar_index[i_g][i_gvar];
       var_flag = ebe_lib[i_ebel].gvar_flag[i_g][i_gvar];

       if (var_flag == global.I_NV) {
         gvar[i_g][i_gvar] = nd[var_number];
       } else if (var_flag == global.I_EAUX) {
         gvar[i_g][i_gvar] = aux[var_number];
       } else if (var_flag == global.I_XVR) {
         gvar[i_g][i_gvar] = xvr[var_number];
       } else if (var_flag == global.I_ESTV) {
         gvar[i_g][i_gvar] = stv[var_number];
       } else {
         cout << "EbeUsr: incorrect value of var_flag in g." << endl;
         cout << "  i_g = " << i_g << ", i_gavr = " << i_gvar << endl;
         cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
         exit(1);
       }
     }
   }
// startup equations:
   for (int i_h=0; i_h < n_h1; i_h++) {
     n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];

     for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
       var_number = ebe_lib[i_ebel].hvar_index[i_h][i_hvar];
       var_flag = ebe_lib[i_ebel].hvar_flag[i_h][i_hvar];

       if (var_flag == global.I_NV) {
         hvar[i_h][i_hvar] = nd[var_number];
       } else if (var_flag == global.I_EAUXS) {
         hvar[i_h][i_hvar] = auxs[var_number];
       } else if (var_flag == global.I_XVR) {
         hvar[i_h][i_hvar] = xvr[var_number];
       } else {
         cout << "EbeUsr: incorrect value of var_flag in g." << endl;
         cout << "  i_h = " << i_h << ", i_havr = " << i_hvar << endl;
         cout << "  ebe_lib is " << ebe_lib[i_ebel].name << ". Halting.." << endl;
         exit(1);
       }
     }
   }

   return;
}
