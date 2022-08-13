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

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <complex>
#include <iomanip>
#include <math.h>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>
#include <filesystem>

#include "global.h"
#include "matrix_2.h"

#define int_F  0
#define int_T  1

using namespace std;
namespace fs = std::filesystem;

void replace_first(
   std::string& text,
   const std::string& from,
   const std::string& to);
int compare_double_eq_1(
   const double a1,
   const double a2,
   const double eps);
void assign_all_int_1(
   int* array1,
   const int n,
   const int k);
void assign_all_double_1(
   double* array1,
   const int n,
   const double k);
void assign_all_complex_1(
   std::complex<double>* array1,
   const int n,
   const std::complex<double> k);
int isSubstring(const string &s1,const string &s2);
vector<string> tokenize_string(
   const string &s,
   const string &del);
void next_line(
   std::fstream &file,
   const string &del,
   vector<string> &v1,
   bool &flag_eof);
void next_line(
   std::fstream &file,
   const string &del,
   vector<string> &v1);
void next_line(
   std::fstream &file,
   vector<string> &v1,
   bool &flag_eof);
void next_line(
   std::fstream &file,
   vector<string> &v1);
double next_double_1(
   std::fstream &file);
std::string next_string_1(
   std::fstream &file);
int count_lines_1(
   const std::string &filename);
int stoi_check(const std::string &s1);
double stod_check(const std::string &s1);
double stod_suffix(const std::string &s1);
void check_word_1(
   const vector<std::string> &v1,
   const int position,
   const std::string &word_given);
bool check_word_1a(
   const vector<std::string> &v1,
   const std::string &word_given);
void check_word_2(
   const vector<std::string> &v1,
   const int position_string,
   const std::string &word_given,
   const int &k0);
void check_word_3(
   const std::string &s1,
   const std::string &word_given);
bool check_word_4(
   const std::string &s1,
   const std::string &word_given);
std::string extract_string_1(
   const std::string &s1,
   char chr_left,
   char chr_right);
void extract_string_3(
   const std::string &s1,
   std::string &s2,
   std::string &s3,
   bool &flag_found,
   char chr_left,
   char chr_right);
void find_word_1(
   const vector<std::string> &v1,
   const std::string &word_given,
   int &position,
   bool &flag_found);
void find_word_2(
   const vector<std::string> &v1,
   const std::string &word_given,
   int &position);
void find_word_3(
   const vector<std::string> &v1,
   const std::string &word_given,
   const int &offset,
   const int &increment,
   int &position,
   bool &flag_found);
bool find_word_4(
   const vector<std::string> &v1,
   const std::string &word_given,
   int &position);
int assign_int_2(
   const vector<std::string> &v1,
   const int &position,
   const std::string &keyword);
int assign_int_2a(
   std::fstream &inf,
   const int &position,
   const std::string &keyword);
std::string assign_string_1(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   const std::string &keyword);
std::string assign_string_2(
   const vector<std::string> &v1,
   const int &position,
   const std::string &keyword);
std::string assign_string_3(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   int &position,
   const std::string &word_given);
bool assign_bool_3(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   const std::string &keyword,
   const bool &x_default);
void assign_bool_4(
   const vector<std::string> &v1,
   bool &x,
   const int &position,
   const std::string &keyword1,
   const std::string &keyword2,
   const bool &b1,
   const bool &b2);
void assign_vec_int_1(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   vector<int> &v2);
void assign_vec_double_1(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   vector<double> &v2);
void assign_vec_string_1(
   const vector<std::string> &v1,
   const int &offset,
   const int &increment,
   vector<string> &v2);
void extract_left_right_1(
   const std::string &x,
   const std::string &substring,
   std::string &x_left,
   std::string &x_right);
void split_vec_string_1(
   const vector<std::string> &v1,
   const std::string &keyword,
   vector<std::string> &v_left,
   vector<std::string> &v_right);
void assign_parms_int_2(
   const vector<std::string> &v1,
   const vector<std::string> &prm_name,
   vector<int> &prm_dest,
   vector<bool> &tick);
