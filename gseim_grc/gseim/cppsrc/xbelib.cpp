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

#include "xbelib.h"

XbeLib::XbeLib(){}

XbeLib::XbeLib(
   const std::string &filename,
   Global &global) {

// filename contains information about a single xbe.

   std::fstream inf;
   bool flag_1,flag_2;
   vector<std::string> v1; 
   vector<int> x1,x2; 
   int pos,count1;
   std::string s1;
   int j1,n_gvar1,var_number,var_flag;
   std::string homedir;
   std::string filename1;

   filename1.assign(filename);
   homedir = ".";
   replace_first(filename1,"~",homedir);

   inf.open(filename1,ios::in|ios::binary);
   if (!inf.is_open()) {
     cout << "XbeLib: " << filename1 << " could not be opened. Halting..." << endl;
     exit(1);
   }

   next_line(inf,v1);
   check_word_1(v1,0,"xbe");
   name = assign_string_2(v1,1,"name");

   flag_evaluate  = assign_bool_3(v1,1,2,"evaluate" ,false);
   flag_integrate = assign_bool_3(v1,1,2,"integrate",false);
   flag_delay     = assign_bool_3(v1,1,2,"delay"    ,false);

// check that only one of evaluate/integrate/delay is true:
   count1 = 0;
   if (flag_evaluate ) count1++;
   if (flag_integrate) count1++;
   if (flag_delay    ) count1++;

   if (count1 != 1) {
     cout << "XbeLib: XbeLib type must be one of "
       << "integrate/evaluate/delay" << endl;
     cout << "  Check this element: " << name << ".xbe" << endl;
     cout << "  Halting..." << endl;
     exit (1);
   }

   flag_lmttstep   = assign_bool_3(v1,1,2,"limit_tstep"       ,false);
   flag_lmtnewt    = assign_bool_3(v1,1,2,"limit_newton"      ,false);
   flag_savehist   = assign_bool_3(v1,1,2,"save_history"      ,false);
   flag_modulo     = assign_bool_3(v1,1,2,"limit_modulo"      ,false);
   flag_setrparm   = assign_bool_3(v1,1,2,"set_rparm_1"       ,false);
   flag_getrparm   = assign_bool_3(v1,1,2,"get_rparm_1"       ,false);
   flag_reset      = assign_bool_3(v1,1,2,"reset"             ,false);
   flag_allow_ssw  = assign_bool_3(v1,1,2,"allow_ssw"         ,true );
   flag_time_parms = assign_bool_3(v1,1,2,"compute_time_parms",false);

   next_line(inf,v1);
   check_word_1(v1,0,"Jacobian:");
   assign_bool_4(v1,flag_jac_const,1,"constant","variable",true,false);

   assign_names_1(inf,"input_vars:",vr_name,n_ipvr);
   flag_source = (vr_name.size() == 0);

   next_line(inf,v1);
   check_word_1(v1,0,"output_vars:");
   assign_vec_string_1(v1,1,1,vr_name);
   n_vr = vr_name.size();
   n_opvr = n_vr - n_ipvr;

   assign_names_1(inf,"aux_vars:",aux_name,n_aux);

   assign_names_values_int_1(inf,"iparms:",iprm_name,iprm,n_iprm);
   assign_names_values_string_1(inf,"sparms:",sprm_name,sprm,n_sprm);
   assign_names_values_double_1(inf,"rparms:",rprm_name,rprm,n_rprm);
   assign_names_values_double_1(inf,"stparms:",stprm_name,stprm,n_stprm);
   assign_names_values_double_1(inf,"igparms:",igprm_name,igprm,n_igprm);

   assign_names_1(inf,"outparms:",outprm_name,n_outprm);

   next_line(inf,v1);
   n_f = assign_int_2(v1,0,"n_f");
   if ((flag_evaluate) || (flag_delay)) {
     if (n_f != 0) {
       cout << "XbeLib: n_f must be 0 for evaluate type elements.." << endl;
       cout << "  Check this element: " << name << ".xbe" << endl;
       cout << "  Halting..." << endl; exit(1);
     }
   }

   for (int i=0; i < n_f; i++) {
//   f_x: d_dt(xx) or
//   f_x:
     next_line(inf,v1);
     check_word_2(v1,0,"f_",i+1);
     if (v1.size() > 1) {
       s1 = extract_string_1(v1[1],'(',')');
       find_word_1(vr_name,s1,pos,flag_1);
       if (flag_1) {
         var_flag = global.I_XVR;
         var_number = pos;
       } else {
         find_word_1(aux_name,s1,pos,flag_2);
         if (flag_2) {
           var_flag = global.I_XAUX;
           var_number = pos;
         } else {
           cout << "XbeLib: " << s1 << " is not in the vr/auxvr list." << endl;
           cout << "   Check this element: " << name << ".xbe" << endl;
           cout << "   Halting..." << endl; exit(1);
         }
       }
     } else {
       var_flag = -1;
       var_number = 0;
     }
     ddt_varflag.push_back(var_flag);
     ddt_varnumber.push_back(var_number);
   }

// g_x: xx xx (Note: d_dt is not allowed)
// For integrator type elements, d_dt information is already
// availabe in f_xx and sis therefore not required to be supplied
// in g_xx.

   n_g = assign_int_2a(inf,0,"n_g");
   if (flag_integrate) {
     if (n_g != n_f) {
       cout << "XbeLib: n_g != n_f." << endl;
       cout << "   Check this element: " << name << ".xbe (g_xx)" << endl;
       cout << "   Halting..." << endl; exit(1);
     }
   }
   gvar_index.resize(n_g);
   gvar_flag.resize(n_g);
   gvar_count.resize(n_g);
   n_gvar.resize(n_g);

   for (int i=0; i < n_g; i++) {
     next_line(inf,v1);
     check_word_2(v1,0,"g_",i+1);
     n_gvar1 = v1.size()-1;
     n_gvar[i] = n_gvar1;
     gvar_index[i].resize(n_gvar1);
     gvar_flag[i].resize(n_gvar1);
     gvar_count[i].resize(n_gvar1);

     for (int j=0; j < n_gvar1; j++) {
       j1 = j + 1;
       s1 = v1[j1];
       if (find_word_4(vr_name,s1,pos)) {
         var_flag = global.I_XVR;
         var_number = pos;
       } else if (find_word_4(aux_name,s1,pos)) {
         var_flag = global.I_XAUX;
         var_number = pos;
       } else {
         cout << "XbeLib: <" << s1 << "> must be auxvar/xvr." << endl;
         cout << "   Check this element: " << name << ".xbe (g_xx)" << endl;
         cout << "   Halting..." << endl; exit(1);
       }
       gvar_index[i][j] = var_number;
       gvar_flag[i][j] = var_flag;

       gvar_count[i][j] = 1;

       if (flag_integrate) {
         if (var_flag == ddt_varflag[i]) {
           if (var_number == ddt_varnumber[i]) {
             gvar_count[i][j] = 2;
           }
         }
       }
     }
   } //end of n_g loop

// start-up:

   if (flag_integrate) {
     n_h = n_f;
   } else {
     n_h = 0;
   }

   inf.close();
   return;
} //end of XbeLib::XbeLib
