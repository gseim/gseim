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

#include "xbejac.h"

XbeJac::XbeJac(){}

void XbeJac::initialise(
   const vector<XbeLib> &xbe_lib,
   const int i_xbel) {

   n_vr   = xbe_lib[i_xbel].n_vr;
   n_aux  = xbe_lib[i_xbel].n_aux;

   n_f = xbe_lib[i_xbel].n_f;
   n_g = xbe_lib[i_xbel].n_g;
   n_h = xbe_lib[i_xbel].n_h;

   dgdvr .resize(n_g);
   dgdaux.resize(n_g);

   for (int i=0; i < n_g; i++) {
     dgdvr[i] .resize(xbe_lib[i_xbel].n_vr);
     dgdaux[i].resize(xbe_lib[i_xbel].n_aux);
   }

   assign_const_2d_1(0.0,dgdvr);
   assign_const_2d_1(0.0,dgdaux);

   return;
} //end of XbeJac::initialise
