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

#ifndef GET_YYY_H
#define GET_YYY_H

using namespace std;

#include "global.h"
#include "xbeusr.h"

void get_xbe(
   Global &global,
   XbeUsr &X,
   XbeJac &J);

void get_ebe(
   Global &global,
   EbeUsr &X,
   EbeJac &J);

#endif
