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

#ifndef MATOP_H
#define MATOP_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <iomanip>
#include <vector>
#include "utils.h"

using namespace std;

class MatOp {

  public:

   int n_matttlop1;
   int n_matttlop2;
   int n_matttlop3;

   int *ip_matop1;
   int *ip_matop3;
   int *ip_matnp;
   int *row_ordr_w;
   int *col_ordr_w;
   int *row_ordr_inv_w;
   int *col_ordr_inv_w;
   int *mat_rowexch;
   int *mat_np;
   int *mat_pp;
   int **ip1_matop2;

   bool *l_np_zero;

   vector<int> mat_nn;
   vector<int> mat_op_1;
   vector<int> mat_op_2;
   vector<int> mat_op_3;
   vector<int> mat_pn_1;
   vector<int> mat_pn_2;
   vector<int> mat_pn_3;
   vector<int> mat_np_2;
   vector<int> mat_pn_indx;

   bool *l_occ_1;
   bool *l_occ_2;
   bool *l_occ_3;
   bool *l_new;

   int *inv_col_1;
   int *inv_col_3;
   int *nnz_row;
   int *nnz_col;
   int *optype;
   int *opindex;

   double *valmax_row;
   double *valmax_col;

   bool flag_alloc,flag_delete;

  public:
   MatOp();

   void allocate_1(
    const int n_row);

   void delete_1(
    const int n_row);

};

#endif
