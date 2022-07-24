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

#include "matrix_2.h"
// -----------------------------------------------------------------------------
void write_vector_double_2(
   const int n,
   double *b,
   char *fname,
   char *message) {

   fstream outf;
   outf.open(fname,ios::out|ios::binary);
   outf << message << endl;
   outf << scientific;
   outf << setprecision(4);

   for (int i=0; i < n; i++) {
     outf << setw(3) << i << "  " << setw(11) << b[i] << endl;
   }
   outf.close();
   return;
}
// -----------------------------------------------------------------------------
void negative_double_1(
   const int n,
   double *b) {

   for (int i=0; i < n; i++) {
     b[i] = -b[i];
   }
   return;
}
// -----------------------------------------------------------------------------
void write_mat_knuth_3(
   KnuthMat &a_knuth,
   char *fname) {

// write matrix in knuth form row-wise (eac entry in a new line)

   fstream outf;
   int j1,j_col;

   outf.open(fname,ios::out|ios::binary);

   outf << scientific;
   outf << setprecision(4);

   outf << "n_nz = " << a_knuth.n_nz << endl;
   for (int i_row=0; i_row < a_knuth.n_row; i_row++) {
     outf << "i_row = " << i_row << endl;
     j1 = a_knuth.srow[i_row];
     for (int i1=0; i1 <= a_knuth.n_col; i1++) {
       if (j1 == -1) {
         break;
       } else {
         j_col = a_knuth.col[j1];
         outf << setw(3) << j_col << "  " << setw(11) << a_knuth.val[j1] << endl;
         j1 = a_knuth.prow[j1];
       }
     }
   }
   outf.close();
   return;
}
// -----------------------------------------------------------------------------
void knuth_addentry(
   KnuthMat &a,
   const int row_given,
   const int col_given,
   const double val_given) {

   bool l_add_r,l_add_c;
   int pntr_r=0,pntr_r_1=0,pntr_r_last=0;
   int pntr_c=0,pntr_c_1=0,pntr_c_last=0;
   int i_row=0,i_col=0;
   int nnzm1;

// cout << "knuth_addentry (1): a.val.size() = " << a.val.size() << endl;

   l_add_r = false;
   pntr_r = a.srow[row_given];

   for (int i=0; i <= a.n_col; i++) {
     if (pntr_r == -1) {
       l_add_r = true;
     } else {
       pntr_r_1 = a.prow[pntr_r];
       i_col = a.col[pntr_r];
       if (i_col == col_given) {
         cout << "knuth_addentry: i_col=col_given !" << endl;
         cout << "  Something wrong with the system matrix?" << endl;
         cout << "  i_col = " << i_col << ", col_given = " << col_given << endl;
         cout << "  row_given = " << row_given << endl;
         cout << "  Halting..." << endl;
         exit (1);
       } else if (i_col > col_given) {
         l_add_r = true;
       }
     }
     if (l_add_r) {
       nnzm1 = a.n_nz;
       a.n_nz++;

       a.val.push_back(val_given);
       a.row.push_back(row_given);
       a.col.push_back(col_given);

       if (i == 0) a.srow[row_given] = nnzm1;
       if (pntr_r != -1) {
         a.prow.push_back(pntr_r);
       } else {
         a.prow.push_back(-1);
         a.erow[row_given] = nnzm1;
       }
       if (i != 0) a.prow[pntr_r_last] = nnzm1;

       l_add_c = false;
       pntr_c = a.scol[col_given];

       for (int j=0; j <= a.n_row; j++) {
         if (pntr_c == -1) {
           l_add_c = true;
         } else {
           pntr_c_1 = a.pcol[pntr_c];
           i_row = a.row[pntr_c];
           if (i_row > row_given) l_add_c = true;
         }
         if (l_add_c) {
           if (j == 0) a.scol[col_given] = nnzm1;
           if (pntr_c != -1) {
             a.pcol.push_back(pntr_c);
           } else {
             a.pcol.push_back(-1);
             a.ecol[col_given] = nnzm1;
           }
           if (j != 0) a.pcol[pntr_c_last] = nnzm1;
           goto jump1;
         }
         pntr_c_last = pntr_c;
         pntr_c = pntr_c_1;
       }
       cout << "knuth_addentry: did not exit from inner loop. Halting..." << endl;
       exit (1);
     }
     pntr_r_last = pntr_r;
     pntr_r = pntr_r_1;
   }
   cout << "knuth_addentry: did not exit from outer loop. Halting..." << endl;
   exit (1);
   jump1: ;
// cout << "knuth_addentry (2): a.val.size() = " << a.val.size() << endl;

   return;
} // end of knuth_addentry
// -----------------------------------------------------------------------------
void check_knuth(
   KnuthMat &a,
   char *string_message) {

// perform a few consistency checks on the input matrix.

   int nzero_row,nzero_col;
   int pntr,pntr_a;
   int i_row_old,i_col_old;
   int i_row_1,i_col_1;
   bool **l_rowwise;
   bool **l_colwise;

   l_rowwise = new bool*[a.n_row];
   for (int i_row=0;i_row < a.n_row;i_row++) {
     l_rowwise[i_row] = new bool[a.n_col];
   }

   l_colwise = new bool*[a.n_row];
   for (int i_row=0;i_row < a.n_row;i_row++) {
     l_colwise[i_row] = new bool[a.n_col];
   }

// each row and col must end somewhere; check this by looking at
// srow,prow etc.

   nzero_row = 0;
   nzero_col = 0;

   for (int i_row=0; i_row < a.n_row; i_row++) {
     if (a.srow[i_row] == -1) nzero_row++;
   }
   for (int i=0; i < a.n_nz; i++) {
     if (a.prow[i] == -1) nzero_row++;
   }

   for (int i_col=0; i_col < a.n_col; i_col++) {
     if (a.scol[i_col] == -1) nzero_col++;
   }
   for (int i=0; i < a.n_nz; i++) {
     if (a.pcol[i] == -1) nzero_col++;
   }

   if (nzero_row != a.n_row) {
     cout << "check_knuth: nzero_row must be equal to n_row" << endl;
     cout << "  nzero_row = " << nzero_row << ", n_row = " << a.n_row << endl;
     cout << "  Halting..." << endl;
     cout << "  Message from calling routine:" << endl;
     cout << "  " << string_message << endl;
     exit (1);
   }
   if (nzero_col != a.n_col) {
     cout << "check_knuth: nzero_col must be equal to n_col" << endl;
     cout << "  nzero_col = " << nzero_col << ", n_col = " << a.n_col << endl;
     cout << "  Halting..." << endl;
     cout << "  Message from calling routine:" << endl;
     cout << "  " << string_message << endl;
     exit (1);
   }
// check erow, ecol:

   for (int i_row=0; i_row < a.n_row; i_row++) {
     pntr_a = a.erow[i_row];
     if (pntr_a != -1) {
       pntr = a.prow[pntr_a];
       if (pntr != -1) {
         cout << "check_knuth: i_row = " << i_row
              << ", erow = " << pntr_a << endl;
         cout << "  Halting..." << endl;
         cout << "  Message from calling routine:" << endl;
         cout << "  " << string_message << endl;
         exit (1);
       }
     }
   }
   for (int i_col=0; i_col < a.n_col; i_col++) {
     pntr_a = a.ecol[i_col];
     if (pntr_a != -1) {
       pntr = a.pcol[pntr_a];
       if (pntr != -1) {
         cout << "check_knuth: i_col = " << i_col
              << ", ecol = " << pntr_a << endl;
         cout << "  Halting..." << endl;
         cout << "  Message from calling routine:" << endl;
         cout << "  " << string_message << endl;
         exit (1);
       }
     }
   }

// Check that srow and erow have the same number of zeros, etc.

   for (int i_row=0; i_row < a.n_row; i_row++) {
     if (a.srow[i_row] == -1) {
       if (a.erow[i_row] != -1) {
         cout << "check_knuth: check srow, erow (1). Halting... " << endl;
         cout << "  Message from calling routine:" << endl;
         cout << "  " << string_message << endl;
         exit (1);
       }
     } else {
       if (a.erow[i_row] == -1) {
         cout << "check_knuth: check srow, erow (2). Halting... " << endl;
         cout << "  Message from calling routine:" << endl;
         cout << "  " << string_message << endl;
         exit (1);
       }
     }
   }
   for (int i_col=0; i_col < a.n_col; i_col++) {
     if (a.scol[i_col] == -1) {
       if (a.ecol[i_col] != -1) {
         cout << "check_knuth: check scol, ecol (1). Halting... " << endl;
         cout << "  Message from calling routine:" << endl;
         cout << "  " << string_message << endl;
         exit (1);
       }
     } else {
       if (a.ecol[i_col] == -1) {
         cout << "check_knuth: check scol, ecol (2). Halting... " << endl;
         cout << "  Message from calling routine:" << endl;
         cout << "  " << string_message << endl;
         exit (1);
       }
     }
   }

// construct matrix row-wise and col-wise and compare

   for (int i_row=0; i_row < a.n_row; i_row++) {
     for (int i_col=0; i_col < a.n_col; i_col++) {
       l_rowwise[i_row][i_col] = false;
       l_colwise[i_row][i_col] = false;
     }
   }
   for (int i_row=0; i_row < a.n_row; i_row++) {
     pntr = a.srow[i_row];
     i_col_old = -1;
     for (int i_col=0; i_col <= a.n_col; i_col++) {
       if (pntr != -1) {
         i_row_1 = a.row[pntr];
         i_col_1 = a.col[pntr];
         l_rowwise[i_row_1][i_col_1] = true;
         if (i_col_old >= i_col_1) {
           cout << "check_knuth: check prow. Halting... " << endl;
           cout << "  Message from calling routine:" << endl;
           cout << "  " << string_message << endl;
           exit (1);
         }
         pntr = a.prow[pntr];
         i_col_old = i_col_1;
       } else {
         break;
       }
     }
   }
   for (int i_col=0; i_col < a.n_col; i_col++) {
     pntr = a.scol[i_col];
     i_row_old = -1;
     for (int i_row=0; i_row <= a.n_row; i_row++) {
       if (pntr != -1) {
         i_row_1 = a.row[pntr];
         i_col_1 = a.col[pntr];
         l_colwise[i_row_1][i_col_1] = true;
         if (i_row_old >= i_row_1) {
           cout << "check_knuth: check pcol. Halting... " << endl;
           cout << "  Message from calling routine:" << endl;
           cout << "  " << string_message << endl;
           exit (1);
         }
         pntr = a.pcol[pntr];
         i_row_old = i_row_1;
       } else {
         break;
       }
     }
   }

   for (int i=0; i < a.n_row; i++) {
     for (int j=0; j < a.n_col; j++) {
       if (l_rowwise[i][j] != l_colwise[i][j]) {
         cout << "check_knuth: i = " << i << ", j = " << j << endl;
         cout << "  l_rowwise[i][j] = " << l_rowwise[i][j] << endl;
         cout << "  l_colwise[i][j] = " << l_colwise[i][j] << endl;
         cout << "  Message from calling routine:" << endl;
         cout << "  " << string_message << endl;
         exit (1);
       }
     }
   }

   for (int i_row=0; i_row < a.n_row; i_row++) {
     delete[] l_rowwise[i_row];
   }
   delete[] l_rowwise;

   for (int i_row=0; i_row < a.n_row; i_row++) {
     delete[] l_colwise[i_row];
   }
   delete[] l_colwise;

   return;
}
// -----------------------------------------------------------------------------
void knuth_append_2a(
   KnuthMat &a,
   KnuthMat &b) {

// append a matrix in the Knuth format:
//         A
// input: ---  , result: A
//         B
//
// This assumes that allocation has already been done.

   int nrow_a_old,nnz_a_old,srow1,i_row_a,i_a,
       prow1,scolb1,pntr,pntr_1;

// temporary arrays:
   int *srow_a_old;
   int *scol_a_old;
   int *ecol_a_old;

   srow_a_old = new int[a.n_row];
   scol_a_old = new int[a.n_col];
   ecol_a_old = new int[a.n_col];

   assign_array_1<int>(srow_a_old,a.n_row,0);
   assign_array_1<int>(scol_a_old,a.n_col,0);
   assign_array_1<int>(ecol_a_old,a.n_col,0);

   if (a.n_col != b.n_col) {
     cout << "knuth_append_2a: a.n_col .ne. b.n_col?" << endl;
     cout << "   a.n_col = " << a.n_col << ", b.n_col = " << b.n_col << endl;
     cout << "   Halting..." << endl;
     exit (1);
   }

   nrow_a_old = a.n_row;
   nnz_a_old = a.n_nz;
   for (int i=0; i < a.n_col; i++) {
     scol_a_old[i] = a.scol[i];
     ecol_a_old[i] = a.ecol[i];
   }
   for (int i=0; i < a.n_row; i++) {
     srow_a_old[i] = a.srow[i];
   }

   a.n_row = a.n_row + b.n_row;

// append val,row,col

   for (int i=0; i < b.n_nz; i++) {
     a.val.push_back(0.0);
     a.row.push_back(0);
     a.col.push_back(0);
     a.prow.push_back(0);
     a.pcol.push_back(0);

     a.val[a.n_nz] = b.val[i];
     a.col[a.n_nz] = b.col[i];
     a.row[a.n_nz] = b.row[i] + nrow_a_old;
     a.n_nz++;
   }

// row pointers:
   for (int i_row_b=0; i_row_b < b.n_row; i_row_b++) {
     srow1 = b.srow[i_row_b];
     i_row_a = nrow_a_old + i_row_b;
     if (srow1 != -1) {
       a.srow[i_row_a] = nnz_a_old + srow1;
       a.erow[i_row_a] = nnz_a_old + b.erow[i_row_b];
     } else {
       a.srow[i_row_a] = -1;
       a.erow[i_row_a] = -1;
     }
   }
   for (int i_b=0; i_b < b.n_nz; i_b++) {
     i_a = i_b + nnz_a_old;
     prow1 = b.prow[i_b];
     if (prow1 != -1) {
       a.prow[i_a] = nnz_a_old + prow1;
     } else {
       a.prow[i_a] = -1;
     }
   }

// col pointers:
   for (int i_col=0; i_col < a.n_col; i_col++) {
     scolb1 = b.scol[i_col];
     if (scol_a_old[i_col] == -1) {
       if (scolb1 != -1) {
         a.scol[i_col] = nnz_a_old + scolb1;
       }
     }
     if (scolb1 != -1) {
       if (scol_a_old[i_col] != -1) {
         pntr = ecol_a_old[i_col];
         a.pcol[pntr] = scolb1 + nnz_a_old;
       }
       pntr = scolb1;
       for (int i_row_b=0; i_row_b <= b.n_row; i_row_b++) {
         pntr_1 = b.pcol[pntr];
         if (pntr_1 != -1) {
           a.pcol[pntr + nnz_a_old] = pntr_1 + nnz_a_old;
           pntr = pntr_1;
         } else {
           a.pcol[pntr + nnz_a_old] = -1;
           a.ecol[i_col] = pntr + nnz_a_old;
           break;
         }
       }
     }
   }

// delete temporary arrays:
   delete[] srow_a_old;
   delete[] scol_a_old;
   delete[] ecol_a_old;

   return;
} // end knuth_append_2a
// -----------------------------------------------------------------------------
void knuth_zero_1(
   KnuthMat &a) {

// allocation has already been made; here we only initialise.
// also, n_row and n_col have already been assigned

   a.n_nz = 0;

   for (int i=0; i < a.n_row; i++) {
     a.srow[i] = -1;
     a.erow[i] = -1;
   }

   for (int i=0; i < a.n_col; i++) {
     a.scol[i] = -1;
     a.ecol[i] = -1;
   }

   a.row.clear();
   a.col.clear();
   a.prow.clear();
   a.pcol.clear();
   a.val.clear();

   return;
}
// -----------------------------------------------------------------------------
void knuth_copy(
   KnuthMat &a,
   KnuthMat &b) {

// Copy a Knuth matrix A to B. A is not modified.
// (allocation is assumed to be already taken care of)

   b.n_row = a.n_row;
   b.n_col = a.n_col;
   b.n_nz  = a.n_nz;

   for (int i=0; i < a.n_nz; i++) {
//    cout << "knuth_copy: i = " << i << endl;
      b.val[i] = a.val[i];
      b.row[i] = a.row[i];
      b.col[i] = a.col[i];
      b.prow[i] = a.prow[i];
      b.pcol[i] = a.pcol[i];
   }
   for (int i=0; i < a.n_row; i++) {
      b.srow[i] = a.srow[i];
      b.erow[i] = a.erow[i];
   }
   for (int i=0; i < a.n_col; i++) {
      b.scol[i] = a.scol[i];
      b.ecol[i] = a.ecol[i];
   }
   return;
}
// -----------------------------------------------------------------------------
void knuth_copy_1(
   KnuthMat &a,
   KnuthMat &b) {

// Copy a Knuth matrix A to B. A is not modified.
// DO NOT copy val.
// (allocation is assumed to be already taken care of)

   b.n_row = a.n_row;
   b.n_col = a.n_col;
   b.n_nz  = a.n_nz;

   for (int i=0; i < a.n_nz; i++) {
      b.row[i] = a.row[i];
      b.col[i] = a.col[i];
      b.prow[i] = a.prow[i];
      b.pcol[i] = a.pcol[i];
   }
   for (int i=0; i < a.n_row; i++) {
      b.srow[i] = a.srow[i];
      b.erow[i] = a.erow[i];
   }
   for (int i=0; i < a.n_col; i++) {
      b.scol[i] = a.scol[i];
      b.ecol[i] = a.ecol[i];
   }
   return;
}
// -----------------------------------------------------------------------------
void knuth_check_ij(
   KnuthMat &a,
   const int i,
   const int j,
   bool &flag_1,
   int &nnz_indx) {

// Check if (i,j) is occupied. If yes, set flag_1 = true.
// (need not be a square matrix)

   int i_row,j1,j_col;

   flag_1 = false;
   nnz_indx = -1;
   i_row = i;

   j1 = a.srow[i_row];

   for (int i1=0; i1 <= a.n_col; i1++) {
     if (j1 == -1) {
       break;
     } else {
       j_col = a.col[j1];
       if (j_col == j) {
         flag_1 = true;
         nnz_indx = j1;
         break;
       }
       j1 = a.prow[j1];
     }
   }
   return;
}
// -----------------------------------------------------------------------------
void invrs(
   const int n,
   double **a,
   int *indxc,
   int *indxr,
   int *ipiv) {

   int icol=0,irow=0;
   double big=0.0,dum=0.0,pivinv=0.0;

   for (int i=0; i < n; i++) {ipiv[i] = -1;}

   for (int i=0; i < n; i++) {
     big = 0.0;
     for (int j=0; j < n; j++) {
       if (ipiv[j] != 0) {
         for (int k=0; k < n; k++) {
           if (ipiv[k] == -1) {
             if (fabs(a[j][k]) >= big) {
               big = fabs(a[j][k]);
               irow = j;
               icol = k;
             }
           } else if (ipiv[k] > 0) {
             cout << "invrs: singular matrix. Halting.." << endl;
             exit (1);
           }
         }
       }
     }
     ipiv[icol] = ipiv[icol] + 1;
     if (irow != icol) {
       for (int l=0; l < n; l++) {
         dum = a[irow][l];
         a[irow][l] = a[icol][l];
         a[icol][l] = dum;
       }
     }
     indxr[i] = irow;
     indxc[i] = icol;
     if (a[icol][icol] == 0.0) {
       cout << "invrs: singular matrix. Halting.." << endl;
       exit (1);
     }
     pivinv = 1.0/a[icol][icol];
     a[icol][icol] = 1.0;
     for (int l=0; l < n; l++) {
       a[icol][l] = a[icol][l]*pivinv;
     }
     for (int l1=0; l1 < n; l1++) {
       if (l1 != icol) {
         dum = a[l1][icol];
         a[l1][icol] = 0.0;
         for (int l=0; l < n; l++) {
           a[l1][l] = a[l1][l]-a[icol][l]*dum;
         }
       }
     }
   }

   for (int l=n-1; l >= 0; l--) {
     if (indxr[l] != indxc[l]) {
       for (int k=0; k < n; k++) {
         dum = a[k][indxr[l]];
         a[k][indxr[l]] = a[k][indxc[l]];
         a[k][indxc[l]] = dum;
       }
     }
   }

   return;
}
// -----------------------------------------------------------------------------
void mat_mult_1(
   const int n,
   double **a,
   double * b,
   double * c) {

// compute product Ax where A is n x n, and b is n x 1.

   double s1=0.0;

   for (int i=0; i < n; i++) {
     s1 = 0.0;
     for (int k=0; k < n; k++) {
       s1 = s1 + a[i][k]*b[k];
     }
     c[i] = s1;
   }

   return;
}
// -----------------------------------------------------------------------------
void mat_solve(
   const int n,
   double **a,
   double **ainvrs,
   double * b,
   double * x,
   int *indxc,
   int *indxr,
   int *ipiv) {

// call invrs and mat_mult to give directly
// the solution of Ax=b without writing over A or b.

// copy matrix to ainvrs (so that the original matrix remains
// unaltered)

   for (int i=0; i < n; i++) {
     for (int j=0; j < n; j++) {
       ainvrs[i][j] = a[i][j];
     }
   }

   invrs(n,ainvrs,indxc,indxr,ipiv);
   mat_mult_1(n,ainvrs,b,x);

   return;
}
// -----------------------------------------------------------------------------
void nnz_active_k(
   const int k_given,
   int *nnz_row,
   int *nnz_col,
   KnuthMat &a) {

   int i_col,pntr;

   for (int i=k_given; i < a.n_row; i++) {
     nnz_row[i] = 0;
     nnz_col[i] = 0;
   }
   for (int i_row=k_given; i_row < a.n_row; i_row++) {
     pntr = a.srow[i_row];
     for (int i=0; i < (a.n_col+1); i++) {
        if (pntr !=  -1) {
          i_col = a.col[pntr];
          if (i_col >= k_given) {
            nnz_row[i_row]++;
            nnz_col[i_col]++;
          }
          pntr = a.prow[pntr];
        } else {
          break;
        }
     }
   }
   return;
}
// -----------------------------------------------------------------------------
void markow_1_k(
   const int k_given,
   int *nnz_row,
   int *nnz_col,
   int & pntr_out,
   int & row_out,
   int & col_out,
   KnuthMat &a) {

   int prdct,prdct_min;
   int i_col,pntr,pntr_min;

   prdct_min = 5000;

   for (int i_row = k_given; i_row < a.n_row; i_row++) {
     pntr = a.srow[i_row];
     for (int i=0; i < (a.n_col+1); i++) {
       if (pntr !=  -1) {
         i_col = a.col[pntr];

         if (i_col >= k_given) {
           prdct = (nnz_row[i_row]-1)*(nnz_col[i_col]-1);
           if (prdct < prdct_min) {
             prdct_min = prdct;
             pntr_min = pntr;
             row_out = i_row;
             col_out = i_col;
           }
         }
         pntr = a.prow[pntr];
       } else {
         break;
       }
     }
   }
   pntr_out = pntr_min;
   return;
}
// -----------------------------------------------------------------------------
void row_exch_k(
   const int row_1,
   const int row_2,
   int *row_order,
   KnuthMat &a,
   double *rhs) {

   int idum;
   double dum;

   idum = row_order[row_1];
   row_order[row_1] = row_order[row_2];
   row_order[row_2] = idum;

   idum = a.srow[row_1];
   a.srow[row_1] = a.srow[row_2];
   a.srow[row_2] = idum;

   idum = a.erow[row_1];
   a.erow[row_1] = a.erow[row_2];
   a.erow[row_2] = idum;

   dum = rhs[row_1];
   rhs[row_1] = rhs[row_2];
   rhs[row_2] = dum;

   for (int i_nz=0; i_nz < a.n_nz; i_nz++) {
     if (a.row[i_nz] == row_1) {
       a.row[i_nz] = row_2;
       continue;
     }
     if (a.row[i_nz] == row_2) {
       a.row[i_nz] = row_1;
       continue;
     }
   }
   knuth_makecols(a);

   return;
}
// -----------------------------------------------------------------------------
void knuth_makecols(
   KnuthMat &a) {

   int *bcol;
   int pntr=0,col_indx=0,k=0;

   bcol = new int[a.n_col];

   for (int i=0; i < a.n_col; i++) {
     a.scol[i] = -1; bcol[i] = -1; a.ecol[i] = -1;
   }
   for (int i_row=0; i_row < a.n_row; i_row++) {
     pntr = a.srow[i_row];
     for (int i1=0; i1 < (a.n_col+1); i1++) {
       if (pntr != -1) {
         col_indx = a.col[pntr];
         if (a.scol[col_indx] == -1) {
           a.scol[col_indx] = pntr;
           bcol[col_indx] = pntr;
         } else {
           k = bcol[col_indx];
           a.pcol[k] = pntr;
           bcol[col_indx] = pntr;
         }
         pntr = a.prow[pntr];
       } else {
         break;
       }
     }
   }
   for (int i_col=0; i_col < a.n_col; i_col++) {
     pntr = bcol[i_col];
     if (pntr != -1) {
       a.ecol[i_col] = pntr;
       a.pcol[pntr] = -1;
     }
   }
   delete[] bcol;
   return;
}
// -----------------------------------------------------------------------------
void col_exch_k(
   const int col_1,
   const int col_2,
   int *col_order,
   KnuthMat &a) {

   int idum;
   idum = col_order[col_1];
   col_order[col_1] = col_order[col_2];
   col_order[col_2] = idum;

   idum = a.scol[col_1];
   a.scol[col_1] = a.scol[col_2];
   a.scol[col_2] = idum;

   idum = a.ecol[col_1];
   a.ecol[col_1] = a.ecol[col_2];
   a.ecol[col_2] = idum;

   for (int i_nz=0; i_nz < a.n_nz; i_nz++) {
     if (a.col[i_nz] == col_1) {
       a.col[i_nz] = col_2;
       continue;
     }
     if (a.col[i_nz] == col_2) {
       a.col[i_nz] = col_1;
       continue;
     }
   }
   knuth_makerows(a);

   return;
}
// -----------------------------------------------------------------------------
void knuth_makerows(
   KnuthMat &a) {

   int *brow;
   int pntr,row_indx,k;

   brow = new int[a.n_row];

   for (int i=0; i < a.n_row; i++) {
     a.srow[i] = -1; brow[i] = -1; a.erow[i] = -1;
   }

   for (int i_col=0; i_col < a.n_col; i_col++) {
     pntr = a.scol[i_col];
     for (int i1=0; i1 < (a.n_row+1); i1++) {
       if (pntr != -1) {
         row_indx = a.row[pntr];
         if (a.srow[row_indx] == -1) {
           a.srow[row_indx] = pntr;
           brow[row_indx] = pntr;
         } else {
           k = brow[row_indx];
           a.prow[k] = pntr;
           brow[row_indx] = pntr;
         }
         pntr = a.pcol[pntr];
       } else {
         break;
       }
     }
   }
   for (int i_row=0; i_row < a.n_row; i_row++) {
     pntr = brow[i_row];
     if (pntr != -1) {
       a.erow[i_row] = pntr;
       a.prow[pntr] = -1;
     }
   }
   delete[] brow;
   return;
}
// -----------------------------------------------------------------------------
void nnz_active_k_1(
   const int k_given,
   int *nnz_row,
   int *nnz_col,
   double *valmax_row,
   double *valmax_col,
   KnuthMat &a) {

// find the number of nonzero's in the active submatrix in each row
// and each column.
// the submatrix (square) is k_given.le.row.le.nrow
// Also, record the entry with max abs value in each row and col.

   double epsln=1.0e-40;
   double absval;
   int i_col,pntr;

   for (int i=k_given; i < a.n_row; i++) {
     nnz_row[i] = 0;
     nnz_col[i] = 0;
     valmax_row[i] = 0.0;
     valmax_col[i] = 0.0;
   }
   for (int i_row=k_given; i_row < a.n_row; i_row++) {
     pntr = a.srow[i_row];
     for (int i=0; i < (a.n_col+1); i++) {
        if (pntr !=  -1) {
          i_col = a.col[pntr];
          if (i_col >= k_given) {
//          cout << "nnz_active_k_1: i_row=" << i_row
//             << ", pntr=" << pntr << ", val=" << a.val[pntr] << endl;
            absval = fabs(a.val[pntr]);
            if (absval > epsln) {
              nnz_row[i_row]++;
              nnz_col[i_col]++;

              if (absval > valmax_row[i_row]) {
                valmax_row[i_row] = absval;
              }
              if (absval > valmax_col[i_col]) {
                valmax_col[i_col] = absval;
              }
            }
          }
          pntr = a.prow[pntr];
        } else {
          break;
        }
     }
   }
   for (int i=k_given;i < a.n_row;i++) {
     if (valmax_row[i] == 0.0) {
       cout << "nnz_active_k_1: valmax_row=0. i_row=" << i << endl;
       cout << "  Singular matrix? Halting..." << endl;
       exit (1);
     }
     if (valmax_col[i] == 0.0) {
       cout << "nnz_active_k_1: valmax_col=0. i_col=" << i << endl;
       cout << "  Singular matrix? Halting..." << endl;
       exit (1);
     }
   }
   return;
}
// -----------------------------------------------------------------------------
void markow_2_k0(
   const int k_given,
   int *nnz_row,
   int *nnz_col,
   int &pntr_out,
   int &row_out,
   int &col_out,
   vector<bool> l_mark,
   double *valmax_row,
   double *valmax_col,
   const double u_threshold,
   KnuthMat &a) {

// Pick the best nonzero in the active submatrix according to
// nr(i)*nc(j). (if two nonzero's are equal in the above respect,
// the first will be chosen). Check if it satisfies the threshold
// stability critrion; if not, pick another one and so on.
// u_threshold: defined by Eq.3.74 (p 88) of Pissanetzky

// Note that l_mark is actually a local array. It could be allocated
// and deleted within this routine. However, we will allocate it only
// once (outside this routine) and not delete it. This will be more
// efficient if markow_2_k has to be called more than once.

   int prdct,prdct_min;
   int i_col,pntr;
   double valmax_row_1,valmax_col_1,absval;

   if (a.n_nz > (int)l_mark.size()) {
     l_mark.resize(a.n_nz);
   }

// cout << "markow_2_k: a.n_nz = " << a.n_nz << endl;

   for (int i_row=k_given; i_row < a.n_row; i_row++) {
     pntr = a.srow[i_row];
     for (int i=0; i < (a.n_col+1); i++) {
       if (pntr !=  -1) {
//       cout << "markow_2_k: pntr (1) = " << pntr << endl;
         l_mark[pntr] = true;
         pntr = a.prow[pntr];
       } else {
         break;
       }
     }
   }
   for (int i_outer=0; i_outer < a.n_nz; i_outer++) {
     prdct_min = 5000;
     for (int i_row=k_given; i_row < a.n_row; i_row++) {
       pntr = a.srow[i_row];
       for (int i=0; i < (a.n_col+1); i++) {
         if (pntr !=  -1) {
//         cout << "markow_2_k: pntr (2) = " << pntr << endl;
           if (l_mark[pntr]) {
             i_col = a.col[pntr];

             if (i_col >= k_given) {
               prdct = (nnz_row[i_row]-1)*(nnz_col[i_col]-1);
               if (prdct < prdct_min) {
                 prdct_min = prdct;
                 pntr_out = pntr;
                 row_out = i_row;
                 col_out = i_col;
               }
             }
           }
           pntr = a.prow[pntr];
         } else {
           break;
         }
       }
     }
     valmax_row_1 = u_threshold*valmax_row[row_out];
     valmax_col_1 = u_threshold*valmax_col[col_out];

     absval = fabs(a.val[pntr_out]);
     if ((absval >= valmax_row_1) || (absval >= valmax_col_1)) {
       break;
     } else {
//     cout << "markow_2_k: pntr (3) = " << pntr << endl;
       l_mark[pntr_out] = false;
     }
   }
   return;
}
// -----------------------------------------------------------------------------
void invrs_c(
   const int n,
   std::complex<double>** a,
   int* indxc,
   int* indxr,
   int* ipiv) {

   int icol=0,irow=0;
   double big=0.0;
   std::complex<double> pivinv,dum;
   std::complex<double> zero_c,one_c;

   zero_c = std::complex<double>(0.0,0.0);
   one_c  = std::complex<double>(1.0,0.0);

   for (int i=0; i < n; i++) {ipiv[i] = -1;}

   for (int i=0; i < n; i++) {
     big = 0.0;
     for (int j=0; j < n; j++) {
       if (ipiv[j] != 0) {
         for (int k=0; k < n; k++) {
           if (ipiv[k] == -1) {
             if (abs(a[j][k]) >= big) {
               big = abs(a[j][k]);
               irow = j;
               icol = k;
             }
           } else if (ipiv[k] > 0) {
             cout << "invrs_c: singular matrix. Halting.." << endl;
             exit (1);
           }
         }
       }
     }
     ipiv[icol] = ipiv[icol] + 1;
     if (irow != icol) {
       for (int l=0; l < n; l++) {
         dum = a[irow][l];
         a[irow][l] = a[icol][l];
         a[icol][l] = dum;
       }
     }
     indxr[i] = irow;
     indxc[i] = icol;
     if (abs(a[icol][icol]) == 0.0) {
       cout << "invrs_c: singular matrix. Halting.." << endl;
       exit (1);
     }
     pivinv = one_c/a[icol][icol];
     a[icol][icol] = one_c;
     for (int l=0; l < n; l++) {
       a[icol][l] = a[icol][l]*pivinv;
     }
     for (int l1=0; l1 < n; l1++) {
       if (l1 != icol) {
         dum = a[l1][icol];
         a[l1][icol] = zero_c;
         for (int l=0; l < n; l++) {
           a[l1][l] = a[l1][l]-a[icol][l]*dum;
         }
       }
     }
   }

   for (int l=n-1; l >= 0; l--) {
     if (indxr[l] != indxc[l]) {
       for (int k=0; k < n; k++) {
         dum = a[k][indxr[l]];
         a[k][indxr[l]] = a[k][indxc[l]];
         a[k][indxc[l]] = dum;
       }
     }
   }

   return;
}
// -----------------------------------------------------------------------------
void mat_mult_1_c(
   const int n,
   std::complex<double>** a,
   std::complex<double>*  b,
   std::complex<double>*  c) {

// compute product Ax where A is n x n, and b is n x 1.

   std::complex<double> s1;

   for (int i=0; i < n; i++) {
     s1 = std::complex<double>(0.0,0.0);
     for (int k=0; k < n; k++) {
       s1 = s1 + a[i][k]*b[k];
     }
     c[i] = s1;
   }

   return;
}
// -----------------------------------------------------------------------------
void mat_solve_c(
   const int n,
   std::complex<double>** a,
   std::complex<double>** ainvrs,
   std::complex<double>*  b,
   std::complex<double>*  x,
   int* indxc,
   int* indxr,
   int* ipiv) {

// copy matrix to ainvrs (so that the original matrix remains
// unaltered)

   for (int i=0; i < n; i++) {
     for (int j=0; j < n; j++) {
       ainvrs[i][j] = a[i][j];
     }
   }

   invrs_c(n,ainvrs,indxc,indxr,ipiv);
   mat_mult_1_c(n,ainvrs,b,x);

   return;
}
