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

#include "sysmat.h"

SysMat::SysMat() {
   flag_alloc = true; flag_delete = false;
   return;
}
// -----------------------------------------------------------------------------
void SysMat::allocate_1(
   Global &global,
   vector<EbeLib> &ebe_lib,
   vector<XbeLib> &xbe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct) {

// allocate variables whose size will not change (so they will not need to
// be deleted)

   int i_ebeu,i_ebel;
   int n,i_xbeu,i_xbel;
   int n_f1,n_g1,n_h1;
   int n_fvar1,n_gvar1,n_hvar1;
   int n_row1,n_nd1;
   int i_f,i_g,i_h;

   n = max(global.I_NV,global.I_EAUX);
   n = max(n,global.I_XVR);
   n = max(n,global.I_XAUX);
   n = max(n,global.I_EAUXS);
   n = max(n,global.I_ESTV);
   n = max(n,global.I_NDCUR);

   offs.resize(n+1);

   ebe_nd_to_row = new int[cct.n_ebeu_nd];

   map_fvar_to_ebe = new int**[cct.n_ebeu];
   map_stv_to_ebe  = new int**[cct.n_ebeu];

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     map_fvar_to_ebe[i_ebeu] = new int*[n_f1];
     map_stv_to_ebe [i_ebeu] = new int*[n_f1];

     for (i_f=0; i_f < n_f1; i_f++) {
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       map_fvar_to_ebe[i_ebeu][i_f] = new int[n_fvar1];
       map_stv_to_ebe [i_ebeu][i_f] = new int[n_fvar1];
     }
   }

   map_gvar_to_ebe = new int**[cct.n_ebeu];
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_g1 = ebe_lib[i_ebel].n_g;
     map_gvar_to_ebe[i_ebeu] = new int*[n_g1];

     for (i_g=0; i_g < n_g1; i_g++) {
       n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];
       map_gvar_to_ebe[i_ebeu][i_g] = new int[n_gvar1];
     }
   }

   map_hvar_to_ebe = new int**[cct.n_ebeu];
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_h1 = ebe_lib[i_ebel].n_h;
     map_hvar_to_ebe[i_ebeu] = new int*[n_h1];

     for (i_h=0; i_h < n_h1; i_h++) {
       n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
       map_hvar_to_ebe[i_ebeu][i_h] = new int[n_hvar1];
     }
   }

   ebe_f_to_row    = new int*[cct.n_ebeu];
   ebe_f_stv_index = new int*[cct.n_ebeu];
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     ebe_f_to_row   [i_ebeu] = new int[n_f1];
     ebe_f_stv_index[i_ebeu] = new int[n_f1];
   }

   ebe_g_to_row = new int*[cct.n_ebeu];
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_g1 = ebe_lib[i_ebel].n_g;
     ebe_g_to_row[i_ebeu] = new int[n_g1];
   }

   ebe_h_to_row = new int*[cct.n_ebeu];
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_h1 = ebe_lib[i_ebel].n_h;
     ebe_h_to_row[i_ebeu] = new int[n_h1];
   }

   map_gvar_to_xbe = new int**[cct.n_xbeu];
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_g1 = xbe_lib[i_xbel].n_g;
     map_gvar_to_xbe[i_xbeu] = new int*[n_g1];

     for (i_g=0; i_g < n_g1; i_g++) {
       n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
       map_gvar_to_xbe[i_xbeu][i_g] = new int[n_gvar1];
     }
   }

   xbe_f_to_row = new int*[cct.n_xbeu];
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_f1 = xbe_lib[i_xbel].n_f;
     xbe_f_to_row[i_xbeu] = new int[n_f1];
   }

   xbe_g_to_row = new int*[cct.n_xbeu];
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_g1 = xbe_lib[i_xbel].n_g;
     xbe_g_to_row[i_xbeu] = new int[n_g1];
   }

   n_row1 = cct.e_n_ttlnd + cct.e_n_ttlstv;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (i_f=n_nd1; i_f < n_f1; i_f++) n_row1++;
   }

   nrow_ebce_1 = n_row1;

   map_jac_ebe_to_m = NULL;

   rhs_m_e = NULL;
   rhs_w_e = NULL;
   rhs_m_x = NULL;
   rhs_w_x = NULL;
   rhs_m_ex = NULL;
   rhs_w_ex = NULL;
   rhs_m_ssw = NULL;
   rhs_w_ssw = NULL;

   svec_e_previous  = NULL;
   svec_x_previous  = NULL;
   svec_ex_previous = NULL;

   return;
} // end of SysMat::allocate_1
// -----------------------------------------------------------------------------
void SysMat::set_values_1(
   Global &global,
   SolveBlocks &slv,
   Circuit &cct) {

   offs_nv    = 0;
   offs_eaux  = 0;
   offs_xvr   = 0;
   offs_xaux  = 0;
   offs_eauxs = 0;

   offs_estv  = 0;
   offs_ndcur = 0;

   offs_xvr_in  = 0;
   offs_xvr_out = 0;

   n_solvec_ex = 0;
   n_solvec_e  = 0;
   n_solvec_x  = 0;

   n_rhs_e0 = 0;
   n_rhs_x0 = 0;

   n_solvec_e0  = max(0,cct.n_ebeu_nd - 1 + cct.e_n_ttlaux);
   n_solvec_e0s = cct.n_ebeu_nd - 1 + cct.e_n_ttlauxs;
   n_solvec_x0  = cct.n_xbeu_vr + cct.x_n_ttlaux;

   svec_flag_ex.clear();
   svec_flag_e.clear();
   svec_flag_x.clear();

   n_nv = cct.n_ebeu_nd - 1;
   n_eaux = cct.e_n_ttlaux;
   n_eauxs = cct.e_n_ttlauxs;
   n_xvr = cct.n_xbeu_vr;
   n_xaux = cct.x_n_ttlaux;

   if (slv.flag_ssw) {
//   For SSW, we will allow only implicit schemes and only exc if
//   there are both e and x elements.

     if (cct.flag_x_only) {
       n_solvec_x = n_solvec_x0;
       n_rhs_x0 = n_solvec_x0;
     } else if (cct.flag_e_only) {
       n_solvec_e = cct.e_n_ttlnd + cct.n_ebeu_nd - 1 + cct.e_n_ttlaux + cct.e_n_ttlstv;
       n_rhs_e0 = n_solvec_e;
     } else {
       n_solvec_ex = cct.e_n_ttlnd + cct.n_ebeu_nd - 1 + cct.e_n_ttlaux + cct.e_n_ttlstv
          + n_solvec_x0;
       n_rhs_e0 = n_solvec_ex;
       n_rhs_x0 = n_solvec_ex;
     }
     offs_nv = 0;
     if (cct.flag_e) {
       offs_ndcur = offs_nv + cct.n_ebeu_nd - 1;
       offs_eaux = offs_ndcur + cct.e_n_ttlnd;
       offs_estv = offs_eaux + cct.e_n_ttlaux;
     } else {
       offs_ndcur = offs_nv;
       offs_eaux  = offs_nv;
       offs_estv  = offs_nv;
     }
     offs_xvr = offs_estv + cct.e_n_ttlstv;
     offs_xaux = offs_xvr + cct.n_xbeu_vr;

     if (cct.flag_x_only) {
       for (int i=offs_xvr; i < offs_xaux; i++) {
         svec_flag_x.push_back(global.I_XVR);
       }
       for (int i=offs_xaux; i < n_solvec_x; i++) {
         svec_flag_x.push_back(global.I_XAUX);
       }
     } else if (cct.flag_e_only) {
       for (int i=offs_nv; i < offs_ndcur; i++) {
         svec_flag_e.push_back(global.I_NV);
       }
       for (int i=offs_ndcur; i < offs_eaux; i++) {
         svec_flag_e.push_back(global.I_NDCUR);
       }
       for (int i=offs_eaux; i < offs_estv; i++) {
         svec_flag_e.push_back(global.I_EAUX);
       }
       for (int i=offs_estv; i < n_solvec_e; i++) {
         svec_flag_e.push_back(global.I_ESTV);
       }
     } else {
       for (int i=offs_nv; i < offs_ndcur; i++) {
         svec_flag_ex.push_back(global.I_NV);
       }
       for (int i=offs_ndcur; i < offs_eaux; i++) {
         svec_flag_ex.push_back(global.I_NDCUR);
       }
       for (int i=offs_eaux; i < offs_estv; i++) {
         svec_flag_ex.push_back(global.I_EAUX);
       }
       for (int i=offs_estv; i < offs_xvr; i++) {
         svec_flag_ex.push_back(global.I_ESTV);
       }
       for (int i=offs_xvr; i < offs_xaux; i++) {
         svec_flag_ex.push_back(global.I_XVR);
       }
       for (int i=offs_xaux; i < n_solvec_ex; i++) {
         svec_flag_ex.push_back(global.I_XAUX);
       }
     }
   } else {
     if (cct.flag_x_only) {

       n_solvec_x = n_solvec_x0;

       if (slv.flag_trns) {
         n_rhs_x0 = n_solvec_x0;
       }
       offs_xvr = 0;
       offs_xaux = offs_xvr + cct.n_xbeu_vr;

       for (int i=offs_xvr; i < offs_xaux; i++) {
         svec_flag_x.push_back(global.I_XVR);
       }
       for (int i=offs_xaux; i < n_solvec_x; i++) {
         svec_flag_x.push_back(global.I_XAUX);
       }
     } else if (cct.flag_e_only) {

       if (slv.flag_dc || slv.flag_trns) {
         n_solvec_e = n_solvec_e0;

         if (slv.flag_trns) {
           n_rhs_e0 = n_solvec_e0;
         }
         offs_nv = 0;
         offs_eaux = offs_nv + cct.n_ebeu_nd - 1;

         for (int i=offs_nv; i < offs_eaux; i++) {
           svec_flag_e.push_back(global.I_NV);
         }
         for (int i=offs_eaux; i < n_solvec_e; i++) {
           svec_flag_e.push_back(global.I_EAUX);
         }
       } else if (slv.flag_startup) {
         n_solvec_e = n_solvec_e0s;

         offs_nv = 0;
         offs_eauxs = offs_nv + cct.n_ebeu_nd - 1;

         for (int i=offs_nv; i < offs_eauxs; i++) {
           svec_flag_e.push_back(global.I_NV);
         }
         for (int i=offs_eauxs; i < n_solvec_e; i++) {
           svec_flag_e.push_back(global.I_EAUXS);
         }
       }
     } else {

       if (slv.flag_startup) {
         n_solvec_ex = n_solvec_e0s + n_solvec_x0;

         offs_nv = 0;
         offs_eauxs = offs_nv + cct.n_ebeu_nd - 1;
         offs_xvr = offs_eauxs + cct.e_n_ttlauxs;
         offs_xaux = offs_xvr + cct.n_xbeu_vr;

         for (int i=offs_nv; i < offs_eauxs; i++) {
           svec_flag_ex.push_back(global.I_NV);
         }
         for (int i=offs_eauxs; i < offs_xvr; i++) {
           svec_flag_ex.push_back(global.I_EAUXS);
         }
         for (int i=offs_xvr; i < offs_xaux; i++) {
           svec_flag_ex.push_back(global.I_XVR);
         }
         for (int i=offs_xaux; i < n_solvec_ex; i++) {
           svec_flag_ex.push_back(global.I_XAUX);
         }
       } else if (slv.flag_trns) {
         if (cct.flag_exc) {
           n_solvec_ex = n_solvec_e0 + n_solvec_x0;

           n_rhs_e0 = n_solvec_ex;
           n_rhs_x0 = n_solvec_ex;

           offs_nv = 0;
           offs_eaux = offs_nv + cct.n_ebeu_nd - 1;
           offs_xvr = offs_eaux + cct.e_n_ttlaux;
           offs_xaux = offs_xvr + cct.n_xbeu_vr;

           for (int i=offs_nv; i < offs_eaux; i++) {
             svec_flag_ex.push_back(global.I_NV);
           }
           for (int i=offs_eaux; i < offs_xvr; i++) {
             svec_flag_ex.push_back(global.I_EAUX);
           }
           for (int i=offs_xvr; i < offs_xaux; i++) {
             svec_flag_ex.push_back(global.I_XVR);
           }
           for (int i=offs_xaux; i < n_solvec_ex; i++) {
             svec_flag_ex.push_back(global.I_XAUX);
           }
         } else {
           cout << "SysMat::set_values_1: exs not implemented. Halting..." << endl;
           exit(1);
         }
       }
     }
   }

   offs[global.I_NV   ] = offs_nv;
   offs[global.I_EAUX ] = offs_eaux;
   offs[global.I_EAUXS] = offs_eauxs;
   offs[global.I_XVR  ] = offs_xvr;
   offs[global.I_XAUX ] = offs_xaux;
   offs[global.I_ESTV ] = offs_estv;
   offs[global.I_NDCUR] = offs_ndcur;

   cout << "SysMat::set_values_1:" << endl;
   cout << "  n_solvec_x = " << n_solvec_x << endl;
   cout << "  n_solvec_ex = " << n_solvec_ex << endl;
   cout << "  n_solvec_e = " << n_solvec_e << endl;
   cout << "  n_solvec_e0 = " << n_solvec_e0 << endl;
   cout << "  n_solvec_x0 = " << n_solvec_x0 << endl;

// todo
// use this also for SSW (just find the sizes as required for ssw)
// (note that the arrays in this block will get deleted in delete_1;
// that is why we don't want to allocate them elsewhere)

   if (flag_alloc) {
     svec_ex = new double[n_solvec_ex];
     svec_e  = new double[n_solvec_e ];
     svec_x  = new double[n_solvec_x ];

     svec_w_ex = new double[n_solvec_ex];
     svec_w_e  = new double[n_solvec_e ];
     svec_w_x  = new double[n_solvec_x ];

     svec_orig_ex = new double[n_solvec_ex];
     svec_orig_e  = new double[n_solvec_e ];
     svec_orig_x  = new double[n_solvec_x ];

     svec_old_nr_1_e  = new double[n_solvec_e ];
     svec_old_nr_2_e  = new double[n_solvec_e ];
     svec_old_nr_1_ex = new double[n_solvec_ex];
     svec_old_nr_2_ex = new double[n_solvec_ex];

     svec_old_1_e  = new double[n_solvec_e ];
     svec_old_2_e  = new double[n_solvec_e ];
     svec_old_1_ex = new double[n_solvec_ex];
     svec_old_2_ex = new double[n_solvec_ex];
     svec_old_1_x  = new double[n_solvec_x ];
     svec_old_2_x  = new double[n_solvec_x ];

     delsvec_ex = new double[n_solvec_ex];
     delsvec_e  = new double[n_solvec_e ];
     delsvec_x  = new double[n_solvec_x ];

     tol_spice_e   = new double[n_solvec_e0];
     norm_spice_e  = new double[n_solvec_e0];

     ebe_rhs_ddt_flag      = new bool[n_rhs_e0];
     ebe_rhs_ddt_varnumber = new int [n_rhs_e0];
     ebe_rhs_ddt_pntr      = new int [n_rhs_e0];
     ebe_rhs_ddt_varflag   = new int [n_rhs_e0];
     ssw_ebe_rhs_flag      = new int [n_rhs_e0];
     ebe_rhs_ddt_i_ebeu    = new int [n_rhs_e0];
     ebe_rhs_ddt_i_f       = new int [n_rhs_e0];
     ebe_rhs_ddt_i_g       = new int [n_rhs_e0];

     ebe_stv_index         = new int [n_rhs_e0];
     flag_ebe_stv          = new bool[n_rhs_e0];
     ddt_ebe_pntr          = new int [n_rhs_e0];

     xbe_rhs_ddt_flag      = new bool[n_rhs_x0];
     xbe_rhs_ddt_varnumber = new int [n_rhs_x0];
     xbe_rhs_ddt_pntr      = new int [n_rhs_x0];
     xbe_rhs_ddt_varflag   = new int [n_rhs_x0];
     xbe_rhs_ddt_i_xbeu    = new int [n_rhs_x0];
     xbe_rhs_ddt_i_f       = new int [n_rhs_x0];

     flag_ebe_ddt          = new bool[n_rhs_e0];

     flag_alloc = false; flag_delete = true;
   } else {
     cout << "SysMat::set_values_1: trying to allocate" << endl;
     cout << "  with flag_alloc = false? Halting..." << endl; exit(1);
   }

   return;
} // end of SysMat::set_values_1
// -----------------------------------------------------------------------------
void SysMat::mat_dc_1_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file) {

   int i_ebel,i_ebeu;
   int n_col1,n_row1,n_row2,n_f1,n_nd1;
   int n_rows;
   int var_flag,var_number;
   int row0,col0;
   int nd0,n_fvar1,nnz0;
   bool flag_refnode,flag_ddt,flag_gtref,flag_1;
   int i_f;

   cct.val_nd[cct.ref_nd] = 0.0;

// KCL equations:
   n_row1 = cct.n_ebeu_nd - 1;
   n_col1 = n_solvec_e;

// add non-KCL equations:
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (i_f=n_nd1; i_f < n_f1; i_f++) n_row1++;
   }

   assign_array_1<int>(ebe_nd_to_row,cct.n_ebeu_nd,-1);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     for (i_f=0; i_f < n_f1; i_f++) {
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = -1;
       }
       ebe_f_to_row[i_ebeu][i_f] = -1;
     }
   }

   m_e.allocate_1(0,n_row1,n_col1);
   knuth_zero_1(m_e);
   n_rows = 0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
     for (i_f=0; i_f < n_nd1; i_f++) {
       nd0 = ebe_usr[i_ebeu].nd[i_f];

       if (nd0 != cct.ref_nd) {
         n_row2 = ebe_nd_to_row[nd0];
         if (n_row2 == -1) {
           ebe_nd_to_row[nd0] = n_rows;
           row0 = n_rows;
           n_rows++;
         } else {
           row0 = n_row2;
         }

         ebe_f_to_row[i_ebeu][i_f] = row0;

         n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
         for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
           var_flag   = ebe_lib[i_ebel].fvar_flag[i_f][i_fvar];
           var_number = ebe_usr[i_ebeu].fvar[i_f][i_fvar];
           flag_refnode = (var_flag == global.I_NV)
             && (var_number == cct.ref_nd);
           if (!flag_refnode) {
             flag_ddt = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];
             if (!flag_ddt) {
               flag_gtref = (var_flag == global.I_NV)
                 && (var_number > cct.ref_nd);
               if (flag_gtref) {
                 col0 = offs[var_flag] + var_number - 1;
               } else {
                 col0 = offs[var_flag] + var_number;
               }
               knuth_check_ij(m_e,row0,col0,flag_1,nnz0);
               if (flag_1) {
                 map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = nnz0;
               } else {
                 map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = m_e.n_nz;
                 knuth_addentry(m_e,row0,col0,0.0);
               }
             }
           }
         }
       }
     }
