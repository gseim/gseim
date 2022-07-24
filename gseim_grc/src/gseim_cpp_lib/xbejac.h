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

#ifndef XBE_JAC_H
#define XBE_JAC_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>

#include "xbelib.h"

using namespace std;

class XbeJac {

  public:

   vector< vector<double> > dgdvr;
   vector< vector<double> > dgdaux;

   int n_vr,n_aux;
   int n_f,n_g,n_h;

  public:
   XbeJac();

   void initialise(
    const vector<XbeLib> &xbe_lib,
    const int i_xbel);

};
#endif
