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

#ifndef MATRIX_2_H
#define MATRIX_2_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <iomanip>

#include "utils.h"
#include "knuthmat.h"
#include "ijmat.h"

using namespace std;

void write_vector_double_2(
   const int n,
   double *b,
   char *fname,
   char *message);
void negative_double_1(
   const int n,
   double *b);
void write_mat_knuth_3(
   KnuthMat &a_knuth,
   char *fname);
void knuth_addentry(
   KnuthMat &a,
   const int row_given,
   const int col_given,
   const double val_given);
void check_knuth(
   KnuthMat &a,
   char *string_message);
void knuth_append_2a(
   KnuthMat &a,
   KnuthMat &b);
void knuth_zero_1(
   KnuthMat &a);
void knuth_copy(
   KnuthMat &a,
   KnuthMat &b);
void knuth_copy_1(
   KnuthMat &a,
   KnuthMat &b);
void knuth_check_ij(
   KnuthMat &a,
   const int i,
   const int j,
   bool &flag_1,
   int &nnz_indx);
void invrs(
   const int n,
   double **a,
   int *indxc,
   int *indxr,
   int *ipiv);
void mat_mult_1(
   const int n,
   double **a,
   double * b,
   double * c);
void mat_solve(
   const int n,
   double **a,
   double **ainvrs,
   double * b,
   double * x,
   int *indxc,
   int *indxr,
   int *ipiv);
void nnz_active_k(
   const int k_given,
   int *nnz_row,
   int *nnz_col,
   KnuthMat &a);
void markow_1_k(
   const int k_given,
   int *nnz_row,
   int *nnz_col,
   int & pntr_out,
   int & row_out,
   int & col_out,
   KnuthMat &a);
void row_exch_k(
   const int row_1,
   const int row_2,
   int *row_order,
   KnuthMat &a,
   double *rhs);
void knuth_makecols(
   KnuthMat &a);
void col_exch_k(
   const int col_1,
   const int col_2,
   int *col_order,
   KnuthMat &a);
void knuth_makerows(
   KnuthMat &a);
void nnz_active_k_1(
   const int k_given,
   int *nnz_row,
   int *nnz_col,
   double *valmax_row,
   double *valmax_col,
   KnuthMat &a);
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
   KnuthMat &a);
void invrs_c(
   const int n,
   std::complex<double>** a,
   int* indxc,
   int* indxr,
   int* ipiv);
void mat_mult_1_c(
   const int n,
   std::complex<double>** a,
   std::complex<double>*  b,
   std::complex<double>*  c);
void mat_solve_c(
   const int n,
   std::complex<double>** a,
   std::complex<double>** ainvrs,
   std::complex<double>*  b,
   std::complex<double>*  x,
   int* indxc,
   int* indxr,
   int* ipiv);

#endif