//   non-KCL equations:
     for (i_f=n_nd1; i_f < n_f1; i_f++) {
       row0 = n_rows;
       cout << "mat_dc_1_e (2): i_ebeu = " << i_ebeu
         << ", i_f = " << i_f
         << ", row0 = " << row0
         << endl;
       ebe_f_to_row[i_ebeu][i_f] = row0;
       n_rows++;

       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         var_flag   = ebe_lib[i_ebel].fvar_flag[i_f][i_fvar];
         var_number = ebe_usr[i_ebeu].fvar[i_f][i_fvar];
         flag_refnode = (var_flag == global.I_NV)
           && (var_number == cct.ref_nd);
         if (!flag_refnode) {
           flag_ddt = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];
           if (!flag_ddt) {
             flag_gtref = (var_flag == global.I_NV)
               && (var_number > cct.ref_nd);
             if (flag_gtref) {
               col0 = offs[var_flag] + var_number - 1;
             } else {
               col0 = offs[var_flag] + var_number;
             }
             knuth_check_ij(m_e,row0,col0,flag_1,nnz0);
             if (flag_1) {
               map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = nnz0;
             } else {
               map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = m_e.n_nz;
               knuth_addentry(m_e,row0,col0,0.0);
             }
           }
         }
       }
     }
   }

   check_knuth(m_e,(char*)"mat_dc_1_e: matrix error");

   map_jac_ebe_to_m = new int[m_e.n_nz];

   rhs_m_e = new double[m_e.n_row];
   rhs_w_e = new double[m_e.n_row];

   assign_array_1<double>(rhs_m_e,m_e.n_row,0.0);
   assign_array_1<double>(rhs_w_e,m_e.n_row,0.0);

   return;
} //end of SysMat::mat_dc_1_e
// -----------------------------------------------------------------------------
void SysMat::mat_startup_1_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   CctFile &cct_file) {

   int i_xbel,i_xbeu,n_f1,n_g1,n_gvar1;
   int n_col1,n_row1;
   int var_flag,var_number;
   int n_rows,row0,col0;
   int i_f,i_g;

   n_row1 = cct.x_n_ttlg;
   if (cct.flag_exc) {
     n_col1 = n_solvec_ex;
   } else {
     n_col1 = n_solvec_x;
   }

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     n_f1 = xbe_lib[i_xbel].n_f;
     n_g1 = xbe_lib[i_xbel].n_g;

     for (i_f=0; i_f < n_f1; i_f++) {
       xbe_f_to_row[i_xbeu][i_f] = -1;
     }
     for (i_g=0; i_g < n_g1; i_g++) {
       n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
       for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
         map_gvar_to_xbe[i_xbeu][i_g][i_gvar] = -1;
       }
       xbe_g_to_row[i_xbeu][i_g] = -1;
     }
   }

   m_x.allocate_1(0,n_row1,n_col1);
   knuth_zero_1(m_x);
   n_rows = 0;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if (xbe_lib[i_xbel].flag_integrate) {
       n_f1 = xbe_lib[i_xbel].n_f;
       for (i_f=0; i_f < n_f1; i_f++) {
         var_number = xbe_usr[i_xbeu].fvar[i_f];
         var_flag = xbe_lib[i_xbel].ddt_varflag[i_f];

         col0 = offs[var_flag] + var_number;
         row0 = n_rows;
         xbe_f_to_row[i_xbeu][i_f] = row0;
         knuth_addentry(m_x,row0,col0,1.0);
         n_rows++;
       }
     } else {
       n_g1 = xbe_lib[i_xbel].n_g;
       for (i_g=0; i_g < n_g1; i_g++) {
         n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
         for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
           var_number = xbe_usr[i_xbeu].gvar[i_g][i_gvar];
           var_flag = xbe_lib[i_xbel].gvar_flag[i_g][i_gvar];

           col0 = offs[var_flag] + var_number;
           row0 = n_rows;
           xbe_g_to_row[i_xbeu][i_g] = row0;
           map_gvar_to_xbe[i_xbeu][i_g][i_gvar] = m_x.n_nz;
           knuth_addentry(m_x,row0,col0,0.0);
         }
         n_rows++;
       }
     }
   }

   check_knuth(m_x,(char*)"mat_startup_1_x: matrix error");

   rhs_m_x = new double[m_x.n_row];
   rhs_w_x = new double[m_x.n_row];

   return;
} //end of SysMat::mat_startup_1_x
// -----------------------------------------------------------------------------
void SysMat::mat_startup_1_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file) {

   int i_ebel,i_ebeu,i_h;
   int n_col1,n_row1,n_row2,n_h1,n_nd1;
   int n_rows;
   int var_flag,var_number;
   int row0,col0;
   int nd0,n_hvar1,nnz0;
   bool flag_refnode,flag_gtref,flag_1;

   cct.val_nd[cct.ref_nd] = 0.0;
   n_row1 = cct.n_ebeu_nd - 1;
   if (cct.flag_exc) {
     n_col1 = n_solvec_ex;
   } else {
     n_col1 = n_solvec_e;
   }

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_h1 = ebe_lib[i_ebel].n_h;
     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (i_h=n_nd1; i_h < n_h1; i_h++) n_row1++;
   }

   assign_array_1<int>(ebe_nd_to_row,cct.n_ebeu_nd,-1);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_h1 = ebe_lib[i_ebel].n_h;
     for (i_h=0; i_h < n_h1; i_h++) {
       n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
       for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
         map_hvar_to_ebe[i_ebeu][i_h][i_hvar] = -1;
       }
     }
   }

   m_e.allocate_1(0,n_row1,n_col1);
   knuth_zero_1(m_e);
   n_rows = 0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_h1 = ebe_lib[i_ebel].n_h;
     n_nd1 = ebe_lib[i_ebel].n_nd;

     for (i_h=0; i_h < n_nd1; i_h++) {
       ebe_h_to_row[i_ebeu][i_h] = -1;
     }
     for (i_h=n_nd1; i_h < n_h1; i_h++) {
       ebe_h_to_row[i_ebeu][i_h] = -1;
     }
   }

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_h1 = ebe_lib[i_ebel].n_h;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
     for (i_h=0; i_h < n_nd1; i_h++) {
       nd0 = ebe_usr[i_ebeu].nd[i_h];

       if (nd0 != cct.ref_nd) {
         n_row2 = ebe_nd_to_row[nd0];
         if (n_row2 == -1) {
           ebe_nd_to_row[nd0] = n_rows;
           row0 = n_rows;
           n_rows++;
         } else {
           row0 = n_row2;
         }
         ebe_h_to_row[i_ebeu][i_h] = row0;

         n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
         for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
           var_flag   = ebe_lib[i_ebel].hvar_flag[i_h][i_hvar];
           var_number = ebe_usr[i_ebeu].hvar[i_h][i_hvar];
           flag_refnode = (var_flag == global.I_NV)
             && (var_number == cct.ref_nd);
           if (!flag_refnode) {
             flag_gtref = (var_flag == global.I_NV)
               && (var_number > cct.ref_nd);
             if (flag_gtref) {
               col0 = offs[var_flag] + var_number - 1;
             } else {
               col0 = offs[var_flag] + var_number;
             }
             knuth_check_ij(m_e,row0,col0,flag_1,nnz0);
             if (flag_1) {
               map_hvar_to_ebe[i_ebeu][i_h][i_hvar] = nnz0;
             } else {
               map_hvar_to_ebe[i_ebeu][i_h][i_hvar] = m_e.n_nz;
               knuth_addentry(m_e,row0,col0,0.0);
             }
           }
         }
       }
     }
