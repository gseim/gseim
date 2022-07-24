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


#include "utils.h"
#include "matrix_2.h"
#include "matgnrl1.h"
// -----------------------------------------------------------------------------
void gauss_1(
   const bool flag_debug,
   KnuthMat &m,
   KnuthMat &w,
   MatOp &mo) {

// define operations involved in gauss elim (do not perform the
// operations here; only define them)

   int irowop1,nrowp1,count_np;
   int pntr,pntr_1,pntr_piv,row_piv,col_piv;
   int pntr_np,pntr_nn;
   bool l_new_1,l_new_2;
   int i_row_outer,i_col,k1,k2;
   int op_type,op_indx;

   double* rhs_dumy;

   if (m.n_row != m.n_col) {
     cout << "gauss_1: m.n_row = " << m.n_row << ", m.n_col = " << m.n_col
       << "? Halting..." << endl; exit(1);
   }

   rhs_dumy = new double[w.n_row];
   assign_array_1<double>(rhs_dumy,w.n_row,0.0);

   for (int i=0; i < (w.n_row*(w.n_row+1))/2; i++) {
     mo.mat_np[i] = -1;
   }

   nrowp1 = w.n_row + 1;
   mo.n_matttlop1 = 0;
   mo.ip_matop1[0] = 0;

   mo.n_matttlop2 = 0;
   mo.n_matttlop3 = 0;
   mo.ip_matnp[0] = 0;
   count_np = 0;

   for (int i_row=0; i_row < w.n_row; i_row++) {
     mo.row_ordr_w[i_row] = i_row;
     mo.col_ordr_w[i_row] = i_row;
   }

   cout << "gauss_1: w.n_nz = " << w.n_nz << endl;

   for (int i=0; i < w.n_nz; i++) {
     w.val[i] = m.val[i];
   }

   for (int i_row_outer=0; i_row_outer < w.n_row-1; i_row_outer++) {
     nnz_active_k(i_row_outer,mo.nnz_row,mo.nnz_col,w);

     markow_1_k(i_row_outer,mo.nnz_row,mo.nnz_col,
       pntr_piv,row_piv,col_piv,w);

//   interchange rows and columns

     if (i_row_outer != row_piv) {
       row_exch_k(i_row_outer,row_piv,mo.row_ordr_w,w,rhs_dumy);
     }
     if (i_row_outer != col_piv) {
       col_exch_k(i_row_outer,col_piv,mo.col_ordr_w,w);
     }

//   store row_piv for processing of RHS

     mo.mat_rowexch[i_row_outer] = row_piv;

//   pivot row

     mo.mat_pp[i_row_outer] = pntr_piv;

     pntr = w.prow[pntr_piv];

     for (int i_pn=0; i_pn < nrowp1; i_pn++) {
       if (pntr != -1) {
         mo.n_matttlop1++;
         mo.mat_op_1.push_back(2);
         mo.mat_pn_1.push_back(pntr);
         pntr = w.prow[pntr];
       } else {
         break;
       }
     }

//   increment pointer for pivot row operations

     irowop1 = i_row_outer + 1;
     mo.ip_matop1[irowop1] = mo.n_matttlop1;

//   Process all non pivot rows for the given pivot row
//   define ip_matnp

     mo.ip_matnp[irowop1] = mo.ip_matnp[i_row_outer] + w.n_row - i_row_outer - 1;

     for (int i=irowop1; i < w.n_row; i++) {
       mo.l_occ_1[i] = false;
     }

     pntr = pntr_piv;
     for (int i=0; i < nrowp1; i++) {
       if (pntr != -1) {
         i_col = w.col[pntr];
         mo.inv_col_1[i_col] = pntr;
         mo.l_occ_1[i_col] = true;

         pntr = w.prow[pntr];
       } else {
         break;
       }
     }

//   loop for np rows

     l_new_2 = false;

     for (int i_row_inner=irowop1; i_row_inner < w.n_row; i_row_inner++) {

       mo.l_np_zero[count_np] = true;

//     define inv_col for the non-pivot row

       for (int i=i_row_outer; i < w.n_row; i++) {
         mo.l_occ_2[i] = false;
       }

       pntr = w.srow[i_row_inner];
       for (int i=0; i < nrowp1; i++) {
         if (pntr != -1) {
           i_col = w.col[pntr];
           if (i_col >= i_row_outer) {
             mo.inv_col_3[i_col] = pntr;
             mo.l_occ_2[i_col] = true;
           }
           pntr = w.prow[pntr];
         } else {
           break;
         }
       }

       mo.ip1_matop2[i_row_inner][i_row_outer] = mo.n_matttlop2;

       if (mo.l_occ_2[i_row_outer]) {
         pntr_np = mo.inv_col_3[i_row_outer];
         mo.mat_np[count_np] = pntr_np;
         mo.l_np_zero[count_np] = false;

         for (int i_col=irowop1; i_col < w.n_row; i_col++) {
           mo.l_occ_3[i_col] = mo.l_occ_2[i_col];
           mo.optype[i_col] = 13;
         }

         for (int i_col=irowop1; i_col < w.n_row; i_col++) {
           if (mo.l_occ_1[i_col]) {
             if (mo.l_occ_2[i_col]) {
               pntr_nn = mo.inv_col_3[i_col];

               mo.n_matttlop2++;

               mo.mat_nn.push_back(0);
               mo.mat_op_2.push_back(0);
               mo.mat_pn_2.push_back(0);
               mo.mat_np_2.push_back(0);

               mo.opindex[i_col] = mo.n_matttlop2 - 1;
               mo.optype[i_col] = 6;
             } else {
               mo.l_occ_3[i_col] = true;
               w.n_nz++;

               w.row.push_back(0); 
               w.col.push_back(0); 
               w.prow.push_back(0);
               w.pcol.push_back(0);
               w.val.push_back(0.0);

               mo.inv_col_3[i_col] = w.n_nz - 1;
               mo.n_matttlop2++;

               mo.mat_nn.push_back(0);
               mo.mat_op_2.push_back(0);
               mo.mat_pn_2.push_back(0);
               mo.mat_np_2.push_back(0);

               mo.opindex[i_col] = mo.n_matttlop2 - 1;
               mo.optype[i_col] = 5;
             }
           }
         }

//       define arrays required depending on the op type

         for (int i_col=irowop1; i_col < w.n_row; i_col++) {
           op_type = mo.optype[i_col];

           if (op_type != 13) {
             pntr_nn = mo.inv_col_3[i_col];
             op_indx = mo.opindex[i_col];
             mo.mat_nn[op_indx] = pntr_nn;
             mo.mat_op_2[op_indx] = op_type;

             if (op_type == 2) {
               mo.mat_np_2[op_indx] = pntr_np;
             } else if (op_type == 3) {
               mo.mat_np_2[op_indx] = pntr_np;
             } else if (op_type == 5) {
               mo.mat_pn_2[op_indx] = mo.inv_col_1[i_col];
               mo.mat_np_2[op_indx] = pntr_np;
             } else if (op_type == 6) {
               mo.mat_pn_2[op_indx] = mo.inv_col_1[i_col];
               mo.mat_np_2[op_indx] = pntr_np;
             } else {
               cout << "gauss_1: wrong op type (1)? Halting..." << endl;
               exit(1);
             }
             mo.l_new[i_col] = !mo.l_occ_2[i_col] && mo.l_occ_3[i_col];

             if (mo.l_new[i_col]) {
               w.row[pntr_nn] = i_row_inner;
               w.col[pntr_nn] = i_col;
             }
           }
         }

//       if new entries have been added, assign prow, etc.

         l_new_1 = false;
         for (int i_col=irowop1; i_col < w.n_row; i_col++) {
           if (mo.l_new[i_col]) {
             l_new_1 = true;
             l_new_2 = true;
             break;
           }
         }
         if (l_new_1) {
           pntr = pntr_np;

           for (int i_col=irowop1; i_col < w.n_row; i_col++) {
             if (mo.l_occ_3[i_col]) {
               pntr_1 = mo.inv_col_3[i_col];
               w.prow[pntr] = pntr_1;
               pntr = pntr_1;
             }
           }
           w.erow[i_row_inner] = pntr;
           w.prow[pntr] = -1;
         }
       }
       count_np++;
     }
     mo.ip1_matop2[w.n_row][i_row_outer] = mo.n_matttlop2;

//   assign pcol if there are new entries

     if (l_new_2) {
       knuth_makecols(w);
     }
   }

   for (int i_row=0; i_row < w.n_row; i_row++) {
     k1 = mo.row_ordr_w[i_row];
     mo.row_ordr_inv_w[k1] = i_row;

     k2 = mo.col_ordr_w[i_row];
     mo.col_ordr_inv_w[k2] = i_row;
   }

// treat the last row here

   i_row_outer = w.n_row - 1;

// find pntr_piv

   pntr = w.srow[w.n_row-1];

   for (int i=0; i < nrowp1; i++) {
     if (pntr == -1) {
       cout << "gauss_1: last row: zero pivot?" << endl;
       cout << "   Error in Gauss elimination routine" << endl;
       cout << "   zero pivot" << endl;
       exit(1);
     }
     if (w.col[pntr] == w.n_col-1) {
       pntr_piv = pntr;
       break;
     } else {
       pntr = w.prow[pntr];
     }
   }
   mo.mat_pp[i_row_outer] = pntr_piv;

// back substitution

   mo.n_matttlop3 = 0;
   mo.ip_matop3[w.n_row-1] = 0;

   for (int i_row=w.n_row-2; i_row >= 0; i_row--) {
     pntr = w.srow[i_row];

     for (int i=0; i < nrowp1; i++) {
       if (pntr != -1) {
         i_col = w.col[pntr];

         if (i_col > i_row) {
           mo.n_matttlop3++;
           mo.mat_pn_indx.push_back(i_col);
           mo.mat_op_3.push_back(3);
           mo.mat_pn_3.push_back(pntr);
         }
         pntr = w.prow[pntr];
       } else {
         break;
       }
     }
     mo.ip_matop3[i_row] = mo.n_matttlop3;
   }

// delete temporary arrays:

   delete[] rhs_dumy;

   return;
} // end of gauss_1
// -----------------------------------------------------------------------------
void gauss_2(
   const int flag,
   const bool flag_debug,
   const double zero_piv,
   bool &flag_error,
   Global &global,
   KnuthMat &m,
   KnuthMat &w,
   MatOp &mo,
   double* rhs_w,
   double* svec_w,
   double* svec_orig_w) {

// note: Backsub includes operations on RHS

   int irowop1;
   int pntr_piv;
   double val_piv,val_pivinv,val_fn,val_pn,val_np,val_nn;
   double rhs_temp,rhs_piv,x_pn;
   int optype1,optype2;
   int i_start,i_end;
   int pntr_nn,pntr_pn,pntr_np;
   int i_col,row_2;
   int k,count_np;

   if (m.n_row != m.n_col) {
     cout << "gauss_2: m.n_row = " << m.n_row << ", m.n_col = " << m.n_col
       << "? Halting..." << endl; exit(1);
   }

   flag_error = false;

   if ((flag == global.I_LU) || (flag == global.I_LU_BSUB)) {
     w.n_nz = m.n_nz;
     for (int i=0; i < w.n_nz; i++) {
       w.val[i] = m.val[i];
     }
     for (int i_row_outer=0; i_row_outer < w.n_row-1; i_row_outer++) {
//     type 1 operations

       irowop1 = i_row_outer + 1;
       pntr_piv = mo.mat_pp[i_row_outer];
       val_piv = w.val[pntr_piv];

       if (fabs(val_piv) < zero_piv) {
         cout << "gauss_2: type 1 val_piv = " << val_piv << endl;
         cout << "         zero_piv = " << zero_piv << endl;
         cout << "         pntr_piv = " << pntr_piv << endl;
         cout << "         i_row_outer = " << i_row_outer << endl;
         cout << "*** error in gauss_2: pivot too small ***" << endl;
         flag_error = true;
         return;
       }
       val_pivinv = 1.0/val_piv;

       for (int i=mo.ip_matop1[i_row_outer]; i < mo.ip_matop1[irowop1]; i++) {
         optype1 = mo.mat_op_1[i];
         pntr_pn = mo.mat_pn_1[i];

         if (optype1 == 2) {
           w.val[pntr_pn] = val_pivinv*w.val[pntr_pn];
         } else if (optype1 == 3) {
           w.val[pntr_pn] = val_pivinv;
         } else {
           w.val[pntr_pn] = 0.0;
         }
       }

//     type 2 operations

       for (int i_row_inner=irowop1; i_row_inner < w.n_row; i_row_inner++) {
         i_start = mo.ip1_matop2[i_row_inner  ][i_row_outer];
         i_end   = mo.ip1_matop2[i_row_inner+1][i_row_outer];

         for (int i=i_start; i < i_end; i++) {
           optype2 = mo.mat_op_2[i];
           pntr_nn = mo.mat_nn[i];

           if (optype2 >= 11) {
             val_fn = 0.0;
           } else if (optype2 >= 5) {
             pntr_pn = mo.mat_pn_2[i];
             val_pn = w.val[pntr_pn];
           }
//         perform the required operation

           if (optype2 == 2) {
             pntr_np = mo.mat_np_2[i];
             val_np = w.val[pntr_np];
             w.val[pntr_nn] = val_np;
           } else if (optype2 == 3) {
             pntr_np = mo.mat_np_2[i];
             val_np = w.val[pntr_np];
             val_nn = w.val[pntr_nn];
             w.val[pntr_nn] = val_nn - val_np;
           } else if (optype2 == 5) {
             pntr_np = mo.mat_np_2[i];
             val_np = w.val[pntr_np];
             w.val[pntr_nn] = -val_np*val_pn;
           } else if (optype2 == 6) {
             pntr_np = mo.mat_np_2[i];
             val_np = w.val[pntr_np];
             val_nn = w.val[pntr_nn];
             w.val[pntr_nn] = val_nn - val_np*val_pn;
           } else {
             val_nn = w.val[pntr_nn];
             w.val[pntr_nn] = val_nn - val_fn;
           }
         }
       }
     }
   }

   if ((flag == global.I_BSUB) || (flag == global.I_LU_BSUB)) {
//   RHS operations
     for (int i_row_outer=0; i_row_outer < w.n_row-1; i_row_outer++) {
//     row exchange
       row_2 = mo.mat_rowexch[i_row_outer];

       if (row_2 != i_row_outer) {
         rhs_temp = rhs_w[i_row_outer];
         rhs_w[i_row_outer] = rhs_w[row_2];
         rhs_w[row_2] = rhs_temp;
       }
       pntr_piv = mo.mat_pp[i_row_outer];
       val_piv = w.val[pntr_piv];

       if (fabs(val_piv) < zero_piv) {
         cout << "gauss_2: val_piv = " << val_piv << endl;
         cout << "gauss_2: i_row_outer = " << i_row_outer << endl;
         flag_error = true;
         return;
       }
       rhs_w[i_row_outer] = rhs_w[i_row_outer]/val_piv;
       rhs_piv = rhs_w[i_row_outer];

       count_np = mo.ip_matnp[i_row_outer];
       irowop1 = i_row_outer + 1;

       for (int i_row_inner=irowop1; i_row_inner < w.n_row; i_row_inner++) {
         if (!mo.l_np_zero[count_np]) {
           pntr_np = mo.mat_np[count_np];
           val_np = w.val[pntr_np];
           rhs_w[i_row_inner] = rhs_w[i_row_inner] - val_np*rhs_piv;
         }
         count_np++;
       }
     }

//   Back sub

     pntr_piv = mo.mat_pp[w.n_row-1];
     val_piv = w.val[pntr_piv];

     if (fabs(val_piv) < zero_piv) {
       cout << "gauss_2: pntr_piv = " << pntr_piv << endl;
       cout << "gauss_2: val_piv = " << val_piv << endl;
       cout << "gauss_2: zero pivot in the last row!" << endl;
       flag_error = true;
       return;
     }
     svec_w[w.n_row-1] = rhs_w[w.n_row-1]/val_piv;

     for (int i_row=w.n_row-2; i_row >= 0; i_row--) {
       svec_w[i_row] = rhs_w[i_row];
       i_start = mo.ip_matop3[i_row+1];
       i_end   = mo.ip_matop3[i_row];

       for (int i=i_start; i < i_end; i++) {
         i_col = mo.mat_pn_indx[i];
         x_pn = svec_w[i_col];

         pntr_pn = mo.mat_pn_3[i];
         val_pn = w.val[pntr_pn];
         svec_w[i_row] = svec_w[i_row]-val_pn*x_pn;
       }
     }
//   solution vector in original order

     for (int i_col=0; i_col < w.n_col; i_col++) {
       k = mo.col_ordr_inv_w[i_col];
       svec_orig_w[i_col] = svec_w[k];
     }
   }
   return;
} // end of gauss_2
// -----------------------------------------------------------------------------
void gauss_1a(
   const int flag_debug,
   const double gauss_epsln,
   KnuthMat &m,
   KnuthMat &w,
   MatOp &mo) {

// Define operations involved in gauss elim.
// Use threshold pivoting and Markowitz sparsity critrion.
//
// For simplicity, call gauss_1a and gauss_2 for symbolic and
// numerical, respectively, although some of the operations in the
// first call to gauss_2 are already done in gauss_1a.

   int irowop1,nrowp1,count_np;
   int pntr,pntr_1,pntr_piv,row_piv,col_piv;
   int pntr_pn,pntr_np,pntr_nn;
   int l_new_1,l_new_2;
   int i_row_outer,i_col,k1,k2;
   int op_type,op_indx;

   double u_threshold=0.25;
   double val_piv,val_pivinv;
   double epsln;
   double val_pn,val_np,val_nn;

   cout << "gauss_1a starts..." << endl;

   if (m.n_row != m.n_col) {
     cout << "gauss_1a: m.n_row = " << m.n_row << ", m.n_col = " << m.n_col
       << "? Halting..." << endl; exit(1);
   }

   epsln = gauss_epsln;

   bool *l_mark;
   vector<bool> l_mark0;
   double *rhs_dumy;

   l_mark = new bool[m.n_nz+1];
   assign_array_1<bool>(l_mark,m.n_nz+1,false);

   l_mark0.resize(m.n_nz+1);
   assign_const_1<bool>(l_mark0,false);

   rhs_dumy   = new double[w.n_row];

   assign_array_1<double>(rhs_dumy,w.n_row,0.0);
   assign_array_1<double>(mo.valmax_row,w.n_row,0.0);
   assign_array_1<double>(mo.valmax_col,w.n_row,0.0);

// initialize

   for (int i=0; i < (w.n_row*(w.n_row+1))/2; i++) {
     mo.mat_np[i] = -1;
   }
   nrowp1 = w.n_row + 1;
   mo.n_matttlop1 = 0;
   mo.ip_matop1[0] = 0;

   mo.n_matttlop2 = 0;
   mo.n_matttlop3 = 0;
   mo.ip_matnp[0] = 0;
   count_np = 0;

   for (int i_row=0; i_row < w.n_row; i_row++) {
     mo.row_ordr_w[i_row] = i_row;
     mo.col_ordr_w[i_row] = i_row;
   }
   for (int i=0; i < w.n_nz; i++) {
     w.val[i] = m.val[i];
   }
   for (int i_row_outer=0; i_row_outer < w.n_row-1; i_row_outer++) {
     nnz_active_k_1(i_row_outer,mo.nnz_row,mo.nnz_col,
       mo.valmax_row,mo.valmax_col,w);

     markow_2_k0(i_row_outer,mo.nnz_row,mo.nnz_col,
       pntr_piv,row_piv,col_piv,
       l_mark0,mo.valmax_row,mo.valmax_col,u_threshold,w);

//   interchange rows and columns

     if (i_row_outer != row_piv) {
       row_exch_k(i_row_outer,row_piv,mo.row_ordr_w,w,rhs_dumy);
     }
     if (i_row_outer != col_piv) {
       col_exch_k(i_row_outer,col_piv,mo.col_ordr_w,w);
     }

//   store row_piv for processing of RHS

     mo.mat_rowexch[i_row_outer] = row_piv;

//   pivot row

     mo.mat_pp[i_row_outer] = pntr_piv;

     val_piv = w.val[pntr_piv];
     if (fabs(val_piv) < epsln) {
       cout << "gauss_1a: val_piv=" << val_piv << endl;
       cout << "   Error in Gauss elimination routine." << endl;
       cout << "   Pivot is too small. Halting..." << endl;
       exit(1);
     }
     val_pivinv = 1.0/val_piv;

     pntr = w.prow[pntr_piv];

     for (int i_pn=0; i_pn < nrowp1; i_pn++) {
       if (pntr != -1) {
         val_pn = w.val[pntr];

         mo.n_matttlop1++;
         mo.mat_op_1.push_back(2);
         mo.mat_pn_1.push_back(pntr);

         w.val[pntr] = val_pivinv*val_pn;
         pntr = w.prow[pntr];
       } else {
         break;
       }
     }

//   increment pointer for pivot row operations

     irowop1 = i_row_outer + 1;
     mo.ip_matop1[irowop1] = mo.n_matttlop1;

//   Process all non pivot rows for the given pivot row
//   define ip_matnp

     mo.ip_matnp[irowop1] = mo.ip_matnp[i_row_outer] + w.n_row - i_row_outer - 1;

//   define inv_col for the pivot row

     for (int i=irowop1; i < w.n_row; i++) {
       mo.l_occ_1[i] = false;
     }

     pntr = pntr_piv;
     for (int i=0; i < nrowp1; i++) {
       if (pntr != -1) {
         i_col = w.col[pntr];
         mo.inv_col_1[i_col] = pntr;
         mo.l_occ_1[i_col] = true;

         pntr = w.prow[pntr];
       } else {
         break;
       }
     }

//   loop for np rows

     l_new_2 = false;

     for (int i_row_inner=irowop1; i_row_inner < w.n_row; i_row_inner++) {

       mo.l_np_zero[count_np] = true;

//     define inv_col for the non-pivot row

       for (int i=i_row_outer; i < w.n_row; i++) {
         mo.l_occ_2[i] = false;
       }

       pntr = w.srow[i_row_inner];
       for (int i=0; i < nrowp1; i++) {
         if (pntr != -1) {
           i_col = w.col[pntr];
           if (i_col >= i_row_outer) {
             mo.inv_col_3[i_col] = pntr;
             mo.l_occ_2[i_col] = true;
           }
           pntr = w.prow[pntr];
         } else {
           break;
         }
       }

       mo.ip1_matop2[i_row_inner][i_row_outer] = mo.n_matttlop2;

       if (mo.l_occ_2[i_row_outer]) {
         pntr_np = mo.inv_col_3[i_row_outer];
         mo.mat_np[count_np] = pntr_np;
         val_np = w.val[pntr_np];
         mo.l_np_zero[count_np] = false;

         for (int i_col=irowop1; i_col < w.n_row; i_col++) {
           mo.l_occ_3[i_col] = mo.l_occ_2[i_col];
           mo.optype[i_col] = 13;
         }

         for (int i_col=irowop1; i_col < w.n_row; i_col++) {
           if (mo.l_occ_1[i_col]) {
             pntr_pn = mo.inv_col_1[i_col];
             val_pn = w.val[pntr_pn];

             if (mo.l_occ_2[i_col]) {
               pntr_nn = mo.inv_col_3[i_col];
               val_nn = w.val[pntr_nn];

               mo.n_matttlop2++;

               mo.mat_nn.push_back(0);
               mo.mat_op_2.push_back(0);
               mo.mat_pn_2.push_back(0);
               mo.mat_np_2.push_back(0);

               mo.opindex[i_col] = mo.n_matttlop2 - 1;
               mo.optype[i_col] = 6;
             } else {
               mo.l_occ_3[i_col] =  true ;
               w.n_nz++;

               w.row.push_back(0); 
               w.col.push_back(0); 
               w.prow.push_back(0);
               w.pcol.push_back(0);
               w.val.push_back(0.0);

               mo.inv_col_3[i_col] = w.n_nz - 1;
               mo.n_matttlop2++;

               mo.mat_nn.push_back(0);
               mo.mat_op_2.push_back(0);
               mo.mat_pn_2.push_back(0);
               mo.mat_np_2.push_back(0);

               mo.opindex[i_col] = mo.n_matttlop2 - 1;
               mo.optype[i_col] = 5;
             }
           }
         }

//       define arrays required depending on the op type

         for (int i_col=irowop1; i_col < w.n_row; i_col++) {
           op_type = mo.optype[i_col];

           if (op_type != 13) {
             pntr_nn = mo.inv_col_3[i_col];
             val_nn = w.val[pntr_nn];
             pntr_pn = mo.inv_col_1[i_col];
             val_pn = w.val[pntr_pn];
             op_indx = mo.opindex[i_col];
             mo.mat_nn[op_indx] = pntr_nn;
             mo.mat_op_2[op_indx] = op_type;

             if (op_type == 2) {
               mo.mat_np_2[op_indx] = pntr_np;
               w.val[pntr_nn] = val_np;
             } else if (op_type == 3) {
               mo.mat_np_2[op_indx] = pntr_np;
               w.val[pntr_nn] = val_nn-val_np;
             } else if (op_type == 5) {
               mo.mat_pn_2[op_indx] = mo.inv_col_1[i_col];
               mo.mat_np_2[op_indx] = pntr_np;
               w.val[pntr_nn] = -val_np*val_pn;
             } else if (op_type == 6) {
               mo.mat_pn_2[op_indx] = mo.inv_col_1[i_col];
               mo.mat_np_2[op_indx] = pntr_np;
               w.val[pntr_nn] = val_nn-val_np*val_pn;
             } else {
               cout << "gauss_1a: wrong op type (1)? Halting..." << endl;
               exit(1);
             }
             mo.l_new[i_col] = !mo.l_occ_2[i_col] && mo.l_occ_3[i_col];

             if (mo.l_new[i_col]) {
               w.row[pntr_nn] = i_row_inner;
               w.col[pntr_nn] = i_col;
             }
           }
         }

//       if new entries have been added, assign prow, etc.

         l_new_1 =  false ;
         for (int i_col=irowop1; i_col < w.n_row; i_col++) {
           if (mo.l_new[i_col]) {
             l_new_1 =  true ;
             l_new_2 =  true ;
             break;
           }
         }
         if (l_new_1) {
           pntr = pntr_np;

           for (int i_col=irowop1; i_col < w.n_row; i_col++) {
             if (mo.l_occ_3[i_col]) {
               pntr_1 = mo.inv_col_3[i_col];
               w.prow[pntr] = pntr_1;
               pntr = pntr_1;
             }
           }
           w.erow[i_row_inner] = pntr;
           w.prow[pntr] = -1;
         }
       }
       count_np++;
     }
     mo.ip1_matop2[w.n_row][i_row_outer] = mo.n_matttlop2;

//   assign pcol if there are new entries

     if (l_new_2) {
       knuth_makecols(w);
     }
   }

   for (int i_row=0; i_row < w.n_row; i_row++) {
     k1 = mo.row_ordr_w[i_row];
     mo.row_ordr_inv_w[k1] = i_row;

     k2 = mo.col_ordr_w[i_row];
     mo.col_ordr_inv_w[k2] = i_row;
   }

// treat the last row here

   i_row_outer = w.n_row - 1;

// find pntr_piv

   pntr = w.srow[w.n_row-1];
// cout << "gauss_1a debug 1: pntr: " << pntr << endl;

   for (int i=0; i < nrowp1; i++) {
     if (pntr == -1) {
       cout << "gauss_1a: last row: zero pivot?" << endl;
       cout << "   Error in Gauss elimination routine" << endl;
       cout << "   zero pivot" << endl;
       cout << "   i: " << i << endl;
       exit(1);
     }
     if (w.col[pntr] == w.n_col-1) {
       pntr_piv = pntr;
       break;
     } else {
       pntr = w.prow[pntr];
     }
   }
   mo.mat_pp[i_row_outer] = pntr_piv;

// back substitution

   mo.n_matttlop3 = 0;
   mo.ip_matop3[w.n_row-1] = 0;

   for (int i_row=w.n_row-2; i_row >= 0; i_row--) {
     pntr = w.srow[i_row];

     for (int i=0; i < nrowp1; i++) {
       if (pntr != -1) {
         i_col = w.col[pntr];

         if (i_col > i_row) {
           mo.n_matttlop3++;
           mo.mat_pn_indx.push_back(i_col);
           mo.mat_op_3.push_back(3);
           mo.mat_pn_3.push_back(pntr);
         }
         pntr = w.prow[pntr];
       } else {
         break;
       }
     }
     mo.ip_matop3[i_row] = mo.n_matttlop3;
   }

// delete temporary arrays:

   delete[] l_mark;
   delete[] rhs_dumy;

   return;
} // end of gauss_1a
