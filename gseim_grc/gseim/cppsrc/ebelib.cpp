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

#include "ebelib.h"

EbeLib::EbeLib(){}

EbeLib::EbeLib(
   const std::string &filename,
   Global &global) {

   std::fstream inf;
   vector<std::string> v1; 
   int j1,pos,pos1,pos2,pos3;
   int i_stv;
   bool flag_1,flag_2,flag_3,flag_4,flag_5;
   std::string s1,s2,s3;
   int n_fvar1,n_gvar1,n_hvar1,var_number,var_flag;
   std::string homedir;
   std::string filename1;

   filename1.assign(filename);
   homedir = ".";
   replace_first(filename1,"~",homedir);

   inf.open(filename1,ios::in|ios::binary);
   if (!inf.is_open()) {
     cout << "EbeLib: " << filename1 << " could not be opened. Halting..." << endl;
     exit(1);
   }

   next_line(inf,v1);
   check_word_1(v1,0,"ebe");
   name = assign_string_2(v1,1,"name");
   flag_ground = (name == "ground");

   flag_lmttstep  = assign_bool_3(v1,1,2,"limit_tstep" ,false);
   flag_lmtnewt   = assign_bool_3(v1,1,2,"limit_newton",false);
   flag_savehist  = assign_bool_3(v1,1,2,"save_history",false);
   flag_gmin_step = assign_bool_3(v1,1,2,"gmin_step"   ,false);
   flag_x_inputs  = assign_bool_3(v1,1,2,"x_inputs"    ,false);
   flag_x_outputs = assign_bool_3(v1,1,2,"x_outputs"   ,false);
   flag_allow_ssw = assign_bool_3(v1,1,2,"allow_ssw"   ,true );

   if (flag_x_inputs && flag_x_outputs) {
     cout << "EbeLib: flag_x_inputs and flag_x_outputs both true?" << endl;
     cout << "  Check " << name << ".ebe. Halting..." << endl;
     exit(1);
   }

   next_line(inf,v1);
   check_word_1(v1,0,"Jacobian:");
   assign_bool_4(v1,flag_jac_const,1,"constant","variable",true,false);

   assign_names_1(inf,"nodes:",nd_name,n_nd);
   assign_names_1(inf,"state_vars:",stv_name,n_stv);
   assign_names_1(inf,"aux_vars:",aux_name,n_aux);
   assign_names_1(inf,"aux_vars_startup:",auxs_name,n_auxs);
   assign_names_1(inf,"x_vars:",xvr_name,n_xvr);

   if (n_xvr > 0) {
     if (!flag_x_inputs) {
       if (!flag_x_outputs) {
         cout << "EbeLib: flag_x_inputs and flag_x_outputs both false?" << endl;
         cout << "  Check " << name << ".ebe. Halting..." << endl;
         exit(1);
       }
     }
   }

   assign_names_values_int_1(inf,"iparms:",iprm_name,iprm,n_iprm);
   assign_names_values_string_1(inf,"sparms:",sprm_name,sprm,n_sprm);
   assign_names_values_double_1(inf,"rparms:",rprm_name,rprm,n_rprm);
   assign_names_values_double_1(inf,"stparms:",stprm_name,stprm,n_stprm);
   assign_names_values_double_1(inf,"igparms:",igprm_name,igprm,n_igprm);

   assign_names_1(inf,"outparms:",outprm_name,n_outprm);

// functions:

   n_f = assign_int_2a(inf,0,"n_f");
   if (n_f < n_nd) {
     cout << "n_f < n_nodes for " << name << ".ebe. Halting..." << endl;
     exit(1);
   }
   fvar_index.resize(n_f);
   fvar_flag.resize(n_f);
   fvar_ddt.resize(n_f);
   n_fvar.resize(n_f);
   f_ddt.resize(n_f);
   f_ddt_var_index.resize(n_f);
   f_ddt_var_flag.resize(n_f);
   f_ddt_stv_eqn.resize(n_f);
   f_ddt_stv_index.resize(n_f);

   assign_const_1<bool>(f_ddt,false);
   assign_const_1<int>(f_ddt_var_index,-1);
   assign_const_1<int>(f_ddt_var_flag,-1);
   assign_const_1<int>(f_ddt_stv_eqn,-1);
   assign_const_1<int>(f_ddt_stv_index,-1);

// f_x: d_dt(xx) xx xx

   for (int i_f=0; i_f < n_f; i_f++) {
     next_line(inf,v1);
     check_word_2(v1,0,"f_",i_f+1);
     n_fvar1 = v1.size()-1;
     n_fvar[i_f] = n_fvar1;
     fvar_index[i_f].resize(n_fvar1);
     fvar_flag[i_f].resize(n_fvar1);
     fvar_ddt[i_f].resize(n_fvar1);
     assign_const_1<bool>(fvar_ddt[i_f],false);

     if (i_f < n_nd) {

       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         j1 = i_fvar + 1;
         if (check_word_4(v1[j1],"d_dt")) {
           fvar_ddt[i_f][i_fvar] = true;
           s1 = extract_string_1(v1[j1],'(',')');
           find_word_1(stv_name,s1,pos,flag_1);
           if (flag_1) {
             var_flag = global.I_ESTV;
             var_number = pos;
           } else {
             cout << "EbeLib: " << s1 << " is not in the state_vars list." << endl;
             cout << "   Check this element: " << name << ".ebe (f_xx)" << endl;
             cout << "   Halting..." << endl; exit(1);
           }
           f_ddt[i_f] = true;
           f_ddt_var_index[i_f] = var_number;
           f_ddt_var_flag[i_f] = var_flag;
         } else {
//         It is not d_dt(xx); look for v(xx):
           extract_string_3(v1[j1],s2,s3,flag_2,'(',')');
           if (flag_2) {
             if (s2 == "v") {
               find_word_1(nd_name,s3,pos1,flag_3);
               if (flag_3) {
                 var_flag = global.I_NV;
                 var_number = pos1;
               } else {
                 cout << "EbeLib: <" << v1[j1] << "> is not of the form v(xx)." << endl;
                 cout << "   (<" << s3 << "> is not in the node list.)" << endl;
                 cout << "   Check this element: " << name << ".ebe (f_xx)" << endl;
                 cout << "   Halting..." << endl; exit(1);
               }
             } else {
               cout << "EbeLib: <" << v1[j1] << "> is not of the form v(xx)." << endl;
               cout << "   Check this element: " << name << ".ebe (f_xx)" << endl;
               cout << "   Halting..." << endl; exit(1);
             }
           } else {
//           It is not d_dt(xx) or v(xx); look for eaux/xvr:
             find_word_1(aux_name,v1[j1],pos2,flag_4);
             if (flag_4) {
               var_flag = global.I_EAUX;
               var_number = pos2;
             } else {
               find_word_1(xvr_name,v1[j1],pos3,flag_5);
               if (flag_5) {
                 var_flag = global.I_XVR;
                 var_number = pos3;
               } else {
                 cout << "EbeLib: <" << v1[j1] << "> is not eaux/xvr." << endl;
                 cout << "   Check this element: " << name << ".ebe (f_xx)" << endl;
                 cout << "   Halting..." << endl; exit(1);
               }
             }
           }
         }
         fvar_index[i_f][i_fvar] = var_number;
         fvar_flag[i_f][i_fvar] = var_flag;
       }
     } else {
//     non-KCL equation:
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         j1 = i_fvar + 1;
         if (check_word_4(v1[j1],"d_dt")) {
           fvar_ddt[i_f][i_fvar] = true;
           s1 = extract_string_1(v1[j1],'(',')');
         } else {
           s1 = v1[j1];
         }
         if (find_word_4(aux_name,s1,pos)) {
           var_flag = global.I_EAUX;
           var_number = pos;
         } else if (find_word_4(xvr_name,s1,pos)) {
           var_flag = global.I_XVR;
           var_number = pos;
         } else {
           extract_string_3(s1,s2,s3,flag_2,'(',')');
           if (flag_2) {
             if (s2 == "v") {
               find_word_1(nd_name,s3,pos1,flag_3);
               if (flag_3) {
                 var_flag = global.I_NV;
                 var_number = pos1;
               } else {
                 cout << "EbeLib: <" << s1 << "> is not of the form v(xx)." << endl;
                 cout << "   (<" << s3 << "> is not in the node list.)" << endl;
                 cout << "   Check this element: " << name << ".ebe (f_xx)" << endl;
                 cout << "   Halting..." << endl; exit(1);
               }
             } else {
               cout << "EbeLib: <" << s1 << "> is not of the form v(xx)." << endl;
               cout << "   Check this element: " << name << ".ebe (f_xx)" << endl;
               cout << "   Halting..." << endl; exit(1);
             }
           } else {
             cout << "EbeLib: <" << s1 << "> must be auxvar/xvr/v(xx)." << endl;
             cout << "   Check this element: " << name << ".ebe (f_xx)" << endl;
             cout << "   Halting..." << endl; exit(1);
           }
         }
         if (check_word_4(v1[j1],"d_dt")) {
           if (var_flag == global.I_NV) {
             cout << "EbeLib: d_dt term cannot have a node voltage." << endl;
             cout << "   Check this element: " << name << ".ebe (f_xx)" << endl;
             cout << "   Halting..." << endl; exit(1);
           }
           f_ddt[i_f] = true;
           f_ddt_var_index[i_f] = var_number;
           f_ddt_var_flag[i_f] = var_flag;
         }
         fvar_index[i_f][i_fvar] = var_number;
         fvar_flag[i_f][i_fvar] = var_flag;
       }
     }
   } //end of n_f loop