//   non-KCL equations:
     for (i_h=n_nd1; i_h < n_h1; i_h++) {
       row0 = n_rows;
       ebe_h_to_row[i_ebeu][i_h] = row0;
       n_rows++;

       n_hvar1 = ebe_lib[i_ebel].n_hvar[i_h];
       for (int i_hvar=0; i_hvar < n_hvar1; i_hvar++) {
         var_flag   = ebe_lib[i_ebel].hvar_flag[i_h][i_hvar];
         var_number = ebe_usr[i_ebeu].hvar[i_h][i_hvar];
         flag_refnode = (var_flag == global.I_NV)
           && (var_number == cct.ref_nd);
         if (!flag_refnode) {
           flag_gtref = (var_flag == global.I_NV)
             && (var_number > cct.ref_nd);
           if (flag_gtref) {
             col0 = offs[var_flag] + var_number - 1;
           } else {
             col0 = offs[var_flag] + var_number;
           }
           knuth_check_ij(m_e,row0,col0,flag_1,nnz0);
           if (flag_1) {
             map_hvar_to_ebe[i_ebeu][i_h][i_hvar] = nnz0;
           } else {
             map_hvar_to_ebe[i_ebeu][i_h][i_hvar] = m_e.n_nz;
             knuth_addentry(m_e,row0,col0,0.0);
           }
         }
       }
     }
   }

   check_knuth(m_e,(char*)"mat_startup_1_e: matrix error");

   map_jac_ebe_to_m = new int[m_e.n_nz];

   rhs_m_e = new double[m_e.n_row];
   rhs_w_e = new double[m_e.n_row];

   return;
} //end of SysMat::mat_startup_1_e
// -----------------------------------------------------------------------------
void SysMat::mat_startup_1_ex(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file) {

   mat_startup_1_e(ebe_lib,ebe_usr,global,cct,cct_file);
   mat_startup_1_x(xbe_lib,xbe_usr,cct,cct_file);

   m_ex.allocate_1(m_e.n_nz,(m_e.n_row+m_x.n_row),n_solvec_ex);
   knuth_copy(m_e,m_ex);
   knuth_append_2a(m_ex,m_x);

   check_knuth(m_ex,(char*)"mat_startup_1_ex: matrix error");

   rhs_m_ex = new double[m_ex.n_row];
   rhs_w_ex = new double[m_ex.n_row];

   return;
} //end of SysMat::mat_startup_1_ex
// -----------------------------------------------------------------------------
void SysMat::mat_trns_1_x0(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global,
   CctFile &cct_file) {

   int i_xbel,i_xbeu,n_f1,n_g1,n_gvar1;
   int n_col1,n_row1;
   int var_flag,var_number;
   int n_rows,row0,col0;
   int *flag_col,*nnz_dummy;
   int flag_col_1;
   int i_f,i_g;
   fstream outf;

   n_row1 = cct.x_n_ttlg;

   if (cct.flag_exc) {
     n_col1 = n_solvec_ex;
   } else {
     n_col1 = n_solvec_x;
   }

   cout << "SysMat::mat_trns_1_x0: n_row1 = " << n_row1
     << " n_col1 = " << n_col1 << endl;

   flag_col  = new int[n_col1];
   nnz_dummy = new int[n_col1];

   for (int i=0; i < n_col1; i++) {
     flag_col [i] = -1;
     nnz_dummy[i] = -1;
   }

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_f1 = xbe_lib[i_xbel].n_f;
     for (i_f=0; i_f < n_f1; i_f++) {
       xbe_f_to_row[i_xbeu][i_f] = -1;
     }
     n_g1 = xbe_lib[i_xbel].n_g;
     for (i_g=0; i_g < n_g1; i_g++) {
       xbe_g_to_row[i_xbeu][i_g] = -1;

       n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
       for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
         map_gvar_to_xbe[i_xbeu][i_g][i_gvar] = -1;
       }
     }
   }

   m_x.allocate_1(0,n_row1,n_col1);
   knuth_zero_1(m_x);
   n_rows = 0;

   assign_array_1<bool>(xbe_rhs_ddt_flag,     n_rhs_x0,false);
   assign_array_1<int> (xbe_rhs_ddt_varnumber,n_rhs_x0,-1);
   assign_array_1<int> (xbe_rhs_ddt_varflag,  n_rhs_x0,-1);
   assign_array_1<int> (xbe_rhs_ddt_pntr,     n_rhs_x0,-1);
   assign_array_1<int> (xbe_rhs_ddt_i_xbeu,   n_rhs_x0,-1);
   assign_array_1<int> (xbe_rhs_ddt_i_f,      n_rhs_x0,-1);

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_g1 = xbe_lib[i_xbel].n_g;

     if (xbe_lib[i_xbel].flag_integrate) {
       for (i_f=0; i_f < n_g1; i_f++) {
         var_number = xbe_usr[i_xbeu].fvar[i_f];
         var_flag = xbe_lib[i_xbel].ddt_varflag[i_f];

         col0 = offs[var_flag] + var_number;
         row0 = n_rows;

         flag_col_1 = i_f + col0;
         flag_col[col0] = flag_col_1;

         xbe_rhs_ddt_flag[row0] = true;
         xbe_rhs_ddt_i_xbeu[row0] = i_xbeu;
         xbe_rhs_ddt_i_f[row0] = i_f;
         xbe_rhs_ddt_varflag[row0] = var_flag;
         xbe_rhs_ddt_varnumber[row0] = col0;

         xbe_f_to_row[i_xbeu][i_f] = row0;
         nnz_dummy[col0] = m_x.n_nz;
         xbe_rhs_ddt_pntr[row0] = m_x.n_nz;

         knuth_addentry(m_x,row0,col0,0.0);

//       the non_ddt terms will come from g

         n_gvar1 = xbe_lib[i_xbel].n_gvar[i_f];
         for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
           var_number = xbe_usr[i_xbeu].gvar[i_f][i_gvar];
           var_flag = xbe_lib[i_xbel].gvar_flag[i_f][i_gvar];
           col0 = offs[var_flag] + var_number;

           flag_col_1 = i_f + col0;

           if (flag_col[col0] != flag_col_1) {

             map_gvar_to_xbe[i_xbeu][i_f][i_gvar] = m_x.n_nz;
             flag_col[col0] = flag_col_1;
             nnz_dummy[col0] = m_x.n_nz;

             knuth_addentry(m_x,row0,col0,0.0);

           } else {
             map_gvar_to_xbe[i_xbeu][i_f][i_gvar] = nnz_dummy[col0];
           }
         }
         n_rows++;
       }
     } else {
       for (i_g=0; i_g < n_g1; i_g++) {
         n_gvar1 = xbe_lib[i_xbel].n_gvar[i_g];
         for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
           var_number = xbe_usr[i_xbeu].gvar[i_g][i_gvar];
           var_flag = xbe_lib[i_xbel].gvar_flag[i_g][i_gvar];
           col0 = offs[var_flag] + var_number;
           row0 = n_rows;
           xbe_g_to_row[i_xbeu][i_g] = row0;

           map_gvar_to_xbe[i_xbeu][i_g][i_gvar] = m_x.n_nz;
           knuth_addentry(m_x,row0,col0,0.0);

         }
         n_rows++;
       }
     }
   }
   check_knuth(m_x,(char*)"mat_trns_1_x0: matrix error");

   delete[] flag_col;
   delete[] nnz_dummy;

   return;
} //end of SysMat::mat_trns_1_x0
// -----------------------------------------------------------------------------
void SysMat::mat_trns_1_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global,
   CctFile &cct_file) {

   mat_trns_1_x0(xbe_lib,xbe_usr,cct,global,cct_file);

   rhs_m_x = new double[m_x.n_row];
   rhs_w_x = new double[m_x.n_row];

   return;
} //end of SysMat::mat_trns_1_x
// -----------------------------------------------------------------------------
void SysMat::mat_trns_1_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file) {

   int i_ebel,i_ebeu;
   int n_col1,n_row1,n_row2,n_f1,n_g1,n_nd1;
   int n_rows;
   int var_flag,var_number;
   int var_flag_stv,var_number_stv;
   int row0,col0,col0_stv;
   int nd0,n_fvar1,n_gvar1,nnz0;
   bool flag_refnode,flag_ddt,flag_gtref,flag_1;
   bool flag_refnode_stv,flag_gtref_stv;
   int i_f,i_g;
   fstream outf;

   cct.val_nd[cct.ref_nd] = 0.0;

// KCL equations:
   n_row1 = cct.n_ebeu_nd - 1;

   if (cct.flag_exc) {
     n_col1 = n_solvec_ex;
   } else {
     n_col1 = n_solvec_e;
   }

// add non-KCL equations:
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (i_f=n_nd1; i_f < n_f1; i_f++) n_row1++;
   }

   assign_array_1<int>(ebe_nd_to_row,cct.n_ebeu_nd,-1);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     for (i_f=0; i_f < n_f1; i_f++) {
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = -1;
       }
       ebe_f_to_row[i_ebeu][i_f] = -1;
     }
     n_g1 = ebe_lib[i_ebel].n_g;
     for (i_g=0; i_g < n_g1; i_g++) {
       n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];
       for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
         map_gvar_to_ebe[i_ebeu][i_g][i_gvar] = -1;
       }
     }
   }

   m_e.allocate_1(0,n_row1,n_col1);
   knuth_zero_1(m_e);
   n_rows = 0;

   assign_array_1<bool>(ebe_rhs_ddt_flag,     n_rhs_e0,false);
   assign_array_1<int> (ebe_rhs_ddt_varnumber,n_rhs_e0,-1);
   assign_array_1<int> (ebe_rhs_ddt_varflag,  n_rhs_e0,-1);
   assign_array_1<int> (ssw_ebe_rhs_flag,     n_rhs_e0,-1);
   assign_array_1<int> (ebe_rhs_ddt_pntr,     n_rhs_e0,-1);
   assign_array_1<int> (ebe_rhs_ddt_i_ebeu,   n_rhs_e0,-1);
   assign_array_1<int> (ebe_rhs_ddt_i_f,      n_rhs_e0,-1);
   assign_array_1<int> (ebe_rhs_ddt_i_g,      n_rhs_e0,-1);

   assign_array_1<bool>(flag_ebe_stv,         n_rhs_e0,false);
   assign_array_1<int> (ebe_stv_index,        n_rhs_e0,-1);
   assign_array_1<int> (ddt_ebe_pntr,         n_rhs_e0,-1);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
