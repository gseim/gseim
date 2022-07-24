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

#include "routines2.h"

// -----------------------------------------------------------------------------
void clear_solvec_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_ebeu,i_ebel,n_nd1,n_aux1,n_auxs1,n_stv1,n_xvr1;

   for (int i=0; i < cct.n_ebeu_nd; i++) {
    cct.val_nd[i] = 0.0;
   }

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;

     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (int i=0; i < n_nd1; i++) {
       ebe_usr[i_ebeu].val_nd[i] = 0.0;
       ebe_usr[i_ebeu].val_nd_new[i] = 0.0;
     }

     n_aux1 = ebe_lib[i_ebel].n_aux;
     for (int i=0; i < n_aux1; i++) {
       ebe_usr[i_ebeu].val_aux[i] = 0.0;
       ebe_usr[i_ebeu].val_aux_new[i] = 0.0;
     }

     n_auxs1 = ebe_lib[i_ebel].n_auxs;
     for (int i=0; i < n_auxs1; i++) {
       ebe_usr[i_ebeu].val_auxs[i] = 0.0;
       ebe_usr[i_ebeu].val_auxs_new[i] = 0.0;
     }

     n_stv1 = ebe_lib[i_ebel].n_stv;
     for (int i=0; i < n_stv1; i++) {
       ebe_usr[i_ebeu].val_stv[i] = 0.0;
     }

     n_xvr1 = ebe_lib[i_ebel].n_xvr;
     for (int i=0; i < n_xvr1; i++) {
       ebe_usr[i_ebeu].val_xvr[i] = 0.0;
     }
   }
   for (int i=0; i < smat.n_solvec_e; i++) {
     smat.svec_e[i] = 0.0;
   }

   return;
} // end of clear_solvec_e
// -----------------------------------------------------------------------------
void clear_solvec_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_xbeu,i_xbel,n_vr1,n_aux1;

   for (int i=0; i < cct.n_xbeu_vr; i++) {
    cct.val_xvr[i] = 0.0;
   }

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     n_vr1 = xbe_lib[i_xbel].n_vr;
     for (int i=0; i < n_vr1; i++) {
       xbe_usr[i_xbeu].val_vr[i] = 0.0;
       xbe_usr[i_xbeu].val_vr_new[i] = 0.0;
     }

     n_aux1 = xbe_lib[i_xbel].n_aux;
     for (int i=0; i < n_aux1; i++) {
       xbe_usr[i_xbeu].val_aux[i] = 0.0;
       xbe_usr[i_xbeu].val_aux_new[i] = 0.0;
     }
   }
   for (int i=0; i < smat.n_solvec_x; i++) {
     smat.svec_x[i] = 0.0;
   }

   return;
} // end of clear_solvec_x
// -----------------------------------------------------------------------------
void clear_solvec_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SysMat &smat) {

   int i_ebeu,i_ebel,n_nd1,n_aux1,n_auxs1,n_stv1,n_xvr1;
   int i_xbeu,i_xbel,n_vr1;

   for (int i=0; i < cct.n_ebeu_nd; i++) {
    cct.val_nd[i] = 0.0;
   }

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;

     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (int i=0; i < n_nd1; i++) {
       ebe_usr[i_ebeu].val_nd[i] = 0.0;
       ebe_usr[i_ebeu].val_nd_new[i] = 0.0;
     }
     n_aux1 = ebe_lib[i_ebel].n_aux;
     for (int i=0; i < n_aux1; i++) {
       ebe_usr[i_ebeu].val_aux[i] = 0.0;
       ebe_usr[i_ebeu].val_aux_new[i] = 0.0;
     }
     n_auxs1 = ebe_lib[i_ebel].n_auxs;
     for (int i=0; i < n_auxs1; i++) {
       ebe_usr[i_ebeu].val_auxs[i] = 0.0;
       ebe_usr[i_ebeu].val_auxs_new[i] = 0.0;
     }
     n_stv1 = ebe_lib[i_ebel].n_stv;
     for (int i=0; i < n_stv1; i++) {
       ebe_usr[i_ebeu].val_stv[i] = 0.0;
     }
     n_xvr1 = ebe_lib[i_ebel].n_xvr;
     for (int i=0; i < n_xvr1; i++) {
       ebe_usr[i_ebeu].val_xvr[i] = 0.0;
     }
   }

   for (int i=0; i < cct.n_xbeu_vr; i++) {
    cct.val_xvr[i] = 0.0;
   }
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     n_vr1 = xbe_lib[i_xbel].n_vr;
     for (int i=0; i < n_vr1; i++) {
       xbe_usr[i_xbeu].val_vr[i] = 0.0;
       xbe_usr[i_xbeu].val_vr_new[i] = 0.0;
     }

     n_aux1 = xbe_lib[i_xbel].n_aux;
     for (int i=0; i < n_aux1; i++) {
       xbe_usr[i_xbeu].val_aux[i] = 0.0;
       xbe_usr[i_xbeu].val_aux_new[i] = 0.0;
     }
   }

   for (int i=0; i < smat.n_solvec_ex; i++) {
     smat.svec_ex[i] = 0.0;
   }

   return;
} // end of clear_solvec_ex
// -----------------------------------------------------------------------------
void clear_solvec_x_1(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct) {

   int i_xbeu,i_xbel,n_vr1,n_aux1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     n_vr1 = xbe_lib[i_xbel].n_vr;
     for (int i=0; i < n_vr1; i++) {
       xbe_usr[i_xbeu].val_vr_0[i] = 0.0;
     }
     n_aux1 = xbe_lib[i_xbel].n_aux;
     for (int i=0; i < n_aux1; i++) {
       xbe_usr[i_xbeu].val_aux_0[i] = 0.0;
     }
   }

   return;
} // end of clear_solvec_x_1
// -----------------------------------------------------------------------------
void cct_to_xbe_1(
   const int i_xbeu,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct) {

// assign xbe_usr.val_vr from cct.val_xvr

   int i_xbel,i_xbeu_vr,n_vr1;

   i_xbel = xbe_usr[i_xbeu].index_xbel;

   n_vr1 = xbe_lib[i_xbel].n_vr;
   for (int i=0; i < n_vr1; i++) {
     i_xbeu_vr = xbe_usr[i_xbeu].vr[i];
     xbe_usr[i_xbeu].val_vr[i] = cct.val_xvr[i_xbeu_vr];
   }

   return;
} // end of cct_to_xbe_1
// -----------------------------------------------------------------------------
void cct_to_xbe_all(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct) {

   int i_xbeu,i_xbel,i_xbeu_vr,n_vr1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_vr1 = xbe_lib[i_xbel].n_vr;
     for (int i=0; i < n_vr1; i++) {
       i_xbeu_vr = xbe_usr[i_xbeu].vr[i];
       xbe_usr[i_xbeu].val_vr[i] = cct.val_xvr[i_xbeu_vr];
     }
   }
   return;
} // end of cct_to_xbe_all
// -----------------------------------------------------------------------------
void cct_to_ebe_nd_1(
   const int i_ebeu,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct) {

// assign ebe_usr.val_nd from cct.val_nd

   int i_ebel,i_ebeu_nd,n_nd1;

   i_ebel = ebe_usr[i_ebeu].index_ebel;

   n_nd1 = ebe_lib[i_ebel].n_nd;
   for (int i=0; i < n_nd1; i++) {
     i_ebeu_nd = ebe_usr[i_ebeu].nd[i];
     ebe_usr[i_ebeu].val_nd[i] = cct.val_nd[i_ebeu_nd];
   }

   return;
} // end of cct_to_ebe_nd_1
// -----------------------------------------------------------------------------
void cct_to_ebe_nd_all(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct) {

// assign ebe_usr.val_nd from cct.val_nd

   int i_ebeu,i_ebel,i_ebeu_nd,n_nd1;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;

     for (int i=0; i < n_nd1; i++) {
       i_ebeu_nd = ebe_usr[i_ebeu].nd[i];
       ebe_usr[i_ebeu].val_nd[i] = cct.val_nd[i_ebeu_nd];
//     cout << "cct_to_ebe_nd_all: i_ebeu = " << i_ebeu
//       << ", i_ebeu_nd = " << i_ebeu_nd
//       << ", i = " << i << ", val_nd = " << ebe_usr[i_ebeu].val_nd[i]
//       << endl;
     }
   }

   return;
} // end of cct_to_ebe_nd_all
// -----------------------------------------------------------------------------
void cct_to_ebe_xvr_all(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct) {

// assign ebe_usr.val_xvr from cct.val_xvr

   int i_ebeu,i_ebel,i_xbeu_vr,n_xvr1;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_xvr1 = ebe_lib[i_ebel].n_xvr;

     for (int i=0; i < n_xvr1; i++) {
       i_xbeu_vr = ebe_usr[i_ebeu].xvr[i];
       ebe_usr[i_ebeu].val_xvr[i] = cct.val_xvr[i_xbeu_vr];
     }
   }

   return;
} // end of cct_to_ebe_xvr_all
// -----------------------------------------------------------------------------
void xbe_to_cct_op_1(
   const int i_xbeu,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct) {

   int i_xbel,i_xbeu_vr,n_ipvr1,n_vr1;

   i_xbel = xbe_usr[i_xbeu].index_xbel;
   n_vr1 = xbe_lib[i_xbel].n_vr;
   n_ipvr1 = xbe_lib[i_xbel].n_ipvr;

   for (int i = n_ipvr1; i < n_vr1; i++) {
     i_xbeu_vr = xbe_usr[i_xbeu].vr[i];
     cct.val_xvr[i_xbeu_vr] = xbe_usr[i_xbeu].val_vr[i];
   }

   return;
} // end of xbe_to_cct_op_1
// -----------------------------------------------------------------------------
void xbe_to_cct_1(
   const int i_xbeu,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct) {

   int i_xbel,i_xbeu_vr,n_vr1;

   i_xbel = xbe_usr[i_xbeu].index_xbel;
   n_vr1 = xbe_lib[i_xbel].n_vr;

   for (int i=0; i < n_vr1; i++) {
     i_xbeu_vr = xbe_usr[i_xbeu].vr[i];
     cct.val_xvr[i_xbeu_vr] = xbe_usr[i_xbeu].val_vr[i];
   }

   return;
} // end of xbe_to_cct_1
// -----------------------------------------------------------------------------
void xbe_to_cct_all(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct) {

   int i_xbeu,i_xbel,i_xbeu_vr,n_vr1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_vr1 = xbe_lib[i_xbel].n_vr;

     for (int i=0; i < n_vr1; i++) {
       i_xbeu_vr = xbe_usr[i_xbeu].vr[i];
       cct.val_xvr[i_xbeu_vr] = xbe_usr[i_xbeu].val_vr[i];
     }
   }
   return;
} // end of xbe_to_cct_all
// -----------------------------------------------------------------------------
void assign_xvr_ebe_in(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct) {

   int i_xbeu_vr,i_xbeu_vr1;
   int i_ebeu,i_ebel;
   int n_xvr1;
   double val1;

// process xvr's which are inputs to ebe's

   for (int i=0; i < cct.n_xvr_ebe_in; i++) {
     i_xbeu_vr = cct.map1_xvr_ebe_in[i];
     val1 = cct.val_xvr[i_xbeu_vr];

     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_xvr1 = ebe_lib[i_ebel].n_xvr;
       for (int i_xvr=0; i_xvr < n_xvr1; i_xvr++) {
         i_xbeu_vr1 = ebe_usr[i_ebeu].xvr[i_xvr];
         if (i_xbeu_vr1 == i_xbeu_vr) {
           ebe_usr[i_ebeu].val_xvr[i_xvr] = val1;
         }
       }
     }
   }

   return;
} // end of assign_xvr_ebe_in
// -----------------------------------------------------------------------------
void get_xbe_1(
   const int i_xbeu,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   Global &global) {

   int i_xbel;

   i_xbel = xbe_usr[i_xbeu].index_xbel;

// assign xbe_usr.val_vr from cct.val_xvr
   cct_to_xbe_1(i_xbeu,xbe_lib,xbe_usr,cct);

   get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbeu]);