void assign_parms_double_2(
   const vector<std::string> &v1,
   const vector<std::string> &prm_name,
   vector<double> &prm_dest,
   vector<bool> &tick);
void assign_parms_string_2(
   const vector<std::string> &v1,
   const vector<std::string> &prm_name,
   vector<std::string> &prm_dest,
   vector<bool> &tick);
void assign_names_1(
   std::fstream &inf,
   const std::string &keyword,
   vector<std::string> &prm_name,
   int &nprm);
void assign_names_values_int_1(
   std::fstream &inf,
   const std::string &keyword,
   vector<std::string> &prm_name,
   vector<int> &prm_val,
   int &nprm);
void assign_names_values_double_1(
   std::fstream &inf,
   const std::string &keyword,
   vector<std::string> &prm_name,
   vector<double> &prm_val,
   int &nprm);
void assign_names_values_string_1(
   std::fstream &inf,
   const std::string &keyword,
   vector<std::string> &prm_name,
   vector<std::string> &prm_val,
   int &nprm);
void print_vec_double_1(
   std::fstream &outf,
   const vector<double> &v1,
   const int word_width);
void read_vec_double_1(
   std::fstream &inf,
   vector<double> &v1);
bool flag_nan(
   double &x);
void set_vector_1(
   double* x,
   const int n,
   const int i0);
void check_array_for_nan_2(
   const int n,
   double *x,
   bool &flag_nan_1);
double norm_2(
   const int n,
   double *x);
double norm_2_1(
   const int offset,
   const int n,
   double *x);
double norm_inf(
   const int n,
   double *x);
double norm_inf_1(
   const int offset,
   const int n,
   double *x);
double exp_lmt(
   const double x,
   const double x1);
void exp_lmt_1(
   const double x,
   const double x1,
   double &y,
   double &yp);
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
   const double delt_nrml);
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
   const double eps2);
void laguer(
   std::complex<double>* a,
   int m,
   std::complex<double>* x,
   int* its);
void zroots(
   std::complex<double>* a,
   const int m,
   std::complex<double>* roots,
   int polish);
void classify_roots(
   const int m,
   std::complex<double>* roots,
   int& n_r_root,
   int& n_c_root,
   int* r_order,
   int* c_order,
   double* r_root,
   std::complex<double>* c_root,
   const double eps2);
void n_by_d_1(
   const int degree_n,
   const int degree_d,
   double* coef_n,
   double* coef_d,
   int& degree_n_1,
   double* coef_n_1,
   double* coef_d_1,
   double& gain,
   double& a0);
void initialise_coeff_1(
   const int max_degree,
   std::complex<double>* coef);
void compute_coeff_1(
   const int max_degree,
   std::complex<double>* coef,
   const std::complex<double> z1);

bool hasEnding(
   std::string const &fullString,
   std::string const &ending);

double calc_avg(
   std::queue<double> q_t,
   std::queue<double> q_x,
   const double T);

double calc_avg_1(
   std::queue<double> q_t,
   std::queue<double> q_x,
   const double t_current,
   const double x_current,
   const double T);

template <typename T>
void print_vec_2(const vector<T> &v1) {
   for (unsigned int i=0; i < v1.size(); i++) {
     cout << " " << v1[i];
   }
   if (v1.size() > 0) cout << endl;
   return;
};

template <typename T>
void assign_const_1(vector<T> &v1,const T &a) {
   for (unsigned int i=0; i < v1.size(); i++) {
     v1[i] = a;
   }
   return;
};

template <typename T>
void assign_array_1(
   T *x,
   const int n,
   const T k) {

   for (int i = 0; i < n; i++) {
     x[i] = k;
   }
   return;
}

template <typename T>
void print_array_1(
   T *x,
   const int n,
   char* message) {

   cout << message << endl;

   for (int i=0; i < n; i++){
     cout << i << "  " << x[i] << endl;
   }
   return;
}