// todo
// ammeter_fb: check if xvr is being treated correctly
     for (i_f=0; i_f < n_nd1; i_f++) {
       nd0 = ebe_usr[i_ebeu].nd[i_f];

       if (nd0 != cct.ref_nd) {
         n_row2 = ebe_nd_to_row[nd0];
         if (n_row2 == -1) {
           ebe_nd_to_row[nd0] = n_rows;
           row0 = n_rows;
           n_rows++;
         } else {
           row0 = n_row2;
         }
         ebe_f_to_row[i_ebeu][i_f] = row0;

         n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
         for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
           var_flag   = ebe_lib[i_ebel].fvar_flag[i_f][i_fvar];
           var_number = ebe_usr[i_ebeu].fvar[i_f][i_fvar];
           flag_refnode = (var_flag == global.I_NV)
             && (var_number == cct.ref_nd);
           if (!flag_refnode) {
             flag_ddt = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];
             if (!flag_ddt) {
               flag_gtref = (var_flag == global.I_NV)
                 && (var_number > cct.ref_nd);
               if (flag_gtref) {
                 col0 = offs[var_flag] + var_number - 1;
               } else {
                 col0 = offs[var_flag] + var_number;
               }
               knuth_check_ij(m_e,row0,col0,flag_1,nnz0);
               if (flag_1) {
                 map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = nnz0;
               } else {
                 map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = m_e.n_nz;
                 knuth_addentry(m_e,row0,col0,0.0);
               }
             } else {
//             this is a ddt term
               i_g = ebe_lib[i_ebel].f_ddt_stv_eqn[i_f];
               n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];

               for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
                 var_flag_stv   = ebe_lib[i_ebel].gvar_flag[i_g][i_gvar];
                 var_number_stv = ebe_usr[i_ebeu].gvar[i_g][i_gvar];
                 flag_refnode_stv = (var_flag_stv == global.I_NV)
                   && (var_number_stv == cct.ref_nd);
                 if (!flag_refnode_stv) {
                   flag_gtref_stv = (var_flag_stv == global.I_NV)
                     && (var_number_stv > cct.ref_nd);
                   if (flag_gtref_stv) {
                     col0_stv = offs[var_flag_stv] + var_number_stv - 1;
                   } else {
                     col0_stv = offs[var_flag_stv] + var_number_stv;
                   }
                   knuth_check_ij(m_e,row0,col0_stv,flag_1,nnz0);
                   if (flag_1) {
                     map_gvar_to_ebe[i_ebeu][i_g][i_gvar] = nnz0;
                   } else {
                     map_gvar_to_ebe[i_ebeu][i_g][i_gvar] = m_e.n_nz;
                     knuth_addentry(m_e,row0,col0_stv,0.0);
                   }
                 }
               }
             }
           }
         }
       }
     }