// stv equations

   n_g = assign_int_2a(inf,0,"n_g");
   if (n_g != n_stv) {
     cout << "EbeLib: n_g != n_stv." << endl;
     cout << "   Check this element: " << name << ".ebe (g_xx)" << endl;
     cout << "   Halting..." << endl; exit(1);
   }
   gvar_index.resize(n_g);
   gvar_flag.resize(n_g);
   n_gvar.resize(n_g);
   gvar_stv_index.resize(n_g);
   assign_const_1<int>(gvar_stv_index,-1);

   for (int i_g=0; i_g < n_g; i_g++) {
     next_line(inf,v1);
     check_word_2(v1,0,"g_",i_g+1);
     n_gvar1 = v1.size()-1;
     n_gvar[i_g] = n_gvar1;
     gvar_index[i_g].resize(n_gvar1);
     gvar_flag[i_g].resize(n_gvar1);

     s1 = v1[1];
     find_word_1(stv_name,s1,i_stv,flag_1);
     if (flag_1) {
       gvar_stv_index[i_g] = i_stv;
       gvar_index[i_g][0] = i_stv;
       gvar_flag[i_g][0] = global.I_ESTV;

       for (int i_f=0; i_f < n_nd; i_f++) {
         if (f_ddt[i_f]) {
           if (f_ddt_var_flag[i_f] == global.I_ESTV) {
             if (f_ddt_var_index[i_f] == i_stv) {
               f_ddt_stv_eqn[i_f] = i_g;
               f_ddt_stv_index[i_f] = i_stv;
               break;
             }
           }
         }
       }
     } else {
       cout << "EbeLib: The first term of stv eq. " << i_g
            << " is expected to be a state var." << endl;
       cout << "   Check this element: " << name << ".ebe (g_xx)" << endl;
       cout << "   Halting..." << endl; exit(1);
     }

     for (int i_gvar = 1; i_gvar < n_gvar1; i_gvar++) {
       j1 = i_gvar + 1;
       s1 = v1[j1];
       if (find_word_4(aux_name,s1,pos)) {
         var_flag = global.I_EAUX;
         var_number = pos;
       } else if (find_word_4(xvr_name,s1,pos)) {
         var_flag = global.I_XVR;
         var_number = pos;
       } else {
         extract_string_3(s1,s2,s3,flag_2,'(',')');
         if (flag_2) {
           if (s2 == "v") {
             find_word_1(nd_name,s3,pos1,flag_3);
             if (flag_3) {
               var_flag = global.I_NV;
               var_number = pos1;
             } else {
               cout << "EbeLib: <" << s1 << "> is not of the form v(xx)." << endl;
               cout << "   (<" << s3 << "> is not in the node list.)" << endl;
               cout << "   Check this element: " << name << ".ebe (g_xx)" << endl;
               cout << "   Halting..." << endl; exit(1);
             }
           } else {
             cout << "EbeLib: <" << s1 << "> is not of the form v(xx)." << endl;
             cout << "   Check this element: " << name << ".ebe (g_xx)" << endl;
             cout << "   Halting..." << endl; exit(1);
           }
         } else {
           cout << "EbeLib: <" << s1 << "> must be auxvar/xvr/v(xx)." << endl;
           cout << "   Check this element: " << name << ".ebe (g_xx)" << endl;
           cout << "   Halting..." << endl; exit(1);
         }
       }
       gvar_index[i_g][i_gvar] = var_number;
       gvar_flag[i_g][i_gvar] = var_flag;
     }
   } //end of n_g loop

   for (int i_f=0; i_f < n_nd; i_f++) {
     if (f_ddt[i_f]) {
       if (f_ddt_stv_eqn[i_f] == -1) {
         cout << "EbeLib: a state var equation for KCL no. " << i_f << endl;
         cout << "   was not found." << endl;
         cout << "   Check this element: " << name << ".ebe (f_xx)" << endl;
         cout << "   Halting..." << endl; exit(1);
       }
     }
   }

