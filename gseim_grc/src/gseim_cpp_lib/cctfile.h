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

#ifndef CCT_FILE_H
#define CCT_FILE_H

#include "utils.h"

using namespace std;

class CctFile {

  public:
   int line_end_cf;
   int line_begin_circuit;
   int line_end_circuit;

   int n_solve;
   vector<int> line_begin_solve;
   vector<int> line_end_solve;

   int n_ov;
   int n_ov_xbe;
   int n_ov_xvr;
   int n_ov_ebe;
   int n_ov_nodev;

   vector<std::string> ov_name;
   vector<std::string> ovl_name;
   vector<std::string> ovr_name;

   vector<bool> tick_cf;

   vector<int> ov1,ov2,ov_flag;

  public:
   CctFile();

   CctFile(const std::string &filename);

   void write_outvar(const std::string &filename);

};
#endif