//   non-KCL equations:
     for (i_f=n_nd1; i_f < n_f1; i_f++) {
       row0 = n_rows;
       ebe_f_to_row[i_ebeu][i_f] = row0;
       n_rows++;

       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         var_flag   = ebe_lib[i_ebel].fvar_flag[i_f][i_fvar];
         var_number = ebe_usr[i_ebeu].fvar[i_f][i_fvar];
         flag_refnode = (var_flag == global.I_NV)
           && (var_number == cct.ref_nd);
         if (!flag_refnode) {
           flag_gtref = (var_flag == global.I_NV)
             && (var_number > cct.ref_nd);
           flag_ddt = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];
           if (flag_ddt) {
             ebe_rhs_ddt_flag[row0] = true;
             ebe_rhs_ddt_varflag[row0] = var_flag;
             ebe_rhs_ddt_i_ebeu[row0] = i_ebeu;
             ebe_rhs_ddt_i_f[row0] = i_f;
             if (flag_gtref) {
               ebe_rhs_ddt_varnumber[row0] = offs[var_flag] + var_number - 1;
             } else {
               ebe_rhs_ddt_varnumber[row0] = offs[var_flag] + var_number;
             }
           }
           if (flag_gtref) {
             col0 = offs[var_flag] + var_number - 1;
           } else {
             col0 = offs[var_flag] + var_number;
           }
           knuth_check_ij(m_e,row0,col0,flag_1,nnz0);
           if (flag_1) {
             map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = nnz0;
             if (flag_ddt) ebe_rhs_ddt_pntr[row0] = nnz0;
           } else {
             map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = m_e.n_nz;
             if (flag_ddt) ebe_rhs_ddt_pntr[row0] = m_e.n_nz;
             knuth_addentry(m_e,row0,col0,0.0);
           }
         }
       }
     }
   }
   check_knuth(m_e,(char*)"mat_trns_1_e: matrix error");

   map_jac_ebe_to_m = new int[m_e.n_nz];

   rhs_m_e = new double[m_e.n_row];
   rhs_w_e = new double[m_e.n_row];

   return;
} //end of SysMat::mat_trns_1_e
// -----------------------------------------------------------------------------
void SysMat::mat_trns_1_ex(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file) {

   mat_trns_1_e(ebe_lib,ebe_usr,global,cct,cct_file);
   mat_trns_1_x(xbe_lib,xbe_usr,cct,global,cct_file);

   m_ex.allocate_1(m_e.n_nz,(m_e.n_row+m_x.n_row),n_solvec_ex);
   knuth_copy(m_e,m_ex);
   knuth_append_2a(m_ex,m_x);

   check_knuth(m_ex,(char*)"mat_trns_1_ex: matrix error");

   rhs_m_ex = new double[m_ex.n_row];
   rhs_w_ex = new double[m_ex.n_row];

   return;
} //end of SysMat::mat_trns_1_ex
// -----------------------------------------------------------------------------
void SysMat::mat_ssw_1_e0(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file) {

   int i_ebel,i_ebeu;
   int n_f1,n_fvar1,n_nd1,n_g1,n_gvar1,n_fvar_stv;
   int n_row1,n_col1,n_rows,row0,col0,row0a,col0_stv;
   int nnz0;
   int i_f,i_g,i_stv,i_stv_ebeu;
   int var_flag,var_number;
   int var_flag_stv,var_number_stv;
   int pntr,pntr_m;
   bool l_locveqn_added;
   bool flag_refnode,flag_ddt,flag_gtref;
   bool flag_refnode_stv,flag_gtref_stv,flag_1;

   cct.val_nd[cct.ref_nd] = 0.0;
   l_locveqn_added = false;

   n_row1 = cct.e_n_ttlnd + cct.e_n_ttlstv;
   if (cct.flag_e_only) {
     n_col1 = n_solvec_e;
   } else {
     n_col1 = n_solvec_ex;
   }

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (i_f=n_nd1; i_f < n_f1; i_f++) n_row1++;
   }
   m_e.allocate_1(0,n_row1,n_col1);
   knuth_zero_1(m_e);

   kcl_allocate_1(ebe_lib,ebe_usr,cct);
   kcl_mat_2(ebe_lib,ebe_usr,global,cct,cct_file);

   assign_array_1<bool>(ebe_rhs_ddt_flag,     n_rhs_e0,false);
   assign_array_1<int> (ebe_rhs_ddt_varnumber,n_rhs_e0,-1);
   assign_array_1<int> (ebe_rhs_ddt_varflag,  n_rhs_e0,-1);
   assign_array_1<int> (ssw_ebe_rhs_flag,     n_rhs_e0,-1);
   assign_array_1<int> (ebe_rhs_ddt_pntr,     n_rhs_e0,-1);
   assign_array_1<int> (ebe_rhs_ddt_i_ebeu,   n_rhs_e0,-1);
   assign_array_1<int> (ebe_rhs_ddt_i_f,      n_rhs_e0,-1);
   assign_array_1<int> (ebe_rhs_ddt_i_g,      n_rhs_e0,-1);
   assign_array_1<bool>(flag_ebe_ddt,         n_rhs_e0,false);

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     for (i_f=0; i_f < n_f1; i_f++) {
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = -1;
       }
       ebe_f_to_row   [i_ebeu][i_f] = -1;
       ebe_f_stv_index[i_ebeu][i_f] = -1;
     }
     n_g1 = ebe_lib[i_ebel].n_g;
     for (i_g=0; i_g < n_g1; i_g++) {
       n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];
       for (int i_gvar=0; i_gvar < n_gvar1; i_gvar++) {
         map_gvar_to_ebe[i_ebeu][i_g][i_gvar] = -1;
       }
       ebe_g_to_row[i_ebeu][i_g] = -1;
     }
   }

   n_rows = 0;
   pntr = 0;

   cout << "mat_ssw_1_e0: cct.n_ebeu: " << cct.n_ebeu << endl;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;

