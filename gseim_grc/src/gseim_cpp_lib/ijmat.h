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

#ifndef IJMAT_H
#define IJMAT_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <iomanip>

using namespace std;

class IJMat {

  public:

   int n_row;
   int n_col;

   double **val;

  public:
   IJMat();

   void allocate_1(
    const int n_row0,
    const int n_col0);

   void delete_1();

};

#endif