// startup equations
// h_x: xx xx (Note: d_dt is not allowed)

   n_h = assign_int_2a(inf,0,"n_h");

   hvar_index.resize(n_h);
   hvar_flag.resize(n_h);
   n_hvar.resize(n_h);

// read h data:

   for (int i_h=0; i_h < n_h; i_h++) {
     next_line(inf,v1);
     check_word_2(v1,0,"h_",i_h+1);
     n_hvar1 = v1.size()-1;
     n_hvar[i_h] = n_hvar1;
     hvar_index[i_h].resize(n_hvar1);
     hvar_flag[i_h].resize(n_hvar1);

     for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
       j1 = i_hvar + 1;
       s1 = v1[j1];
       if (find_word_4(auxs_name,s1,pos)) {
         var_flag = global.I_EAUXS;
         var_number = pos;
       } else if (find_word_4(xvr_name,s1,pos)) {
         var_flag = global.I_XVR;
         var_number = pos;
       } else {
         extract_string_3(s1,s2,s3,flag_2,'(',')');
         if (flag_2) {
           if (s2 == "v") {
             find_word_1(nd_name,s3,pos1,flag_3);
             if (flag_3) {
               var_flag = global.I_NV;
               var_number = pos1;
             } else {
               cout << "EbeLib: <" << s1 << "> is not of the form v(xx)." << endl;
               cout << "   (<" << s3 << "> is not in the node list.)" << endl;
               cout << "   Check this element: " << name << ".ebe (h_xx)" << endl;
               cout << "   Halting..." << endl; exit(1);
             }
           } else {
             cout << "EbeLib: <" << s1 << "> is not of the form v(xx)." << endl;
             cout << "   Check this element: " << name << ".ebe (h_xx)" << endl;
             cout << "   Halting..." << endl; exit(1);
           }
         } else {
           cout << "EbeLib: <" << s1 << "> must be auxs/xvr/v(xx)." << endl;
           cout << "   Check this element: " << name << ".ebe (h_xx)" << endl;
           cout << "   Halting..." << endl; exit(1);
         }
       }
       hvar_index[i_h][i_hvar] = var_number;
       hvar_flag[i_h][i_hvar] = var_flag;
     }
   } //end of n_h loop

   inf.close();
   return;
} //end of EbeLib::EbeLib