//   KCL equations:
     for (i_f=0; i_f < n_nd1; i_f++) {
       row0 = n_rows;
       ssw_ebe_rhs_flag[row0] = global.I_EBE_KCL;
       ebe_rhs_ddt_i_ebeu[row0] = i_ebeu;
       ebe_rhs_ddt_i_f[row0] = i_f;
       n_rows++;

       ebe_f_to_row[i_ebeu][i_f] = row0;

       flag_ebe_stv[row0] = false;

       col0 = offs_ndcur + pntr;
       knuth_addentry(m_e,row0,col0,1.0);

       flag_ebe_ddt[row0] = false;

       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         var_flag   = ebe_lib[i_ebel].fvar_flag[i_f][i_fvar];
         var_number = ebe_usr[i_ebeu].fvar[i_f][i_fvar];
         flag_refnode = (var_flag == global.I_NV) && (var_number == cct.ref_nd);
         if (!flag_refnode) {
           flag_gtref = (var_flag == global.I_NV) && (var_number > cct.ref_nd);
           flag_ddt = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];
           flag_ebe_stv[row0] = flag_ddt;

           if (!flag_ddt) {
             if (flag_gtref) {
               col0 = offs[var_flag] + var_number - 1;
             } else {
               col0 = offs[var_flag] + var_number;
             }
             if (l_locveqn_added) {
               row0a = row0 - 1;
             } else {
               row0a = row0;
             }
             map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = m_e.n_nz;
             knuth_addentry(m_e,row0a,col0,0.0);
           } else {
             i_g = ebe_lib[i_ebel].f_ddt_stv_eqn[i_f];
             i_stv = ebe_lib[i_ebel].f_ddt_var_index[i_f];
             i_stv_ebeu = ebe_usr[i_ebeu].stv[i_stv];
             ebe_rhs_ddt_varnumber[row0] = offs_estv + i_stv_ebeu;

             ebe_stv_index[row0] = i_stv_ebeu;
             col0 = offs[var_flag] + i_stv_ebeu;
             map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = m_e.n_nz;
             knuth_addentry(m_e,row0,col0,0.0);
             ebe_f_stv_index[i_ebeu][i_f] = i_stv_ebeu;

             row0 = n_rows;
             ssw_ebe_rhs_flag[row0] = global.I_EBE_STV;
             ebe_rhs_ddt_i_ebeu[row0] = i_ebeu;
             ebe_rhs_ddt_i_f[row0] = i_f;
             ebe_rhs_ddt_i_g[row0] = i_g;

             n_rows++;
             ebe_g_to_row[i_ebeu][i_g] = row0;
             flag_ebe_stv[row0] = false;

             map_stv_to_ebe[i_ebeu][i_f][i_fvar] = m_e.n_nz;
             knuth_addentry(m_e,row0,col0,0.0);
             ebe_g_to_row[i_ebeu][i_stv] = row0;

             n_gvar1 = ebe_lib[i_ebel].n_gvar[i_g];

             for (int i_gvar=1; i_gvar < n_gvar1; i_gvar++) {
               var_flag_stv = ebe_lib[i_ebel].gvar_flag[i_g][i_gvar];
               var_number_stv = ebe_usr[i_ebeu].gvar[i_g][i_gvar];
               flag_refnode_stv = (var_flag_stv == global.I_NV)
                 && (var_number_stv == cct.ref_nd);

               if (!flag_refnode_stv) {
                 flag_gtref_stv = (var_flag_stv == global.I_NV)
                   && (var_number_stv > cct.ref_nd);
                 if (flag_gtref_stv) {
                   col0_stv = offs[var_flag_stv] + var_number_stv - 1;
                 } else {
                   col0_stv = offs[var_flag_stv] + var_number_stv;
                 }
                 map_gvar_to_ebe[i_ebeu][i_g][i_gvar] = m_e.n_nz;
                 knuth_addentry(m_e,row0,col0_stv,0.0);
               }
             }
             l_locveqn_added = true;
           }
         }
       }
       l_locveqn_added = false;
       pntr++;
     }
