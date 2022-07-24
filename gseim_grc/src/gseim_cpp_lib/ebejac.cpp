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

#include "ebejac.h"

EbeJac::EbeJac(){}

void EbeJac::initialise(
   const vector<EbeLib> &ebe_lib,
   const int i_ebel) {

   n_nd   = ebe_lib[i_ebel].n_nd;
   n_aux  = ebe_lib[i_ebel].n_aux;
   n_auxs = ebe_lib[i_ebel].n_auxs;
   n_xvr  = ebe_lib[i_ebel].n_xvr;

   n_f = ebe_lib[i_ebel].n_f;
   n_g = ebe_lib[i_ebel].n_g;
   n_h = ebe_lib[i_ebel].n_h;

   dfdv  .resize(n_f);
   dfdaux.resize(n_f);
   dfdxvr.resize(n_f);

   for (int i=0; i < n_f; i++) {
     dfdv[i]  .resize(ebe_lib[i_ebel].n_nd);
     dfdaux[i].resize(ebe_lib[i_ebel].n_aux);
     dfdxvr[i].resize(ebe_lib[i_ebel].n_xvr);
   }
 
   assign_const_2d_1(0.0,dfdv);
   assign_const_2d_1(0.0,dfdaux);
   assign_const_2d_1(0.0,dfdxvr);

   dgdv  .resize(n_g);
   dgdaux.resize(n_g);
   dgdxvr.resize(n_g);

   for (int i=0; i < n_g; i++) {
     dgdv[i]  .resize(ebe_lib[i_ebel].n_nd);
     dgdaux[i].resize(ebe_lib[i_ebel].n_aux);
     dgdxvr[i].resize(ebe_lib[i_ebel].n_xvr);
   }

   assign_const_2d_1(0.0,dgdv);
   assign_const_2d_1(0.0,dgdaux);
   assign_const_2d_1(0.0,dgdxvr);

   dhdv   .resize(n_h);
   dhdauxs.resize(n_h);
   dhdxvr .resize(n_h);

   for (int i=0; i < n_h; i++) {
     dhdv[i]   .resize(ebe_lib[i_ebel].n_nd);
     dhdauxs[i].resize(ebe_lib[i_ebel].n_auxs);
     dhdxvr[i] .resize(ebe_lib[i_ebel].n_xvr);
   }

   assign_const_2d_1(0.0,dhdv);
   assign_const_2d_1(0.0,dhdauxs);
   assign_const_2d_1(0.0,dhdxvr);

   return;
} //end of EbeJac::initialise
