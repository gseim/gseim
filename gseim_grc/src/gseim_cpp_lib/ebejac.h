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

#ifndef EBE_JAC_H
#define EBE_JAC_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>

#include "ebelib.h"

using namespace std;

class EbeJac {

  public:

   vector< vector<double> > dfdv;
   vector< vector<double> > dfdaux;
   vector< vector<double> > dfdxvr;

   vector< vector<double> > dgdv;
   vector< vector<double> > dgdaux;
   vector< vector<double> > dgdxvr;

   vector< vector<double> > dhdv;
   vector< vector<double> > dhdauxs;
   vector< vector<double> > dhdxvr;

   int n_nd,n_aux,n_auxs,n_xvr;
   int n_f,n_g,n_h;

  public:
   EbeJac();

   void initialise(
    const vector<EbeLib> &ebe_lib,
    const int i_ebel);

};
#endif