//   non-KCL equations:
     for (i_f=n_nd1; i_f < n_f1; i_f++) {
       row0 = n_rows;
       n_rows++;

       ebe_f_to_row[i_ebeu][i_f] = row0;

       ebe_rhs_ddt_i_ebeu[row0] = i_ebeu;
       ebe_rhs_ddt_i_f[row0] = i_f;
       flag_ebe_ddt[row0] = false;

       flag_ebe_stv[row0] = false;
       pntr_m = ddt_ebe_pntr[row0];

       ebe_rhs_ddt_flag[row0] = false;
       ssw_ebe_rhs_flag[row0] = global.I_EBE_NONKCL;

       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         var_flag   = ebe_lib[i_ebel].fvar_flag[i_f][i_fvar];
         var_number = ebe_usr[i_ebeu].fvar[i_f][i_fvar];
         flag_refnode = (var_flag == global.I_NV) && (var_number == cct.ref_nd);
         flag_gtref = (var_flag == global.I_NV) && (var_number > cct.ref_nd);

         if (!flag_refnode) {
           flag_ddt = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];

           if (flag_ddt) {
             ebe_rhs_ddt_flag[row0] = true;
             flag_ebe_ddt[row0] = true;
             ssw_ebe_rhs_flag[row0] = global.I_EBE_DDT_NONKCL;
             ebe_rhs_ddt_varnumber[row0] = offs[var_flag] + var_number;

             ebe_rhs_ddt_i_f[row0] = i_f;
           }
           if (flag_gtref) {
             col0 = offs[var_flag] + var_number - 1;
           } else {
             col0 = offs[var_flag] + var_number;
           }
           knuth_check_ij(m_e,row0,col0,flag_1,nnz0);
           if (flag_1) {
             map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = nnz0;
             if (flag_ddt) {
               ddt_ebe_pntr[row0] = nnz0;
             }
           } else {
             map_fvar_to_ebe[i_ebeu][i_f][i_fvar] = m_e.n_nz;
             if (flag_ddt) {
               ddt_ebe_pntr[row0] = m_e.n_nz;
             }
             knuth_addentry(m_e,row0,col0,0.0);
           }
         }
       }
     }
   }

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     n_nd1 = ebe_lib[i_ebel].n_nd;

     for (i_f=0; i_f < n_nd1; i_f++) {

       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];

       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         flag_ddt = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];
         if (flag_ddt) {
           i_stv = ebe_lib[i_ebel].f_ddt_var_index[i_f];
           i_stv_ebeu = ebe_usr[i_ebeu].stv[i_stv];
           ebe_f_stv_index[i_ebeu][i_f] = i_stv_ebeu;
         }
       }
     }

     for (i_f=n_nd1; i_f < n_f1; i_f++) {
       row0 = ebe_f_to_row[i_ebeu][i_f];
       pntr_m = ddt_ebe_pntr[row0];

       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         var_flag   = ebe_lib[i_ebel].fvar_flag[i_f][i_fvar];
         var_number = ebe_usr[i_ebeu].fvar[i_f][i_fvar];
         flag_refnode = (var_flag == global.I_NV) && (var_number == cct.ref_nd);
         flag_gtref = (var_flag == global.I_NV) && (var_number > cct.ref_nd);

         if (!flag_refnode) {
           flag_ddt = ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar];

           if (flag_ddt) {
             ebe_rhs_ddt_pntr[row0] = pntr_m;
           }
         }
       }
     }
   }
   check_knuth(m_e,(char*)"mat_ssw_1_e0: matrix error");

   return;
} //end of SysMat::mat_ssw_1_e0
// -----------------------------------------------------------------------------
void SysMat::mat_ssw_1_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file) {

   int i_ebel,i_ebeu;
   int n_f1,n_fvar1;
   int i_f;
   int var_flag,var_number;
   int var_flag_stv,var_number_stv;

   n_statevar = 0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     for (i_f=0; i_f < n_f1; i_f++) {
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         if (ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar]) {
           ssw_flag1[n_statevar] = ebe_lib[i_ebel].fvar_flag[i_f][i_fvar];
           ssw_indx1[n_statevar] = ebe_usr[i_ebeu].fvar[i_f][i_fvar];
           n_statevar++;
         }
       }
     }
   }

   cout << "mat_ssw_1_e: n_statevar: " << n_statevar << endl;

   mat_ssw_1_e0(ebe_lib,ebe_usr,global,cct,cct_file);

   for (int i_statevar=0; i_statevar < n_statevar; i_statevar++) {
     var_flag = ssw_flag1[i_statevar];
     var_number = ssw_indx1[i_statevar];

     if (var_flag == global.I_NV) {
       ssw_indx2[i_statevar] = offs_nv + var_number;
       ssw_indx3[offs_nv + var_number] = i_statevar;
     } else if (var_flag == global.I_ESTV) {
       ssw_indx2[i_statevar] = offs_estv + var_number;
       ssw_indx3[offs_estv + var_number] = i_statevar;
     } else if (var_flag == global.I_EAUX) {
       ssw_indx2[i_statevar] = offs_eaux + var_number;
       ssw_indx3[offs_eaux + var_number] = i_statevar;
     } else {
       cout << "mat_ssw_1_e: var_flag not identified." << endl;
       cout << "   var_flag = " << var_flag << endl;
       cout << "   Halting..." << endl;
       exit(1);
     }
   }

   m_ssw.allocate_1(m_kcl.n_nz,(m_kcl.n_row+m_e.n_row),n_solvec_e);
   knuth_copy(m_kcl,m_ssw);
   knuth_append_2a(m_ssw,m_e);

   w_ssw.allocate_1(m_ssw.n_nz,m_ssw.n_row,m_ssw.n_col);
   mo_ssw.allocate_1(m_ssw.n_row);

   rhs_m_ssw = new double[m_ssw.n_row];
   rhs_w_ssw = new double[m_ssw.n_row];

   return;
} //end of SysMat::mat_ssw_1_e
// -----------------------------------------------------------------------------
void SysMat::mat_ssw_1_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file) {

   int i_ebel,i_ebeu;
   int i_xbel,i_xbeu;
   int n_f1,n_fvar1;
   int i_f;
   int var_flag,var_number;
   int var_flag_stv,var_number_stv;

   n_statevar = 0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     for (i_f=0; i_f < n_f1; i_f++) {
       n_fvar1 = ebe_lib[i_ebel].n_fvar[i_f];
       for (int i_fvar=0; i_fvar < n_fvar1; i_fvar++) {
         if (ebe_lib[i_ebel].fvar_ddt[i_f][i_fvar]) {
           ssw_flag1[n_statevar] = ebe_lib[i_ebel].fvar_flag[i_f][i_fvar];
           ssw_indx1[n_statevar] = ebe_usr[i_ebeu].fvar[i_f][i_fvar];
           n_statevar++;
         }
       }
     }
   }
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_f1 = xbe_lib[i_xbel].n_f;
     for (i_f=0; i_f < n_f1; i_f++) {
       ssw_flag1[n_statevar] = xbe_lib[i_xbel].ddt_varflag[i_f];
       ssw_indx1[n_statevar] = xbe_usr[i_xbeu].fvar[i_f];
       n_statevar++;
     }
   }

   cout << "mat_ssw_1_ex: n_statevar: " << n_statevar << endl;

   mat_ssw_1_e0(ebe_lib,ebe_usr,global,cct,cct_file);
   mat_trns_1_x0(xbe_lib,xbe_usr,cct,global,cct_file);

   for (int i_statevar=0; i_statevar < n_statevar; i_statevar++) {
     var_flag = ssw_flag1[i_statevar];
     var_number = ssw_indx1[i_statevar];

     if (var_flag == global.I_NV) {
       ssw_indx2[i_statevar] = offs_nv + var_number;
       ssw_indx3[offs_nv + var_number] = i_statevar;
     } else if (var_flag == global.I_ESTV) {
       ssw_indx2[i_statevar] = offs_estv + var_number;
       ssw_indx3[offs_estv + var_number] = i_statevar;
     } else if (var_flag == global.I_EAUX) {
       ssw_indx2[i_statevar] = offs_eaux + var_number;
       ssw_indx3[offs_eaux + var_number] = i_statevar;
     } else if (var_flag == global.I_XVR) {
       ssw_indx2[i_statevar] = offs_xvr + var_number;
       ssw_indx3[offs_xvr + var_number] = i_statevar;
     } else if (var_flag == global.I_XAUX) {
       ssw_indx2[i_statevar] = offs_xaux + var_number;
       ssw_indx3[offs_xaux + var_number] = i_statevar;
     } else {
       cout << "mat_ssw_1_ex: var_flag not identified." << endl;
       cout << "   var_flag = " << var_flag << endl;
       cout << "   Halting..." << endl;
       exit(1);
     }
   }

   m_ssw.allocate_1(m_kcl.n_nz,(m_kcl.n_row+m_e.n_row+m_x.n_row),n_solvec_ex);
   knuth_copy(m_kcl,m_ssw);
   knuth_append_2a(m_ssw,m_e);
   knuth_append_2a(m_ssw,m_x);

   w_ssw.allocate_1(m_ssw.n_nz,m_ssw.n_row,m_ssw.n_col);
   mo_ssw.allocate_1(m_ssw.n_row);

   rhs_m_ssw = new double[m_ssw.n_row];
   rhs_w_ssw = new double[m_ssw.n_row];

   if (m_ssw.n_row != m_ssw.n_col) {
     cout << "mat_ssw_1_ex: system matrix is not square!" << endl;
     cout << "  m_ssw.n_row = " << m_ssw.n_row << endl;
     cout << "  m_ssw.n_col = " << m_ssw.n_col << endl;
     cout << "  Halting..." << endl;
     exit(1);
   }
   cout << "mat_ssw_1_ex: system matrix size is "
        << m_ssw.n_row << " x " << m_ssw.n_col << endl;

   return;
} //end of SysMat::mat_ssw_1_ex
// -----------------------------------------------------------------------------
void SysMat::mat_ssw_1_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file) {

   int i_xbel,i_xbeu;
   int n_f1,n_fvar1;
   int i_f;
   int var_flag,var_number;

// see comments in mat_ssw_1_ex if required

   n_statevar = 0;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     cout << "mat_ssw_1_x: xbe name: " << xbe_lib[i_xbel].name << endl;
     n_f1 = xbe_lib[i_xbel].n_f;
     for (i_f=0; i_f < n_f1; i_f++) {
       ssw_flag1[n_statevar] = xbe_lib[i_xbel].ddt_varflag[i_f];
       ssw_indx1[n_statevar] = xbe_usr[i_xbeu].fvar[i_f];
       n_statevar++;
     }
   }

   cout << "mat_ssw_1_x: n_statevar: " << n_statevar << endl;

   mat_trns_1_x0(xbe_lib,xbe_usr,cct,global,cct_file);

   for (int i_statevar=0; i_statevar < n_statevar; i_statevar++) {
     var_flag = ssw_flag1[i_statevar];
     var_number = ssw_indx1[i_statevar];

     if (var_flag == global.I_XVR) {
       ssw_indx2[i_statevar] = offs_xvr + var_number;
       ssw_indx3[offs_xvr + var_number] = i_statevar;
     } else if (var_flag == global.I_XAUX) {
       ssw_indx2[i_statevar] = offs_xaux + var_number;
       ssw_indx3[offs_xaux + var_number] = i_statevar;
     } else {
       cout << "mat_ssw_1_x: var_flag not identified." << endl;
       cout << "   var_flag = " << var_flag << endl;
       cout << "   Halting..." << endl;
       exit(1);
     }
   }

   m_ssw.allocate_1(m_x.n_nz,m_x.n_row,n_solvec_x);
   knuth_copy(m_x,m_ssw);

   w_ssw.allocate_1(m_ssw.n_nz,m_ssw.n_row,m_ssw.n_col);
   mo_ssw.allocate_1(m_ssw.n_row);

   rhs_m_ssw = new double[m_ssw.n_row];
   rhs_w_ssw = new double[m_ssw.n_row];

   if (m_ssw.n_row != m_ssw.n_col) {
     cout << "mat_ssw_1_x: system matrix is not square!" << endl;
     cout << "  m_ssw.n_row = " << m_ssw.n_row << endl;
     cout << "  m_ssw.n_col = " << m_ssw.n_col << endl;
     cout << "  Halting..." << endl;
     exit(1);
   }
   cout << "mat_ssw_1_x: system matrix size is "
        << m_ssw.n_row << " x " << m_ssw.n_col << endl;

   return;
} //end of SysMat::mat_ssw_1_x
// -----------------------------------------------------------------------------
void SysMat::get_offs_new(
   const int var_number,
   int &offs_new,
   int &var_number_new,
   Circuit &cct) {

   int i_xvr_in,i_xvr_out;

   i_xvr_in = cct.map2_xvr_ebe_in[var_number];
   if (i_xvr_in == -1) {
     i_xvr_out = cct.map2_xvr_ebe_out[var_number];
     if (i_xvr_out == -1) {
       cout << "SysMat::get_offs_new: i_xvr_in and i_xvr_out are both -1?" << endl;
       cout << "  var_number = " << var_number << endl;
       cout << "  Halting..." << endl; exit(1);
     } else {
       offs_new = offs_xvr_out;
       var_number_new = i_xvr_out;
     }
   } else {
     offs_new = offs_xvr_in;
     var_number_new = i_xvr_in;
   }

   return;
} // end of SysMat::get_offs_new
// -----------------------------------------------------------------------------
void SysMat::ssw_allocate_1(
   vector<EbeLib> &ebe_lib,
   vector<XbeLib> &xbe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct) {

   int i_ebeu,i_xbeu;
   int i_ebel,i_xbel;
   int n_f1;
   int i_f;

   n_statevar = 0;

// state vars coming from ebe's:

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_f1 = ebe_lib[i_ebel].n_f;
     for (i_f=0; i_f < n_f1; i_f++) {
       if (ebe_lib[i_ebel].f_ddt[i_f]) n_statevar++;
     }
   }

