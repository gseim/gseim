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

#include "matop.h"

MatOp::MatOp() {

   flag_alloc = true;
   flag_delete = false;

   return;
}
// -----------------------------------------------------------------------------
void MatOp::allocate_1(
   const int n_row) {

   if (flag_alloc) {
     ip_matop1      = new int[n_row + 1];
     ip_matop3      = new int[n_row + 1];
     ip_matnp       = new int[n_row + 1];
     row_ordr_w     = new int[n_row];
     col_ordr_w     = new int[n_row];
     mat_rowexch    = new int[n_row];
     mat_pp         = new int[n_row];
     mat_np         = new int[(n_row*(n_row+1))/2];
     row_ordr_inv_w = new int[n_row];
     col_ordr_inv_w = new int[n_row];

     l_np_zero = new bool[(n_row*(n_row+1))/2];

     ip1_matop2 = new int*[n_row+1];
     for (int i=0; i <= n_row; i++) {
       ip1_matop2[i] = new int[n_row+1];
     }

     l_occ_1 = new bool[n_row];
     l_occ_2 = new bool[n_row];
     l_occ_3 = new bool[n_row];
     l_new   = new bool[n_row];

     inv_col_1 = new int[n_row];
     inv_col_3 = new int[n_row];
     nnz_row   = new int[n_row];
     nnz_col   = new int[n_row];
     optype    = new int[n_row];
     opindex   = new int[n_row];

     mat_nn.clear();
     mat_op_1.clear();
     mat_op_2.clear();
     mat_op_3.clear();
     mat_pn_1.clear();
     mat_pn_2.clear();
     mat_pn_3.clear();
     mat_np_2.clear();
     mat_pn_indx.clear();

     valmax_row = new double[n_row];
     valmax_col = new double[n_row];

     flag_alloc = false;
     flag_delete = true;
   } else {
     cout << "MatOp::allocate_1: trying to allocate with" << endl;
     cout << "  flag_alloc = false? Halting..." << endl; exit(1);
   }

   assign_array_1<bool>(l_occ_1,n_row,false);
   assign_array_1<bool>(l_occ_2,n_row,false);
   assign_array_1<bool>(l_occ_3,n_row,false);
   assign_array_1<bool>(l_new  ,n_row,false);

   assign_array_1<int>(inv_col_1,n_row,0);
   assign_array_1<int>(inv_col_3,n_row,0);
   assign_array_1<int>(nnz_row  ,n_row,0);
   assign_array_1<int>(nnz_col  ,n_row,0);
   assign_array_1<int>(optype   ,n_row,0);
   assign_array_1<int>(opindex  ,n_row,0);

   assign_array_1<int>(ip_matop1,n_row+1,0);
   assign_array_1<int>(ip_matop3,n_row+1,0);
   assign_array_1<int>(ip_matnp,n_row+1,0);
   assign_array_1<int>(row_ordr_w,n_row,0);
   assign_array_1<int>(col_ordr_w,n_row,0);
   assign_array_1<int>(mat_rowexch,n_row,0);
   assign_array_1<int>(mat_pp,n_row,0);
   assign_array_1<int>(mat_np,(n_row*(n_row+1))/2,0);
   assign_array_1<int>(row_ordr_inv_w,n_row,0);
   assign_array_1<int>(col_ordr_inv_w,n_row,0);

   assign_array_1<double>(valmax_row,n_row,0.0);
   assign_array_1<double>(valmax_col,n_row,0.0);

   return;
}
// -----------------------------------------------------------------------------
void MatOp::delete_1(
   const int n_row) {

   if (flag_delete) {
     delete[] ip_matop1;
     delete[] ip_matop3;
     delete[] ip_matnp;
     delete[] row_ordr_w;
     delete[] col_ordr_w;
     delete[] mat_rowexch;
     delete[] mat_pp;
     delete[] mat_np;
     delete[] row_ordr_inv_w;
     delete[] col_ordr_inv_w;

     delete[] l_np_zero;

     for (int i=0; i <= n_row; i++) {
       delete[] ip1_matop2[i];
     }
     delete[] ip1_matop2;

     delete[] l_occ_1;
     delete[] l_occ_2;
     delete[] l_occ_3;
     delete[] l_new;

     delete[] inv_col_1;
     delete[] inv_col_3;
     delete[] nnz_row;
     delete[] nnz_col;
     delete[] optype;
     delete[] opindex;

     delete[] valmax_row;
     delete[] valmax_col;

     flag_alloc = true;
     flag_delete = false;
   } else {
     cout << "MatOp::allocate_1: trying to delete with" << endl;
     cout << "  flag_delete = false? Halting..." << endl; exit(1);
   }

   return;
}
