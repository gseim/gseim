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

#include "ijmat.h"

IJMat::IJMat() {
   val = NULL;
}

void IJMat::allocate_1(
   const int n_row0,
   const int n_col0) {

   n_row = n_row0;
   n_col = n_col0;

   val = new double*[n_row]; 
   for (int i=0; i < n_row; i++) {
     val[i] = new double[n_col];
   }

   return;
}

void IJMat::delete_1() {
   if (val != NULL) {
     for (int i=0; i < n_row; i++) {
       delete[] val[i];
     }
     delete[] val;
   }
   return;
}
