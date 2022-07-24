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

#ifndef MATGNRL1_H
#define MATGNRL1_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <iomanip>

#include "global.h"
#include "matrix_2.h"
#include "matop.h"

using namespace std;

void gauss_1(
   const bool flag_debug,
   KnuthMat &m,
   KnuthMat &w,
   MatOp &mo);

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
   double* svec_orig_w);

void gauss_1a(
   const int flag_debug,
   const double gauss_epsln,
   KnuthMat &m,
   KnuthMat &w,
   MatOp &mo);

#endif