template <typename T>
void print_array_2(
   vector<T> x,
   const int n,
   char* message) {

   cout << message << endl;

   for (int i=0; i < n; i++){
     cout << i << "  " << x[i] << endl;
   }
   return;
}

template <typename T>
void assign_const_2d_1(
   const T &a,
   vector< vector<T> > &v1) {

   for (unsigned int i=0; i < v1.size(); i++) {
     for (unsigned int j=0; j < v1[i].size(); j++) {
       v1[i][j] = a;
     }
   }
   return;
};

template <typename T>
void copy_vec_1(const vector<T> &v_src,vector<T> &v_dest) {
   for (unsigned int i=0; i < v_src.size(); i++) {
     v_dest.push_back(v_src[i]);
   }
   return;
};

template <typename T>
void copy_array_1(const int n,T *x_src,T *x_dest) {
   for (int i=0; i < n; i++) {
     x_dest[i] = x_src[i];
   }
   return;
};

template <typename T>
void add_arrays_1(const int n,T *x_src,T *x_dest) {
// x_dest <- x_dest + x_src
   for (int i=0; i < n; i++) {
     x_dest[i] += x_src[i];
   }
   return;
};

template <typename T>
void add_arrays_2(const int n,T *x,T *y,T *z) {
// z <- x + y
   for (int i=0; i < n; i++) {
     z[i] = x[i] + y[i];
   }
   return;
};

template <typename T>
void diff_arrays_1(const int n,T *x1,T *x2,T *delx) {
// delx = x2 - x1 (on all elements)
   for (int i=0; i < n; i++) {
     delx[i] = x2[i] - x1[i];
   }
   return;
};

template <typename T>
void mult_array_1(const int n,T *x,const T k) {
// x <- k*x
   for (int i=0; i < n; i++) {
     x[i] = k*x[i];
   }
   return;
};

template <typename T>
vector<T> get_lib_elements(
   Global &global,
   const std::string &folder,
   const std::string &extension) {
// use this function to read/save info about library elements (XbeLib, etc)
   T x;
   vector<T> xvec;
   int n;
   std::string element_filename;
   std::fstream inf;
   vector<std::string> v1;

   const string filename = "";

   for (const auto &entry : fs::directory_iterator(folder)) {
     const auto &path = entry.path();
     if (path.extension() == extension) {
       x = T(path.string(), global);
       xvec.push_back(x);
     }
   }

   return xvec;
};

template <typename T>
int find_name(
   const vector<T> &xvec,
   const std::string &word_given) {

   int n,position;
   bool flag_found = false;
   n = xvec.size();

   for (int i=0; i < n; i++) {
     if (xvec[i].name == word_given) {
       position = i; flag_found = true; break;
     }
   }
   if (!flag_found) {
     cout << "find_name: <" << word_given << "> was not found. Halting..." << endl;
     exit(1);
   }
   return position;
};

template <typename T>
bool check_vec_const_1(
   const vector<T> &xvec,
   const T &a0) {

// check if each element of xvec is equal to a0;
// if not, make flag_ok = false

   bool flag_ok;
   int n;

   flag_ok = true;
   n = xvec.size();

   for (int i=0; i < n; i++) {
     if (xvec[i] != a0) {
       cout << "check_vec_const_1: xvec[" << i
         << "] does not match " << a0 << endl;
       flag_ok = false; break;
     }
   }
   return flag_ok;
};

template <typename T>
void check_vec_const_2(
   const vector<T> &xvec,
   const T &a0,
   bool &flag_found,
   int &pos) {

// check if any element of xvec is equal to a0;
// return true in that case.

   int n;

   flag_found = false;
   n = xvec.size();

   for (int i=0; i < n; i++) {
     if (xvec[i] == a0) {
       flag_found = true; pos = i; break;
     }
   }
   return;
};

template <typename T>
void print_queue(std::queue<T> q) {

   cout << "print_queue:" << endl;
   while (!q.empty()){
     cout << q.front() << endl;
     q.pop();
   }   
}

#endif