// assign cct.val_xvr from xbe_usr.val_vr only for the output nodes
   xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);

   return;
} // end of get_xbe_1
// -----------------------------------------------------------------------------
void form_solvec_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   SolveBlocks &slv,
   Circuit &cct) {

   int i_svec;
   int i_ebeu,i_ebel,i_ebeu_nd;
   int n_aux1,n_auxs1;

   i_svec = smat.offs_nv;
   for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
     if (i_ebeu_nd != cct.ref_nd) {
       smat.svec_e[i_svec] = cct.val_nd[i_ebeu_nd];
       i_svec++;
     }
   }
   if (slv.flag_startup) {
     i_svec = smat.offs_eauxs;
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_auxs1 = ebe_lib[i_ebel].n_auxs;

       for (int i=0; i < n_auxs1; i++) {
         smat.svec_e[i_svec] = ebe_usr[i_ebeu].val_auxs[i];
         i_svec++;
       }
     }
   } else {
     i_svec = smat.offs_eaux;
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_aux1 = ebe_lib[i_ebel].n_aux;

       for (int i=0; i < n_aux1; i++) {
         smat.svec_e[i_svec] = ebe_usr[i_ebeu].val_aux[i];
         i_svec++;
       }
     }
   }
   return;
} // end of form_solvec_e
// -----------------------------------------------------------------------------
void form_solvec_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   SysMat &smat,
   Circuit &cct) {

   int i_svec;
   int i_xbeu,i_xbel,i_xbeu_vr;
   int n_aux1;

   i_svec = smat.offs_xvr;
   for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
     smat.svec_x[i_svec] = cct.val_xvr[i_xbeu_vr];
     i_svec++;
   }
   i_svec = smat.offs_xaux;
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_aux1 = xbe_lib[i_xbel].n_aux;

     for (int i=0; i < n_aux1; i++) {
       smat.svec_x[i_svec] = xbe_usr[i_xbeu].val_aux[i];
       i_svec++;
     }
   }
   return;
} // end of form_solvec_x
// -----------------------------------------------------------------------------
void form_map_xbeuvr_1(
   SysMat &smat,
   Circuit &cct) {

   int i_svec;
   int i_xbeu,i_xbel,i_xbeu_vr;

   i_svec = smat.offs_xvr;
   for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
     cct.map_xbeuvr_to_svec[i_xbeu_vr] = i_svec;
     i_svec++;
   }
   return;
} // end of form_map_xbeuvr_1
// -----------------------------------------------------------------------------
void form_solvec_ex(
   vector<EbeLib> &ebe_lib,
   vector<XbeLib> &xbe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeUsr> &xbe_usr,
   SysMat &smat,
   SolveBlocks &slv,
   Circuit &cct) {

   int i_svec;
   int i_ebeu,i_ebel,i_ebeu_nd;
   int i_xbeu,i_xbel,i_xbeu_vr;
   int n_aux1,n_auxs1;

   i_svec = smat.offs_nv;
   for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
     if (i_ebeu_nd != cct.ref_nd) {
       smat.svec_ex[i_svec] = cct.val_nd[i_ebeu_nd];
       i_svec++;
     }
   }
   if (slv.flag_startup) {
     i_svec = smat.offs_eauxs;
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_auxs1 = ebe_lib[i_ebel].n_auxs;

       for (int i=0; i < n_auxs1; i++) {
         smat.svec_ex[i_svec] = ebe_usr[i_ebeu].val_auxs[i];
         i_svec++;
       }
     }
   } else {
     i_svec = smat.offs_eaux;
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_aux1 = ebe_lib[i_ebel].n_aux;

       for (int i=0; i < n_aux1; i++) {
         smat.svec_ex[i_svec] = ebe_usr[i_ebeu].val_aux[i];
         i_svec++;
       }
     }
   }
   i_svec = smat.offs_xvr;
   for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
     smat.svec_ex[i_svec] = cct.val_xvr[i_xbeu_vr];
     i_svec++;
   }
   i_svec = smat.offs_xaux;
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_aux1 = xbe_lib[i_xbel].n_aux;

     for (int i=0; i < n_aux1; i++) {
       smat.svec_ex[i_svec] = xbe_usr[i_xbeu].val_aux[i];
       i_svec++;
     }
   }
   return;
} // end of form_solvec_ex
// -----------------------------------------------------------------------------
void dcmp_solvec_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   SolveBlocks &slv,
   Circuit &cct) {

   int i_svec;
   int i_ebeu,i_ebel,i_ebeu_nd;
   int n_aux1,n_auxs1;

   i_svec = smat.offs_nv;
   for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
     if (i_ebeu_nd != cct.ref_nd) {
       cct.val_nd[i_ebeu_nd] = smat.svec_e[i_svec];
       i_svec++;
     }
   }
   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);

   if (slv.flag_startup) {
     i_svec = smat.offs_eauxs;
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_auxs1 = ebe_lib[i_ebel].n_auxs;
       for (int i=0; i < n_auxs1; i++) {
         ebe_usr[i_ebeu].val_auxs[i] = smat.svec_e[i_svec];
         i_svec++;
       }
     }
   } else {
     i_svec = smat.offs_eaux;
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_aux1 = ebe_lib[i_ebel].n_aux;
       for (int i=0; i < n_aux1; i++) {
         ebe_usr[i_ebeu].val_aux[i] = smat.svec_e[i_svec];
         i_svec++;
       }
     }
   }
   return;
} // end of dcmp_solvec_e
// -----------------------------------------------------------------------------
void dcmp_solvec_ssw_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   SolveBlocks &slv,
   Circuit &cct) {

   int i_svec;
   int i_ebeu,i_ebel,i_ebeu_nd;
   int n_aux1,n_stv1,n_nd1;

   i_svec = smat.offs_nv;
   for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
     if (i_ebeu_nd != cct.ref_nd) {
       cct.val_nd[i_ebeu_nd] = smat.svec_e[i_svec];
       i_svec++;
     }
   }
   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);

   i_svec = smat.offs_eaux;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_aux1 = ebe_lib[i_ebel].n_aux;
     for (int i=0; i < n_aux1; i++) {
       ebe_usr[i_ebeu].val_aux[i] = smat.svec_e[i_svec];
       i_svec++;
     }
   }

   i_svec = smat.offs_estv;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_stv1 = ebe_lib[i_ebel].n_stv;
     for (int i=0; i < n_stv1; i++) {
       ebe_usr[i_ebeu].val_stv[i] = smat.svec_e[i_svec];
       i_svec++;
     }
   }

   i_svec = smat.offs_ndcur;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (int i=0; i < n_stv1; i++) {
       ebe_usr[i_ebeu].cur_nd[i] = smat.svec_e[i_svec];
       i_svec++;
     }
   }

   return;
} // end of dcmp_solvec_ssw_e
// -----------------------------------------------------------------------------
void dcmp_solvec_ssw_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   SolveBlocks &slv,
   Circuit &cct) {

   int i_svec;
   int i_ebeu,i_ebel,i_ebeu_nd;
   int i_xbeu,i_xbel,i_xbeu_vr;
   int n_aux1,n_stv1,n_nd1;

   i_svec = smat.offs_nv;
   for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
     if (i_ebeu_nd != cct.ref_nd) {
       cct.val_nd[i_ebeu_nd] = smat.svec_ex[i_svec];
       i_svec++;
     }
   }
   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);

   i_svec = smat.offs_eaux;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_aux1 = ebe_lib[i_ebel].n_aux;
     for (int i=0; i < n_aux1; i++) {
       ebe_usr[i_ebeu].val_aux[i] = smat.svec_ex[i_svec];
       i_svec++;
     }
   }

   i_svec = smat.offs_estv;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_stv1 = ebe_lib[i_ebel].n_stv;
     for (int i=0; i < n_stv1; i++) {
       ebe_usr[i_ebeu].val_stv[i] = smat.svec_ex[i_svec];
       i_svec++;
     }
   }

   i_svec = smat.offs_ndcur;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (int i=0; i < n_nd1; i++) {
       ebe_usr[i_ebeu].cur_nd[i] = smat.svec_ex[i_svec];
       i_svec++;
     }
   }

   i_svec = smat.offs_xvr;
   for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
     cct.val_xvr[i_xbeu_vr] = smat.svec_ex[i_svec];
     i_svec++;
   }
   cct_to_xbe_all(xbe_lib,xbe_usr,cct);

   i_svec = smat.offs_xaux;
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_aux1 = xbe_lib[i_xbel].n_aux;
     for (int i=0; i < n_aux1; i++) {
       xbe_usr[i_xbeu].val_aux[i] = smat.svec_ex[i_svec];
       i_svec++;
     }
   }
   return;
} // end of dcmp_solvec_ssw_ex
// -----------------------------------------------------------------------------
void dcmp_solvec_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   SysMat &smat,
   Circuit &cct) {

   int i_svec;
   int i_xbeu,i_xbel,i_xbeu_vr;
   int n_aux1;

   i_svec = smat.offs_xvr;
   for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
     cct.val_xvr[i_xbeu_vr] = smat.svec_x[i_svec];
     i_svec++;
   }
   cct_to_xbe_all(xbe_lib,xbe_usr,cct);

   i_svec = smat.offs_xaux;
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_aux1 = xbe_lib[i_xbel].n_aux;
     for (int i=0; i < n_aux1; i++) {
       xbe_usr[i_xbeu].val_aux[i] = smat.svec_x[i_svec];
       i_svec++;
     }
   }
   return;
} // end of dcmp_solvec_x
// -----------------------------------------------------------------------------
void dcmp_solvec_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   SolveBlocks &slv,
   Circuit &cct) {

   int i_svec;
   int i_ebeu,i_ebel,i_ebeu_nd;
   int i_xbeu,i_xbel,i_xbeu_vr;
   int n_aux1,n_auxs1;

   i_svec = smat.offs_nv;
   for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
     if (i_ebeu_nd != cct.ref_nd) {
       cct.val_nd[i_ebeu_nd] = smat.svec_ex[i_svec];
       i_svec++;
     }
   }
   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);

   if (slv.flag_startup) {
     i_svec = smat.offs_eauxs;
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_auxs1 = ebe_lib[i_ebel].n_auxs;
       for (int i=0; i < n_auxs1; i++) {
         ebe_usr[i_ebeu].val_auxs[i] = smat.svec_ex[i_svec];
         i_svec++;
       }
     }
   } else {
     i_svec = smat.offs_eaux;
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_aux1 = ebe_lib[i_ebel].n_aux;
       for (int i=0; i < n_aux1; i++) {
         ebe_usr[i_ebeu].val_aux[i] = smat.svec_ex[i_svec];
         i_svec++;
       }
     }
   }
   i_svec = smat.offs_xvr;
   for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
     cct.val_xvr[i_xbeu_vr] = smat.svec_ex[i_svec];
     i_svec++;
   }
   cct_to_xbe_all(xbe_lib,xbe_usr,cct);
   cct_to_ebe_xvr_all(ebe_lib,ebe_usr,cct);

   i_svec = smat.offs_xaux;
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     n_aux1 = xbe_lib[i_xbel].n_aux;
     for (int i=0; i < n_aux1; i++) {
       xbe_usr[i_xbeu].val_aux[i] = smat.svec_ex[i_svec];
       i_svec++;
     }
   }
   return;
} // end of dcmp_solvec_ex
// -----------------------------------------------------------------------------
void one_time_parms_x(
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_one_time_parms] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);
   }

   global.flags[global.i_one_time_parms] = false;

   return;
} // end of one_time_parms_x
// -----------------------------------------------------------------------------
void one_time_parms_e(
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_one_time_parms] = true;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
   }

   global.flags[global.i_one_time_parms] = false;

   return;
} // end of one_time_parms_e
// -----------------------------------------------------------------------------
void save_history_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_save_history] = true;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     if (ebe_lib[i_ebel].flag_savehist) {
       get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
     }
   }
   global.flags[global.i_save_history] = false;

   return;
} // end of save_history_e
// -----------------------------------------------------------------------------
void save_history_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_save_history] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_savehist) {
       get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbel]);
     }
   }
   global.flags[global.i_save_history] = false;

   return;
} // end of save_history_x
// -----------------------------------------------------------------------------
void init_sol_e(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   SysMat &smat,
   Global &global) {

   int i_svec,i_svec_previous;
   int i_ebeu,i_ebel,i_ebeu_nd,n_aux1,n_auxs1,n_nd1,n_stv1;

   if (slv.flag_read_solution) {
     for (int i=0; i < smat.n_solvec_e; i++) {
       smat.svec_e[i] = 0.0;
     }
     read_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
     form_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);
   } else if (slv.flag_init_solution) {
     assign_const_1<bool>(global.flags,false);
     clear_solvec_e(ebe_lib,ebe_usr,cct,smat);
     global.time_given_e = global.time_begin;
     ebe_init_guess(ebe_usr,ebe_jac,cct,global);
     form_solvec_e(ebe_lib,ebe_usr,smat,slv,cct);
   } else if (slv.flag_prev_solution) {
     if (!slv.flag_prev_solution_exists) {
       cout << "init_sol_e: previous solution does not exist?" << endl;
       cout << "   check initial_sol statement. Halting..." << endl;
       exit(1);
     }

     for (int i=0; i < smat.n_solvec_e; i++) {
       smat.svec_e[i] = 0.0;
     }
     if (slv.solve_type_previous == global.I_STARTUP) {
       if (slv.solve_type == global.I_STARTUP) {
         copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_e_previous);
       } else if ((slv.solve_type_previous == global.I_TRNS) ||
                  (slv.solve_type_previous == global.I_DC)) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_e[i_svec] = smat.svec_e_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_e[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eaux;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_aux1 = ebe_lib[i_ebel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_e[i_svec] = ebe_usr[i_ebeu].val_aux[i];
             i_svec++;
           }
         }
       } else if (slv.solve_type == global.I_SSW) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_e[i_svec] = smat.svec_e_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_e[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eaux;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_aux1 = ebe_lib[i_ebel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_e[i_svec] = ebe_usr[i_ebeu].val_aux[i];
             i_svec++;
           }
         }
         i_svec = smat.offs_estv;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_stv1 = ebe_lib[i_ebel].n_stv;
           for (int i=0; i < n_stv1; i++) {
             smat.svec_e[i_svec] = ebe_usr[i_ebeu].val_stv[i];
             i_svec++;
           }
         }
         i_svec = smat.offs_ndcur;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_nd1 = ebe_lib[i_ebel].n_nd;
           for (int i=0; i < n_stv1; i++) {
             smat.svec_e[i_svec] = ebe_usr[i_ebeu].cur_nd[i];
             i_svec++;
           }
         }
       }
     } else if ((slv.solve_type_previous == global.I_TRNS) ||
                (slv.solve_type_previous == global.I_DC)) {
       if (slv.solve_type == global.I_STARTUP) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_e[i_svec] = smat.svec_e_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_e[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eauxs;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_auxs1 = ebe_lib[i_ebel].n_auxs;
           for (int i=0; i < n_auxs1; i++) {
             smat.svec_e[i_svec] = 0.0;
             ebe_usr[i_ebeu].val_auxs[i] = 0.0;
             i_svec++;
           }
         }
       } else if ((slv.solve_type_previous == global.I_TRNS) ||
                  (slv.solve_type_previous == global.I_DC)) {
         copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_e_previous);
       } else if (slv.solve_type == global.I_SSW) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_e[i_svec] = smat.svec_e_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_e[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eaux;
         i_svec_previous = smat.offs_eaux_previous;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_aux1 = ebe_lib[i_ebel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_e[i_svec] = smat.svec_e_previous[i_svec_previous];
             ebe_usr[i_ebeu].val_aux[i] = smat.svec_e[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_estv;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_stv1 = ebe_lib[i_ebel].n_stv;
           for (int i=0; i < n_stv1; i++) {
             smat.svec_e[i_svec] = ebe_usr[i_ebeu].val_stv[i];
             i_svec++;
           }
         }
         i_svec = smat.offs_ndcur;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_nd1 = ebe_lib[i_ebel].n_nd;
           for (int i=0; i < n_stv1; i++) {
             smat.svec_e[i_svec] = ebe_usr[i_ebeu].cur_nd[i];
             i_svec++;
           }
         }
       }
     } else if (slv.solve_type_previous == global.I_SSW) {
       if (slv.solve_type == global.I_STARTUP) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_e[i_svec] = smat.svec_e_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_e[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eauxs;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_auxs1 = ebe_lib[i_ebel].n_auxs;
           for (int i=0; i < n_auxs1; i++) {
             smat.svec_e[i_svec] = 0.0;
             ebe_usr[i_ebeu].val_auxs[i] = 0.0;
             i_svec++;
           }
         }
       } else if ((slv.solve_type_previous == global.I_TRNS) ||
                  (slv.solve_type_previous == global.I_DC)) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_e[i_svec] = smat.svec_e_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_e[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eaux;
         i_svec_previous = smat.offs_eaux_previous;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_aux1 = ebe_lib[i_ebel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_e[i_svec] = smat.svec_e_previous[i_svec_previous];
             ebe_usr[i_ebeu].val_aux[i] = smat.svec_e[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
       } else if (slv.solve_type == global.I_SSW) {
         copy_array_1<double>(smat.n_solvec_e,smat.svec_e,smat.svec_e_previous);
       }
     }
   } else {
     cout << "init_sol_e: initial_sol option? Halting..." << endl; exit(1);
   }

   return;
} // end of init_sol_e
// -----------------------------------------------------------------------------
void init_sol_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SolveBlocks &slv,
   Circuit &cct,
   SysMat &smat,
   Global &global) {

   if (slv.flag_read_solution) {
     for (int i=0; i < smat.n_solvec_x; i++) {
       smat.svec_x[i] = 0.0;
     }
     read_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
     cct_to_xbe_all(xbe_lib,xbe_usr,cct);
     if (cct.flag_x_matrix) {
       form_solvec_x(xbe_lib,xbe_usr,smat,cct);
     }
   } else if (slv.flag_init_solution) {
     assign_const_1<bool>(global.flags,false);
     clear_solvec_x(xbe_lib,xbe_usr,cct,smat);
     clear_solvec_x_1(xbe_lib,xbe_usr,cct);
     global.time_given_x = global.time_begin;
     xbe_init_guess(xbe_lib,xbe_usr,xbe_jac,cct,global);
     if (cct.flag_x_matrix) {
       form_solvec_x(xbe_lib,xbe_usr,smat,cct);
     }
   } else if (slv.flag_prev_solution) {
     if (!slv.flag_prev_solution_exists) {
       cout << "init_sol_x: previous solution does not exist?" << endl;
       cout << "   check initial_sol statement. Halting..." << endl;
       exit(1);
     }

     copy_array_1<double>(smat.n_solvec_x,smat.svec_x_previous,smat.svec_x);

     dcmp_solvec_x(xbe_lib,xbe_usr,smat,cct);
   } else {
     cout << "init_sol_x: initial_sol option? Halting..." << endl; exit(1);
   }

   return;
} // end of init_sol_x
// -----------------------------------------------------------------------------
void init_sol_ex(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   SysMat &smat,
   Global &global) {

   int i_svec,i_svec_previous;
   int i_ebeu,i_ebel,i_ebeu_nd,n_aux1,n_auxs1,n_nd1,n_stv1;
   int i_xbeu,i_xbel,i_xbeu_vr;

   if (slv.flag_read_solution) {
     for (int i=0; i < smat.n_solvec_ex; i++) {
       smat.svec_ex[i] = 0.0;
     }
     read_solution(xbe_lib,ebe_lib,xbe_usr,ebe_usr,slv,cct);
     cct_to_xbe_all(xbe_lib,xbe_usr,cct);
     form_solvec_ex(ebe_lib,xbe_lib,ebe_usr,xbe_usr,smat,slv,cct);
   } else if (slv.flag_init_solution) {
     clear_solvec_ex(xbe_lib, xbe_usr, ebe_lib, ebe_usr, cct, smat);

     assign_const_1<bool>(global.flags,false);
     global.time_given_e = global.time_begin;
     ebe_init_guess(ebe_usr,ebe_jac,cct,global);

     clear_solvec_x_1(xbe_lib,xbe_usr,cct);
     global.time_given_x = global.time_begin;
     xbe_init_guess(xbe_lib,xbe_usr,xbe_jac,cct,global);

     form_solvec_ex(ebe_lib,xbe_lib,ebe_usr,xbe_usr,smat,slv,cct);
   } else if (slv.flag_prev_solution) {
     if (!slv.flag_prev_solution_exists) {
       cout << "init_sol_ex: previous solution does not exist?" << endl;
       cout << "   check initial_sol statement. Halting..." << endl; exit(1);
     }
     if (slv.flag_sync_x_e != slv.flag_sync_x_e_previous) {
       cout << "init_sol_ex: flag_sync_x_e != flag_sync_x_e_previous." << endl;
       cout << "   Halting..." << endl; exit(1);
     }
     if (!slv.flag_sync_x_e) {
       cout << "init_sol_ex: flag_sync_x_e must be true." << endl;
       cout << "   Halting..." << endl; exit(1);
     }

     for (int i=0; i < smat.n_solvec_ex; i++) {
       smat.svec_ex[i] = 0.0;
     }

     if (slv.solve_type_previous == global.I_STARTUP) {
       if (slv.solve_type == global.I_STARTUP) {
         copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_ex_previous);
       } else if ((slv.solve_type_previous == global.I_TRNS) ||
                  (slv.solve_type_previous == global.I_DC)) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eaux;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_aux1 = ebe_lib[i_ebel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_ex[i_svec] = ebe_usr[i_ebeu].val_aux[i];
             i_svec++;
           }
         }
         i_svec = smat.offs_xvr;
         i_svec_previous = smat.offs_xvr_previous;
         for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
           smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
           cct.val_xvr[i_xbeu_vr] = smat.svec_ex[i_svec];
           i_svec++; i_svec_previous++;
         }
         i_svec = smat.offs_xaux;
         i_svec_previous = smat.offs_xaux_previous;
         for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
           i_xbel = xbe_usr[i_xbeu].index_xbel;
           n_aux1 = xbe_lib[i_xbel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             xbe_usr[i_xbeu].val_aux[i] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
       } else if (slv.solve_type == global.I_SSW) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eaux;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_aux1 = ebe_lib[i_ebel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_ex[i_svec] = ebe_usr[i_ebeu].val_aux[i];
             i_svec++;
           }
         }
         i_svec = smat.offs_estv;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_stv1 = ebe_lib[i_ebel].n_stv;
           for (int i=0; i < n_stv1; i++) {
             smat.svec_ex[i_svec] = ebe_usr[i_ebeu].val_stv[i];
             i_svec++;
           }
         }
         i_svec = smat.offs_ndcur;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_nd1 = ebe_lib[i_ebel].n_nd;
           for (int i=0; i < n_stv1; i++) {
             smat.svec_ex[i_svec] = ebe_usr[i_ebeu].cur_nd[i];
             i_svec++;
           }
         }
         i_svec = smat.offs_xvr;
         i_svec_previous = smat.offs_xvr_previous;
         for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
           smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
           cct.val_xvr[i_xbeu_vr] = smat.svec_ex[i_svec];
           i_svec++; i_svec_previous++;
         }
         i_svec = smat.offs_xaux;
         i_svec_previous = smat.offs_xaux_previous;
         for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
           i_xbel = xbe_usr[i_xbeu].index_xbel;
           n_aux1 = xbe_lib[i_xbel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             xbe_usr[i_xbeu].val_aux[i] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
       }
     } else if ((slv.solve_type_previous == global.I_TRNS) ||
                (slv.solve_type_previous == global.I_DC)) {
       if (slv.solve_type == global.I_STARTUP) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eauxs;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_auxs1 = ebe_lib[i_ebel].n_auxs;
           for (int i=0; i < n_auxs1; i++) {
             smat.svec_ex[i_svec] = 0.0;
             ebe_usr[i_ebeu].val_auxs[i] = 0.0;
             i_svec++;
           }
         }
         i_svec = smat.offs_xvr;
         i_svec_previous = smat.offs_xvr_previous;
         for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
           smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
           cct.val_xvr[i_xbeu_vr] = smat.svec_ex[i_svec];
           i_svec++; i_svec_previous++;
         }
         i_svec = smat.offs_xaux;
         i_svec_previous = smat.offs_xaux_previous;
         for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
           i_xbel = xbe_usr[i_xbeu].index_xbel;
           n_aux1 = xbe_lib[i_xbel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             xbe_usr[i_xbeu].val_aux[i] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
       } else if ((slv.solve_type_previous == global.I_TRNS) ||
                  (slv.solve_type_previous == global.I_DC)) {
         copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_ex_previous);
       } else if (slv.solve_type == global.I_SSW) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eaux;
         i_svec_previous = smat.offs_eaux_previous;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_aux1 = ebe_lib[i_ebel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             ebe_usr[i_ebeu].val_aux[i] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_estv;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_stv1 = ebe_lib[i_ebel].n_stv;
           for (int i=0; i < n_stv1; i++) {
             smat.svec_ex[i_svec] = ebe_usr[i_ebeu].val_stv[i];
             i_svec++;
           }
         }
         i_svec = smat.offs_ndcur;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_nd1 = ebe_lib[i_ebel].n_nd;
           for (int i=0; i < n_stv1; i++) {
             smat.svec_ex[i_svec] = ebe_usr[i_ebeu].cur_nd[i];
             i_svec++;
           }
         }
         i_svec = smat.offs_xvr;
         i_svec_previous = smat.offs_xvr_previous;
         for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
           smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
           cct.val_xvr[i_xbeu_vr] = smat.svec_ex[i_svec];
           i_svec++; i_svec_previous++;
         }
         i_svec = smat.offs_xaux;
         i_svec_previous = smat.offs_xaux_previous;
         for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
           i_xbel = xbe_usr[i_xbeu].index_xbel;
           n_aux1 = xbe_lib[i_xbel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             xbe_usr[i_xbeu].val_aux[i] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
       }
     } else if (slv.solve_type_previous == global.I_SSW) {
       if (slv.solve_type == global.I_STARTUP) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eauxs;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_auxs1 = ebe_lib[i_ebel].n_auxs;
           for (int i=0; i < n_auxs1; i++) {
             smat.svec_ex[i_svec] = 0.0;
             ebe_usr[i_ebeu].val_auxs[i] = 0.0;
             i_svec++;
           }
         }
         i_svec = smat.offs_xvr;
         i_svec_previous = smat.offs_xvr_previous;
         for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
           smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
           cct.val_xvr[i_xbeu_vr] = smat.svec_ex[i_svec];
           i_svec++; i_svec_previous++;
         }
         i_svec = smat.offs_xaux;
         i_svec_previous = smat.offs_xaux_previous;
         for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
           i_xbel = xbe_usr[i_xbeu].index_xbel;
           n_aux1 = xbe_lib[i_xbel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             xbe_usr[i_xbeu].val_aux[i] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
       } else if ((slv.solve_type_previous == global.I_TRNS) ||
                  (slv.solve_type_previous == global.I_DC)) {
         i_svec = smat.offs_nv;
         i_svec_previous = smat.offs_nv_previous;
         for (i_ebeu_nd=0; i_ebeu_nd < cct.n_ebeu_nd; i_ebeu_nd++) {
           if (i_ebeu_nd != cct.ref_nd) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             cct.val_nd[i_ebeu_nd] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_eaux;
         i_svec_previous = smat.offs_eaux_previous;
         for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
           i_ebel = ebe_usr[i_ebeu].index_ebel;
           n_aux1 = ebe_lib[i_ebel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             ebe_usr[i_ebeu].val_aux[i] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
         i_svec = smat.offs_xvr;
         i_svec_previous = smat.offs_xvr_previous;
         for (i_xbeu_vr=0; i_xbeu_vr < cct.n_xbeu_vr; i_xbeu_vr++) {
           smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
           cct.val_xvr[i_xbeu_vr] = smat.svec_ex[i_svec];
           i_svec++; i_svec_previous++;
         }
         i_svec = smat.offs_xaux;
         i_svec_previous = smat.offs_xaux_previous;
         for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
           i_xbel = xbe_usr[i_xbeu].index_xbel;
           n_aux1 = xbe_lib[i_xbel].n_aux;
           for (int i=0; i < n_aux1; i++) {
             smat.svec_ex[i_svec] = smat.svec_ex_previous[i_svec_previous];
             xbe_usr[i_xbeu].val_aux[i] = smat.svec_ex[i_svec];
             i_svec++; i_svec_previous++;
           }
         }
       } else if (slv.solve_type == global.I_SSW) {
         copy_array_1<double>(smat.n_solvec_ex,smat.svec_ex,smat.svec_ex_previous);
       }
     }
   } else {
     cout << "init_sol_ex: initial_sol option? Halting..." << endl; exit(1);
   }

   return;
} // end of init_sol_ex
// -----------------------------------------------------------------------------
void ebe_init_guess(
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;

   assign_const_1<bool>(global.flags,false);
   global.flags[global.i_init_guess] = true;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
   }
   global.flags[global.i_init_guess] = false;

   return;
} // end of ebe_init_guess
// -----------------------------------------------------------------------------
void xbe_init_guess(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   assign_const_1<bool>(global.flags,false);
   global.flags[global.i_init_guess] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_integrate) {
       get_xbe_1(i_xbeu,xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
   }
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     if (xbe_lib[i_xbel].flag_evaluate) {
       if (xbe_lib[i_xbel].flag_source) {
         get_xbe_1(i_xbeu,xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
     }
   }

   if (cct.flag_alg_loop) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       get_xbe_1(i_xbeu,xbe_lib,xbe_usr,xbe_jac,cct,global);
     }
   } else {
     for (int i_pass=0; i_pass < cct.x_n_pass; i_pass++) {
       for (int i=0; i < cct.x_pass_n_beu[i_pass]; i++) {
         i_xbeu = cct.x_pass_beu[i_pass][i];
         get_xbe_1(i_xbeu,xbe_lib,xbe_usr,xbe_jac,cct,global);
       }
     }
   }

   global.flags[global.i_init_guess] = false;

   return;
} // end of xbe_init_guess
// -----------------------------------------------------------------------------
void read_solution(
   vector<XbeLib> &xbe_lib,
   vector<EbeLib> &ebe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<EbeUsr> &ebe_usr,
   SolveBlocks &slv,
   Circuit &cct) {

   fstream inf;
   std::string s1;
   int i_xbeu,i_xbel;
   int i_ebeu,i_ebel;

   inf.open(slv.infile_sol,ios::in|ios::binary);

   if (!inf) {
     cout << "read_solution: file " << slv.infile_sol
       << " does not exist. Halting..." << endl; exit(1);
   }

   s1 = next_string_1(inf);
   if (s1 != "cct.val_xvr:") {
     cout << "expected cct.val_xvr: in file " << slv.infile_sol << endl;
     cout << "  but found <" << s1 << ">. Halting..." << endl; exit(1);
   }
   read_vec_double_1(inf,cct.val_xvr);

   s1 = next_string_1(inf);
   if (s1 != "xbe_usr.val_aux:") {
     cout << "expected xbe_usr.val_aux: in file " << slv.infile_sol << endl;
     cout << "  but found <" << s1 << ">. Halting..." << endl; exit(1);
   }
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     for (int i=0; i < xbe_lib[i_xbel].n_aux; i++) {
       xbe_usr[i_xbeu].val_aux[i] = next_double_1(inf);
     }
   }

   s1 = next_string_1(inf);
   if (s1 != "cct.val_nd:") {
     cout << "expected cct.val_nd: in file " << slv.infile_sol << endl;
     cout << "  but found <" << s1 << ">. Halting..." << endl; exit(1);
   }
   read_vec_double_1(inf,cct.val_nd);

   s1 = next_string_1(inf);
   if (s1 != "ebe_usr.cur_nd:") {
     cout << "expected ebe_usr.cur_nd: in file " << slv.infile_sol << endl;
     cout << "  but found <" << s1 << ">. Halting..." << endl; exit(1);
   }
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_nd; i++) {
       ebe_usr[i_ebeu].cur_nd[i] = next_double_1(inf);
     }
   }

   s1 = next_string_1(inf);
   if (s1 != "ebe_usr.val_aux:") {
     cout << "expected ebe_usr.val_aux: in file " << slv.infile_sol << endl;
     cout << "  but found <" << s1 << ">. Halting..." << endl; exit(1);
   }
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_aux; i++) {
       ebe_usr[i_ebeu].val_aux[i] = next_double_1(inf);
     }
   }

   s1 = next_string_1(inf);
   if (s1 != "ebe_usr.val_auxs:") {
     cout << "expected ebe_usr.val_auxs: in file " << slv.infile_sol << endl;
     cout << "  but found <" << s1 << ">. Halting..." << endl; exit(1);
   }
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_auxs; i++) {
       ebe_usr[i_ebeu].val_auxs[i] = next_double_1(inf);
     }
   }

   s1 = next_string_1(inf);
   if (s1 != "ebe_usr.val_stv:") {
     cout << "expected ebe_usr.val_stv: in file " << slv.infile_sol << endl;
     cout << "  but found <" << s1 << ">. Halting..." << endl; exit(1);
   }
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_stv; i++) {
       ebe_usr[i_ebeu].val_stv[i] = next_double_1(inf);
     }
   }

   s1 = next_string_1(inf);
   if (s1 != "ebe_usr.val_xvr:") {
     cout << "expected ebe_usr.val_xvr: in file " << slv.infile_sol << endl;
     cout << "  but found <" << s1 << ">. Halting..." << endl; exit(1);
   }
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_xvr; i++) {
       ebe_usr[i_ebeu].val_xvr[i] = next_double_1(inf);
     }
   }

   inf.close();

   return;
} // end of read_solution
// -----------------------------------------------------------------------------
void write_iter_x(
   SolveBlocks &slv) {

   if (slv.iter_trns_x == 0) {
     cout << "Transient simulation starts..." << endl;
     if (slv.flag_write_time_x) {
       cout << "i=" << slv.iter_trns_x
            << " time=" << slv.time_present_x << endl;
     } else {
       cout << "i=" << slv.iter_trns_x << endl;
     }
   } else {
     if (slv.write_iter_n1_x == slv.write_iter_n_x) {
       if (slv.flag_write_time_x) {
         cout << "i=" << slv.iter_trns_x
              << ", time=" << slv.time_present_x << endl;
       } else {
         cout << "i=" << slv.iter_trns_x << endl;
       }
       slv.write_iter_n1_x = 0;
     }
   }
   slv.write_iter_n1_x++;

   return;
} // end of write_iter_x
// -----------------------------------------------------------------------------
void write_iter_e(
   SolveBlocks &slv) {

   if (slv.iter_trns_e == 0) {
     cout << "Transient simulation starts..." << endl;
     if (slv.flag_write_time_e) {
       cout << "i=" << slv.iter_trns_e
            << " time=" << slv.time_present_e << endl;
     } else {
       cout << "i=" << slv.iter_trns_e << endl;
     }
   } else {
     if (slv.write_iter_n1_e == slv.write_iter_n_e) {
       if (slv.flag_write_time_e) {
         cout << "i=" << slv.iter_trns_e
              << ", time=" << slv.time_present_e << endl;
       } else {
         cout << "i=" << slv.iter_trns_e << endl;
       }
       slv.write_iter_n1_e = 0;
     }
   }
   slv.write_iter_n1_e++;

   return;
} // end of write_iter_e
// -----------------------------------------------------------------------------
void write_solution(
   vector<XbeLib> &xbe_lib,
   vector<EbeLib> &ebe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<EbeUsr> &ebe_usr,
   SolveBlocks &slv,
   Circuit &cct) {

   int width_r;
   int i_xbeu,i_xbel;
   int i_ebeu,i_ebel;
   int i0;

   i0 = slv.index_file_solution;
   width_r = slv.outf_sol_word_width_real;

   slv.f_output[i0] << "cct.val_xvr:" << endl;
   print_vec_double_1(slv.f_output[i0],cct.val_xvr,width_r);

   slv.f_output[i0] << "xbe_usr.val_aux:" << endl;
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     for (int i=0; i < xbe_lib[i_xbel].n_aux; i++) {
       slv.f_output[i0] << setw(width_r)
         << xbe_usr[i_xbeu].val_aux[i] << endl;
     }
   }

   slv.f_output[i0] << "cct.val_nd:" << endl;
   print_vec_double_1(slv.f_output[i0],cct.val_nd,width_r);

   slv.f_output[i0] << "ebe_usr.cur_nd:" << endl;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_nd; i++) {
       slv.f_output[i0] << setw(width_r)
         << ebe_usr[i_ebeu].cur_nd[i] << endl;
     }
   }

   slv.f_output[i0] << "ebe_usr.val_aux:" << endl;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_aux; i++) {
       slv.f_output[i0] << setw(width_r)
         << ebe_usr[i_ebeu].val_aux[i] << endl;
     }
   }

   slv.f_output[i0] << "ebe_usr.val_auxs:" << endl;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_auxs; i++) {
       slv.f_output[i0] << setw(width_r)
         << ebe_usr[i_ebeu].val_auxs[i] << endl;
     }
   }

   slv.f_output[i0] << "ebe_usr.val_stv:" << endl;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_stv; i++) {
       slv.f_output[i0] << setw(width_r)
         << ebe_usr[i_ebeu].val_stv[i] << endl;
     }
   }

   slv.f_output[i0] << "ebe_usr.val_xvr:" << endl;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_xvr; i++) {
       slv.f_output[i0] << setw(width_r)
         << ebe_usr[i_ebeu].val_xvr[i] << endl;
     }
   }

   return;
} // end of write_solution
// -----------------------------------------------------------------------------
void write_solution_1_e(
   std::string filename,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SolveBlocks &slv,
   Circuit &cct) {

   int width_r;
   int i_ebeu,i_ebel;
   fstream outf;

   outf.open(filename,ios::out|ios::binary);
   width_r = slv.outf_sol_word_width_real;

   outf << "cct.val_xvr:" << endl;
   print_vec_double_1(outf,cct.val_xvr,width_r);

   outf << "xbe_usr.val_aux:" << endl;

   outf << "cct.val_nd:" << endl;
   print_vec_double_1(outf,cct.val_nd,width_r);

   outf << "ebe_usr.cur_nd:" << endl;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_nd; i++) {
       outf << setw(width_r)
         << ebe_usr[i_ebeu].cur_nd[i] << endl;
     }
   }

   outf << "ebe_usr.val_aux:" << endl;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_aux; i++) {
       outf << setw(width_r)
         << ebe_usr[i_ebeu].val_aux[i] << endl;
     }
   }

   outf << "ebe_usr.val_auxs:" << endl;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_auxs; i++) {
       outf << setw(width_r)
         << ebe_usr[i_ebeu].val_auxs[i] << endl;
     }
   }

   outf << "ebe_usr.val_stv:" << endl;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_stv; i++) {
       outf << setw(width_r)
         << ebe_usr[i_ebeu].val_stv[i] << endl;
     }
   }

   outf << "ebe_usr.val_xvr:" << endl;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     for (int i=0; i < ebe_lib[i_ebel].n_xvr; i++) {
       outf << setw(width_r)
         << ebe_usr[i_ebeu].val_xvr[i] << endl;
     }
   }
   outf.close();
   return;
} // end of write_solution_1_e
// -----------------------------------------------------------------------------
void write_solution_1_x(
   std::string filename,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   SolveBlocks &slv,
   Circuit &cct) {

   int width_r;
   int i_xbeu,i_xbel;
   fstream outf;

   outf.open(filename,ios::out|ios::binary);
   width_r = slv.outf_sol_word_width_real;

   outf << "cct.val_xvr:" << endl;
   print_vec_double_1(outf,cct.val_xvr,width_r);

   outf << "xbe_usr.val_aux:" << endl;
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     for (int i=0; i < xbe_lib[i_xbel].n_aux; i++) {
       outf << setw(width_r)
         << xbe_usr[i_xbeu].val_aux[i] << endl;
     }
   }
   outf.close();
   return;
} // end of write_solution_1_x
// -----------------------------------------------------------------------------
void xbe_ov_prm(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   CctFile &cct_file,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_outvar] = true;

   for (int i=0; i < cct_file.n_ov; i++) {
     if (cct_file.ov_flag[i] == global.I_OV_XBE) {
       i_xbeu = cct_file.ov1[i];
       i_xbel = xbe_usr[i_xbeu].index_xbel;

//     assign xbe_usr.val_vr from cct.val_xvr
       cct_to_xbe_1(i_xbeu,xbe_lib,xbe_usr,cct);

       get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbeu]);
     }
   }
   global.flags[global.i_outvar] = false;

   return;
} // end of xbe_ov_prm
// -----------------------------------------------------------------------------
void ebe_ov_prm(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   Circuit &cct,
   CctFile &cct_file,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_outvar] = true;

   for (int i=0; i < cct_file.n_ov; i++) {
     if (cct_file.ov_flag[i] == global.I_OV_EBE) {
       i_ebeu = cct_file.ov1[i];
       i_ebel = ebe_usr[i_ebeu].index_ebel;

//     assign ebe_usr.val_nd from cct.val_nd
       cct_to_ebe_nd_1(i_ebeu,ebe_lib,ebe_usr,cct);

       get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
     }
   }
   global.flags[global.i_outvar] = false;

   return;
} // end of ebe_ov_prm
// -----------------------------------------------------------------------------
void assign_ov_prm(
   const int i_ov,
   const int i_file,
   const int i_var,
   vector<XbeUsr> &xbe_usr,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   SolveBlocks &slv,
   CctFile &cct_file,
   Global &global) {

   int i_xbeu,i_rprm,i_xvr;
   int i_ebeu,i_nd;

   if (cct_file.ov_flag[i_ov] == global.I_OV_EBE) {
     i_ebeu = cct_file.ov1[i_ov];
     i_rprm = cct_file.ov2[i_ov];
     slv.outvar_temp[i_file][i_var] = ebe_usr[i_ebeu].outprm[i_rprm];
   } else if (cct_file.ov_flag[i_ov] == global.I_OV_XBE) {
     i_xbeu = cct_file.ov1[i_ov];
     i_rprm = cct_file.ov2[i_ov];
     slv.outvar_temp[i_file][i_var] = xbe_usr[i_xbeu].outprm[i_rprm];
   } else if (cct_file.ov_flag[i_ov] == global.I_OV_XVR) {
     i_xvr = cct_file.ov1[i_ov];
     slv.outvar_temp[i_file][i_var] = cct.val_xvr[i_xvr];
   } else if (cct_file.ov_flag[i_ov] == global.I_OV_NODEV) {
     i_nd = cct_file.ov1[i_ov];
     slv.outvar_temp[i_file][i_var] = cct.val_nd[i_nd];
   }

   return;
} // end of assign_ov_prm
// -----------------------------------------------------------------------------
void xbe_find_nextbreak(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_next_time] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;
     global.time_nextbreak_x = xbe_usr[i_xbeu].next_break;

     if (xbe_lib[i_xbel].flag_lmttstep) {
       get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbeu]);
       xbe_usr[i_xbeu].next_break = global.time_nextbreak_x;
     }
   }
   global.flags[global.i_next_time] = false;

   return;
} // end of xbe_find_nextbreak
// -----------------------------------------------------------------------------
void ebe_find_nextbreak(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_next_time] = true;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     global.time_nextbreak_e = ebe_usr[i_ebeu].next_break;

     if (ebe_lib[i_ebel].flag_lmttstep) {
       get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebeu]);
       ebe_usr[i_ebeu].next_break = global.time_nextbreak_e;
     }
   }
   global.flags[global.i_next_time] = false;

   return;
} // end of xbe_find_nextbreak
// -----------------------------------------------------------------------------
void get_tnext_x(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;
   double time_next_1,t_next,delt1;

   global.flags[global.i_next_time] = true;

   global.time_given_x = slv.time_present_x;
   time_next_1 = slv.time_present_x + slv.delt_x;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if ((time_next_1 >= xbe_usr[i_xbeu].next_break) ||
         (xbe_lib[i_xbel].flag_savehist)) {

       global.time_nextbreak_x = global.time_end;
       cct_to_xbe_1(i_xbeu,xbe_lib,xbe_usr,cct);

       if (xbe_lib[i_xbel].flag_lmttstep) {
         get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbeu]);
       }
       xbe_usr[i_xbeu].next_break = global.time_nextbreak_x;
     }
   }
   global.flags[global.i_next_time] = false;

   if (slv.delt_x != slv.delt_min_x) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       if (xbe_lib[i_xbel].flag_lmttstep) {
         t_next = xbe_usr[i_xbeu].next_break;
         if (t_next <= time_next_1) {
           time_next_1 = t_next;
         }
       }
     }
     delt1 = time_next_1 - slv.time_present_x;
     slv.delt_x = max(delt1,slv.delt_min_x);

     slv.delt_x = min(slv.delt_x,slv.delt_max_x);
     slv.delt_x = max(slv.delt_x,slv.delt_min_x);
   }

   return;
} // end of get_tnext_x
// -----------------------------------------------------------------------------
void get_tnext_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;
   double time_next_1,t_next,delt1;

   global.flags[global.i_next_time] = true;

   global.time_given_e = slv.time_present_e;
   time_next_1 = slv.time_present_e + slv.delt_e;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;

     if ((time_next_1 >= ebe_usr[i_ebeu].next_break) ||
         (ebe_lib[i_ebel].flag_savehist)) {

       global.time_nextbreak_e = global.time_end;
       cct_to_ebe_nd_1(i_ebeu,ebe_lib,ebe_usr,cct);

       if (ebe_lib[i_ebel].flag_lmttstep) {
         get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
       }
       ebe_usr[i_ebeu].next_break = global.time_nextbreak_e;
     }
   }
   global.flags[global.i_next_time] = false;

   if (slv.delt_e != slv.delt_min_e) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       if (ebe_lib[i_ebel].flag_lmttstep) {
         t_next = ebe_usr[i_ebeu].next_break;
         if (t_next <= time_next_1) {
           time_next_1 = t_next;
         }
       }
     }
     delt1 = time_next_1 - slv.time_present_e;
     slv.delt_e = max(delt1,slv.delt_min_e);

     slv.delt_e = min(slv.delt_e,slv.delt_max_e);
     slv.delt_e = max(slv.delt_e,slv.delt_min_e);
   }

   return;
} // end of get_tnext_e
// -----------------------------------------------------------------------------
void get_tnext_ex(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;
   int i_xbeu,i_xbel;
   double time_next_1,t_next,delt1;

   global.flags[global.i_next_time] = true;

   global.time_given_e = slv.time_present_e;
   global.time_given_x = slv.time_present_x;

   time_next_1 = slv.time_present_e + slv.delt_e;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     if ((time_next_1 >= ebe_usr[i_ebeu].next_break) ||
         (ebe_lib[i_ebel].flag_savehist)) {

       global.time_nextbreak_e = global.time_end;
       cct_to_ebe_nd_1(i_ebeu,ebe_lib,ebe_usr,cct);

       if (ebe_lib[i_ebel].flag_lmttstep) {
         get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
       }
       ebe_usr[i_ebeu].next_break = global.time_nextbreak_e;
     }
   }
   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if ((time_next_1 >= xbe_usr[i_xbeu].next_break) ||
         (xbe_lib[i_xbel].flag_savehist)) {

       global.time_nextbreak_x = global.time_end;
       cct_to_xbe_1(i_xbeu,xbe_lib,xbe_usr,cct);

       get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbeu]);
       xbe_usr[i_xbeu].next_break = global.time_nextbreak_x;
     }
   }
   global.flags[global.i_next_time] = false;

   if (slv.delt_e != slv.delt_min_ex) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       if (ebe_lib[i_ebel].flag_lmttstep) {
         t_next = ebe_usr[i_ebeu].next_break;
         if (t_next <= time_next_1) {
           time_next_1 = t_next;
         }
       }
     }
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       if (xbe_lib[i_xbel].flag_lmttstep) {
         t_next = xbe_usr[i_xbeu].next_break;
         if (t_next <= time_next_1) {
           time_next_1 = t_next;
         }
       }
     }
     delt1 = time_next_1 - slv.time_present_e;
     slv.delt_e = min(delt1,slv.delt_max_ex);

     slv.delt_e = min(slv.delt_e,slv.delt_max_ex);
     slv.delt_e = max(slv.delt_e,slv.delt_min_ex);
     slv.delt_x = slv.delt_e;
   }

   return;
} // end of get_tnext_ex
// -----------------------------------------------------------------------------
void xbe_modulo(
   Circuit &cct,
   vector<XbeUsr> &xbe_usr) {

   int i_xbeu;
   int i_xbeu_vr;
   int n1,i_xbeu_1,i_vr_1;
   double x1,x2,delx,x_old,x_new;

   for (int i=0; i < cct.nttl_xbeu_modulo; i++) {
     i_xbeu = cct.xbeu_modulo_map[i];

     x1 = xbe_usr[i_xbeu].rprm[0];
     x2 = xbe_usr[i_xbeu].rprm[1];
     delx = x2-x1;

     i_xbeu_vr = xbe_usr[i_xbeu].vr[0];
     x_old = cct.val_xvr[i_xbeu_vr];

     if (x_old > x1) {
       x_new = x1 + fmod((x_old-x1),delx);
     } else {
       x_new = x2 - fmod((x1-x_old),delx);
     }

     if (x_old != x_new) {
       cct.val_xvr[i_xbeu_vr] = x_new;
       n1 = cct.n_map_vr[i_xbeu_vr];

       for (int i1=0; i1 < n1; i1++) {
         i_xbeu_1 = cct.map_vr_1[i_xbeu_vr][i1];
         i_vr_1   = cct.map_vr_2[i_xbeu_vr][i1];
         xbe_usr[i_xbeu_1].val_vr[i_vr_1] = x_new;
       }
     }
   }
   return;
} // end of xbe_modulo
// -----------------------------------------------------------------------------
void xbe_modulo_implicit(
   Circuit &cct,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   SysMat &smat) {

   int i_xbeu;
   int i_xbeu_vr,i_svec;
   int n1,i_xbeu_1,i_vr_1;
   double x1,x2,delx,x_old,x_new;

   for (int i=0; i < cct.nttl_xbeu_modulo; i++) {
     i_xbeu = cct.xbeu_modulo_map[i];

     x1 = xbe_usr[i_xbeu].rprm[0];
     x2 = xbe_usr[i_xbeu].rprm[1];
     delx = x2-x1;

     i_xbeu_vr = xbe_usr[i_xbeu].vr[0];
     x_old = cct.val_xvr[i_xbeu_vr];

     if (x_old > x1) {
       x_new = x1 + fmod((x_old-x1),delx);
     } else {
       x_new = x2 - fmod((x1-x_old),delx);
     }

     if (x_old != x_new) {
       cct.val_xvr[i_xbeu_vr] = x_new;

       i_svec = cct.map_xbeuvr_to_svec[i_xbeu_vr];
       smat.svec_old_1_x[i_svec] = x_new;

       n1 = cct.n_map_vr[i_xbeu_vr];

       for (int i1=0; i1 < n1; i1++) {
         i_xbeu_1 = cct.map_vr_1[i_xbeu_vr][i1];
         i_vr_1   = cct.map_vr_2[i_xbeu_vr][i1];
         xbe_usr[i_xbeu_1].val_vr[i_vr_1] = x_new;
       }
     }
   }

   return;
} // end of xbe_modulo_implicit
// -----------------------------------------------------------------------------
void xbe_reset_1(
   const int flag_implicit,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   SysMat &smat,
   Global &global) {

   int i_xbeu,i_xbel;
   int n_vr1,i_xbeu_vr,i_svec;

   global.flags[global.i_reset_x] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if (xbe_lib[i_xbel].flag_reset) {
       if (!flag_implicit) {
         cct_to_xbe_1(i_xbeu,xbe_lib,xbe_usr,cct);
       }
       get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbeu]);

       if (flag_implicit) {

         n_vr1 = xbe_lib[i_xbel].n_vr;

         for (int i=0; i < n_vr1; i++) {
           i_xbeu_vr = xbe_usr[i_xbeu].vr[i];
           cct.val_xvr[i_xbeu_vr] = xbe_usr[i_xbeu].val_vr[i];
           i_svec = cct.map_xbeuvr_to_svec[i_xbeu_vr];
           smat.svec_x[i_svec] = xbe_usr[i_xbeu].val_vr[i];
         }

       } else {
         xbe_to_cct_op_1(i_xbeu,xbe_lib,xbe_usr,cct);
       }
     }
   }

   global.flags[global.i_reset_x] = false;

   return;
} // end of xbe_reset_1
// -----------------------------------------------------------------------------
void xbe_reset_1_exc(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   SolveBlocks &slv,
   Circuit &cct,
   SysMat &smat,
   Global &global) {

   int i_xbeu,i_xbel;
   int n_vr1,i_xbeu_vr,i_svec;

   global.flags[global.i_reset_x] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if (xbe_lib[i_xbel].flag_reset) {
       get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbeu]);

       n_vr1 = xbe_lib[i_xbel].n_vr;

       for (int i=0; i < n_vr1; i++) {
         i_xbeu_vr = xbe_usr[i_xbeu].vr[i];
         cct.val_xvr[i_xbeu_vr] = xbe_usr[i_xbeu].val_vr[i];
         i_svec = cct.map_xbeuvr_to_svec[i_xbeu_vr];
         smat.svec_ex[i_svec] = xbe_usr[i_xbeu].val_vr[i];
       }

     }
   }
   global.flags[global.i_reset_x] = false;

   return;
} // end of xbe_reset_1_exc
// -----------------------------------------------------------------------------
void xbe_time_parms(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   vector<XbeJac> &xbe_jac,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;

   global.flags[global.i_time_parms] = true;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     if (xbe_lib[i_xbel].flag_time_parms) {
       get_xbe(global,xbe_usr[i_xbeu],xbe_jac[i_xbeu]);
     }
   }
   global.flags[global.i_time_parms] = false;

   return;
} // end of xbe_time_parms
// -----------------------------------------------------------------------------
void xbeu_copy_1(
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct) {

   int i_xbeu,i_xbel;
   int n_vr1,n_aux1;

   for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
     i_xbel = xbe_usr[i_xbeu].index_xbel;

     n_vr1 = xbe_lib[i_xbel].n_vr;
     for (int i=0; i < n_vr1; i++) {
       xbe_usr[i_xbeu].val_vr_0[i] = xbe_usr[i_xbeu].val_vr[i];
     }
     n_aux1 = xbe_lib[i_xbel].n_aux;
     for (int i=0; i < n_aux1; i++) {
       xbe_usr[i_xbeu].val_aux_0[i] = xbe_usr[i_xbeu].val_aux[i];
     }
   }
   return;
} //end of xbe_copy_1
// -----------------------------------------------------------------------------
void xbeu_copy_2(
   const int flag,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct) {

// copy f to f0 or f1 ...

   int i_xbeu,i_xbel;
   int n_func1;

   if (flag == 0) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_func1 = xbe_lib[i_xbel].n_f;
       for (int i=0; i < n_func1; i++) {
         xbe_usr[i_xbeu].f0[i] = xbe_usr[i_xbeu].f[i];
       }
     }
   } else if (flag == 1) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_func1 = xbe_lib[i_xbel].n_f;
       for (int i=0; i < n_func1; i++) {
         xbe_usr[i_xbeu].f1[i] = xbe_usr[i_xbeu].f[i];
       }
     }
   } else if (flag == 2) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_func1 = xbe_lib[i_xbel].n_f;
       for (int i=0; i < n_func1; i++) {
         xbe_usr[i_xbeu].f2[i] = xbe_usr[i_xbeu].f[i];
       }
     }
   } else if (flag == 3) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_func1 = xbe_lib[i_xbel].n_f;
       for (int i=0; i < n_func1; i++) {
         xbe_usr[i_xbeu].f3[i] = xbe_usr[i_xbeu].f[i];
       }
     }
   } else if (flag == 4) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_func1 = xbe_lib[i_xbel].n_f;
       for (int i=0; i < n_func1; i++) {
         xbe_usr[i_xbeu].f4[i] = xbe_usr[i_xbeu].f[i];
       }
     }
   } else if (flag == 5) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_func1 = xbe_lib[i_xbel].n_f;
       for (int i=0; i < n_func1; i++) {
         xbe_usr[i_xbeu].f5[i] = xbe_usr[i_xbeu].f[i];
       }
     }
   }
   return;
} //end of xbe_copy_2
// -----------------------------------------------------------------------------
void ebeu_copy_stv_1(
   const int &flag_1,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;
   int n_stv1;

   if (flag_1 == global.I_COPY_0_TO_1) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_stv1 = ebe_lib[i_ebel].n_stv;
       for (int i=0; i < n_stv1; i++) {
         ebe_usr[i_ebeu].val_stv_1[i] = ebe_usr[i_ebeu].val_stv[i];
       }
     }
   } else if (flag_1 == global.I_COPY_1_TO_0) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_stv1 = ebe_lib[i_ebel].n_stv;
       for (int i=0; i < n_stv1; i++) {
         ebe_usr[i_ebeu].val_stv[i] = ebe_usr[i_ebeu].val_stv_1[i];
       }
     }
   } else if (flag_1 == global.I_COPY_1_TO_2) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_stv1 = ebe_lib[i_ebel].n_stv;
       for (int i=0; i < n_stv1; i++) {
         ebe_usr[i_ebeu].val_stv_2[i] = ebe_usr[i_ebeu].val_stv_1[i];
       }
     }
   } else if (flag_1 == global.I_COPY_2_TO_0) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_stv1 = ebe_lib[i_ebel].n_stv;
       for (int i=0; i < n_stv1; i++) {
         ebe_usr[i_ebeu].val_stv[i] = ebe_usr[i_ebeu].val_stv_2[i];
       }
     }
   } else if (flag_1 == global.I_COPY_2_TO_1) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_stv1 = ebe_lib[i_ebel].n_stv;
       for (int i=0; i < n_stv1; i++) {
         ebe_usr[i_ebeu].val_stv_1[i] = ebe_usr[i_ebeu].val_stv_2[i];
       }
     }
   }
   return;
} //end of ebeu_copy_stv_1
// -----------------------------------------------------------------------------
void copy_func_to_old_e(
   const int flag_1,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;
   int n_f1,n_g1,n_nd1;

   if (flag_1 == global.I_COPY_0_TO_1) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_f1 = ebe_lib[i_ebel].n_f;
       for (int i=0; i < n_f1; i++) {
         ebe_usr[i_ebeu].f_old_1[i] = ebe_usr[i_ebeu].f[i];
       }
       n_g1 = ebe_lib[i_ebel].n_g;
       for (int i=0; i < n_g1; i++) {
         ebe_usr[i_ebeu].g_old_1[i] = ebe_usr[i_ebeu].g[i];
       }
       n_nd1 = ebe_lib[i_ebel].n_nd;
       for (int i=0; i < n_nd1; i++) {
         ebe_usr[i_ebeu].cur_nd_1[i] = ebe_usr[i_ebeu].cur_nd[i];
       }
     }
   } else if (flag_1 == global.I_COPY_1_TO_2) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_f1 = ebe_lib[i_ebel].n_f;
       for (int i=0; i < n_f1; i++) {
         ebe_usr[i_ebeu].f_old_2[i] = ebe_usr[i_ebeu].f_old_1[i];
       }
       n_g1 = ebe_lib[i_ebel].n_g;
       for (int i=0; i < n_g1; i++) {
         ebe_usr[i_ebeu].g_old_2[i] = ebe_usr[i_ebeu].g_old_1[i];
       }
       n_nd1 = ebe_lib[i_ebel].n_nd;
       for (int i=0; i < n_nd1; i++) {
         ebe_usr[i_ebeu].cur_nd_2[i] = ebe_usr[i_ebeu].cur_nd_1[i];
       }
     }
   } else if (flag_1 == global.I_COPY_2_TO_0) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_f1 = ebe_lib[i_ebel].n_f;
       for (int i=0; i < n_f1; i++) {
         ebe_usr[i_ebeu].f[i] = ebe_usr[i_ebeu].f_old_2[i];
       }
       n_g1 = ebe_lib[i_ebel].n_g;
       for (int i=0; i < n_g1; i++) {
         ebe_usr[i_ebeu].g[i] = ebe_usr[i_ebeu].g_old_2[i];
       }
       n_nd1 = ebe_lib[i_ebel].n_nd;
       for (int i=0; i < n_nd1; i++) {
         ebe_usr[i_ebeu].cur_nd[i] = ebe_usr[i_ebeu].cur_nd_2[i];
       }
     }
   }
   return;
} //end of copy_func_to_old_e
// -----------------------------------------------------------------------------
void copy_cur_nd_nr_1(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct) {

   int i_ebeu,i_ebel;
   int n_nd1;

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (int i=0; i < n_nd1; i++) {
       ebe_usr[i_ebeu].cur_nd_old_nr_1[i] = ebe_usr[i_ebeu].cur_nd[i];
     }
   }
   return;
} //end of copy_cur_nd_nr_1
// -----------------------------------------------------------------------------
void copy_func_to_old_x(
   const int flag_1,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global) {

   int i_xbeu,i_xbel;
   int n_g1;

   if (flag_1 == global.I_COPY_0_TO_1) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_g1 = xbe_lib[i_xbel].n_g;
       for (int i=0; i < n_g1; i++) {
         xbe_usr[i_xbeu].g_old_1[i] = xbe_usr[i_xbeu].g[i];
       }
     }
   } else if (flag_1 == global.I_COPY_1_TO_2) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_g1 = xbe_lib[i_xbel].n_g;
       for (int i=0; i < n_g1; i++) {
         xbe_usr[i_xbeu].g_old_2[i] = xbe_usr[i_xbeu].g_old_1[i];
       }
     }
   } else if (flag_1 == global.I_COPY_2_TO_0) {
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_g1 = xbe_lib[i_xbel].n_g;
       for (int i=0; i < n_g1; i++) {
         xbe_usr[i_xbeu].g[i] = xbe_usr[i_xbeu].g_old_2[i];
       }
     }
   }
   return;
} //end of copy_func_to_old_x
// -----------------------------------------------------------------------------
void copy_func_to_old_ex(
   const int flag_1,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<XbeLib> &xbe_lib,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;
   int i_xbeu,i_xbel;
   int n_f1,n_g1,n_nd1;

   if (flag_1 == global.I_COPY_0_TO_1) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_f1 = ebe_lib[i_ebel].n_f;
       for (int i=0; i < n_f1; i++) {
         ebe_usr[i_ebeu].f_old_1[i] = ebe_usr[i_ebeu].f[i];
       }
       n_g1 = ebe_lib[i_ebel].n_g;
       for (int i=0; i < n_g1; i++) {
         ebe_usr[i_ebeu].g_old_1[i] = ebe_usr[i_ebeu].g[i];
       }
       n_nd1 = ebe_lib[i_ebel].n_nd;
       for (int i=0; i < n_nd1; i++) {
         ebe_usr[i_ebeu].cur_nd_1[i] = ebe_usr[i_ebeu].cur_nd[i];
       }
     }
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_g1 = xbe_lib[i_xbel].n_g;
       for (int i=0; i < n_g1; i++) {
         xbe_usr[i_xbeu].g_old_1[i] = xbe_usr[i_xbeu].g[i];
       }
     }
   } else if (flag_1 == global.I_COPY_1_TO_2) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_f1 = ebe_lib[i_ebel].n_f;
       for (int i=0; i < n_f1; i++) {
         ebe_usr[i_ebeu].f_old_2[i] = ebe_usr[i_ebeu].f_old_1[i];
       }
       n_g1 = ebe_lib[i_ebel].n_g;
       for (int i=0; i < n_g1; i++) {
         ebe_usr[i_ebeu].g_old_2[i] = ebe_usr[i_ebeu].g_old_1[i];
       }
       n_nd1 = ebe_lib[i_ebel].n_nd;
       for (int i=0; i < n_nd1; i++) {
         ebe_usr[i_ebeu].cur_nd_2[i] = ebe_usr[i_ebeu].cur_nd_1[i];
       }
     }
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_g1 = xbe_lib[i_xbel].n_g;
       for (int i=0; i < n_g1; i++) {
         xbe_usr[i_xbeu].g_old_2[i] = xbe_usr[i_xbeu].g_old_1[i];
       }
     }
   } else if (flag_1 == global.I_COPY_2_TO_0) {
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_f1 = ebe_lib[i_ebel].n_f;
       for (int i=0; i < n_f1; i++) {
         ebe_usr[i_ebeu].f[i] = ebe_usr[i_ebeu].f_old_2[i];
       }
       n_g1 = ebe_lib[i_ebel].n_g;
       for (int i=0; i < n_g1; i++) {
         ebe_usr[i_ebeu].g[i] = ebe_usr[i_ebeu].g_old_2[i];
       }
       n_nd1 = ebe_lib[i_ebel].n_nd;
       for (int i=0; i < n_nd1; i++) {
         ebe_usr[i_ebeu].cur_nd[i] = ebe_usr[i_ebeu].cur_nd_2[i];
       }
     }
     for (i_xbeu=0; i_xbeu < cct.n_xbeu; i_xbeu++) {
       i_xbel = xbe_usr[i_xbeu].index_xbel;
       n_g1 = xbe_lib[i_xbel].n_g;
       for (int i=0; i < n_g1; i++) {
         xbe_usr[i_xbeu].g[i] = xbe_usr[i_xbeu].g_old_2[i];
       }
     }
   }
   return;
} //end of copy_func_to_old_ex
// -----------------------------------------------------------------------------
void ebe_form_arrays_1(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct) {

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   if (cct.flag_x) {
     cct_to_ebe_xvr_all(ebe_lib,ebe_usr,cct);
   }

   return;
} // end of ebe_form_arrays_1
// -----------------------------------------------------------------------------
void ebe_form_arrays_ssw_e(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   Circuit &cct) {

   int i_svec,i_ebeu,i_ebel,n_stv1;

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   if (cct.flag_x) {
     cct_to_ebe_xvr_all(ebe_lib,ebe_usr,cct);
   }
   i_svec = smat.offs_estv;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_stv1 = ebe_lib[i_ebel].n_stv;

     for (int i=0; i < n_stv1; i++) {
       ebe_usr[i_ebeu].val_stv[i] = smat.svec_e[i_svec];
       i_svec++;
     }
   }

   return;
} // end of ebe_form_arrays_ssw_e
// -----------------------------------------------------------------------------
void ebe_form_arrays_ssw_ex(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   SysMat &smat,
   Circuit &cct) {

   int i_svec,i_ebeu,i_ebel,n_stv1;

   cct_to_ebe_nd_all(ebe_lib,ebe_usr,cct);
   if (cct.flag_x) {
     cct_to_ebe_xvr_all(ebe_lib,ebe_usr,cct);
   }
   i_svec = smat.offs_estv;
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_stv1 = ebe_lib[i_ebel].n_stv;

     for (int i=0; i < n_stv1; i++) {
       ebe_usr[i_ebeu].val_stv[i] = smat.svec_ex[i_svec];
       i_svec++;
     }
   }
   return;
} // end of ebe_form_arrays_ssw_ex
// -----------------------------------------------------------------------------
void ebe_update_stv(
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   vector<EbeJac> &ebe_jac,
   Circuit &cct,
   Global &global) {

   int i_ebeu,i_ebel;

   global.flags[global.i_update_states] = true;
  
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     if (ebe_lib[i_ebel].n_stv > 0) {
       get_ebe(global,ebe_usr[i_ebeu],ebe_jac[i_ebel]);
     }
   }
   global.flags[global.i_update_states] = false;

   return;
} // end of ebe_update_stv
// -----------------------------------------------------------------------------
void e_assign_nextbreak_1(
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   Global &global) {

   for (int i=0; i < cct.n_ebeu; i++) {
     ebe_usr[i].next_break = global.time_end;
   }
   return;
} // end of e_assign_nextbreak_1
// -----------------------------------------------------------------------------
void x_assign_nextbreak_1(
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global) {

   for (int i=0; i < cct.n_xbeu; i++) {
     xbe_usr[i].next_break = global.time_end;
   }
   return;
} // end of x_assign_nextbreak_1
// -----------------------------------------------------------------------------
void ex_assign_nextbreak_1(
   vector<EbeUsr> &ebe_usr,
   vector<XbeUsr> &xbe_usr,
   Circuit &cct,
   Global &global) {

   for (int i=0; i < cct.n_ebeu; i++) {
     ebe_usr[i].next_break = global.time_end;
   }
   for (int i=0; i < cct.n_xbeu; i++) {
     xbe_usr[i].next_break = global.time_end;
   }
   return;
} // end of ex_assign_nextbreak_1
// -----------------------------------------------------------------------------
void check_convergence_e(
   SysMat &smat,
   SolveBlocks &slv,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct) {

   int precision_real = 4;
   int width = precision_real + 7;
   bool flag_1;

   cout << scientific; cout << setprecision(precision_real);
   slv.flags_failed_default();

// Compute norms if check/write is specified:

   if (slv.e_nr_flag_check_spice) {
     compute_norm_spice_e(smat,slv,ebe_lib,ebe_usr,cct);
   }
   if (slv.flag_ssw) {
     if (slv.e_nr_flag_check_rhs2 || slv.e_nr_flag_write_rhs2) {
       slv.e_nr_norm_rhs2 = norm_2(smat.n_solvec_e,smat.rhs_m_ssw);
       if (flag_nan(slv.e_nr_norm_rhs2)) {
         cout << "check_convergence_e: slv.e_nr_norm_rhs2 is NAN. Halting..." << endl; exit(1);
       }
     }
     if (slv.e_nr_flag_write_rhsinf) {
       slv.e_nr_norm_rhsinf = norm_inf(smat.n_solvec_e,smat.rhs_m_ssw);
       if (flag_nan(slv.e_nr_norm_rhsinf)) {
         cout << "check_convergence_e: slv.e_nr_norm_rhsinf is NAN. Halting..." << endl; exit(1);
       }
     }
   } else {
     if (slv.e_nr_flag_check_rhs2 || slv.e_nr_flag_write_rhs2) {
       slv.e_nr_norm_rhs2 = norm_2(smat.n_solvec_e,smat.rhs_m_e);
       if (flag_nan(slv.e_nr_norm_rhs2)) {
         cout << "check_convergence_e: slv.e_nr_norm_rhs2 is NAN. Halting..." << endl; exit(1);
       }
     }
     if (slv.e_nr_flag_write_rhsinf) {
       slv.e_nr_norm_rhsinf = norm_inf(smat.n_solvec_e,smat.rhs_m_e);
       if (flag_nan(slv.e_nr_norm_rhsinf)) {
         cout << "check_convergence_e: slv.e_nr_norm_rhsinf is NAN. Halting..." << endl; exit(1);
       }
     }
   }

   if (slv.e_nr_flag_check_delx_volt || slv.e_nr_flag_write_delx_volt) {
     slv.e_nr_norm_delx_volt = norm_inf(smat.n_nv,&(smat.delsvec_e[smat.offs_nv]));
     if (flag_nan(slv.e_nr_norm_delx_volt)) {
       cout << "check_convergence_e: slv.e_nr_norm_delx_volt is NAN. Halting..." << endl; exit(1);
     }
   }

// write norms to console

   if (slv.e_nr_flag_write_rhs2) {
     cout << slv.iter_newton << " " << "e_nr_norm_rhs2 =" << setw(width)
       << slv.e_nr_norm_rhs2 << endl;
   }
   if (slv.e_nr_flag_write_delx_volt) {
     cout << slv.iter_newton << " " << "e_nr_norm_delx_volt =" << setw(width)
       << slv.e_nr_norm_delx_volt << endl;
   }
   if (slv.e_nr_flag_write_rhsinf) {
     cout << slv.iter_newton << " " << "e_nr_norm_rhsinf =" << setw(width)
       << slv.e_nr_norm_rhsinf << endl;
   }

// Check convergence

   slv.flag_nr_converged = true;
   slv.flag_nr_norm_large = false;

   while(true) {
     if (slv.e_nr_flag_check_rhs2) {
       if (slv.iter_newton > 0) {
         flag_1 = flag_nan(slv.e_nr_norm_rhs2) ||
           (slv.e_nr_norm_rhs2 > slv.nr_norm_large);
         if (flag_1) {
           cout << "check_convergence_e: e_nr_norm_rhs2 = " << setw(width)
             << slv.e_nr_norm_rhs2 << " is too large." << endl;
           slv.flag_nr_converged = false;
           slv.flag_nr_norm_large = true;
           break;
         }
       }
       if (slv.e_nr_norm_rhs2 > slv.e_nr_eps_rhs) {
         slv.flag_nr_converged = false;
         slv.flag_e_nr_eps_rhs_failed = true;
         break;
       }
     }
     if (slv.e_nr_flag_check_spice) {
       check_spice_e(smat,slv,ebe_lib,ebe_usr,cct,
         slv.flag_nr_converged,slv.flag_nr_norm_large);
       if (!slv.flag_nr_converged) {
         break;
       }
     }
     if (slv.e_nr_flag_check_delx_volt) {
       flag_1 = flag_nan(slv.e_nr_norm_delx_volt) ||
         (slv.e_nr_norm_delx_volt > slv.nr_norm_large);
       if (flag_1) {
         cout << "check_convergence_e: e_nr_norm_delx_volt = " << setw(width)
           << slv.e_nr_norm_delx_volt << " is too large." << endl;
         slv.flag_nr_converged = false;
         slv.flag_nr_norm_large = true;
         break;
       }
       if (slv.e_nr_norm_delx_volt > slv.e_nr_eps_volt) {
         slv.flag_nr_converged = false;
         slv.flag_e_nr_eps_volt_failed = true;
         break;
       }
     }
     if (slv.e_nr_flag_check_delx_all) {
       check_delx_all_e(smat,slv);
       if (!slv.flag_nr_converged) {
         break;
       }
     }
     break;
   }
   return;
} // end of check_convergence_e
// -----------------------------------------------------------------------------
void check_convergence_x(
   SysMat &smat,
   SolveBlocks &slv) {

   int precision_real = 4;
   int width = precision_real + 7;
   bool flag_1;

   cout << scientific; cout << setprecision(precision_real);
   slv.flags_failed_default();

// Compute norms if check/write is specified:

   if (slv.flag_ssw) {
     if (slv.x_nr_flag_check_rhs2 || slv.x_nr_flag_write_rhs2) {
       slv.x_nr_norm_rhs2 = norm_2(smat.n_solvec_x,smat.rhs_m_ssw);
     }
     if (slv.x_nr_flag_write_rhsinf) {
       slv.x_nr_norm_rhsinf = norm_inf(smat.n_solvec_x,smat.rhs_m_ssw);
     }
   } else {
     if (slv.x_nr_flag_check_rhs2 || slv.x_nr_flag_write_rhs2) {
       slv.x_nr_norm_rhs2 = norm_2(smat.n_solvec_x,smat.rhs_m_x);
     }
     if (slv.x_nr_flag_write_rhsinf) {
       slv.x_nr_norm_rhsinf = norm_inf(smat.n_solvec_x,smat.rhs_m_x);
     }
   }

// write norms to console

   if (slv.x_nr_flag_write_rhs2) {
     cout << slv.iter_newton << " " << "x_nr_norm_rhs2 =" << setw(width)
       << slv.x_nr_norm_rhs2 << endl;
   }
   if (slv.x_nr_flag_write_rhsinf) {
     cout << slv.iter_newton << " " << "x_nr_norm_rhsinf =" << setw(width)
       << slv.x_nr_norm_rhsinf << endl;
   }

// Check convergence

   slv.flag_nr_converged = true;
   slv.flag_nr_norm_large = false;

   while(true) {
     if (slv.x_nr_flag_check_rhs2) {
       if (slv.iter_newton > 0) {
         flag_1 = flag_nan(slv.x_nr_norm_rhs2) ||
           (slv.x_nr_norm_rhs2 > slv.nr_norm_large);
         if (flag_1) {
           cout << "check_convergence_x: x_nr_norm_rhs2 = " << setw(width)
             << slv.x_nr_norm_rhs2 << " is too large." << endl;
           slv.flag_nr_converged = false;
           slv.flag_nr_norm_large = true;
           break;
         }
       }
       if (slv.x_nr_norm_rhs2 > slv.x_nr_eps_rhs) {
         slv.flag_nr_converged = false;
         slv.flag_x_nr_eps_rhs_failed = true;
         break;
       }
     }
     if (slv.x_nr_flag_check_delx_all) {
       check_delx_all_x(smat,slv);
       if (!slv.flag_nr_converged) {
         break;
       }
     }
     break;
   }

   return;
} // end of check_convergence_x
// -----------------------------------------------------------------------------
void check_convergence_ex(
   SysMat &smat,
   SolveBlocks &slv,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct) {

   int precision_real = 4;
   int width = precision_real + 7;
   bool flag_1;

   cout << scientific; cout << setprecision(precision_real);
   slv.flags_failed_default();

// Compute norms if check/write is specified:

   if (slv.e_nr_flag_check_spice) {
     compute_norm_spice_ex(smat,slv,ebe_lib,ebe_usr,cct);
   }

   if (slv.flag_ssw) {
     if (slv.ex_nr_flag_check_rhs2 || slv.ex_nr_flag_write_rhs2) {
       slv.ex_nr_norm_rhs2 = norm_2(smat.n_solvec_ex,smat.rhs_m_ssw);
       if (flag_nan(slv.ex_nr_norm_rhs2)) {
         cout << "check_convergence_ex: slv.ex_nr_norm_rhs2 is NAN. Halting..." << endl; exit(1);
       }
     }
   } else {
     if (slv.ex_nr_flag_check_rhs2 || slv.ex_nr_flag_write_rhs2) {
       slv.ex_nr_norm_rhs2 = norm_2(smat.n_solvec_ex,smat.rhs_m_ex);
       if (flag_nan(slv.ex_nr_norm_rhs2)) {
         cout << "check_convergence_ex: slv.ex_nr_norm_rhs2 is NAN. Halting..." << endl; exit(1);
       }
     }
   }

   if (slv.e_nr_flag_check_delx_volt || slv.e_nr_flag_write_delx_volt) {
     slv.e_nr_norm_delx_volt = norm_inf(smat.n_nv,&(smat.delsvec_ex[smat.offs_nv]));
     if (flag_nan(slv.e_nr_norm_delx_volt)) {
       cout << "check_convergence_ex: slv.e_nr_norm_delx_volt is NAN. Halting..." << endl; exit(1);
     }
   }

// write norms to console

   if (slv.ex_nr_flag_write_rhs2) {
     cout << slv.iter_newton << " " << "ex_nr_norm_rhs2 =" << setw(width)
       << slv.ex_nr_norm_rhs2 << endl;
   }
   if (slv.e_nr_flag_write_delx_volt) {
     cout << slv.iter_newton << " " << "e_nr_norm_delx_volt =" << setw(width)
       << slv.e_nr_norm_delx_volt;
   }

// Check convergence

   slv.flag_nr_converged = true;
   slv.flag_nr_norm_large = false;

   while(true) {
     if (slv.ex_nr_flag_check_rhs2) {
       if (slv.iter_newton > 0) {
         flag_1 = flag_nan(slv.ex_nr_norm_rhs2) ||
           (slv.ex_nr_norm_rhs2 > slv.nr_norm_large);
         if (flag_1) {
           cout << "check_convergence_ex: ex_nr_norm_rhs2 = " << setw(width)
             << slv.ex_nr_norm_rhs2 << " is too large." << endl;
           slv.flag_nr_converged = false;
           slv.flag_nr_norm_large = true;
           break;
         }
       }
       if (slv.ex_nr_norm_rhs2 > slv.ex_nr_eps_rhs) {
         slv.flag_nr_converged = false;
         slv.flag_ex_nr_eps_rhs_failed = true;
         break;
       }
     }
     if (slv.e_nr_flag_check_spice) {
       check_spice_e(smat,slv,ebe_lib,ebe_usr,cct,
         slv.flag_nr_converged,slv.flag_nr_norm_large);
       if (!slv.flag_nr_converged) {
         break;
       }
     }
     if (slv.e_nr_flag_check_delx_volt) {
       flag_1 = flag_nan(slv.e_nr_norm_delx_volt) ||
         (slv.e_nr_norm_delx_volt > slv.nr_norm_large);
       if (flag_1) {
         cout << "check_convergence_ex: e_nr_norm_delx_volt = " << setw(width)
           << slv.e_nr_norm_delx_volt << " is too large." << endl;
         slv.flag_nr_converged = false;
         slv.flag_nr_norm_large = true;
         break;
       }
       if (slv.e_nr_norm_delx_volt > slv.e_nr_eps_volt) {
         slv.flag_nr_converged = false;
         slv.flag_e_nr_eps_volt_failed = true;
         break;
       }
     }
     if (slv.ex_nr_flag_check_delx_all) {
       check_delx_all_ex(smat,slv);
       if (!slv.flag_nr_converged) {
         break;
       }
     }
     break;
   }
   return;
} // end of check_convergence_ex
// -----------------------------------------------------------------------------
void compute_norm_spice_e(
   SysMat &smat,
   SolveBlocks &slv,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct) {

   double k1,k2,k;
   int i_svec;
   int i_ebeu,i_ebel;
   int n_nd1;

   i_svec = smat.offs_nv;
   for (int i=0; i < smat.n_nv; i++) {
     k1 = fabs(smat.svec_e[i_svec]);
     k2 = fabs(smat.svec_old_nr_1_e[i_svec]);
     k = max(k1,k2);
     smat.tol_spice_e[i_svec] = slv.e_nr_spice_reltol*k + slv.e_nr_spice_vntol;
     smat.norm_spice_e[i_svec] =
       fabs(smat.svec_e[i_svec]-smat.svec_old_nr_1_e[i_svec]);
     i_svec++;
   }

   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (int i=0; i < n_nd1; i++) {
       k1 = fabs(ebe_usr[i_ebeu].cur_nd[i]);
       k2 = fabs(ebe_usr[i_ebeu].cur_nd_old_nr_1[i]);
       k = max(k1,k2);
       ebe_usr[i_ebeu].tol_spice_cur_nd[i] =
          slv.e_nr_spice_reltol*k + slv.e_nr_spice_abstol;
       ebe_usr[i_ebeu].norm_spice_cur_nd[i] =
          fabs(ebe_usr[i_ebeu].cur_nd[i]-ebe_usr[i_ebeu].cur_nd_old_nr_1[i]);

     }
   }
   return;
} // end of compute_norm_spice_e
// -----------------------------------------------------------------------------
void compute_norm_spice_ex(
   SysMat &smat,
   SolveBlocks &slv,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct) {

   double k1,k2,k;
   int i_svec;
   int i_ebeu,i_ebel;
   int n_nd1;

   i_svec = smat.offs_nv;
   for (int i=0; i < smat.n_nv; i++) {
     k1 = fabs(smat.svec_ex[i_svec]);
     k2 = fabs(smat.svec_old_nr_1_ex[i_svec]);
     k = max(k1,k2);

     smat.tol_spice_e[i_svec] = slv.e_nr_spice_reltol*k + slv.e_nr_spice_vntol;
     smat.norm_spice_e[i_svec] =
       fabs(smat.svec_ex[i_svec]-smat.svec_old_nr_1_ex[i_svec]);

     i_svec++;
   }
   for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
     i_ebel = ebe_usr[i_ebeu].index_ebel;
     n_nd1 = ebe_lib[i_ebel].n_nd;
     for (int i=0; i < n_nd1; i++) {
       k1 = fabs(ebe_usr[i_ebeu].cur_nd[i]);
       k2 = fabs(ebe_usr[i_ebeu].cur_nd_old_nr_1[i]);
       k = max(k1,k2);
       ebe_usr[i_ebeu].tol_spice_cur_nd[i] =
          slv.e_nr_spice_reltol*k + slv.e_nr_spice_abstol;
       ebe_usr[i_ebeu].norm_spice_cur_nd[i] =
          fabs(ebe_usr[i_ebeu].cur_nd[i]-ebe_usr[i_ebeu].cur_nd_old_nr_1[i]);
     }
   }

   return;
} // end of compute_norm_spice_ex
// -----------------------------------------------------------------------------
void check_spice_e(
   SysMat &smat,
   SolveBlocks &slv,
   vector<EbeLib> &ebe_lib,
   vector<EbeUsr> &ebe_usr,
   Circuit &cct,
   bool &flag_converged,
   bool &flag_norm_large) {

   bool flag_1;
   int i_svec,i_ebeu,i_ebel,n_nd1;
   double norm0;

   flag_norm_large = false;

   if (slv.iter_newton > 0) {
     flag_converged = true;
     i_svec = smat.offs_nv;
     for (int i=0; i < smat.n_nv; i++) {
       norm0 = smat.norm_spice_e[i_svec];
       flag_1 = flag_nan(norm0) || (norm0 > slv.nr_norm_large);
       if (flag_1) {
         cout << "check_spice_e: i: " << i << ", norm_spice_e: "
           << norm0 << " is too large." << endl;
         flag_converged = false;
         flag_norm_large = true;
         return;
       }
       if (norm0 > smat.tol_spice_e[i_svec]) {
         flag_converged = false;
         slv.flag_e_nr_spice_nodev_failed = true;
         return;
       }
       i_svec++;
     }
     for (i_ebeu=0; i_ebeu < cct.n_ebeu; i_ebeu++) {
       i_ebel = ebe_usr[i_ebeu].index_ebel;
       n_nd1 = ebe_lib[i_ebel].n_nd;
       for (int i=0; i < n_nd1; i++) {
         norm0 = ebe_usr[i_ebeu].norm_spice_cur_nd[i];
         flag_1 = flag_nan(norm0) || (norm0 > slv.nr_norm_large);
         if (flag_1) {
           cout << "check_spice_e: i_ebeu: " << i_ebeu
             << ", i: " << i << ", norm_spice_cur_nd: "
             << norm0 << " is too large." << endl;
           flag_converged = false;
           flag_norm_large = true;
           return;
         }
         if (norm0 > ebe_usr[i_ebeu].tol_spice_cur_nd[i]) {
           flag_converged = false;
           slv.flag_e_nr_spice_nodecur_failed = true;
           return;
         }
       }
     }
   } else {
//   spice convergence check needs at least two NR iterations, so we should
//   make this flag false for the first NR iteration.
     flag_converged = false;
   }
   return;
} // end of check_spice_e
// -----------------------------------------------------------------------------
void check_delx_all_e(
   SysMat &smat,
   SolveBlocks &slv) {

   double x;

   slv.flag_nr_converged = true;
   for (int i=0; i < smat.n_solvec_e; i++) {
     x = abs(smat.svec_e[i]);
     if (x > slv.eps1_delx_all) {
       if (abs(smat.delsvec_e[i]) > slv.e_nr_eps_delx_all*x) {
         slv.flag_nr_converged = false;
         slv.flag_e_nr_eps_delx_all_failed = true;
         break;
       }
     }
   }
   return;
} // end of check_delx_all_e
// -----------------------------------------------------------------------------
void check_delx_all_x(
   SysMat &smat,
   SolveBlocks &slv) {

   double x;

   slv.flag_nr_converged = true;
   for (int i=0; i < smat.n_solvec_x; i++) {
     x = abs(smat.svec_x[i]);
     if (x > slv.eps1_delx_all) {
       if (abs(smat.delsvec_x[i]) > slv.x_nr_eps_delx_all*x) {
         slv.flag_nr_converged = false;
         slv.flag_x_nr_eps_delx_all_failed = true;
         break;
       }
     }
   }
   return;
} // end of check_delx_all_x
// -----------------------------------------------------------------------------
void check_delx_all_ex(
   SysMat &smat,
   SolveBlocks &slv) {

   double x;

   slv.flag_nr_converged = true;
   for (int i=0; i < smat.n_solvec_ex; i++) {
     x = abs(smat.svec_ex[i]);
     if (x > slv.eps1_delx_all) {
       if (abs(smat.delsvec_ex[i]) > slv.ex_nr_eps_delx_all*x) {
         slv.flag_nr_converged = false;
         slv.flag_ex_nr_eps_delx_all_failed = true;
         break;
       }
     }
   }
   return;
} // end of check_delx_all_ex
// -----------------------------------------------------------------------------
void check_convergence_count_e(
   SolveBlocks &slv) {

   int count;

   count = 0;
   if (slv.e_nr_flag_check_spice    ) count++;
   if (slv.e_nr_flag_check_rhs2     ) count++;
   if (slv.e_nr_flag_check_delx_volt) count++;
   if (slv.e_nr_flag_check_delx_all ) count++;
   if (count == 0) {
     cout << "check_convergence_count_e: no convergence criterion?" << endl;
     cout << "  Halting..." << endl; exit(1);
   }
   return;
} // end of check_convergence_count_e
// -----------------------------------------------------------------------------
void check_convergence_count_x(
   SolveBlocks &slv) {

   int count;

   count = 0;
   if (slv.x_nr_flag_check_rhs2    ) count++;
   if (slv.x_nr_flag_check_delx_all) count++;

   if (count == 0) {
     cout << "check_convergence_count_x: no convergence criterion?" << endl;
     cout << "  Halting..." << endl; exit(1);
   }
   return;
} // end of check_convergence_count_x
// -----------------------------------------------------------------------------
void check_convergence_count_ex(
   SolveBlocks &slv) {

   int count;

   count = 0;
   if (slv.ex_nr_flag_check_rhs2    ) count++;
   if (slv.ex_nr_flag_check_delx_all) count++;
   if (slv.e_nr_flag_check_spice    ) count++;
   if (slv.e_nr_flag_check_delx_volt) count++;

   if (count == 0) {
     cout << "check_convergence_count_ex: no convergence criterion?" << endl;
     cout << "  Halting..." << endl; exit(1);
   }
   return;
} // end of check_convergence_count_ex
