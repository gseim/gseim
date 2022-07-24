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

#include "cctfile.h"

CctFile::CctFile(){}

CctFile::CctFile(const std::string &filename) {

// filename: name of circuit file

   std::fstream inf;
   bool flag_found_end_cf,flag_found_begin,flag_found_end;
   vector<std::string> v1,v2; 
   std::string s1;
   int n_words;
   int n_lines;

   inf.open(filename,ios::in|ios::binary);
   if (!inf.is_open()) {
     cout << "CctFile: " << filename << " could not be opened. Halting..." << endl;
     exit(1);
   }

// Need to look only between begin_circuit and end_circuit.
// Mark these first.

   flag_found_begin  = false;
   flag_found_end    = false;
   flag_found_end_cf = false;
   n_solve = 0;

   n_lines = count_lines_1(filename);

   for (int i_line=0; i_line < n_lines; i_line++) {
     if (flag_found_end_cf) {
       break;
     }
     next_line(inf,v1);
     if (v1[0] == "begin_circuit") {
       flag_found_begin = true;
       line_begin_circuit = i_line;
     }
     if (v1[0] == "end_circuit") {
       flag_found_end = true;
       line_end_circuit = i_line;
     }
     if (v1[0] == "begin_solve") {
       line_begin_solve.push_back(i_line);
     }
     if (v1[0] == "end_solve") {
       line_end_solve.push_back(i_line);
       n_solve++;
     }
     if (v1[0] == "end_cf") {
       flag_found_end_cf = true;
       line_end_cf = i_line;
     }
   }
   inf.close();

   if ((!flag_found_begin) || (!flag_found_end)) {
     cout << "CctFile: begin_circuit/end_circuit not found." << endl;
     cout << "  in file: " << filename << ". Halting..." << endl;
     exit(1);
   }
   if (!flag_found_end_cf) {
     cout << "CctFile: end_cf not found." << endl;
     cout << "  in file: " << filename << ". Halting..." << endl;
     exit(1);
   }

   tick_cf.resize(line_end_cf);

   inf.open(filename,ios::in|ios::binary);
   for (int i_line=0; i_line < line_end_circuit; i_line++) {
     next_line(inf,v1);
     if (i_line > line_begin_circuit) {
       if (v1[0] == "outvar:") {
         assign_vec_string_1(v1,1,2,ov_name);
         assign_vec_string_1(v1,2,2,v2);
         split_vec_string_1(v2,"_of_",ovl_name,ovr_name);
         n_ov = ov_name.size();
       }
     }
   }
   inf.close();

   ov1.resize(n_ov);
   ov2.resize(n_ov);
   ov_flag.resize(n_ov);

   assign_const_1<int>(ov1,-1);
   assign_const_1<int>(ov2,-1);
   assign_const_1<int>(ov_flag,-1);

   return;
} //end of CctFile::CctFile