// state vars coming from xbe's:
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) n_statevar += xbe_lib[i_xbel].n_f;
   }

   offs_ssw  = new int[n_statevar];
   ssw_flag1 = new int[n_statevar];
   ssw_indx1 = new int[n_statevar];
   ssw_indx2 = new int[n_statevar];

   assign_all_int_1(offs_ssw,n_statevar,0);
   assign_all_int_1(ssw_flag1,n_statevar,0);
   assign_all_int_1(ssw_indx1,n_statevar,0);
   assign_all_int_1(ssw_indx2,n_statevar,0);

   svec_ssw_2     = new double[n_statevar];
   svec_ssw_2_old = new double[n_statevar];
   delsvec_ssw_2  = new double[n_statevar];
   rhs_ssw        = new double[n_statevar];
   ssw_rhs        = new double[n_statevar];

   assign_all_double_1(svec_ssw_2    ,n_statevar,0.0);
   assign_all_double_1(svec_ssw_2_old,n_statevar,0.0);
   assign_all_double_1(delsvec_ssw_2 ,n_statevar,0.0);
   assign_all_double_1(rhs_ssw       ,n_statevar,0.0);
   assign_all_double_1(ssw_rhs       ,n_statevar,0.0);

   ssw_mat_pntr = new double[n_statevar*n_statevar];
   assign_all_double_1(ssw_mat_pntr,n_statevar*n_statevar,0.0);

   ssw_mat = new double*[n_statevar];

   for (int k = 0; k < n_statevar; k++) {
     ssw_mat[k] = ssw_mat_pntr + (k*n_statevar);
     ssw_mat[k] = new double[n_statevar];
   }

   ssw_mat_pntr_1 = new double[n_statevar*n_statevar];
   assign_all_double_1(ssw_mat_pntr_1,n_statevar*n_statevar,0.0);

   ssw_mat_1 = new double*[n_statevar];

   for (int k = 0; k < n_statevar; k++) {
     ssw_mat_1[k] = ssw_mat_pntr_1 + (k*n_statevar);
     ssw_mat_1[k] = new double[n_statevar];
   }

   indxc_ssw = new int[n_statevar];
   indxr_ssw = new int[n_statevar];
   ipiv_ssw  = new int[n_statevar];

   assign_all_int_1(indxc_ssw,n_statevar,0);
   assign_all_int_1(indxr_ssw,n_statevar,0);
   assign_all_int_1(ipiv_ssw ,n_statevar,0);

   ssw_trz_1_pntr = new double[n_statevar*n_statevar];
   assign_all_double_1(ssw_trz_1_pntr,n_statevar*n_statevar,0.0);

   ssw_trz_1 = new double*[n_statevar];

   for (int k = 0; k < n_statevar; k++) {
     ssw_trz_1[k] = ssw_trz_1_pntr + (k*n_statevar);
     ssw_trz_1[k] = new double[n_statevar];
   }

   cout << "ssw_allocate_1 (2): n_statevar=" << n_statevar << endl;

   return;
} // end of SysMat::ssw_allocate_1
// -----------------------------------------------------------------------------
void SysMat::ssw_allocate_2(
   vector<EbeLib> &ebe_lib,
   vector<XbeLib> &xbe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct) {

   int n_solvec0,nrow_m0;

   if (cct.flag_x_only) {
     n_solvec0 = n_solvec_x;
     nrow_m0 = cct.x_n_ttlg;
   } else if (cct.flag_e_only) {
     n_solvec0 = n_solvec_e;
     nrow_m0 = cct.n_ebeu_nd - 1 + nrow_ebce_1;
   } else {
     n_solvec0 = n_solvec_ex;
     nrow_m0 = cct.n_ebeu_nd - 1 + nrow_ebce_1 + cct.x_n_ttlg;
   }

   svec_ssw_1 = new double[n_solvec0*n_statevar];

   ssw_indx3 = new int[nrow_m0];

   assign_all_double_1(svec_ssw_1,n_solvec0*n_statevar,0.0);
   assign_all_int_1(ssw_indx3,nrow_m0,-1);

   return;
} // end of SysMat::ssw_allocate_2
// -----------------------------------------------------------------------------
void SysMat::kcl_allocate_1(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct) {

   int n_nz,n_row,n_col;
   int i_ebeu,i_ebel,n_nd;
   int nd0;

   n_row = cct.n_ebeu_nd - 1;
   if (cct.flag_e_only) {
     n_col = n_solvec_e;
   } else if (cct.flag_exc) {
     n_col = n_solvec_ex;
   }
   n_nz = 0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd = ebe_lib[i_ebel].n_nd;

     for (int i=0; i < n_nd; i++) {
       nd0 = ebe_usr[i_ebeu].nd[i];
       if (nd0 != cct.ref_nd) {
         n_nz++;
       }
     }
   }
   m_kcl.allocate_1(n_nz,n_row,n_col);

   return;
} // end of SysMat::kcl_allocate_1
// -----------------------------------------------------------------------------
void SysMat::kcl_mat_2(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Global &global,
   Circuit &cct,
   CctFile &cct_file) {

   int i_ebel,i_ebeu;
   int n_nd1,nd0;
   int row0,col0;
   int pntr;

   knuth_zero_1(m_kcl);
   pntr = 0;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;

     for (int i=0; i < n_nd1; i++) {
       nd0 = ebe_usr[i_ebeu].nd[i];
       if (nd0 != cct.ref_nd) {
         if (nd0 < cct.ref_nd) {
           row0 = nd0;
         } else {
           row0 = nd0 - 1;
         }

         col0 = offs_ndcur + pntr;
         knuth_addentry(m_kcl,row0,col0,1.0);
       }
       pntr++;
     }
   }
   check_knuth(m_kcl,(char*)"kcl_mat_2: matrix error");

   return;
} //end of SysMat::kcl_mat_2
// -----------------------------------------------------------------------------
void SysMat::delete_1(
   SolveBlocks &slv) {

   if (map_jac_ebe_to_m != NULL) {
     delete[] map_jac_ebe_to_m; map_jac_ebe_to_m = NULL;
   }

   if (rhs_m_e != NULL) {
     delete[] rhs_m_e; rhs_m_e = NULL;
   }
   if (rhs_w_e != NULL) {
     delete[] rhs_w_e; rhs_w_e = NULL;
   }
   if (rhs_m_x != NULL) {
     delete[] rhs_m_x; rhs_m_x = NULL;
   }
   if (rhs_w_x != NULL) {
     delete[] rhs_w_x; rhs_w_x = NULL;
   }
   if (rhs_m_ex != NULL) {
     delete[] rhs_m_ex; rhs_m_ex = NULL;
   }
   if (rhs_w_ex != NULL) {
     delete[] rhs_w_ex; rhs_w_ex = NULL;
   }
   if (rhs_m_ssw != NULL) {
     delete[] rhs_m_ssw; rhs_m_ssw = NULL;
   }
   if (rhs_w_ssw != NULL) {
     delete[] rhs_w_ssw; rhs_w_ssw = NULL;
   }

   if (flag_delete) {
     delete[] svec_e;
     delete[] svec_ex;
     delete[] svec_x;

     delete[] svec_w_e;
     delete[] svec_w_ex;
     delete[] svec_w_x;

     delete[] svec_orig_e;
     delete[] svec_orig_ex;
     delete[] svec_orig_x;

     delete[] svec_old_nr_1_e;
     delete[] svec_old_nr_2_e;
     delete[] svec_old_nr_1_ex;
     delete[] svec_old_nr_2_ex;

     delete[] svec_old_1_e;
     delete[] svec_old_2_e;
     delete[] svec_old_1_ex;
     delete[] svec_old_2_ex;
     delete[] svec_old_1_x;
     delete[] svec_old_2_x;

     delete[] delsvec_e;
     delete[] delsvec_ex;
     delete[] delsvec_x;

     delete[] tol_spice_e;
     delete[] norm_spice_e;

     delete[] ebe_rhs_ddt_flag;
     delete[] ebe_rhs_ddt_varnumber;
     delete[] ebe_rhs_ddt_pntr;
     delete[] ebe_rhs_ddt_varflag;
     delete[] ssw_ebe_rhs_flag;
     delete[] ebe_rhs_ddt_i_ebeu;
     delete[] ebe_rhs_ddt_i_f;
     delete[] ebe_rhs_ddt_i_g;

     delete[] ebe_stv_index;
     delete[] flag_ebe_stv;
     delete[] ddt_ebe_pntr;

     delete[] flag_ebe_ddt;

     flag_alloc = true; flag_delete = false;
   } else {
     cout << "SysMat::delete_1: trying to delete" << endl;
     cout << "  with flag_delete = false? Halting..." << endl; exit(1);
   }

   if (m_e.flag_delete) m_e.delete_1();
   if (w_e.flag_delete) w_e.delete_1();

   if (m_x.flag_delete) m_x.delete_1();
   if (w_x.flag_delete) w_x.delete_1();

   if (m_ex.flag_delete) m_ex.delete_1();
   if (w_ex.flag_delete) w_ex.delete_1();

   if (m_ssw.flag_delete) m_ssw.delete_1();
   if (w_ssw.flag_delete) w_ssw.delete_1();

   if (m_kcl.flag_delete) m_kcl.delete_1();

   if (mo_e.flag_delete) mo_e.delete_1(m_e.n_row);
   if (mo_x.flag_delete) mo_x.delete_1(m_x.n_row);
   if (mo_ex.flag_delete) mo_ex.delete_1(m_ex.n_row);
   if (mo_ssw.flag_delete) mo_ssw.delete_1(m_ssw.n_row);

   return;
} // end of SysMat::delete_1
// -----------------------------------------------------------------------------
void SysMat::ssw_delete_1() {

   delete[] offs_ssw;
   delete[] ssw_flag1;
   delete[] ssw_indx1;
   delete[] ssw_indx2;

   delete[] svec_ssw_2;
   delete[] svec_ssw_2_old;
   delete[] delsvec_ssw_2;
   delete[] rhs_ssw;
   delete[] ssw_rhs;

   delete[] ssw_mat_pntr;

   for (int i=0; i < n_statevar; i++) {
     delete[] ssw_mat[i];
   }
   delete[] ssw_mat;

   delete[] ssw_mat_pntr_1;

   for (int i=0; i < n_statevar; i++) {
     delete[] ssw_mat_1[i];
   }
   delete[] ssw_mat_1;

   delete[] indxc_ssw;
   delete[] indxr_ssw;
   delete[] ipiv_ssw;

   delete[] ssw_trz_1_pntr;

   for (int i=0; i < n_statevar; i++) {
     delete[] ssw_trz_1[i];
   }
   delete[] ssw_trz_1;

   delete[] svec_ssw_1;
   delete[] ssw_indx3;

   return;
} // end of SysMat::ssw_delete_1
// -----------------------------------------------------------------------------
void SysMat::copy_svec_to_previous() {

   if (svec_e_previous  != NULL) delete[] svec_e_previous;
   if (svec_x_previous  != NULL) delete[] svec_x_previous;
   if (svec_ex_previous != NULL) delete[] svec_ex_previous;

   svec_e_previous  = new double[n_solvec_e];
   svec_x_previous  = new double[n_solvec_x];
   svec_ex_previous = new double[n_solvec_ex];

   copy_array_1<double>(n_solvec_e ,svec_e ,svec_e_previous);
   copy_array_1<double>(n_solvec_x ,svec_x ,svec_x_previous);
   copy_array_1<double>(n_solvec_ex,svec_ex,svec_ex_previous);

   return;
} // end of SysMat::copy_svec_to_previous
