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

#include "utils.h"

// -----------------------------------------------------------------------------
void replace_first(
   std::string& text,
   const std::string& from,
   const std::string& to) {

   const auto at = text.find(from, 0);

   if (at != std::string::npos) text.replace(at, from.length(), to);
}
// -----------------------------------------------------------------------------
int compare_double_eq_1(
   const double a1,
   const double a2,
   const double eps) {

   int a=0;

   if (fabs(a1-a2) <= eps) {
     a = int_T;
   } else {
     a = int_F;
   }
   return a;
}
// -----------------------------------------------------------------------------
void assign_all_int_1(
   int* array1,
   const int n,
   const int k) {

   for (int i = 0; i < n; i++) {
     array1[i] = k;
   }

   return;
}
// -----------------------------------------------------------------------------
void assign_all_double_1(
   double* array1,
   const int n,
   const double k) {

   for (int i = 0; i < n; i++) {
     array1[i] = k;
   }

   return;
}
// -----------------------------------------------------------------------------
void assign_all_complex_1(
   std::complex<double>* array1,
   const int n,
   const std::complex<double> k) {

   for (int i = 0; i < n; i++) {
     array1[i] = k;
   }

   return;
}
// -----------------------------------------------------------------------------
int isSubstring(const string &s1,const string &s2) { 
// Returns position if s1 is substring of s2 
// (taken from https://www.geeksforgeeks.org/check-string-substring-another/)

   int M = s1.length(); 
   int N = s2.length(); 
  
   for (int i=0; i <= N - M; i++) { 
     int j;  
/*   For current index i, check for pattern match */
     for (j=0; j < M; j++) 
       if (s2[i + j] != s1[j]) break; 
  
     if (j == M) return i;  
   }   
  
   return -1; 
} 
// -----------------------------------------------------------------------------
vector<string> tokenize_string(
   const string &s,
   const string &del) {

// taken from
// https://stackoverflow.com/questions/7621727/split-a-string-into-words-by-multiple-delimiters
   const int dictSize = 256;    
   static bool dict[dictSize] = { false};

   vector<string> res;
   for (unsigned int i=0; i < del.size(); ++i) {
     dict[del[i]] = true;
   }

   string token("");
   for (auto &i : s) {
     if (dict[i]) {
       if (!token.empty()) {
         res.push_back(token);
         token.clear();
       }
     } else {
       token += i;
     }
   }
   if (!token.empty()) {
     res.push_back(token);
   }
   return res;
}
// -----------------------------------------------------------------------------
void next_line(
   std::fstream &file,
   const string &del,
   vector<string> &v1,
   bool &flag_eof) {

   std::string str;
   int N=500;
   vector<string> temp;
   int temp_size,location;

   flag_eof = false;
   v1.clear();

   for (int i=0; i < N; i++) {
     if (file.eof()) {
       flag_eof = true;
       return;
     } else {
       location = file.tellg();
       std::getline(file, str);
       temp = tokenize_string(str, del);
       temp_size = temp.size();
       if (temp_size == 0) {
         continue;
       } else if (temp[0][0] == '#') {
         continue;
       }
       if (v1.size() == 0) {
         for (int k=0; k < temp_size; k++) {
           v1.push_back(temp[k]);
         }
       } else {
         if (temp[0] == "+") {
           for (int k=1; k < temp_size; k++) {
             v1.push_back(temp[k]);
           }
         } else {
           file.seekp(location);
           return;
         }
       }
     }
   }
}
// -----------------------------------------------------------------------------
void next_line(
   std::fstream &file,
   const string &del,
   vector<string> &v1) {

   std::string str;
   int N=500;
   vector<string> temp;
   int temp_size,location;

   v1.clear();

   for (int i=0; i < N; i++) {
     if (file.eof()) {
       return;
     } else {
       location = file.tellg();
       std::getline(file, str);
       temp = tokenize_string(str, del);
       temp_size = temp.size();
       if (temp_size == 0) {
         continue;
       } else if (temp[0][0] == '#') {
         continue;
       }
       if (v1.size() == 0) {
         for (int k=0; k < temp_size; k++) {
           v1.push_back(temp[k]);
         }
       } else {
         if (temp[0] == "+") {
           for (int k=1; k < temp_size; k++) {
             v1.push_back(temp[k]);
           }
         } else {
           file.seekp(location);
           return;
         }
       }
     }
   }
}
// -----------------------------------------------------------------------------
void next_line(
   std::fstream &file,
   vector<string> &v1,
   bool &flag_eof) {

// next_line with a fixed delimiter set

   std::string str;
   int N=500;
   vector<string> temp;
   int temp_size,location;

   flag_eof = false;
   v1.clear();

   for (int i=0; i < N; i++) {
     if (file.eof()) {
       flag_eof = true;
       return;
     } else {
       location = file.tellg();
       std::getline(file, str);
       temp = tokenize_string(str, " =");
       temp_size = temp.size();
       if (temp_size == 0) {
         continue;
       } else if (temp[0][0] == '#') {
         continue;
       }
       if (v1.size() == 0) {
         for (int k=0; k < temp_size; k++) {
           v1.push_back(temp[k]);
         }
       } else {
         if (temp[0] == "+") {
           for (int k=1; k < temp_size; k++) {
             v1.push_back(temp[k]);
           }
         } else {
           file.seekp(location);
           return;
         }
       }
     }
   }
}
// -----------------------------------------------------------------------------
void next_line(
   std::fstream &file,
   vector<string> &v1) {

// next_line with a fixed delimiter set and halt if eof.

   std::string str;
   int N=500;
   vector<string> temp;
   int temp_size,location;

   v1.clear();

   for (int i=0; i < N; i++) {
     if (file.eof()) {
       return;
     } else {
       location = file.tellg();
       std::getline(file, str);
       temp = tokenize_string(str, " =");
       temp_size = temp.size();
       if (temp_size == 0) {
         continue;
       } else if (temp[0][0] == '#') {
         continue;
       }
       if (v1.size() == 0) {
         for (int k=0; k < temp_size; k++) {
           v1.push_back(temp[k]);
         }
       } else {
         if (temp[0] == "+") {
           for (int k=1; k < temp_size; k++) {
             v1.push_back(temp[k]);
           }
         } else {
           file.seekp(location);
           return;
         }
       }
     }
   }
}
// -----------------------------------------------------------------------------
double next_double_1(
   std::fstream &file) {

   vector<string> v1;
   double x;

   next_line(file,v1);
   x = stod(v1[0]);
   return x;
}
// -----------------------------------------------------------------------------
std::string next_string_1(
   std::fstream &file) {

   vector<string> v1;

   next_line(file,v1);
   return v1[0];
}
// -----------------------------------------------------------------------------
int count_lines_1(
   const std::string &filename) {

   std::fstream inf;
   bool flag_eof;
   vector<std::string> v1;
   int n_lines=0;

   inf.open(filename,ios::in|ios::binary);
   flag_eof = false;
   while (!flag_eof) {
     next_line(inf,v1,flag_eof);
     n_lines++;
   }
   inf.close();
   return n_lines;
}
// -----------------------------------------------------------------------------
int stoi_check(const std::string &s1) {

   int x;
   std::size_t pos;
   try {
//   x = stoi(s1);
     x = stoi(s1, &pos);
     if (pos < s1.size()) {
       cout << "stoi_check: <" << s1 << "> not in correct form. Halting..." << endl;
       exit(1);
     }
   } catch(exception& e) {
     cout << "stoi_check: could not convert <" << s1 << "> to int. Halting..." << endl;
     exit(1);
   }
   return x;
}
// -----------------------------------------------------------------------------
double stod_check(const std::string &s1) {

   double x;
   std::size_t pos;
// cout << "stod_check: s1: " << s1 << endl;
   try {
//   x = stod(s1);
     x = stod(s1, &pos);
     if (pos < s1.size()) {
       cout << "stod_check: <" << s1 << "> not in correct form. Halting..." << endl;
       exit(1);
     }
   } catch(exception& e) {
     cout << "stod_check: could not convert <" << s1 << "> to double. Halting..." << endl;
     exit(1);
   }
   return x;
}
// -----------------------------------------------------------------------------
double stod_suffix(const std::string &s1) {

   int n;
   double x,factor1;
   bool flag_suffix;
   char suffix;

   n = s1.size();
   suffix = s1[n-1];

   flag_suffix = false;

   if (suffix == 'G') {
     factor1=1.0e9;
     flag_suffix = true;
   } else if (suffix == 'M') {
     factor1=1.0e6;
     flag_suffix = true;
   } else if (suffix == 'K') {
     factor1=1.0e3;
     flag_suffix = true;
   } else if (suffix == 'k') {
     factor1=1.0e3;
     flag_suffix = true;
   } else if (suffix == 'm') {
     factor1=1.0e-3;
     flag_suffix = true;
   } else if (suffix == 'u') {
     factor1=1.0e-6;
     flag_suffix = true;
   } else if (suffix == 'n') {
     factor1=1.0e-9;
     flag_suffix = true;
   } else if (suffix == 'p') {
     factor1=1.0e-12;
     flag_suffix = true;
   } else if (suffix == 'f') {
     factor1=1.0e-15;
     flag_suffix = true;
   }

   if (flag_suffix) {
     x = factor1*stod_check(s1.substr(0,n-1));
   } else {
     x = stod_check(s1);
   }
   return x;
}
// -----------------------------------------------------------------------------
void check_word_1(
   const vector<std::string> &v1,
   const int position,
   const std::string &word_given) {

// Check for word_given in v1 at the given position.

   int n;

   n = v1.size();
   if ((position < 0) || (position > (n-1))) {
     cout << "check_word_1: position = " << position << ", n = " << n << endl;
     cout << "  Not allowed. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   if (v1[position] != word_given) {
     cout << "check_word_1: <" << word_given << "> not found. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   return;
}
// -----------------------------------------------------------------------------
bool check_word_1a(
   const vector<std::string> &v1,
   const std::string &word_given) {

// Check for word_given in v1.
// example:
// v1: backward_euler  trz  trz_auto ..
// The given word is trz.

   int n;
   bool flag_1 = false;

   n = v1.size();
   for (int i=0; i < n; i++) {
     if (v1[i] == word_given) {
       flag_1 = true; break;
     }
   }
   if (!flag_1) {
     cout << "check_word_1a: <" << word_given << "> not found." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   return flag_1;
}
// -----------------------------------------------------------------------------
void check_word_2(
   const vector<std::string> &v1,
   const int position_string,
   const std::string &word_given,
   const int &k0) {

// example:
// f_5: xx ..
// position_string = 0
// keyword = "f_"
// k0 = 5

   int n,n1,nchr;
   std::string s1;

   n = v1.size();
   nchr = word_given.size();

   if ((position_string < 0) || (position_string > (n-1))) {
     cout << "check_word_1: position_string = " << position_string
          << ", n = " << n << endl;
     cout << "  Not allowed. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   if (v1[position_string].substr(0,nchr) != word_given) {
     cout << "check_word_2: <" << word_given << "> not found. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   n1 = v1[position_string].size();
   s1 = v1[position_string].substr(nchr,n1-nchr);
   if (s1 != to_string(k0)+':') {
     cout << "check_word_2: s1 = <" << s1 << "> is not equal to " << k0 << endl;
     cout << "  Halting. input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   return;
}
// -----------------------------------------------------------------------------
void check_word_3(
   const std::string &s1,
   const std::string &word_given) {

// check if word_given appears at the beginning of s1.
// e.g., d_dt in d_dt(xx)
// If it does not, report error.

   int n;
   n = word_given.size();

   if (s1.substr(0,n) != word_given) {
     cout << "check_word_3: s1 = <" << s1 << "> does not start with "
          << word_given << ". Halting..." << endl;
     exit(1);
   }
   return;
}
// -----------------------------------------------------------------------------
bool check_word_4(
   const std::string &s1,
   const std::string &word_given) {

// check if word_given appears at the beginning of s1.
// e.g., d_dt in d_dt(xx)
// If it does not, return false.

   int n;
   bool flag;
   n = word_given.size();

   flag = (s1.substr(0,n) == word_given);
   return flag;
}
// -----------------------------------------------------------------------------
std::string extract_string_1(
   const std::string &s1,
   char chr_left,
   char chr_right) {

   int pos1,pos2,n;
   n = s1.size();
   bool flag_1,flag_2;
   std::string s2;

   flag_1 = false;
   for (int i=0; i < n; i++) {
     if (s1[i] == chr_left) {
       flag_1 = true; pos1 = i; break;
     }
   }
   if (!flag_1) {
     cout << "extract_string_1: " << chr_left << " not found. Halting.." << endl;
     exit(1);
   }
   flag_2 = false;
   for (int i=n-1; i > 0; i--) {
     if (s1[i] == chr_right) {
       flag_2 = true; pos2 = i; break;
     }
   }
   if (!flag_2) {
     cout << "extract_string_1: " << chr_right << " not found. Halting.." << endl;
     exit(1);
   }
   s2 = s1.substr(pos1+1,pos2-pos1-1);
   return s2;
} //end of extract_string_1
// -----------------------------------------------------------------------------
void extract_string_3(
   const std::string &s1,
   std::string &s2,
   std::string &s3,
   bool &flag_found,
   char chr_left,
   char chr_right) {

   int pos1,pos2,n;
   n = s1.size();
   bool flag_1,flag_2;

   flag_1 = false;
   for (int i=0; i < n; i++) {
     if (s1[i] == chr_left) {
       flag_1 = true; pos1 = i; break;
     }
   }
   if (!flag_1) {
     flag_found = false;
     return;
   }
   flag_2 = false;
   for (int i=n-1; i > 0; i--) {
     if (s1[i] == chr_right) {
       flag_2 = true; pos2 = i; break;
     }
   }
   if (!flag_2) {
     flag_found = false;
     return;
   }
   flag_found = true;
   s2 = s1.substr(0,pos1);
   s3 = s1.substr(pos1+1,pos2-pos1-1);
   return;
} //end of extract_string_3
// -----------------------------------------------------------------------------
void find_word_1(
   const vector<std::string> &v1,
   const std::string &word_given,
   int &position,
   bool &flag_found) {

// look for word_given in v1. If found, assign position.

   flag_found = false;
   for (unsigned int i=0; i < v1.size(); i++) {
     if (v1[i] == word_given) {
       position = i;
       flag_found = true;
       break;
     }
   }
   return;
}
// -----------------------------------------------------------------------------
void find_word_2(
   const vector<std::string> &v1,
   const std::string &word_given,
   int &position) {

// look for word_given in v1. If found, assign position. If not, stop.
   bool flag_found;

   flag_found = false;
   for (unsigned int i=0; i < v1.size(); i++) {
     if (v1[i] == word_given) {
       position = i;
       flag_found = true;
       break;
     }
   }
   if (!flag_found) {
     cout << "find_word_2: <" << word_given << "> not found. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   return;
}
// -----------------------------------------------------------------------------
void find_word_3(
   const vector<std::string> &v1,
   const std::string &word_given,
   const int &offset,
   const int &increment,
   int &position,
   bool &flag_found) {

// look for word_given in v1. If found, assign position.

   int n;
   flag_found = false;
   n = v1.size();
   for (int i=offset; i < n; i=i+increment) {
     if (v1[i] == word_given) {
       position = i;
       flag_found = true;
       break;
     }
   }
   return;
}
// -----------------------------------------------------------------------------
bool find_word_4(
   const vector<std::string> &v1,
   const std::string &word_given,
   int &position) {

// look for word_given in v1. If found, assign position.
   bool flag_found = false;

   flag_found = false;
   for (unsigned int i=0; i < v1.size(); i++) {
     if (v1[i] == word_given) {
       position = i;
       flag_found = true;
       break;
     }
   }
   return flag_found;
}
// -----------------------------------------------------------------------------
int assign_int_2(
   const vector<std::string> &v1,
   const int &position,
   const std::string &keyword) {

// look for keyword in v1 at the given position, pick the next string in v1,
// convert it to int.

   int n;
   int x;

   n = v1.size();
   if ((position < 0) || (position > (n-2))) {
     cout << "assign_int_2: position = " << position << ", n = " << n << endl;
     cout << "  Not allowed. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   if (v1[position] == keyword) {
     x = stoi(v1[position+1]);
   } else {
     cout << "assign_int_2: <" << keyword << "> not found. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   return x;
}
// -----------------------------------------------------------------------------
int assign_int_2a(
   std::fstream &inf,
   const int &position,
   const std::string &keyword) {

// look for keyword in the next line at the given position,
// pick the next string in v1, convert it to int.

   vector<std::string> v1;
   int x;

   next_line(inf,v1);
   x = assign_int_2(v1,position,keyword);
   return x;
}
// -----------------------------------------------------------------------------
std::string assign_string_1(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   const std::string &keyword) {

// look for keyword in v1, pick the next string in v1, return it.
// Use offset and increment to restrict search to odd/even positions.

   bool flag_found = false;
   int n;
   std::string x;

   n = v1.size();
   for (int i=offset; i < n; i=i+increment) {
     if (v1[i] == keyword) {
       if (i == (n-1)) {
         cout << "assign_string_1: <" << keyword << "> found." << endl;
         cout << "  but at the last position. Halting..." << endl;
         cout << "  input vector is: " << endl;
         print_vec_2<string>(v1); exit(1);
       }
       x = v1[i+1];
       flag_found = true;
       break;
     }
   }
   if (!flag_found) {
     cout << "assign_string_1: <" << keyword << "> not found. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   return x;
}
// -----------------------------------------------------------------------------
std::string assign_string_2(
   const vector<std::string> &v1,
   const int &position,
   const std::string &keyword) {

// look for keyword in v1 at the given position, pick the next string in v1,
// return it.

   int n;
   std::string x;

   n = v1.size();
   if ((position < 0) || (position > (n-2))) {
     cout << "assign_string_2: position = " << position << ", n = " << n << endl;
     cout << "  Not allowed. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   if (v1[position] == keyword) {
     x = v1[position+1];
   } else {
     cout << "assign_string_2: <" << keyword << "> not found. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   return x;
}
// -----------------------------------------------------------------------------
std::string assign_string_3(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   int &position,
   const std::string &word_given) {

// look for keyword in v1, pick the next string in v1, return it.
// Use offset and increment to restrict search to odd/even positions.
// Also, return position where word_given was found.

   bool flag_found = false;
   int n;
   std::string x;

   n = v1.size();
   for (int i=offset; i < n; i=i+increment) {
     if (v1[i] == word_given) {
       if (i == (n-1)) {
         cout << "assign_string_1: <" << word_given << "> found." << endl;
         cout << "  but at the last position. Halting..." << endl;
         cout << "  input vector is: " << endl;
         print_vec_2<string>(v1); exit(1);
       }
       x = v1[i+1];
       flag_found = true;
       position = i;
       break;
     }
   }
   if (!flag_found) {
     cout << "assign_string_1: <" << word_given << "> not found. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   return x;
}
// -----------------------------------------------------------------------------
bool assign_bool_3(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   const std::string &keyword,
   const bool &x_default) {

// look for keyword in v1. If keyword is found,  pick the next string in v1,
// yes: return true, no: return false; else, return x_default.
// Use offset and increment to restrict search to odd/even positions.

   bool flag_found = false;
   int n;
   bool x;
   std::string s1;

   n = v1.size();
   for (int i=offset; i < n; i=i+increment) {
     if (v1[i] == keyword) {
       if (i == (n-1)) {
         cout << "assign_bool_3: <" << keyword << "> found." << endl;
         cout << "  but at the last position. Halting..." << endl;
         cout << "  input vector is: " << endl;
         print_vec_2<string>(v1); exit(1);
       }
       s1 = v1[i+1];
       if (s1 == "yes") {
         x = true;
       } else if (s1 == "no") {
         x = false;
       } else {
         cout << "assign_bool_3: <" << keyword << "> found." << endl;
         cout << "  but the next string must be yes/no. Halting..." << endl;
         cout << "  input vector is: " << endl;
         print_vec_2<string>(v1); exit(1);
       }
       flag_found = true;
       break;
     }
   }
   if (!flag_found) {
     x = x_default;
   }
   return x;
}
// -----------------------------------------------------------------------------
void assign_bool_4(
   const vector<std::string> &v1,
   bool &x,
   const int &position,
   const std::string &keyword1,
   const std::string &keyword2,
   const bool &b1,
   const bool &b2) {

// If keyword1 is found at position, make x = b1.
// If keyword2 is found at position, make x = b2.
// If neither, report error.

   if (v1[position] == keyword1) {
     x = b1;
   } else if (v1[position] == keyword2) {
     x = b2;
   } else {
     cout << "assign_bool_4: expect " << keyword1 << "/" << keyword2
          << " here." << endl;
     cout << "  but neither was found. Halting..." << endl;
     cout << "  input vector is: " << endl;
     print_vec_2<string>(v1); exit(1);
   }
   return;
}
// -----------------------------------------------------------------------------
void assign_vec_int_1(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   vector<int> &v2) {

// From element offset up to the last element, convert strings to int and
// assign to v2.
// Use offset and increment to handle only even/only odd/all strings.

   int n;
   int x;

   n = v1.size();
   for (int i=offset; i < n; i=i+increment) {
     x = stoi(v1[i]);
     v2.push_back(x);
   }
   return;
}
// -----------------------------------------------------------------------------
void assign_vec_double_1(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   vector<double> &v2) {

// From element offset up to the last element, convert strings to double and
// assign to v2.
// Use offset and increment to handle only even/only odd/all strings.

   int n;
   double x;

   n = v1.size();
   for (int i=offset; i < n; i=i+increment) {
     x = stod_suffix(v1[i]);
     v2.push_back(x);
   }
   return;
}
// -----------------------------------------------------------------------------
void assign_vec_string_1(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   vector<string> &v2) {

// From element offset up to the last element, pick strings from v1 and
// assign to v2.
// Use offset and increment to handle only even/only odd/all strings.

   int n;
   std::string x;

   n = v1.size();
   for (int i=offset; i < n; i=i+increment) {
     x = v1[i];
     v2.push_back(x);
   }
   return;
}
// -----------------------------------------------------------------------------
void extract_left_right_1(
   const std::string &x,
   const std::string &substring,
   std::string &x_left,
   std::string &x_right) {

// From a string like "aa_of_bb",
// extract aa and copy to v_left
// extract bb and copy to v_right

   int pos,n1,n2,n3,n4;

   pos = isSubstring(substring,x);
   if (pos == -1) {
     cout << "extract_left_right_1: <" << substring << "> not found." << endl;
     cout << "   Halting..." << endl; exit(1);
   } else {
     n1 = x.size();
     n2 = substring.size();
     x_left  = x.substr(0,pos);
     n3 = pos+n2;
     n4 = n1-n3;
     x_right = x.substr(n3,n4);
   }
   return;
}
// -----------------------------------------------------------------------------
void split_vec_string_1(
   const vector<std::string> &v1,
   const std::string &keyword,
   vector<std::string> &v_left,
   vector<std::string> &v_right) {

// From a vector like "aa_of_bb", "cc_of_dd", ...
// extract aa, cc, .. and copy to v_left
// extract bb, dd, .. and copy to v_right

   int n;
   std::string x_left,x_right;

   n = v1.size();
   v_left.clear();
   v_right.clear();

   for (int i=0; i < n; i++) {
     extract_left_right_1(v1[i],keyword,x_left,x_right);
     v_left.push_back(x_left);
     v_right.push_back(x_right);
   }
   return;
}
// -----------------------------------------------------------------------------
void assign_parms_int_2(
   const vector<std::string> &v1,
   const vector<std::string> &prm_name,
   vector<int> &prm_dest,
   vector<bool> &tick) {

   int n1;
   std::string s1,s2;
   bool flag_1;
   int pos;

   n1 = prm_name.size();

   for (int i=0; i < n1; i++) {
     s1 = prm_name[i];
     find_word_3(v1,s1,1,2,pos,flag_1);
     if (flag_1) {
       tick[pos] = true; tick[pos+1] = true;
       s2 = v1[pos+1];
       prm_dest[i] = stoi_check(s2);
     }
   }
   return;
}
// -----------------------------------------------------------------------------
void assign_parms_double_2(
   const vector<std::string> &v1,
   const vector<std::string> &prm_name,
   vector<double> &prm_dest,
   vector<bool> &tick) {

   int n1;
   std::string s1,s2;
   bool flag_1;
   int pos;

   n1 = prm_name.size();

   for (int i=0; i < n1; i++) {
     s1 = prm_name[i];
     find_word_3(v1,s1,1,2,pos,flag_1);
     if (flag_1) {
       tick[pos] = true; tick[pos+1] = true;
       s2 = v1[pos+1];
       prm_dest[i] = stod_suffix(s2);
     }
   }
   return;
}
// -----------------------------------------------------------------------------
void assign_parms_string_2(
   const vector<std::string> &v1,
   const vector<std::string> &prm_name,
   vector<std::string> &prm_dest,
   vector<bool> &tick) {

   int n1;
   std::string s1,s2;
   bool flag_1;
   int pos;

   n1 = prm_name.size();

   for (int i=0; i < n1; i++) {
     s1 = prm_name[i];
     find_word_3(v1,s1,1,2,pos,flag_1);
     if (flag_1) {
       tick[pos] = true; tick[pos+1] = true;
       s2 = v1[pos+1];
       prm_dest[i] = s2;
     }
   }
   return;
}
// -----------------------------------------------------------------------------
void assign_names_1(
   std::fstream &inf,
   const std::string &keyword,
   vector<std::string> &prm_name,
   int &nprm) {

   vector<std::string> v1;

   next_line(inf,v1);
   check_word_1(v1,0,keyword);
   assign_vec_string_1(v1,1,1,prm_name);
   nprm = prm_name.size();
   return;
}
// -----------------------------------------------------------------------------
void assign_names_values_int_1(
   std::fstream &inf,
   const std::string &keyword,
   vector<std::string> &prm_name,
   vector<int> &prm_val,
   int &nprm) {

   vector<std::string> v1;

   next_line(inf,v1);
   check_word_1(v1,0,keyword);
   assign_vec_string_1(v1,1,2,prm_name);
   assign_vec_int_1   (v1,2,2,prm_val);
   nprm = prm_name.size();

   return;
};
// -----------------------------------------------------------------------------
void assign_names_values_double_1(
   std::fstream &inf,
   const std::string &keyword,
   vector<std::string> &prm_name,
   vector<double> &prm_val,
   int &nprm) {

   vector<std::string> v1;

   next_line(inf,v1);
   check_word_1(v1,0,keyword);
   assign_vec_string_1(v1,1,2,prm_name);
   assign_vec_double_1(v1,2,2,prm_val);
   nprm = prm_name.size();

   return;
};
// -----------------------------------------------------------------------------
void assign_names_values_string_1(
   std::fstream &inf,
   const std::string &keyword,
   vector<std::string> &prm_name,
   vector<std::string> &prm_val,
   int &nprm) {

   vector<std::string> v1;

   next_line(inf,v1);
   check_word_1(v1,0,keyword);
   assign_vec_string_1(v1,1,2,prm_name);
   assign_vec_string_1(v1,2,2,prm_val);
   nprm = prm_name.size();

   return;
};
// -----------------------------------------------------------------------------
void print_vec_double_1(
   std::fstream &outf,
   const vector<double> &v1,
   const int word_width) {

// - format ("scientific", for example) is assumed to be already set.
// - precision should be already set in the calling routine.

   for (unsigned int i=0; i < v1.size(); i++) {
     outf << setw(word_width) << v1[i] << endl;
   }
   return;
}
// -----------------------------------------------------------------------------
void read_vec_double_1(
   std::fstream &inf,
   vector<double> &v1) {

   for (unsigned int i=0; i < v1.size(); i++) {
     v1[i] = next_double_1(inf);
   }
   return;
}
// -----------------------------------------------------------------------------
bool flag_nan(
   double &x) {

   if (x != x) {
     return true;
   } else {
     return false;
   }
}
// -----------------------------------------------------------------------------
void set_vector_1(
   double* x,
   const int n,
   const int i0) {

   for (int i = 0;i < n;i++) {
     x[i] = 0.0;
   }
   x[i0] = 1.0;

   return;
}
// -----------------------------------------------------------------------------
void check_array_for_nan_2(
   const int n,
   double *x,
   bool &flag_nan_1) {

   flag_nan_1 = false;

   for (int i=0; i < n; i++) {
     if (flag_nan(x[i])) {
       flag_nan_1 = true;
       break;
     }
   }
   return;
}
// -----------------------------------------------------------------------------
double norm_2(
   const int n,
   double *x) {

   double norm,sum1 = 0.0;

   for (int i=0; i < n; i++) {
     sum1 += x[i]*x[i];
   }
   norm = sqrt(sum1/(double)n);
   return norm;
}
// -----------------------------------------------------------------------------
double norm_2_1(
   const int offset,
   const int n,
   double *x) {

   double norm,sum1 = 0.0;

   for (int i=offset; i < (offset + n); i++) {
     sum1 += x[i]*x[i];
   }
   norm = sqrt(sum1/(double)n);
   return norm;
}
// -----------------------------------------------------------------------------
double norm_inf(
   const int n,
   double *x) {

   double absval,norm = 0.0;

   for (int i=0; i < n; i++) {
     absval = fabs(x[i]);
     if (absval > norm) norm = absval;
   }
   return norm;
}
// -----------------------------------------------------------------------------
double norm_inf_1(
   const int offset,
   const int n,
   double *x) {

   double absval,norm = 0.0;

   for (int i=offset; i < (offset + n); i++) {
     absval = fabs(x[i]);
     if (absval > norm) norm = absval;
   }
   return norm;
}
// -----------------------------------------------------------------------------
double exp_lmt(
   const double x,
   const double x1) {

// limit exp(x)

   double y1=0.0;
   double y=0.0;

   if (x > x1) {
     y1 = exp(x1);
     y = y1 + y1*(x-x1);
   } else { 
     y = exp(x);
   }

   return y;
}
// -----------------------------------------------------------------------------
void exp_lmt_1(
   const double x,
   const double x1,
   double &y,
   double &yp) {

// limit exp(x)

   double y1=0.0;

   if (x > x1) {
     y1 = exp(x1);
     y = y1 + y1*(x-x1);
     yp = y1;
   } else {
     y = exp(x);
     yp = y ;
   }

   return;
}
// -----------------------------------------------------------------------------
double get_tnext(
   const int iter,
   const int flag_quad,
   const double t_1,
   const double t_2,
   const double x_1,
   const double x_2,
   const double t0,
   const double x0,
   const double epsl,
   const double delt_min,
   const double delt_nrml) {

   double t_next;
   double t_cross,t_new;
   double x12,t12,m12;
   double m23,t_cross_new;
   double t_2p,t_3p,a11,a12,a21,a22,a_fit,b_fit,c_fit;
   double delta,ainv_11,ainv_12,ainv_21,ainv_22;
   double delta1,sq1,t_cross_p1,t_cross_p2,t_low,t_high;
   bool flag_found;

   if (iter >= 2) {
     if (x0*x_1 <= 0) {
       t_next = t0 + delt_nrml;
     } else {
       x12 = x0-x_1;

       if (fabs(x12) < epsl) {
         t_next = t0 + delt_nrml;
       } else {
         t12 = t0-t_1;
         m12 = x12/t12;
         t_cross = t0 - (x0/m12);

         if (t_cross > t0) {
           if (flag_quad == 1) {
             m23 = (x_1-x_2)/(t_1-t_2);
             if (abs(m12-m23) > 0.01*abs(m12+m23)) {
               t_2p = t_1-t0;
               t_3p = t_2-t0;

               a11 = t_2p*t_2p;
               a12 = t_2p;
               a21 = t_3p*t_3p;
               a22 = t_3p;

               delta = a11*a22 - a12*a21;

               ainv_11 =  a22/delta;
               ainv_22 =  a11/delta;
               ainv_12 = -a12/delta;
               ainv_21 = -a21/delta;

               a_fit = ainv_11*(x_1-x0) + ainv_12*(x_2-x0);
               b_fit = ainv_21*(x_1-x0) + ainv_22*(x_2-x0);
               c_fit = x0;

               delta1 = b_fit*b_fit - 4.0*a_fit*c_fit;
               if (delta1 >= 0.0) {
                 sq1 = sqrt(delta1);
                 t_cross_p1 = (-b_fit + sq1)/(2.0*a_fit);
                 t_cross_p2 = (-b_fit - sq1)/(2.0*a_fit);
                 t_low  = min(t_cross_p1,t_cross_p2) + t0;
                 t_high = max(t_cross_p1,t_cross_p2) + t0;

                 flag_found = false;
                 if (t_low > t0) {
                   t_cross_new = t_low;
                   flag_found = true;
                 } else if (t_high > t0) {
                   t_cross_new = t_high;
                   flag_found = true;
                 }
                 if (flag_found) {
                   if ((t_cross_new > t0) && (t_cross_new < t_cross)) {
                     t_cross = t_cross_new;
                   }
                 }
               }
             }
           }
           if (t_cross > t0) {
             if ((t_cross-t0) > 2*delt_min) {
                t_next = t_cross - delt_min;
             } else {
                t_next = t_cross + delt_min;
             }
           } else {
             t_next = t0 + delt_nrml;
           }
         } else {
           t_next = t0 + delt_nrml;
         }
       }
     }
   } else {
     t_next = t0 + delt_nrml;
   }
   return t_next;
}
// -----------------------------------------------------------------------------
void partial_fractions_1(
   const int degree_n,
   const int degree_d,
   double* coef_n,
   double* coef_d,
   double& gain,
   double& a0,
   int& pp_n_roots,
   std::complex<double>* pp_coef,
   std::complex<double>* pp_root,
   int* pp_flag_real,
   int* pp_sign_imag,
   int* pp_power,
   int& ppc_n_roots,
   std::complex<double>* ppc_coef,
   std::complex<double>* ppc_root,
   int* ppc_flag_real,
   int* ppc_power,
   const double eps2) {

   int cf_n_r_root=0;
   int cf_n_c_root=0;
   int degree_n_1=0;
   int pntr=0;
   int i1=0;

   int* cf_r_order;
   int* cf_c_order;
   double* cf_r_root;
   std::complex<double>* cf_c_root;
   std::complex<double>* root_temp;
   double* coef_n_1;
   double* coef_d_1;
   std::complex<double>* coef_d_2;
   int* map_pp_to_cf;
   std::complex<double>** A;
   std::complex<double>*  A_pntr;
   std::complex<double>** Ainvrs;
   std::complex<double>*  Ainvrs_pntr;
   std::complex<double>*  B;
   std::complex<double>*  coef1;
   int* indxc;
   int* indxr;
   int* ipiv;
   int* c_root_pair;
   int* mark;

   std::complex<double> pp1_root;
   int pp1_flag_real=0;
   int pp1_sign_imag=0;
   int pp1_power=0;
   int i_cf=0;
   std::complex<double> z1;

   std::complex<double> zero_c;
   int n1=0;

   zero_c = std::complex<double>(0.0,0.0);

   pp1_root = zero_c;
   z1       = zero_c;

   if (degree_n > degree_d) {
     cout << "partial_fractions_1: degree_n > degree_d not allowed." << endl;
     cout << "  degree_n=" << degree_n << endl;
     cout << "  degree_d=" << degree_d << endl;
     cout << "  Halting..." << endl;
     exit (1);
   }
   if (coef_n[degree_n] == 0.0) {
     cout << "partial_fractions_1: coef_n[degree_n] is zero! Halting..." << endl;
     exit (1);
   }
   if (coef_d[degree_d] == 0.0) {
     cout << "partial_fractions_1: coef_d[degree_d] is zero! Halting..." << endl;
     exit (1);
   }

   coef_n_1 = new double[degree_n+1];
   coef_d_1 = new double[degree_d+1];

   assign_all_double_1(coef_n_1,degree_n+1,0.0);
   assign_all_double_1(coef_d_1,degree_d+1,0.0);

   coef_d_2 = new std::complex<double>[degree_d+1];
   assign_all_complex_1(coef_d_2,degree_d+1,zero_c);

   cf_r_order = new int[degree_d];
   cf_c_order = new int[degree_d];

   assign_all_int_1(cf_r_order,degree_d,0);
   assign_all_int_1(cf_c_order,degree_d,0);

   cf_r_root = new double[degree_d];
   assign_all_double_1(cf_r_root,degree_d,0.0);

   cf_c_root = new std::complex<double>[degree_d];
   root_temp = new std::complex<double>[degree_d+1];

   assign_all_complex_1(cf_c_root,degree_d  ,zero_c);
   assign_all_complex_1(root_temp,degree_d+1,zero_c);

   map_pp_to_cf = new int[degree_d];
   c_root_pair  = new int[degree_d];
   mark         = new int[degree_d];

   assign_all_int_1(map_pp_to_cf,degree_d,0);
   assign_all_int_1(c_root_pair ,degree_d,0);
   assign_all_int_1(mark        ,degree_d,0);

   n_by_d_1(degree_n,degree_d,coef_n,coef_d,
     degree_n_1,coef_n_1,coef_d_1,gain,a0);

   for (int i=0; i <= degree_d; i++) {
     coef_d_2[i] = std::complex<double>(coef_d_1[i],0.0);
   }
   zroots(coef_d_2,degree_d,root_temp,1);

   classify_roots(degree_d,root_temp,cf_n_r_root,cf_n_c_root,
     cf_r_order,cf_c_order,cf_r_root,cf_c_root,eps2);

// handle real roots:

   pp_n_roots = 0;

   for (int i=0; i < cf_n_r_root; i++) {
     for (int j=1; j <= cf_r_order[i]; j++) {
       map_pp_to_cf[pp_n_roots] = i;
       pp_flag_real[pp_n_roots] = int_T;
       pp_sign_imag[pp_n_roots] = 0.0;
       pp_power[pp_n_roots] = j;
       pp_root[pp_n_roots] = std::complex<double>(cf_r_root[i],0.0);
       c_root_pair[pp_n_roots] = -1;

       pp_n_roots++;
     }
   }

// handle complex roots;

   for (int i=0; i < cf_n_c_root; i++) {
     for (int j=1; j <= cf_c_order[i]; j++) {
//     assign a+jb here:
       map_pp_to_cf[pp_n_roots] = i;
       pp_flag_real[pp_n_roots] = int_F;
       pp_power[pp_n_roots] = j;
       pp_root[pp_n_roots] = cf_c_root[i];
       if (pp_root[pp_n_roots].imag() > 0.0) {
         pp_sign_imag[pp_n_roots] =  1.0;
       } else {
         pp_sign_imag[pp_n_roots] = -1.0;
       }
       c_root_pair[pp_n_roots] = pp_n_roots + 1;
       pp_n_roots++;

//     assign a-jb here:
       map_pp_to_cf[pp_n_roots] = i;
       pp_flag_real[pp_n_roots] = int_F;
       pp_power[pp_n_roots] = j;
       pp_root[pp_n_roots] = conj(cf_c_root[i]);
       if (pp_root[pp_n_roots].imag() > 0.0) {
         pp_sign_imag[pp_n_roots] =  1.0;
       } else {
         pp_sign_imag[pp_n_roots] = -1.0;
       }
       c_root_pair[pp_n_roots] = pp_n_roots - 1;
       pp_n_roots++;
     }
   }

   A_pntr = new std::complex<double>[pp_n_roots*pp_n_roots];
   A = new std::complex<double>*[pp_n_roots];
   pntr = 0;
   for (int k = 0; k < pp_n_roots; k++) {
     A[k] = A_pntr + pntr;
     A[k] = new std::complex<double>[pp_n_roots];
     pntr = pntr + pp_n_roots;
   }

   Ainvrs_pntr = new std::complex<double>[pp_n_roots*pp_n_roots];
   Ainvrs = new std::complex<double>*[pp_n_roots];
   pntr = 0;
   for (int k = 0; k < pp_n_roots; k++) {
     Ainvrs[k] = Ainvrs_pntr + pntr;
     Ainvrs[k] = new std::complex<double>[pp_n_roots];
     pntr = pntr + pp_n_roots;
   }

   B = new std::complex<double>[pp_n_roots];
   coef1 = new std::complex<double>[pp_n_roots+1];

   indxc = new int[pp_n_roots];
   indxr = new int[pp_n_roots];
   ipiv  = new int[pp_n_roots];

// B[0] corresponds to the coefficient of s^0 in the new
// numerator (N1), and so on.

   for (int i = 0; i < degree_d; i++) {
     if (i <= degree_n_1) {
       B[i] = std::complex<double>(coef_n_1[i],0.0);
     } else {
       B[i] = zero_c;
     }
   }

// Assemble matrix A:

// initialise:
   for (int i = 0; i < degree_d; i++) {
     for (int j = 0; j < degree_d; j++) {
       A[i][j] = zero_c;
     }
   }

   for (int i_pp = 0; i_pp < pp_n_roots; i_pp++) {
     pp1_root      = pp_root     [i_pp];
     pp1_flag_real = pp_flag_real[i_pp];
     pp1_sign_imag = pp_sign_imag[i_pp];
     pp1_power     = pp_power    [i_pp];

     i_cf = map_pp_to_cf[i_pp];
     initialise_coeff_1(degree_d,coef1);

     if (pp1_flag_real == int_T) {
//     multiply by (s-z) (z real)
       for (int i_cf1 = 0; i_cf1 < cf_n_r_root; i_cf1++) {
         z1 = std::complex<double>(cf_r_root[i_cf1],0.0);
         if (i_cf == i_cf1) {
           n1 = cf_r_order[i_cf1] - pp1_power;
         } else {
           n1 = cf_r_order[i_cf1];
         }
         for (int i_pwr = 0; i_pwr < n1; i_pwr++) {
           compute_coeff_1(degree_d,coef1,z1);
         }
       }
//     multiply by (s-z) (z = a + jb)
       for (int i_cf1 = 0; i_cf1 < cf_n_c_root; i_cf1++) {
         z1 = cf_c_root[i_cf1];
         n1 = cf_c_order[i_cf1];
         for (int i_pwr = 0; i_pwr < n1; i_pwr++) {
           compute_coeff_1(degree_d,coef1,z1);
         }
       }
//     multiply by (s-z) (z = a - jb)
       for (int i_cf1 = 0; i_cf1 < cf_n_c_root; i_cf1++) {
         z1 = conj(cf_c_root[i_cf1]);
         n1 = cf_c_order[i_cf1];
         for (int i_pwr = 0; i_pwr < n1; i_pwr++) {
           compute_coeff_1(degree_d,coef1,z1);
         }
       }
     } else {
//     multiply by (s-z) (z real)
       for (int i_cf1 = 0; i_cf1 < cf_n_r_root; i_cf1++) {
         z1 = std::complex<double>(cf_r_root[i_cf1],0.0);
         n1 = cf_r_order[i_cf1];
         for (int i_pwr = 0; i_pwr < n1; i_pwr++) {
           compute_coeff_1(degree_d,coef1,z1);
         }
       }
//     multiply by (s-z) (z = a + jb)
       for (int i_cf1 = 0; i_cf1 < cf_n_c_root; i_cf1++) {
         z1 = cf_c_root[i_cf1];
         if (pp1_sign_imag == 1) {
           if (i_cf == i_cf1) {
             n1 = cf_c_order[i_cf1] - pp1_power;
           } else {
             n1 = cf_c_order[i_cf1];
           }
         } else {
           n1 = cf_c_order[i_cf1];
         }
         for (int i_pwr = 0; i_pwr < n1; i_pwr++) {
           compute_coeff_1(degree_d,coef1,z1);
         }
       }
//     multiply by (s-z) (z = a - jb)
       for (int i_cf1 = 0; i_cf1 < cf_n_c_root; i_cf1++) {
         z1 = conj(cf_c_root[i_cf1]);
         if (pp1_sign_imag == -1) {
           if (i_cf == i_cf1) {
             n1 = cf_c_order[i_cf1] - pp1_power;
           } else {
             n1 = cf_c_order[i_cf1];
           }
         } else {
           n1 = cf_c_order[i_cf1];
         }
         for (int i_pwr = 0; i_pwr < n1; i_pwr++) {
           compute_coeff_1(degree_d,coef1,z1);
         }
       }
     }

     for (int j = 0; j < degree_d; j++) {
       A[j][i_pp] = coef1[j];
     }
   }

// write A and B to console:

// Solve equations for pp coefficients:

   mat_solve_c(pp_n_roots,A,Ainvrs,B,pp_coef,indxc,indxr,ipiv);

   ppc_n_roots = 0;

   for (int i = 0;i < pp_n_roots;i++) {
     mark[i] = int_F;
   }
   for (int i = 0;i < pp_n_roots;i++) {
     if (pp_flag_real[i] == int_T) {
       ppc_coef     [ppc_n_roots] = pp_coef[i];
       ppc_root     [ppc_n_roots] = pp_root[i];
       ppc_flag_real[ppc_n_roots] = int_T;
       ppc_power    [ppc_n_roots] = pp_power[i];

       mark[i] = int_T;
       ppc_n_roots++;
     } else {
       if (mark[i] == int_F) {
         ppc_flag_real[ppc_n_roots] = int_F;
         ppc_power    [ppc_n_roots] = pp_power[i];

         if (pp_sign_imag[i] == 1) {
           ppc_coef[ppc_n_roots] = pp_coef[i];
           ppc_root[ppc_n_roots] = pp_root[i];
         } else {
           i1 = c_root_pair[i];
           ppc_coef[ppc_n_roots] = pp_coef[i1];
           ppc_root[ppc_n_roots] = pp_root[i1];
         }
         i1 = c_root_pair[i];
         mark[i ] = int_T;
         mark[i1] = int_T;
         ppc_n_roots++;
       }
     }
   }

// delete temp arrays:

   delete[] coef_n_1;
   delete[] coef_d_1;
   delete[] coef_d_2;
   delete[] cf_r_order;
   delete[] cf_c_order;
   delete[] cf_r_root;
   delete[] cf_c_root;
   delete[] root_temp;
   delete[] map_pp_to_cf;

   delete[] B;
   delete[] coef1;

   delete[] A_pntr;
   delete[] Ainvrs_pntr;

   for (int k = 0; k < pp_n_roots; k++) {
     delete[] A[k];
   }
   delete[] A;
  
   for (int k = 0; k < pp_n_roots; k++) {
     delete[] Ainvrs[k];
   }
   delete[] Ainvrs;

   delete[] indxc;
   delete[] indxr;
   delete[] ipiv;

   delete[] c_root_pair;
   delete[] mark;

   return;
}
// -----------------------------------------------------------------------------
void laguer(
   std::complex<double>* a,
   int m,
   std::complex<double>* x,
   int* its) {

   int iter=0,j=0;
   double abx=0.0,abp=0.0,abm=0.0,err=0.0;
   std::complex<double> dx,x1,b,d,f,g,h,sq,gp,gm,g2;
   std::complex<double> k0,k1;

   const int MR = 8;
   const int MT = 10;
   const int MAXIT = MT*MR;
// const double EPSS = 1.0e-7;
   const double EPSS = 1.0e-15;

   static double frac[MR+1] = {0.0,0.5,0.25,0.75,0.13,0.38,0.62,0.88,1.0};
   double p1,p2;

// from Numerical Recipes.

   for (iter=1; iter <= MAXIT; iter++) {
     *its = iter;
     b = a[m];
     err = abs(b);
     d = std::complex<double>(0.0,0.0);
     f = std::complex<double>(0.0,0.0);
     abx = abs(*x);

     for (j=m-1; j >= 0; j--) {
       f = (*x)*f + d;
       d = (*x)*d + b;
       b = (*x)*b + a[j];

       err = abs(b) + abx*err;
     }
     err *= EPSS;
     if (abs(b) <= err) return;
     g = d/b;
     g2 = g*g;
     h = g2 - (2.0*(f/b));

     k0 = ((double)(m))*h - g2;
     k1 = ((double)(m-1))*k0;
     sq = sqrt(k1);

     gp = (g + sq);
     gm = (g - sq);
     abp = abs(gp);
     abm = abs(gm);

     if (abp < abm) gp=gm;

     p1 = cos((double)(iter));
     p2 = sin((double)(iter));

     dx = (max(abp,abm) > 0.0
        ? (std::complex<double>((double)(m),0.0)/gp)
        : (1.0+abx)*(std::complex<double>(p1,p2)));

     x1 = (*x) - dx;

     if (x->real() == x1.real() && x->imag() == x1.imag()) return;
     if (iter % MT) *x=x1;
     else *x = (*x) - frac[iter/MT]*dx;
   }
   cout << "laguer: too many iterations in laguer" << endl;
   cout << "  Halting..." << endl;
   exit (1);
   return;
}
// -----------------------------------------------------------------------------
void zroots(
   std::complex<double>* a,
   const int m,
   std::complex<double>* roots,
   int polish) {

// from Numerical Recipes.
// Given the degree m and the m+1 complex coefficients a[0,m],
// this routine successively calls laguer and finds all m complex
// roots in roots[1,m]. The boolean variable polish should be
// input as true (1) if polishing is desired, false (0) if the
// roots will be subsequently polished by other means.
// ** Note: roots are roots[1] to roots[m]

   const double EPS = 2.0e-6;
   const int MAXM = 10;

   int i=0,its=0,j=0,jj=0;
   std::complex<double> x,b,c,ad[MAXM];

   for (j=0; j<=m; j++) ad[j] = a[j];

   for (j=m; j >= 1; j--) {
     x = std::complex<double>(0.0,0.0);
     laguer(ad,j,&x,&its);
     if (fabs(x.imag()) <= 2.0*EPS*fabs(x.real())) {
       x = std::complex<double>(x.real(),0.0);
     }
     roots[j] = x;
     b = ad[j];

     for (jj=j-1; jj >= 0; jj--) {
       c = ad[jj];
       ad[jj] = b;
       b = (x*b)+c;
     }
   }
   if (polish) {
     for (j=1; j<=m; j++) {
       laguer(a,m,&roots[j],&its);
     }
   }

   for (j=2; j<=m; j++) {
     x = roots[j];
     for (i=j-1; i >= 1; i--) {
       if (roots[i].real() <= x.real()) break;
       roots[i+1] = roots[i];
     }
     roots[i+1] = x;
   }
   return;
}
// -----------------------------------------------------------------------------
void classify_roots(
   const int m,
   std::complex<double>* roots,
   int& n_r_root,
   int& n_c_root,
   int* r_order,
   int* c_order,
   double* r_root,
   std::complex<double>* c_root,
   const double eps2) {

//
// eps2 is used to check if two roots are equal.

   const double eps1 = 1.0e-8;
   int flag_i=0;

   int n_r_root_1=0;
   int n_c_root_1=0;

   double* r_root_1;
   std::complex<double>* c_root_1;

   int* mark;
   double root_real=0.0,root_imag=0.0;
   double root_real_1=0.0,root_imag_1=0.0;
   double root_real_2=0.0,root_imag_2=0.0;
   int n_root_real_check=0,n_root_imag_check=0;

   std::complex<double> zero_c;
   zero_c =  std::complex<double>(0.0,0.0);

   r_root_1 = new double[m];
   assign_all_double_1(r_root_1,m,0.0);

   c_root_1 = new std::complex<double>[m];
   assign_all_complex_1(c_root_1,m,zero_c);

   mark = new int[m];
   assign_all_int_1(mark,m,0);

//

   n_r_root_1 = 0;
   n_c_root_1 = 0;

   for (int i=1; i <=m; i++) {
     if (fabs(roots[i].imag()) >= eps1) {
       flag_i = int_T;
     } else {
       flag_i = int_F;
     }
     if (flag_i == int_T) {
       c_root_1[n_c_root_1] = roots[i];
       n_c_root_1++;
     } else {
       r_root_1[n_r_root_1] = roots[i].real();
       n_r_root_1++;
     }
   }

//
//
//
//

   n_r_root = 0;

   for (int i=0; i < n_r_root_1; i++) {mark[i] = int_F;}

   for (int i=0; i < n_r_root_1; i++) {
     if (mark[i] == int_F) {
       r_root[n_r_root] = r_root_1[i];
       r_order[n_r_root] = 1;
       mark[i] = int_T;

       for (int j=i+1; j < n_r_root_1; j++) {
         if (compare_double_eq_1(r_root_1[i],r_root_1[j],eps2) == int_T) {
           r_order[n_r_root]++;
           mark[j] = int_T;
         }
       }
       n_r_root++;
     }
   }

   n_c_root = 0;

   for (int i=0; i < n_c_root_1; i++) {mark[i] = int_F;}

   for (int i=0; i < n_c_root_1; i++) {
     if (mark[i] == int_F) {

       root_real = c_root_1[i].real();
       root_imag = fabs(c_root_1[i].imag());

       c_root[n_c_root] = std::complex<double>(root_real,root_imag);
       c_order[n_c_root] = 1;
       mark[i] = int_T;

       for (int j=i+1; j < n_c_root_1; j++) {

         root_real_1 = c_root_1[j].real();
         root_real_2 = c_root_1[j].real();

         root_imag_1 = fabs(c_root_1[j].imag());
         root_imag_2 = fabs(c_root_1[j].imag());

         if (compare_double_eq_1(root_real_1,root_real_2,eps2) == int_T) {
           if (compare_double_eq_1(root_imag_1,root_imag_2,eps2) == int_T) {
             c_order[n_c_root]++;
             mark[j] = int_T;
           }
         }
       }
       c_order[n_c_root] = c_order[n_c_root]/2;
       n_c_root++;
     }
   }

   n_root_real_check = 0;

   for (int i=0; i < n_r_root; i++) {
     n_root_real_check = n_root_real_check + r_order[i];
   }
   if (n_root_real_check != n_r_root_1) {
     cout << "classify_roots: n_root_real_check and n_r_root_1" << endl;
     cout << "  are not equal!" << endl;
     cout << "  n_r_root_1=" << n_r_root_1 << endl;
     cout << "  n_root_real_check=" << n_root_real_check << endl;
     cout << "  Halting..." << endl;
     exit (1);
   }

   n_root_imag_check = 0;

   for (int i=0; i < n_c_root; i++) {
     n_root_imag_check = n_root_imag_check + 2*c_order[i];
   }
   if (n_root_imag_check != n_c_root_1) {
     cout << "classify_roots: n_root_imag_check and n_c_root_1" << endl;
     cout << "  are not equal!" << endl;
     cout << "  n_c_root_1=" << n_c_root_1 << endl;
     cout << "  n_root_imag_check=" << n_root_imag_check << endl;
     cout << "  Halting..." << endl;
     exit (1);
   }

   delete[] r_root_1;
   delete[] c_root_1;
   delete[] mark;

   return;
}
// -----------------------------------------------------------------------------
void n_by_d_1(
   const int degree_n,
   const int degree_d,
   double* coef_n,
   double* coef_d,
   int& degree_n_1,
   double* coef_n_1,
   double* coef_d_1,
   double& gain,
   double& a0) {

   double a_last=0.0,b_last=0.0;

   if (degree_n > degree_d) {
     cout << "n_by_d_1: degree_n > degree_d not allowed." << endl;
     cout << "  degree_n=" << degree_n << endl;
     cout << "  degree_d=" << degree_d << endl;
     cout << "  Halting..." << endl;
     exit (1);
   }
   if (coef_n[degree_n] == 0.0) {
     cout << "n_by_d_1: coef_n[degree_n] is zero! Halting..." << endl;
     exit (1);
   }
   if (coef_d[degree_d] == 0.0) {
     cout << "n_by_d_1: coef_d[degree_d] is zero! Halting..." << endl;
     exit (1);
   }

   if (degree_n < degree_d) {

     degree_n_1 = degree_n;
     gain = 1.0;
     a0 = 0.0;
     b_last = coef_d[degree_d];

     for (int i=0; i < degree_d; i++) {
       coef_d_1[i] = coef_d[i]/b_last;
     }
     coef_d_1[degree_d] = 1.0;

     for (int i=0; i <= degree_n; i++) {
       coef_n_1[i] = coef_n[i]/b_last;
     }
     return;
   }

   if (degree_n == degree_d) {

     a_last = coef_n[degree_n];
     b_last = coef_d[degree_d];

     degree_n_1 = degree_n - 1;
     gain = a_last/b_last;
     a0 = 1.0;

     for (int i=0; i < degree_d; i++) {
       coef_d_1[i] = coef_d[i]/b_last;
     }
     coef_d_1[degree_d] = 1.0;

     for (int i=0; i < degree_n; i++) {
       coef_n_1[i] = (coef_n[i]/a_last)-(coef_d[i]/b_last);
     }
     return;
   }

   return;
}
// -----------------------------------------------------------------------------
void initialise_coeff_1(
   const int max_degree,
   std::complex<double>* coef) {

   std::complex<double> zero_c;
   std::complex<double> one_c;

   zero_c = std::complex<double>(0.0,0.0);
   one_c = std::complex<double>(1.0,0.0);

   for (int i=1; i <= max_degree; i++) {
     coef[i] = zero_c;
   }
   coef[0] = one_c;

   return;
}
// -----------------------------------------------------------------------------
void compute_coeff_1(
   const int max_degree,
   std::complex<double>* coef,
   const std::complex<double> z1) {

// max_degree is the highest power of s expected in the
// output poly.

   std::complex<double>* coef_old;
   coef_old = new std::complex<double>[max_degree+1];

   for (int i=0; i <= max_degree; i++) {
     coef_old[i] = coef[i];
   }

   for (int i=1; i <= max_degree; i++) {
     coef[i] = coef_old[i]*(-z1) + coef_old[i-1];
   }
   coef[0] = coef[0]*(-z1);

   delete[] coef_old;

   return;
}
// -----------------------------------------------------------------------------
bool hasEnding(
   std::string const &fullString,
   std::string const &ending) {

// taken from
// https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c

   if (fullString.length() >= ending.length()) {
     return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
   } else {
     return false;
   }   
}
// -----------------------------------------------------------------------------
double calc_avg_1(
   std::queue<double> q_t,
   std::queue<double> q_x,
   const double t_current,
   const double x_current,
   const double T) {

   vector<double> t,x;
   int n_t,n_x;
   double delt,sum,x_interp,x_avg;
   double t_offset;

   n_t = q_t.size();
   n_x = q_x.size();

   if (n_t != n_x) {
     cout << "calc_avg_1: n_t: " << n_t << " n_x: " << n_x << endl;
     cout << "  are not equal. Halting..." << endl; exit(1);
   }
   if (n_t < 2) {
     cout << "calc_avg_1: n_t < 2? Halting..." << endl; exit(1);
   }

   sum = 0;

   t_offset = -t_current + T;

   while (!q_t.empty()){
     t.push_back(q_t.front() + t_offset);
     q_t.pop();
     x.push_back(q_x.front());
     q_x.pop();
   }
   t.push_back(T);
   x.push_back(x_current);

   for (int i=n_t; i > 0; i--) {
     if (t[i-1] >= 0.0) {
       delt = t[i] - t[i-1];
       sum += 0.5*delt*(x[i-1] + x[i]);
     } else{
       if (t[i] > 0.0) {
         delt = t[i];
         x_interp = x[i] - ((x[i]-x[i-1])/(t[i]-t[i-1]))*delt;
         sum += 0.5*delt*(x[i] + x_interp);
       } else {
         break;
       }
     }
   }

   x_avg = sum/T;
   return x_avg;
}
