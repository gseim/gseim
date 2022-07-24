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

#include "knuthmat.h"

KnuthMat::KnuthMat() {
   flag_alloc = true;
   flag_delete = false;
}
// -----------------------------------------------------------------------------
void KnuthMat::allocate_1(
   const int n_nz0,
   const int n_row0,
   const int n_col0) {

   n_nz  = n_nz0;
   n_row = n_row0;
   n_col = n_col0;

   row.resize(n_nz); 
   col.resize(n_nz); 
   prow.resize(n_nz);
   pcol.resize(n_nz);
   val.resize(n_nz); 

   if (flag_alloc) {
     srow = new int[n_row];
     scol = new int[n_col];
     erow = new int[n_row];
     ecol = new int[n_col];

     flag_alloc = false;
     flag_delete = true;
   } else {
     cout << "KnuthMat::allocate_1: trying to allocate with" << endl;
     cout << "  flag_alloc = false? Halting..." << endl; exit(1);
   }
   return;
}
// -----------------------------------------------------------------------------
void KnuthMat::delete_1() {
   row.clear();
   col.clear();
   prow.clear();
   pcol.clear();
   val.clear();

   if (flag_delete) {
     delete[] srow;
     delete[] scol;
     delete[] erow;
     delete[] ecol;

     flag_alloc = true;
     flag_delete = false;
   } else {
     cout << "KnuthMat::allocate_1: trying to delete with" << endl;
     cout << "  flag_delete = false? Halting..." << endl; exit(1);
   }
   return;
}
