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

#include <filesystem>

#include "solveblocks.h"

using namespace std;

SolveBlocks::SolveBlocks(){}
// -----------------------------------------------------------------------------
void SolveBlocks::set_values_1(
   fs::path &element_dir,
   const std::string &filename,
   Circuit &cct,
   Global &global,
   CctFile &cct_file) {

// filename: name of the circuit file

   std::fstream inf;
   vector<std::string> v1; 
   std::string s1,s2,s3,s4,s5,s6;
   int i_line,pos;
   double t_default = -1.0e3;
   int count;
   double delt;
   bool flag_1;

// need to clear vectors for each solve block:
   limit_lines.clear();
   out_nvar.clear();;
   outf_name.clear();
   flag_append.clear();

   flag_out_delt_fixed.clear();
   out_delt.clear();
   out_tstart.clear();
   out_tend.clear();
   out_tnext.clear();

   out_var.clear();

   flag_t_start = false;
   flag_t_end = false;

   flag_delt0_x = false;
   flag_delt_min_x = false;
   flag_delt_max_x = false;

   flag_delt0_e = false;
   flag_delt_min_e = false;
   flag_delt_max_e = false;

   flag_delt0_ex = false;
   flag_delt_min_ex = false;
   flag_delt_max_ex = false;

   flag_ssw_period = false;
   flag_ssw_frequency = false;
   flag_ssw_final_trns = false;

   flag_const_tstep_ex = false;

// assign default values to method parameters and trns constants

   parms.clear();
   method_default(element_dir, global);

// fixed constants (which do not depend on time step):
   trns_constants_1();

// first, get n_outfile, resize vectors and then use a separate loop
// to assign values.

   inf.open(filename,ios::in|ios::binary);
   i_line = -1;
   while (i_line < cct_file.line_end_solve[index_solve]) {
     next_line(inf,v1); i_line++;
     if (i_line > cct_file.line_begin_solve[index_solve]) {
       check_word_1(v1,0,"solve_type");
       next_line(inf,v1); i_line++;
       check_word_1(v1,0,"initial_sol");

       while (i_line < cct_file.line_end_solve[index_solve]) {
         next_line(inf,v1); i_line++;
         if (v1[0] == "begin_output") {
           n_outfile++;
         }
       }
     }
   }
   inf.close();

   flag_solution.resize(n_outfile);
   flag_out_delt_fixed.resize(n_outfile);
   limit_lines.resize(n_outfile);
   flag_append.resize(n_outfile);
   out_nvar.resize(n_outfile);
   out_delt.resize(n_outfile);
   out_tstart.resize(n_outfile);
   out_tend.resize(n_outfile);
   out_tnext.resize(n_outfile);

   f_output.resize(n_outfile);
   total_lines.resize(n_outfile);

   out_var.resize(n_outfile);
   outvar_temp.resize(n_outfile);

// assign default values:
   assign_const_1<bool>(flag_solution,false);
   assign_const_1<bool>(flag_out_delt_fixed,false);
   assign_const_1<double>(out_delt,0.0);
   assign_const_1<double>(out_tstart,0.0);
   assign_const_1<double>(out_tend,t_default);
   assign_const_1<double>(out_tnext,0.0);
   assign_const_1<int>(limit_lines,global.I_LINES_LMT);
   assign_const_1<bool>(flag_append,false);

// now do the assignments:

   inf.open(filename,ios::in|ios::binary);
   i_line = -1;
   n_outfile = 0;
   flag_algo_e = -1;
   flag_algo_x = -1;

   fs::path output_dir = fs::path(filename).parent_path();

   while (i_line < cct_file.line_end_solve[index_solve]) {
     next_line(inf,v1); i_line++;
     if (i_line > cct_file.line_begin_solve[index_solve]) {
//     solve_type=xx
       s1 = assign_string_2(v1,0,"solve_type");
       if (s1 == "startup") {
         solve_type =  global.I_STARTUP;
         flag_startup = true;
       } else if (s1 == "trns") {
         solve_type = global.I_TRNS;
         flag_trns = true;
       } else if (s1 == "dc") {
         solve_type = global.I_DC;
         flag_dc = true;
       } else if (s1 == "ssw") {
         solve_type = global.I_SSW;
         flag_ssw = true;
       } else {
         cout << "SolveBlocks:set_values_1: check solve_type in" << endl;
         cout << "  solve block no. " << index_solve << endl;
         cout << "  Halting..." << endl; exit(1);
       }
//     initial_sol file filename=xx or previous or initialize
       next_line(inf,v1); i_line++;
       s1 = assign_string_2(v1,0,"initial_sol");
       if (s1 == "read_from_file") {
         flag_read_solution = true;
       } else if (s1 == "initialize") {
         flag_init_solution = true;
       } else if (s1 == "previous") {
         flag_prev_solution = true;
       } else {
         cout << "SolveBlocks:set_values_1: check initial_sol in" << endl;
         cout << "  solve block no. " << index_solve << endl;
         cout << "  Halting..." << endl; exit(1);
       }
       while (i_line < cct_file.line_end_solve[index_solve]) {
         next_line(inf,v1); i_line++;
         s1 = v1[0];
         if (s1 == "method:") {
           for (unsigned int i=1; i < (v1.size()-1); i=i+2) {
             s2 = v1[i]; s3 = v1[i+1];
             assign_parm(s2,s3,global);
           }
         } else if (s1 == "initial_sol_file") {
           infile_sol = v1[1];
         } else if (s1 == "begin_output") {
           s4 = "dummy";
           while (s4 != "end_output") {
             next_line(inf,v1); i_line++;
             s4 = v1[0];
             if (s4 == "filename") {
               // Write output files to same directory as input files
               outf_name.push_back(output_dir / v1[1]);
               for (unsigned int i=2; i < (v1.size()-1); i=i+2) {
                 s5 = v1[i]; s6 = v1[i+1];

                 if (s5 == "limit_lines") {
                   limit_lines[n_outfile] = stoi(s6);
                 } else if (s5 == "append") {
                   flag_append[n_outfile] = (s6 == "yes");
                 } else {
                   cout << "SolveBlocks:set_values_1: unknown keyword (2) <"
                     << s5 << ">" << endl;
                   cout << "  in solve block no. " << index_solve << endl;
                   cout << "  Halting..." << endl; exit(1);
                 }
               }
             } else if (s4 == "variables:") {
               if (v1.size() == 1) {
                 cout << "SolveBlocks:set_values_1:" << endl;
                 cout << "  check variables: statement" << endl;
                 cout << "  in solve block no. " << index_solve << endl;
                 cout << "  Halting..." << endl; exit(1);
               }
               for (unsigned int i=1; i < v1.size(); i++) {
                 s5 = v1[i];
                 if (s5 == "solution") {
                   if (flag_write_solution) {
                     cout << "SolveBlocks:set_values_1:" << endl;
                     cout << "  only one write sol allowed." << endl;
                     cout << "  Check solve block no. " << index_solve << endl;
                     cout << "  Halting..." << endl; exit(1);
                   }
                   flag_solution[n_outfile] = true;
                   out_nvar[n_outfile] = 0;
                   flag_write_solution = true;
                   index_file_solution = n_outfile;
                 } else {
                   find_word_2(cct_file.ov_name,s5,pos);
                   out_var[n_outfile].push_back(pos);
                   out_nvar[n_outfile]++;
                 }
               }
             } else if (s4 == "control:") {
               for (unsigned int i=1; i < (v1.size()-1); i=i+2) {
                 s5 = v1[i]; s6 = v1[i+1];
                 if (s5 == "fixed_interval") {
                   flag_out_delt_fixed[n_outfile] = true;
                   out_delt[n_outfile] = stod_suffix(s6);
                 } else if (s5 == "out_tstart") {
                   out_tstart[n_outfile] = stod_suffix(s6);
                 } else if (s5 == "out_tend") {
                   out_tend[n_outfile] = stod_suffix(s6);
                 } else {
                   cout << "SolveBlocks:set_values_1: unknown keyword (3) <"
                     << s5 << ">" << endl;
                   cout << "  in solve block no. " << index_solve << endl;
                   cout << "  Halting..." << endl; exit(1);
                 }
               }
             } else {
               if (s4 != "end_output") {
                 cout << "SolveBlocks:set_values_1: unknown keyword (4) <"
                   << s4 << ">" << endl;
                 cout << "  in solve block no. " << index_solve << endl;
                 cout << "  Halting..." << endl; exit(1);
               }
             }
           }
           n_outfile++;
         } else {
           if (s1 != "end_solve") {
             cout << "SolveBlocks:set_values_1: unknown keyword (5) <"
               << s1 << ">" << endl;
             cout << "  in solve block no. " << index_solve << endl;
             cout << "  Halting..." << endl; exit(1);
           }
         }
       }
     }
   }
   inf.close();

   if (flag_read_solution) {
     if (infile_sol.empty()) {
       cout << "SolveBlocks:set_values_1: infile_sol is empty." << endl;
       cout << "  Halting..." << endl; exit(1);
     }
     if (!hasEnding(infile_sol, ".gsol")) {
       cout << "SolveBlocks:set_values_1: infile_sol must end in .gsol." << endl;
       cout << "  Halting..." << endl; exit(1);
     }
   }

   global.flags[global.i_slv_init    ] = flag_init_solution;
   global.flags[global.i_slv_readfile] = flag_read_solution;
   global.flags[global.i_slv_previous] = flag_prev_solution;

   count = 0;
   if (flag_dc     ) count++;
   if (flag_trns   ) count++;
   if (flag_startup) count++;
   if (flag_ssw    ) count++;
   if (count != 1) {
     cout << "SolveBlocks:set_values_1: only one solve type" << endl;
     cout << "  is allowed. Halting..." << endl; exit(1);
   }

   if (flag_trns) {
     if (cct.flag_e_only) {
       flag_const_tstep_e =
         (flag_algo_e == global.i_be_const ) ||
         (flag_algo_e == global.i_trz_const);
     } else if (cct.flag_x_only) {
       flag_const_tstep_x =
         (flag_algo_x == global.i_be_const ) ||
         (flag_algo_x == global.i_trz_const);
     } else {
       flag_const_tstep_ex =
         (flag_algo_ex == global.i_be_const ) ||
         (flag_algo_ex == global.i_trz_const);
     }
   }

   if (cct.flag_x_only) {
     if (flag_dc) {
       cout << "SolveBlocks:set_values_1:" << endl;
       cout << "  x elements not allowed with dc" << endl;
       cout << "  in solve block no. " << index_solve << endl;
       cout << "  Halting..." << endl; exit(1);
     }
     if (flag_trns) {
       if (flag_algo_x == -1) {
         cout << "SolveBlocks:set_values_1: algorithm_x not specified" << endl;
         cout << "  in solve block no. " << index_solve << endl;
         cout << "  Halting..." << endl; exit(1);
       }
       cct.flag_x_explicit = global.flag_exp[flag_algo_x];
     }
     if (flag_startup) {
       if (cct.flag_alg_loop) {
         if (x_algo_startup_exp) {
           cout << "SolveBlocks:set_values_1: explicit method for" << endl;
           cout << "  startup cannot be used if there are algebraic loops." << endl;
           cout << "  Check solve block no. " << index_solve << endl;
           cout << "  Halting..." << endl; exit(1);
         }
       }
       cct.flag_x_explicit = x_algo_startup_exp;
     }
   }
   if (cct.flag_e_only) {
     if (flag_trns) {
       if (flag_algo_e == -1) {
         cout << "SolveBlocks:set_values_1:" << endl;
         cout << "  algorithm_e not specified" << endl;
         cout << "  in solve block no. " << index_solve << endl;
         cout << "  Halting..." << endl; exit(1);
       }
       if (global.flag_exp[flag_algo_e]) {
         cout << "SolveBlocks:set_values_1:" << endl;
         cout << "  explict method not allowed for e elements" << endl;
         cout << "  in solve block no. " << index_solve << endl;
         cout << "  Halting..." << endl; exit(1);
       }
     }
   }

   if (cct.flag_x) {
     cct.flag_x_matrix = cct.flag_alg_loop || !cct.flag_x_explicit;
   }

// write out all flags in each case; it is easier to read.

   cct.flag_exc     = false;
   cct.flag_exs     = false;
   cct.flag_exs_fex = false;
   cct.flag_exs_fe  = false;
   cct.flag_exs_fx  = false;

   if (cct.flag_x_e) flag_sync_x_e = true;

   if (flag_trns) {
     if (cct.flag_x_e) {
       cct.flag_exc = true;
     }
   } else if (flag_ssw) {
     if (cct.flag_x_e) {
       cct.flag_exc = true;

     }
   } else if (flag_startup) {
     if (cct.flag_x_e) {
       if (cct.flag_x_matrix) {
         cct.flag_exc = true;
       }
     }
   }

   if (flag_trns) {
     if (cct.flag_x_e) {
       count = 0;
       if (cct.flag_exc    ) count++;
       if (cct.flag_exs_fex) count++;
       if (cct.flag_exs_fe ) count++;
       if (cct.flag_exs_fx ) count++;
       if (count != 1) {
         cout << "SolveBlocks:set_values_1: only one of" << endl;
         cout << "  flag_exc/flag_exs_fex/flag_exs_fe/flag_exs_fx" << endl;
         cout << "  can be true. Halting..." << endl;
         cout << "  cct.flag_exc = " << cct.flag_exc << endl;
         cout << "  cct.flag_exs = " << cct.flag_exs << endl;
         cout << "  cct.flag_exs_fex = " << cct.flag_exs_fex << endl;
         cout << "  cct.flag_exs_fe = " << cct.flag_exs_fe << endl;
         cout << "  cct.flag_exs_fx = " << cct.flag_exs_fx << endl;
         exit(1);
       }
     }
     if (!flag_t_start) {
       cout << "SolveBlocks:set_values_1: t_start not specified" << endl;
       cout << "  in solve block no. " << index_solve << endl;
       cout << "  Halting..." << endl; exit(1);
     }
     if (!flag_t_end) {
       cout << "SolveBlocks:set_values_1: t_end not specified" << endl;
       cout << "  in solve block no. " << index_solve << endl;
       cout << "  Halting..." << endl; exit(1);
     }
     if (cct.flag_x) {
       if (cct.flag_exc) {
         if (!flag_delt0_ex) {
           cout << "SolveBlocks:set_values_1: delt0_ex not specified" << endl;
           cout << "  in solve block no. " << index_solve << endl;
           cout << "  Halting..." << endl; exit(1);
         }
         if (flag_delt_min_ex) {
           if (delt0_ex < delt_min_ex) {
             cout << "SolveBlocks:set_values_1: delt0_ex < delt_min_ex" << endl;
             cout << "  in solve block no. " << index_solve << endl;
             cout << "  Halting..." << endl; exit(1);
           }
         } else {
           delt_min_ex = 0.01*delt0_ex;
         }
         if (!flag_delt_max_ex) {
           delt_max_ex = 10.0*delt0_ex;
         }
       } else {
         if (!flag_delt0_x) {
           cout << "SolveBlocks:set_values_1: delt0_x not specified" << endl;
           cout << "  in solve block no. " << index_solve << endl;
           cout << "  Halting..." << endl; exit(1);
         }
         if (flag_delt_min_x) {
           if (delt0_x < delt_min_x) {
             cout << "SolveBlocks:set_values_1: delt0_x < delt_min_x" << endl;
             cout << "  in solve block no. " << index_solve << endl;
             cout << "  Halting..." << endl; exit(1);
           }
         } else {
           delt_min_x = 0.01*delt0_x;
         }
         if (!flag_delt_max_x) {
           delt_max_x = 10.0*delt0_x;
         }
         if (flag_algo_x == -1) {
           cout << "SolveBlocks:set_values_1: algorithm_x not specified" << endl;
           cout << "  in solve block no. " << index_solve << endl;
           cout << "  Halting..." << endl; exit(1);
         }
         cct.flag_x_explicit = global.flag_exp[flag_algo_x];
       }
     }
     if (cct.flag_e) {
       if (cct.flag_exc) {
         cout << "SolveBlocks:set_values_1: delt0_ex = " << delt0_ex << endl;
         if (!flag_delt0_ex) {
           cout << "SolveBlocks:set_values_1: delt0_ex not specified" << endl;
           cout << "  in solve block no. " << index_solve << endl;
           cout << "  Halting..." << endl; exit(1);
         }
         if (flag_delt_min_ex) {
           if (delt0_ex < delt_min_ex) {
             cout << "SolveBlocks:set_values_1: delt0_ex < delt_min_ex" << endl;
             cout << "  in solve block no. " << index_solve << endl;
             cout << "  Halting..." << endl; exit(1);
           }
         } else {
           delt_min_ex = 0.01*delt0_ex;
         }
         if (!flag_delt_max_ex) {
           delt_max_ex = 10.0*delt0_ex;
         }
         cout << "SolveBlocks:set_values_1: delt_min_ex = " << delt_min_ex << endl;
       } else {
         if (!flag_delt0_e) {
           cout << "SolveBlocks:set_values_1: delt0_e not specified" << endl;
           cout << "  in solve block no. " << index_solve << endl;
           cout << "  Halting..." << endl; exit(1);
         }
         if (flag_delt_min_e) {
           if (delt0_e < delt_min_e) {
             cout << "SolveBlocks:set_values_1: delt0_e < delt_min_e" << endl;
             cout << "  in solve block no. " << index_solve << endl;
             cout << "  Halting..." << endl; exit(1);
           }
         } else {
           delt_min_e = 0.01*delt0_e;
         }
         if (!flag_delt_max_e) {
           delt_max_e = 10.0*delt0_e;
         }
       }
     }
     if (cct.flag_exs_fex) {
       if (delt0_e != delt0_x) {
         cout << "SolveBlocks:set_values_1:" << endl;
         cout << "  delt0_e and delt0_x must be equal in exs_fex" << endl;
         cout << "  Check solve block no. " << index_solve << endl;
         cout << "  Halting..." << endl; exit(1);
       }
     }
     if (cct.flag_exs_fe) {
       if (delt_max_x > 0.501*delt0_e) {
         cout << "SolveBlocks:set_values_1:" << endl;
         cout << "  delt_max_x > 0.5*delt0_e not allowed." << endl;
         cout << "  Check solve block no. " << index_solve << endl;
         cout << "  Halting..." << endl; exit(1);
       }
     }
     if (cct.flag_exs_fx) {
       if (delt_max_e > 0.501*delt0_x) {
         cout << "SolveBlocks:set_values_1:" << endl;
         cout << "  delt_max_e > 0.5*delt0_x not allowed." << endl;
         cout << "  delt_max_e = " << delt_max_e << endl;
         cout << "  delt0_x = " << delt0_x << endl;
         cout << "  Check solve block no. " << index_solve << endl;
         cout << "  Halting..." << endl; exit(1);
       }
     }
     for (int i=0; i < n_outfile; i++) {
       if (out_tstart[i] == t_default) {
         out_tstart[i] = global.time_begin;
       }
       if (out_tend[i] == t_default) {
         out_tend[i] = global.time_end + delt_small;
       }
     }
   }

   if (flag_ssw) {
     if (!ssw_nr_flag_check_rhs2) {
       cout << "SolveBlocks:set_values_1: ssw_nr_check_rhs2 must be true. Halting..." << endl;
       exit(1);
     }
     if (cct.flag_e_only) {
       flag_1 = 
         (flag_algo_e == global.i_be       ) ||
         (flag_algo_e == global.i_be_auto  ) ||
         (flag_algo_e == global.i_be_const ) ||
         (flag_algo_e == global.i_trz      ) ||
         (flag_algo_e == global.i_trz_auto ) ||
         (flag_algo_e == global.i_trz_const);
     } else if (cct.flag_x_only) {
       flag_1 = 
         (flag_algo_x == global.i_be       ) ||
         (flag_algo_x == global.i_be_auto  ) ||
         (flag_algo_x == global.i_be_const ) ||
         (flag_algo_x == global.i_trz      ) ||
         (flag_algo_x == global.i_trz_auto ) ||
         (flag_algo_x == global.i_trz_const);
     } else {
       flag_1 = 
         (flag_algo_ex == global.i_be       ) ||
         (flag_algo_ex == global.i_be_auto  ) ||
         (flag_algo_ex == global.i_be_const ) ||
         (flag_algo_ex == global.i_trz      ) ||
         (flag_algo_ex == global.i_trz_auto ) ||
         (flag_algo_ex == global.i_trz_const);
     }
     if (!flag_1) {
       cout << "SolveBlocks:set_values_1:" << endl;
       cout << "  For SSW, one of be/be_auto/be_const/trz/trz_auto/trz_const must be selected." << endl;
       cout << "  Halting..." << endl; exit(1);
     }
     if (flag_ssw_period && flag_ssw_frequency) {
       cout << "SolveBlocks:set_values_1:" << endl;
       cout << "  Both SSW period and SSW frequency cannot be specified." << endl;
       cout << "  Halting..." << endl; exit(1);
     } else if (flag_ssw_period) {
       delt = ssw_period/(double)(ssw_ndiv);
       if (cct.flag_e_only) {
         if (!flag_delt0_e) {
           delt0_e = delt;
           flag_delt0_e = true;
         }
       } else if (cct.flag_x_only) {
         if (!flag_delt0_x) {
           delt0_x = delt;
           flag_delt0_x = true;
         }
       } else {
         if (!flag_delt0_ex) {
           delt0_ex = delt;
           flag_delt0_ex = true;
         }
       }
     } else if (flag_ssw_frequency) {
       ssw_period = 1.0/ssw_frequency;

       delt = ssw_period/(double)(ssw_ndiv);
       if (cct.flag_e_only) {
         if (!flag_delt0_e) {
           delt0_e = delt;
           flag_delt0_e = true;
         }
       } else if (cct.flag_x_only) {
         if (!flag_delt0_x) {
           delt0_x = delt;
           flag_delt0_x = true;
         }
       } else {
         if (!flag_delt0_ex) {
           delt0_ex = delt;
           flag_delt0_ex = true;
         }
       }
     } else {
       cout << "SolveBlocks:set_values_1:" << endl;
       cout << "  Either SSW period or SSW frequency must be specified." << endl;
       cout << "  Halting..." << endl; exit(1);
     }
     ssw_period_1 = ssw_period*(double)ssw_period_mult;
     global.time_end = global.time_begin + ssw_period_1;

     flag_t_end = true;

     if (cct.flag_e_only) {
       if (flag_delt_min_e) {
         if (delt0_e < delt_min_e) {
           cout << "SolveBlocks:set_values_1: delt0_e < delt_min_e" << endl;
           cout << "  in solve block no. " << index_solve << endl;
           cout << "  Halting..." << endl; exit(1);
         }
       } else {
         delt_min_e = 0.01*delt0_e;
       }
       if (!flag_delt_max_e) {
         delt_max_e = 10.0*delt0_e;
       }
     } else if (cct.flag_x_only) {
       if (flag_delt_min_x) {
         if (delt0_x < delt_min_x) {
           cout << "SolveBlocks:set_values_1: delt0_x < delt_min_x" << endl;
           cout << "  in solve block no. " << index_solve << endl;
           cout << "  Halting..." << endl; exit(1);
         }
       } else {
         delt_min_x = 0.01*delt0_x;
       }
       if (!flag_delt_max_x) {
         delt_max_x = 10.0*delt0_x;
       }
     } else {
       if (flag_delt_min_ex) {
         if (delt0_ex < delt_min_ex) {
           cout << "SolveBlocks:set_values_1: delt0_ex < delt_min_ex" << endl;
           cout << "  in solve block no. " << index_solve << endl;
           cout << "  Halting..." << endl; exit(1);
         }
       } else {
         delt_min_ex = 0.01*delt0_ex;
       }
       if (!flag_delt_max_ex) {
         delt_max_ex = 10.0*delt0_ex;
       }
     }

     for (int i=0; i < n_outfile; i++) {
       if (out_tstart[i] == t_default) {
         out_tstart[i] = global.time_begin;
       }
       if (out_tend[i] == t_default) {

         out_tend[i] = global.time_end + delt_small;
       }
     }
   }

   for (int i=0; i < n_outfile; i++) {
     outvar_temp[i].resize(out_nvar[i]);
   }

// assign additional bool flags for convenience:

   x_algo_feuler    = (flag_algo_x == global.i_feuler   );
   x_algo_rk4       = (flag_algo_x == global.i_rk4      );
   x_algo_rkf45     = (flag_algo_x == global.i_rkf45    );
   x_algo_bs23      = (flag_algo_x == global.i_bs23     );
   x_algo_meuler    = (flag_algo_x == global.i_meuler   );
   x_algo_heun      = (flag_algo_x == global.i_heun     );
   x_algo_be        = (flag_algo_x == global.i_be       );
   x_algo_be_auto   = (flag_algo_x == global.i_be_auto  );
   x_algo_be_const  = (flag_algo_x == global.i_be_const );
   x_algo_trz       = (flag_algo_x == global.i_trz      );
   x_algo_trz_auto  = (flag_algo_x == global.i_trz_auto );
   x_algo_trz_const = (flag_algo_x == global.i_trz_const);
   x_algo_trbdf2    = (flag_algo_x == global.i_trbdf2   );

   e_algo_be        = (flag_algo_e == global.i_be       );
   e_algo_be_auto   = (flag_algo_e == global.i_be_auto  );
   e_algo_be_const  = (flag_algo_e == global.i_be_const );
   e_algo_trz       = (flag_algo_e == global.i_trz      );
   e_algo_trz_auto  = (flag_algo_e == global.i_trz_auto );
   e_algo_trz_const = (flag_algo_e == global.i_trz_const);
   e_algo_trbdf2    = (flag_algo_e == global.i_trbdf2   );

   ex_algo_be        = (flag_algo_ex == global.i_be       );
   ex_algo_be_auto   = (flag_algo_ex == global.i_be_auto  );
   ex_algo_be_const  = (flag_algo_ex == global.i_be_const );
   ex_algo_trz       = (flag_algo_ex == global.i_trz      );
   ex_algo_trz_auto  = (flag_algo_ex == global.i_trz_auto );
   ex_algo_trz_const = (flag_algo_ex == global.i_trz_const);
   ex_algo_trbdf2    = (flag_algo_ex == global.i_trbdf2   );

// initialise flags related to failure of convergence criteria:
   flags_failed_default();

   return;
} //end of SolveBlocks::set_values_1
// -----------------------------------------------------------------------------
void SolveBlocks::flags_failed_default() {

   flag_e_nr_eps_rhs_failed = false;
   flag_e_nr_eps_volt_failed = false;
   flag_e_nr_spice_nodev_failed = false;
   flag_e_nr_spice_nodecur_failed = false;
   flag_e_nr_eps_delx_all_failed = false;

   flag_x_nr_eps_rhs_failed = false;
   flag_x_nr_eps_delx_all_failed = false;

   flag_ex_nr_eps_rhs_failed = false;
   flag_ex_nr_eps_delx_all_failed = false;

   flag_ssw_nr_eps_rhs_failed = false;

} //end of SolveBlocks::flags_failed_default
// -----------------------------------------------------------------------------
void SolveBlocks::write_flags_failed() {

// write to console which criteria failed:

   if (flag_e_nr_eps_rhs_failed) {
     cout << "e_nr_eps_rhs condition not satisfied" << endl;
   }
   if (flag_e_nr_eps_volt_failed) {
     cout << "e_nr_eps_volt condition not satisfied" << endl;
   }
   if (flag_e_nr_spice_nodev_failed) {
     cout << "e_nr_spice_nodev condition not satisfied" << endl;
   }
   if (flag_e_nr_spice_nodecur_failed) {
     cout << "e_nr_spice_nodecur condition not satisfied" << endl;
   }
   if (flag_e_nr_eps_delx_all_failed) {
     cout << "e_nr_eps_delx_all condition not satisfied" << endl;
   }
   if (flag_x_nr_eps_rhs_failed) {
     cout << "x_nr_eps_rhs condition not satisfied" << endl;
   }
   if (flag_x_nr_eps_delx_all_failed) {
     cout << "x_nr_eps_delx_all condition not satisfied" << endl;
   }
   if (flag_ex_nr_eps_rhs_failed) {
     cout << "ex_nr_eps_rhs condition not satisfied" << endl;
   }
   if (flag_ex_nr_eps_delx_all_failed) {
     cout << "ex_nr_eps_delx_all condition not satisfied" << endl;
   }
   if (flag_ssw_nr_eps_rhs_failed) {
     cout << "ssw_nr_eps_rhs condition not satisfied" << endl;
   }

} //end of SolveBlocks::write_flags_failed
// -----------------------------------------------------------------------------
void SolveBlocks::method_default(
   fs::path &element_dir,
   Global &global) {

   string filename;
   std::fstream inf;
   int n_lines,n_parms;
   vector<std::string> v1; 
   std::string s2,s3;
   SolveParm parm0;
   bool flag_1;
   std::string homedir;

   flag_dc = false;
   flag_trns = false;
   flag_startup = false;
   flag_ssw = false;
   solve_type = -1;

   delt_min_x  = 0.0;
   delt_max_x  = 0.0;
   delt_min_e  = 0.0;
   delt_max_e  = 0.0;
   delt_min_ex = 0.0;
   delt_max_ex = 0.0;

   filename = element_dir / "slvparms.in";

   flag_fixed_delt_x = false;
   flag_fixed_delt_e = false;
   flag_debug_gauss1 = false;
   flag_debug_gauss2 = false;

   e_nr_iter_debug = 200;
   x_nr_iter_debug = 200;
   ex_nr_iter_debug = 200;
   ssw_nr_iter_debug = 200;

   e_trns_iter_debug = 0;
   x_trns_iter_debug = 0;
   ex_trns_iter_debug = 0;

   e_nr_flag_check_rhs2 = false;
   x_nr_flag_check_rhs2 = false;
   ex_nr_flag_check_rhs2 = false;

   e_nr_flag_check_delx_volt = false;
   e_nr_flag_write_delx_volt = false;

   e_nr_eps_rhs = 1.0e-6;
   x_nr_eps_rhs = 1.0e-6;
   ex_nr_eps_rhs = 1.0e-6;
   e_nr_eps_volt = 1.0e-4;

   if (index_solve == 0) {
     inf.open(filename,ios::in|ios::binary);
     if (!inf.is_open()) {
       cout << "SolveBlocks::method_default: " << endl;
       cout << "  file " << filename << " could not be opened." << endl;
       cout << "  Halting..." << endl; exit(1);
     }

     parms.clear();
     n_lines = count_lines_1(filename);

     next_line(inf,v1);
     if (v1[0] != "begin_file") {
       cout << "SolveBlocks::method_default: " << endl;
       cout << "  file " << filename << " must start with begin_file." << endl;
       cout << "  Halting..." << endl; exit(1);
       cout << "  input vector is: " << endl;
       print_vec_2<string>(v1); exit(1);
     }

     for (int i_line=1; i_line < n_lines; i_line++) {
       next_line(inf,v1);

       if (v1[0] == "end_file") {
         break;
       }
       parm0.options.clear();
       if (v1[0] != "begin_parm") {
         cout << "SolveBlocks::method_default: " << endl;
         cout << "  expect <begin_parm> in file " << filename << "." << endl;
         cout << "  Halting..." << endl; exit(1);
         cout << "  input vector is: " << endl;
         print_vec_2<string>(v1); exit(1);
       }
       next_line(inf,v1);
       if (v1[0] != "keyword:") {
         cout << "SolveBlocks::method_default: " << endl;
         cout << "  expect <keyword:> in file " << filename << "." << endl;
         cout << "  Halting..." << endl; exit(1);
         cout << "  input vector is: " << endl;
         print_vec_2<string>(v1); exit(1);
       }
       parm0.keyword = v1[1];

       next_line(inf,v1);
       if (v1[0] != "options:") {
         cout << "SolveBlocks::method_default: " << endl;
         cout << "  expect <options:> in file " << filename << "." << endl;
         cout << "  Halting..." << endl; exit(1);
         cout << "  input vector is: " << endl;
         print_vec_2<string>(v1); exit(1);
       }
       parm0.n_options = v1.size()-1;
       if (parm0.n_options == 1) {
         if (v1[1] != "none") {
           cout << "SolveBlocks::method_default: " << endl;
           cout << "  expect <none> in file " << filename << "." << endl;
           cout << "  Halting..." << endl; exit(1);
           cout << "  input vector is: " << endl;
           print_vec_2<string>(v1); exit(1);
         }
       }
       for (int i=0; i < parm0.n_options; i++) {
         parm0.options.push_back(v1[i+1]);
       }

       next_line(inf,v1);
       if (v1[0] != "default:") {
         cout << "SolveBlocks::method_default: " << endl;
         cout << "  expect <default:> in file " << filename << "." << endl;
         cout << "  Halting..." << endl; exit(1);
         cout << "  input vector is: " << endl;
         print_vec_2<string>(v1); exit(1);
       }
       parm0.default_option = v1[1];
       if (parm0.n_options > 1) {
         flag_1 = check_word_1a(parm0.options,parm0.default_option);
         if (!flag_1) {
           cout << "SolveBlocks::method_default: " << endl;
           cout << "  default option not found in file " << filename << "." << endl;
           cout << "  Halting..." << endl; exit(1);
         }
       }

       next_line(inf,v1);
       if (v1[0] != "end_parm") {
         cout << "SolveBlocks::method_default: " << endl;
         cout << "  expect <end_parm> in file " << filename << "." << endl;
         cout << "  Halting..." << endl; exit(1);
         cout << "  input vector is: " << endl;
         print_vec_2<string>(v1); exit(1);
       }
       parms.push_back(parm0);
     }
     inf.close();
   }

   n_parms = parms.size();

   for (int i=0; i < n_parms; i++) {
     s2 = parms[i].keyword;
     s3 = parms[i].default_option;

     if (s3 != "none") {
       assign_parm(s2,s3,global);
     }
   }

   nr_norm_large = 1.0e10;

   outf_real_precision = 6;
   outf_time_precision = 6;
   outf_sol_precision = 6;

   outf_real_word_width = outf_real_precision + 7;
   outf_time_word_width = outf_time_precision + 7;
   outf_sol_word_width_real = outf_sol_precision + 7;

   n_outfile = 0;
   flag_read_solution  = false;
   flag_init_solution  = false;
   flag_prev_solution  = false;
   flag_write_solution = false;
   flag_write_time_x   = false;
   flag_write_time_e   = false;

   flag_startup = false;
   flag_trns = false;

   return;
} // end of SolveBlocks::method_default
// -----------------------------------------------------------------------------
void SolveBlocks::trns_constants_1() {

// RKF45 alpha values:

   rkf45_a1 = 0.25;
   rkf45_a2 = 3.0/8.0;
   rkf45_a3 = 12.0/13.0;
   rkf45_a4 = 1.0;
   rkf45_a5 = 0.5;

// RKF45 beta values:

   rkf45_b10 = 0.25;

   rkf45_b20 = 3.0/32.0;
   rkf45_b21 = 9.0/32.0;

   rkf45_b30 =  1932.0/2197.0;
   rkf45_b31 = -7200.0/2197.0;
   rkf45_b32 =  7296.0/2197.0;

   rkf45_b40 =  439.0/216.0;
   rkf45_b41 = -8.0;
   rkf45_b42 =  3680.0/513.0;
   rkf45_b43 = -845.0/4104.0;

   rkf45_b50 = -8.0/27.0;
   rkf45_b51 =  2.0;
   rkf45_b52 = -3544.0/2565.0;
   rkf45_b53 =  1859.0/4104.0;
   rkf45_b54 = -11.0/40.0;

   rkf45_4_g0 =  25.0/216.0;
   rkf45_4_g1 =  0.0;
   rkf45_4_g2 =  1408.0/2565.0;
   rkf45_4_g3 =  2197.0/4104.0;
   rkf45_4_g4 = -0.2;

// 5th order gamma's are not required.

// error coefficients:

   rkf45_e0 =  1.0/360.0;
   rkf45_e1 =  0.0;
   rkf45_e2 = -128.0/4275.0;
   rkf45_e3 = -2197.0/75240.0;
   rkf45_e4 =  1.0/50.0;
   rkf45_e5 =  2.0/55.0;

// variables for Bogacki-Shampine method:

   bs23_a1 = 0.5;
   bs23_a2 = 0.75;

   bs23_b10 = 0.5;

   bs23_b21 = 0.75;

   bs23_b30 = 2.0/9.0;
   bs23_b31 = 1.0/3.0;
   bs23_b32 = 4.0/9.0;

   bs23_b40 = 2.0/9.0;
   bs23_b41 = 1.0/3.0;
   bs23_b42 = 4.0/9.0;

// 3rd order gamma's

   bs23_3_g0 = 2.0/9.0;
   bs23_3_g1 = 1.0/3.0;
   bs23_3_g2 = 4.0/9.0;

// 2nd order gamma's

   bs23_2_g0 = 7.0/24.0;
   bs23_2_g1 = 0.25;
   bs23_2_g2 = 1.0/3.0;
   bs23_2_g3 = 0.125;

// "e" stands for error:

   bs23_e0 = bs23_3_g0 - bs23_2_g0;
   bs23_e1 = bs23_3_g1 - bs23_2_g1;
   bs23_e2 = bs23_3_g2 - bs23_2_g2;
   bs23_e3 =           - bs23_2_g3;

// Heun's method:

   heun_a1  = 2.0/3.0;
   heun_b10 = heun_a1;
   heun_g0  = 0.25;
   heun_g1  = 0.75;

// trbdf2:
   bank_tolr   = 1.0e-5;
   bank_gamma  = 2.0-sqrt(2.0);
   bank_theta1 = 0.9;
   bank_c = (-3.0*(bank_gamma*bank_gamma) + 4.0*bank_gamma-2.0)/
            (12.0*(2.0-bank_gamma));

   return;
} // end of SolveBlocks::trns_constants_1
// -----------------------------------------------------------------------------
void SolveBlocks::trns_constants_2_e() {

   if (delt_e < delt_min_e) {
     cout << "trns_constants_2_e: delt_e < delt_min_e?" << endl;
     cout << "  delt_e = " << delt_e << ", delt_min_e = " << delt_min_e << endl;
     cout << "  Halting..." << endl; exit(1);
   }
   if (e_algo_be0) {
     beuler_1_e = 1.0/delt_e;
   }
   if (e_algo_trz0) {
     trz_1_e = 2.0/delt_e;
   }
   if (e_algo_bdf2) {
     bdf2_1_e = (2.0-bank_gamma)/(delt_e*(1.0-bank_gamma));
     bdf2_2_e = 1.0/(delt_e*bank_gamma*(1.0-bank_gamma));
     bdf2_3_e = (1.0-bank_gamma)/(delt_e*bank_gamma);
   }
   return;
} // end of SolveBlocks::trns_constants_2_e
// -----------------------------------------------------------------------------
void SolveBlocks::trns_constants_2_x() {

   if (delt_x < delt_min_x) {
     cout << "trns_constants_2_x: delt_x < delt_min_x?" << endl;
     cout << "  delt_x = " << delt_x << ", delt_min_x = " << delt_min_x << endl;
     cout << "  Halting..." << endl; exit(1);
   }
   if (x_algo_be0) {
     beuler_1_x = 1.0/delt_x;
   }
   if (x_algo_trz0) {
     trz_1_x = 2.0/delt_x;
   }
   if (x_algo_bdf2) {
     bdf2_1_x = (2.0-bank_gamma)/(delt_x*(1.0-bank_gamma));
     bdf2_2_x = 1.0/(delt_x*bank_gamma*(1.0-bank_gamma));
     bdf2_3_x = (1.0-bank_gamma)/(delt_x*bank_gamma);
   }
   return;
} // end of SolveBlocks::trns_constants_2_x
// -----------------------------------------------------------------------------
void SolveBlocks::trns_constants_2_ex() {

   if (delt_e < delt_min_e) {
     cout << "trns_constants_2_ex: delt_e < delt_min_ex?" << endl;
     cout << "  delt_e = " << delt_e << ", delt_min_ex = " << delt_min_ex << endl;
     cout << "  Halting..." << endl; exit(1);
   }
   if (ex_algo_be0) {
     beuler_1_e = 1.0/delt_e;
   }
   if (ex_algo_trz0) {
     trz_1_e = 2.0/delt_e;
   }
   if (ex_algo_bdf2) {
     bdf2_1_e = (2.0-bank_gamma)/(delt_e*(1.0-bank_gamma));
     bdf2_2_e = 1.0/(delt_e*bank_gamma*(1.0-bank_gamma));
     bdf2_3_e = (1.0-bank_gamma)/(delt_e*bank_gamma);
   }

   if (ex_algo_be0) {
     beuler_1_x = 1.0/delt_x;
   }
   if (ex_algo_trz0) {
     trz_1_x = 2.0/delt_x;
   }
   if (ex_algo_bdf2) {
     bdf2_1_x = (2.0-bank_gamma)/(delt_x*(1.0-bank_gamma));
     bdf2_2_x = 1.0/(delt_x*bank_gamma*(1.0-bank_gamma));
     bdf2_3_x = (1.0-bank_gamma)/(delt_e*bank_gamma);
   }

   return;
} // end of SolveBlocks::trns_constants_2_ex
// -----------------------------------------------------------------------------
void SolveBlocks::open_output_files() {
   string filename;
   for (int i=0; i < n_outfile; i++) {
     // Expect filename to be absolute path
     filename = outf_name[i];

     if (flag_append[i]) {
       f_output[i].open(filename,ios::app|ios::binary);
       total_lines[i] = count_lines_1(filename); 
     } else {
       f_output[i].open(filename,ios::out|ios::binary);
       total_lines[i] = 0; 
     }

     f_output[i] << scientific;
     f_output[i] << setprecision(outf_real_precision);
   }
   return;
} // end of SolveBlocks::open_output_file
// -----------------------------------------------------------------------------
void SolveBlocks::close_output_files() {

   for (int i=0; i < n_outfile; i++) {
     f_output[i].close();
   }
   return;
} // end of SolveBlocks::close_output_file
// -----------------------------------------------------------------------------
void SolveBlocks::get_dmp(
   Circuit &cct) {

   if (flag_dc || flag_startup) {

     e_nr_flag_dmp_a    = e_nr_flag_dmp0;
     e_nr_itermax_a     = e_nr_itermax0;
     e_nr_dmp_k_a       = e_nr_dmp_k0;
     e_nr_dmp_itermax_a = e_nr_dmp_itermax0;

     x_nr_flag_dmp_a    = x_nr_flag_dmp0;
     x_nr_itermax_a     = x_nr_itermax0;
     x_nr_dmp_k_a       = x_nr_dmp_k0;
     x_nr_dmp_itermax_a = x_nr_dmp_itermax0;

     ex_nr_flag_dmp_a    = ex_nr_flag_dmp0;
     ex_nr_itermax_a     = ex_nr_itermax0;
     ex_nr_dmp_k_a       = ex_nr_dmp_k0;
     ex_nr_dmp_itermax_a = ex_nr_dmp_itermax0;
   } else if (flag_trns || flag_ssw) {
     if (cct.flag_x) {
       if (iter_trns_x == 0) {
         x_nr_flag_dmp_a    = x_nr_flag_dmp0;
         x_nr_itermax_a     = x_nr_itermax0;
         x_nr_dmp_k_a       = x_nr_dmp_k0;
         x_nr_dmp_itermax_a = x_nr_dmp_itermax0;
       } else {
         x_nr_flag_dmp_a    = x_nr_flag_dmp;
         x_nr_itermax_a     = x_nr_itermax;
         x_nr_dmp_k_a       = x_nr_dmp_k;
         x_nr_dmp_itermax_a = x_nr_dmp_itermax;
       }
     }
     if (cct.flag_e) {
       if (iter_trns_e == 0) {
         e_nr_flag_dmp_a    = e_nr_flag_dmp0;
         e_nr_itermax_a     = e_nr_itermax0;
         e_nr_dmp_k_a       = e_nr_dmp_k0;
         e_nr_dmp_itermax_a = e_nr_dmp_itermax0;
       } else {
         e_nr_flag_dmp_a    = e_nr_flag_dmp;
         e_nr_itermax_a     = e_nr_itermax;
         e_nr_dmp_k_a       = e_nr_dmp_k;
         e_nr_dmp_itermax_a = e_nr_dmp_itermax;
       }
     }
//   for ex, we can use either iter_trns_e or iter_trns_x in the condition
     if (cct.flag_e && cct.flag_x) {
       if (iter_trns_e == 0) {
         ex_nr_flag_dmp_a    = ex_nr_flag_dmp0;
         ex_nr_itermax_a     = ex_nr_itermax0;
         ex_nr_dmp_k_a       = ex_nr_dmp_k0;
         ex_nr_dmp_itermax_a = ex_nr_dmp_itermax0;
       } else {
         ex_nr_flag_dmp_a    = ex_nr_flag_dmp;
         ex_nr_itermax_a     = ex_nr_itermax;
         ex_nr_dmp_k_a       = ex_nr_dmp_k;
         ex_nr_dmp_itermax_a = ex_nr_dmp_itermax;
       }
     }
   }

   return;
} // end of SolveBlocks::get_dmp
// -----------------------------------------------------------------------------
void SolveBlocks::assign_parm(
   const std::string s2,
   const std::string s3,
   Global &global) {

   ssw_nr_flag_check_rhs2 = true;

   if (s3 == "none") {
     cout << "SolveBlocks::assign_parm: " << s2
          << " not assigned. Halting..." << endl;
     exit(1);
   }
   if (s2 == "algorithm_x_trns") {
     if (s3 == "forward_euler") {
       flag_algo_x = global.i_feuler;
     } else if (s3 == "RK4") {
       flag_algo_x = global.i_rk4;
     } else if (s3 == "RKF45") {
       flag_algo_x = global.i_rkf45;
     } else if (s3 == "BS23") {
       flag_algo_x = global.i_bs23;
     } else if (s3 == "modified_euler") {
       flag_algo_x = global.i_meuler;
     } else if (s3 == "Heun") {
       flag_algo_x = global.i_heun;
     } else if (s3 == "backward_euler") {
       flag_algo_x = global.i_be;
     } else if (s3 == "backward_euler_auto") {
       flag_algo_x = global.i_be_auto;
     } else if (s3 == "backward_euler_const") {
       flag_algo_x = global.i_be_const;
     } else if (s3 == "trz") {
       flag_algo_x = global.i_trz;
     } else if (s3 == "trz_auto") {
       flag_algo_x = global.i_trz_auto;
     } else if (s3 == "trz_const") {
       flag_algo_x = global.i_trz_const;
     } else if (s3 == "trbdf2") {
       flag_algo_x = global.i_trbdf2;
     }
   } else if (s2 == "algorithm_x_startup") {
     x_algo_startup_exp = (s3 == "explicit");
   } else if (s2 == "algorithm_e") {
     if (s3 == "backward_euler") {
       flag_algo_e = global.i_be;
     } else if (s3 == "backward_euler_auto") {
       flag_algo_e = global.i_be_auto;
     } else if (s3 == "backward_euler_const") {
       flag_algo_e = global.i_be_const;
     } else if (s3 == "trz") {
       flag_algo_e = global.i_trz;
     } else if (s3 == "trz_auto") {
       flag_algo_e = global.i_trz_auto;
     } else if (s3 == "trz_const") {
       flag_algo_e = global.i_trz_const;
     } else if (s3 == "trbdf2") {
       flag_algo_e = global.i_trbdf2;
     }
   } else if (s2 == "algorithm_ex") {
     if (s3 == "backward_euler") {
       flag_algo_ex = global.i_be;
     } else if (s3 == "backward_euler_auto") {
       flag_algo_ex = global.i_be_auto;
     } else if (s3 == "backward_euler_const") {
       flag_algo_ex = global.i_be_const;
     } else if (s3 == "trz") {
       flag_algo_ex = global.i_trz;
     } else if (s3 == "trz_auto") {
       flag_algo_ex = global.i_trz_auto;
     } else if (s3 == "trz_const") {
       flag_algo_ex = global.i_trz_const;
     } else if (s3 == "trbdf2") {
       flag_algo_ex = global.i_trbdf2;
     }
   } else if (s2 == "itmax_trns") {
     if (s3 == "INF") {
       flag_limit_iter_trns = false;
     } else {
       flag_limit_iter_trns = true;
       itmax_trns = stoi(s3);
     }
   } else if (s2 == "itmax_trbdf2") {
     itmax_trbdf2 = stoi(s3);
   } else if (s2 == "itmax_stepred") {
     itmax_stepred = stoi(s3);
   } else if (s2 == "write_time_e") {
     flag_write_time_e = (s3 == "yes");
   } else if (s2 == "write_time_x") {
     flag_write_time_x = (s3 == "yes");
   } else if (s2 == "write_iter_n_e") {
     write_iter_n_e = stoi(s3);
   } else if (s2 == "write_iter_n_x") {
     write_iter_n_x = stoi(s3);
   } else if (s2 == "t_start") {
     global.time_begin = stod_suffix(s3);
     flag_t_start = true;
   } else if (s2 == "t_end") {
     global.time_end = stod_suffix(s3);
     flag_t_end = true;
   } else if (s2 == "delt_small") {
     delt_small = stod_suffix(s3);
   } else if (s2 == "tstep0_x") {
     delt0_x = stod_suffix(s3);
     flag_delt0_x = true;
   } else if (s2 == "tstep0_e") {
     delt0_e = stod_suffix(s3);
     flag_delt0_e = true;
   } else if (s2 == "tstep0_ex") {
     delt0_ex = stod_suffix(s3);
     flag_delt0_ex = true;
   } else if (s2 == "delt_min_x") {
     delt_min_x = stod_suffix(s3);
     flag_delt_min_x = true;
   } else if (s2 == "delt_min_e") {
     delt_min_e = stod_suffix(s3);
     flag_delt_min_e = true;
   } else if (s2 == "delt_min_ex") {
     delt_min_ex = stod_suffix(s3);
     flag_delt_min_ex = true;
   } else if (s2 == "delt_max_e") {
     delt_max_e = stod_suffix(s3);
     flag_delt_max_e = true;
   } else if (s2 == "delt_max_x") {
     delt_max_x = stod_suffix(s3);
     flag_delt_max_x = true;
   } else if (s2 == "delt_max_ex") {
     delt_max_ex = stod_suffix(s3);
     flag_delt_max_ex = true;
   } else if (s2 == "t_startup") {
     time_startup = stod_suffix(s3);
   } else if (s2 == "rkf45_tolr") {
     rkf45_tolr = stod_suffix(s3);
   } else if (s2 == "rkf45_fctr_min") {
     rkf45_fctr_min = stod_suffix(s3);
   } else if (s2 == "rkf45_fctr_max") {
     rkf45_fctr_max = stod_suffix(s3);
   } else if (s2 == "bs23_tolr") {
     bs23_tolr = stod_suffix(s3);
   } else if (s2 == "bs23_fctr_min") {
     bs23_fctr_min = stod_suffix(s3);
   } else if (s2 == "bs23_fctr_max") {
     bs23_fctr_max = stod_suffix(s3);
   } else if (s2 == "gauss_epsln") {
     gauss_epsln = stod_suffix(s3);
   } else if (s2 == "zero_piv") {
     zero_piv = stod_suffix(s3);
   } else if (s2 == "factor_step_increase") {
     factor_stepinc = stod_suffix(s3);
   } else if (s2 == "factor_step_decrease") {
     factor_stepdec = stod_suffix(s3);
   } else if (s2 == "e_nr_itermax0") {
     e_nr_itermax0 = stoi(s3);
   } else if (s2 == "e_nr_dmp0") {
     e_nr_flag_dmp0 = (s3 == "yes");
   } else if (s2 == "e_nr_dmp_itermax0") {
     e_nr_dmp_itermax0 = stoi(s3);
   } else if (s2 == "e_nr_dmp_k0") {
     e_nr_dmp_k0 = stod_suffix(s3);
   } else if (s2 == "e_nr_dmp") {
     e_nr_flag_dmp = (s3 == "yes");
   } else if (s2 == "e_nr_itermax") {
     e_nr_itermax = stoi(s3);
   } else if (s2 == "e_nr_dmp_itermax") {
     e_nr_dmp_itermax = stoi(s3);
   } else if (s2 == "e_nr_dmp_k") {
     e_nr_dmp_k = stod_suffix(s3);
   } else if (s2 == "e_nr_check_rhs2") {
     e_nr_flag_check_rhs2 = (s3 == "yes");
   } else if (s2 == "e_nr_check_spice") {
     e_nr_flag_check_spice = (s3 == "yes");
   } else if (s2 == "e_nr_check_delx_volt") {
     e_nr_flag_check_delx_volt = (s3 == "yes");
   } else if (s2 == "e_nr_check_delx_all") {
     e_nr_flag_check_delx_all = (s3 == "yes");
   } else if (s2 == "e_nr_write_rhs2") {
     e_nr_flag_write_rhs2 = (s3 == "yes");
   } else if (s2 == "e_nr_write_rhsinf") {
     e_nr_flag_write_rhsinf = (s3 == "yes");
   } else if (s2 == "e_nr_write_delx_volt") {
     e_nr_flag_write_delx_volt = (s3 == "yes");
   } else if (s2 == "e_nr_eps_rhs") {
     e_nr_eps_rhs = stod_suffix(s3);
   } else if (s2 == "e_nr_eps_volt") {
     e_nr_eps_volt = stod_suffix(s3);
   } else if (s2 == "e_nr_eps_delx_all") {
     e_nr_eps_delx_all = stod_suffix(s3);
   } else if (s2 == "e_nr_spice_vntol") {
     e_nr_spice_vntol = stod_suffix(s3);
   } else if (s2 == "e_nr_spice_abstol") {
     e_nr_spice_abstol = stod_suffix(s3);
   } else if (s2 == "e_nr_spice_reltol") {
     e_nr_spice_reltol = stod_suffix(s3);
   } else if (s2 == "x_nr_itermax0") {
     x_nr_itermax0 = stoi(s3);
   } else if (s2 == "x_nr_dmp0") {
     x_nr_flag_dmp0 = (s3 == "yes");
   } else if (s2 == "x_nr_dmp_itermax0") {
     x_nr_dmp_itermax0 = stoi(s3);
   } else if (s2 == "x_nr_dmp_k0") {
     x_nr_dmp_k0 = stod_suffix(s3);
   } else if (s2 == "x_nr_dmp") {
     x_nr_flag_dmp = (s3 == "yes");
   } else if (s2 == "x_nr_itermax") {
     x_nr_itermax = stoi(s3);
   } else if (s2 == "x_nr_dmp_itermax") {
     x_nr_dmp_itermax = stoi(s3);
   } else if (s2 == "x_nr_dmp_k") {
     x_nr_dmp_k = stod_suffix(s3);
   } else if (s2 == "x_nr_check_rhs2") {
     x_nr_flag_check_rhs2 = (s3 == "yes");
   } else if (s2 == "x_nr_check_delx_all") {
     x_nr_flag_check_delx_all = (s3 == "yes");
   } else if (s2 == "x_nr_write_rhs2") {
     x_nr_flag_write_rhs2 = (s3 == "yes");
   } else if (s2 == "x_nr_write_rhsinf") {
     x_nr_flag_write_rhsinf = (s3 == "yes");
   } else if (s2 == "x_nr_eps_rhs") {
     x_nr_eps_rhs = stod_suffix(s3);
   } else if (s2 == "x_nr_eps_delx_all") {
     x_nr_eps_delx_all = stod_suffix(s3);
   } else if (s2 == "ex_nr_itermax0") {
     ex_nr_itermax0 = stoi(s3);
   } else if (s2 == "ex_nr_dmp0") {
     ex_nr_flag_dmp0 = (s3 == "yes");
   } else if (s2 == "ex_nr_dmp_itermax0") {
     ex_nr_dmp_itermax0 = stoi(s3);
   } else if (s2 == "ex_nr_dmp_k0") {
     ex_nr_dmp_k0 = stod_suffix(s3);
   } else if (s2 == "ex_nr_dmp") {
     ex_nr_flag_dmp = (s3 == "yes");
   } else if (s2 == "ex_nr_check_rhs2") {
     ex_nr_flag_check_rhs2 = (s3 == "yes");
   } else if (s2 == "ex_nr_check_delx_all") {
     ex_nr_flag_check_delx_all = (s3 == "yes");
   } else if (s2 == "ex_nr_write_rhs2") {
     ex_nr_flag_write_rhs2 = (s3 == "yes");
   } else if (s2 == "ex_nr_eps_rhs") {
     ex_nr_eps_rhs = stod_suffix(s3);
   } else if (s2 == "ex_nr_eps_delx_all") {
     ex_nr_eps_delx_all = stod_suffix(s3);
   } else if (s2 == "ex_nr_itermax") {
     ex_nr_itermax = stoi(s3);
   } else if (s2 == "ex_nr_dmp_itermax") {
     ex_nr_dmp_itermax = stoi(s3);
   } else if (s2 == "ex_nr_dmp_k") {
     ex_nr_dmp_k = stod_suffix(s3);
   } else if (s2 == "trbdf2_tolr") {
     bank_tolr = stod_suffix(s3);
   } else if (s2 == "ssw_nr_itermax") {
     ssw_nr_itermax = stoi(s3);
   } else if (s2 == "ssw_nr_dmp") {
     ssw_nr_flag_dmp = (s3 == "yes");
   } else if (s2 == "ssw_nr_dmp_k") {
     ssw_nr_dmp_k = stod_suffix(s3);
   } else if (s2 == "ssw_nr_dmp_itermax") {
     ssw_nr_dmp_itermax = stoi(s3);
   } else if (s2 == "ssw_period") {
     ssw_period = stod_suffix(s3);
     flag_ssw_period = true;
   } else if (s2 == "ssw_frequency") {
     ssw_frequency = stod_suffix(s3);
     flag_ssw_frequency = true;
   } else if (s2 == "ssw_nr_eps_rhs") {
     ssw_nr_eps_rhs = stod_suffix(s3);
   } else if (s2 == "ssw_ndiv") {
     ssw_ndiv = stoi(s3);
   } else if (s2 == "ssw_period_mult") {
     ssw_period_mult = stoi(s3);
   } else {
     cout << "SolveBlocks:assign_parm: unknown keyword <" << s2 << ">" << endl;
     cout << "  Halting..." << endl; exit(1);
   }
   return;
} //end of SolveBlocks::assign_parm
