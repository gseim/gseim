"""
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
"""

import sys
import os

import gutils_gseim as gu

cur_dir = os.getcwd()
print('cur_dir = ',cur_dir)

aux_dir = cur_dir.replace('exec', 'cppsrc')
print('aux_dir =',aux_dir)

xbe_dir = cur_dir.replace('exec', 'xbe') + '/'
print('xbe_dir =',xbe_dir)

xbe_list_filename = xbe_dir + 'xbe.lst'
print('xbe_list_filename =',xbe_list_filename)

f = open(os.path.expanduser(xbe_list_filename), 'r')
xbe_list = []

for line in f:
    xbe_name = line.replace("\n","")
    if (xbe_name not in xbe_list):
        xbe_list.append(xbe_name)
f.close()

for i in range(len(xbe_list)):
    print(i,xbe_list[i])

if not os.path.exists(aux_dir):
    os.makedirs(aux_dir)

gu.write_elements_2(xbe_list,aux_dir,'xbe.aux',xbe_dir,'.xbe')

# write subxbe.cpp

xbe_include_1 = \
  '#include "global.h"\n' \
+ '#include "xbeusr.h"\n' \
+ '#include "xbejac.h"\n' \
+ '#include "utils.h"\n' \
+ '#include <cstdlib>\n' \
+ '#include <iostream>\n' \
+ '#include <string>\n' \
+ '#include <vector>\n' \
+ 'using namespace std;\n'

xbe_include_2 = \
  'Global &G,' \
+ 'XbeUsr &X,' \
+ 'XbeJac &J) {\n'

xbe_include_3 = \
  '   return;\n' \
+ '}\n'

outfile_name = aux_dir + '/subxbe1.cpp'
f_out = open(os.path.expanduser(outfile_name), 'w')
f_out.write(xbe_include_1)

nmax = 80
indent1 = '   '
indent2 = '  '

for xbe_name in xbe_list:
    print('xbe_name = ',xbe_name)
    infile_name = xbe_dir + xbe_name + '.xbe'
    print('infile_name = ',infile_name)
    s1 = 'void x_' + xbe_name + '('
    f_out.write(s1)
    f_out.write(xbe_include_2)

    gu.extract_lines_1a(f_out, infile_name, 'variables:', 'source:')

    in_var_list = []
    gu.extract_strings_2(infile_name, 'input_vars:', in_var_list)

    out_var_list = []
    gu.extract_strings_2(infile_name, 'output_vars:', out_var_list)

    if (len(in_var_list) > 0):
        in_var_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, in_var_list, in_var_decl)

    if (len(out_var_list) > 0):
        out_var_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, out_var_list, out_var_decl)

    auxvar_list = []
    gu.extract_strings_2(infile_name, 'aux_vars:', auxvar_list)

    if (len(auxvar_list) > 0):
        auxvar_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, auxvar_list, auxvar_decl)

    iparm_list = []
    gu.extract_strings_1(infile_name, 'iparms:', iparm_list)

    if (len(iparm_list) > 0):
        iparm_decl = []
        gu.format_string_1a(f_out, nmax, 'int ', indent1, indent2, iparm_list, iparm_decl)

    rparm_list = []
    gu.extract_strings_1(infile_name, 'rparms:', rparm_list)

    if (len(rparm_list) > 0):
        rparm_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, rparm_list, rparm_decl)

    stparm_list = []
    gu.extract_strings_1(infile_name, 'stparms:', stparm_list)

    if (len(stparm_list) > 0):
        stparm_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, stparm_list, stparm_decl)

    igparm_list = []
    gu.extract_strings_1(infile_name, 'igparms:', igparm_list)

    if (len(igparm_list) > 0):
        igparm_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, igparm_list, igparm_decl)

    outparm_list = []
    gu.extract_strings_2(infile_name, 'outparms:', outparm_list)

    if (len(in_var_list) > 0):
        in_var_order = []
        gu.format_string_2a(f_out, 0, 'const int nvr_', indent1, in_var_list, in_var_order)

    if (len(out_var_list) > 0):
        out_var_order = []
        gu.format_string_2a(f_out, len(in_var_list), 'const int nvr_', indent1,\
          out_var_list, out_var_order)

    if (len(iparm_list) > 0):
        iparm_order = []
        gu.format_string_2a(f_out, 0, 'const int ni_', indent1, iparm_list, iparm_order)

    if (len(rparm_list) > 0):
        rparm_order = []
        gu.format_string_2a(f_out, 0, 'const int nr_', indent1, rparm_list, rparm_order)

    if (len(stparm_list) > 0):
        stparm_order = []
        gu.format_string_2a(f_out, 0, 'const int nst_', indent1, stparm_list, stparm_order)

    if (len(igparm_list) > 0):
        igparm_order = []
        gu.format_string_2a(f_out, 0, 'const int nig_', indent1, igparm_list, igparm_order)

    if (len(outparm_list) > 0):
        outparm_order = []
        gu.format_string_2a(f_out, 0, 'const int no_', indent1, outparm_list, outparm_order)

    if (len(auxvar_list) > 0):
        auxvar_order = []
        gu.format_string_2a(f_out, 0, 'const int na_', indent1, auxvar_list, auxvar_order)

    n_f = gu.extract_int_1(infile_name, 'n_f')
    if (n_f > 0):
        eq_order = []
        gu.format_string_3a(f_out, 'const int nf_', indent1, n_f, eq_order)

    n_g = gu.extract_int_1(infile_name, 'n_g')
    if (n_g > 0):
        eq_order = []
        gu.format_string_3a(f_out, 'const int ng_', indent1, n_g, eq_order)

#   source code from xx.xbe:
    gu.extract_lines_1a(f_out, infile_name, 'source:', 'endC')
    f_out.write(xbe_include_3)

f_out.close()

# write getxbe.cpp

getxbe_include_1 = \
  '#include "global.h"\n' \
+ '#include "xbeusr.h"\n' \
+ '#include "xbejac.h"\n' \
+ '#include <cstdlib>\n' \
+ '#include <iostream>\n' \
+ '#include <string>\n' \
+ 'using namespace std;\n'

getxbe_include_2 = \
  ' Global &G,' \
+ ' XbeUsr &X,' \
+ ' XbeJac &J);\n'

getxbe_include_3 = \
  'void get_xbe(\n' \
+ ' const int i_xbel,\n' \
+ ' Global &G,\n' \
+ ' XbeUsr &X,\n' \
+ ' XbeJac &J) {\n'

getxbe_include_4 = \
  'G,X,J);\n'

getxbe_include_5 = \
  '   }\n' \
+ '   return;\n' \
+ '}\n'

outfile_name = aux_dir + '/getxbe.cpp'
f_out = open(os.path.expanduser(outfile_name), 'w')
f_out.write(getxbe_include_1)

for xbe_name in xbe_list:
    s1 = 'void x_' + xbe_name + '(\n'
    f_out.write(s1)
    f_out.write(getxbe_include_2)

f_out.write(getxbe_include_3)
f_out.write('   switch (i_xbel) {\n')
for i in range(len(xbe_list)):
    f_out.write('     case ' + str(i) + ':\n')
    f_out.write('       x_' + xbe_list[i] + '(')
    f_out.write(getxbe_include_4)
    f_out.write('       break;\n')

f_out.write(getxbe_include_5)

f_out.close()
# ------------------------------------------------------------------------------
# ebe files:

ebe_dir = cur_dir.replace('exec', 'ebe') + '/'
print('ebe_dir =',ebe_dir)

ebe_list_filename = ebe_dir + 'ebe.lst'
print('ebe_list_filename =',ebe_list_filename)

f = open(os.path.expanduser(ebe_list_filename), 'r')
ebe_list = []

for line in f:
    ebe_name = line.replace("\n","")
    if (ebe_name not in ebe_list):
        ebe_list.append(ebe_name)
f.close()

for i in range(len(ebe_list)):
    print(i,ebe_list[i])

if not os.path.exists(aux_dir):
    os.makedirs(aux_dir)

gu.write_elements_2(ebe_list,aux_dir,'ebe.aux',ebe_dir,'.ebe')

# write subebe.cpp

ebe_include_1 = \
  '#include "global.h"\n' \
+ '#include "ebeusr.h"\n' \
+ '#include "ebejac.h"\n' \
+ '#include "utils.h"\n' \
+ '#include <cstdlib>\n' \
+ '#include <iostream>\n' \
+ '#include <string>\n' \
+ '#include <vector>\n' \
+ 'using namespace std;\n'

ebe_include_2 = \
  'Global &G,' \
+ 'EbeUsr &X,' \
+ 'EbeJac &J) {\n'

ebe_include_3 = \
  '   return;\n' \
+ '}\n'

outfile_name = aux_dir + '/subebe.cpp'
f_out = open(os.path.expanduser(outfile_name), 'w')
f_out.write(ebe_include_1)

nmax = 80
indent1 = '   '
indent2 = '  '

for ebe_name in ebe_list:
    print('ebe_name = ',ebe_name)
    infile_name = ebe_dir + ebe_name + '.ebe'
    print('infile_name = ',infile_name)
    s1 = 'void e_' + ebe_name + '('
    f_out.write(s1)
    f_out.write(ebe_include_2)

    gu.extract_lines_1a(f_out, infile_name, 'variables:', 'source:')

    nodes_list = []
    gu.extract_strings_2(infile_name, 'nodes:', nodes_list)

    state_vars_list = []
    gu.extract_strings_2(infile_name, 'state_vars:', state_vars_list)

    aux_vars_list = []
    gu.extract_strings_2(infile_name, 'aux_vars:', aux_vars_list)

    aux_vars_startup_list = []
    gu.extract_strings_2(infile_name, 'aux_vars_startup:', aux_vars_startup_list)

    x_vars_list = []
    gu.extract_strings_2(infile_name, 'x_vars:', x_vars_list)

    if (len(state_vars_list) > 0):
        state_vars_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, state_vars_list, state_vars_decl)

    if (len(aux_vars_list) > 0):
        aux_vars_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, aux_vars_list, aux_vars_decl)

    if (len(aux_vars_startup_list) > 0):
        aux_vars_startup_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, aux_vars_startup_list,\
           aux_vars_startup_decl)

    if (len(x_vars_list) > 0):
        x_vars_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, x_vars_list, x_vars_decl)

    iparm_list = []
    gu.extract_strings_1(infile_name, 'iparms:', iparm_list)

    if (len(iparm_list) > 0):
        iparm_decl = []
        gu.format_string_1a(f_out, nmax, 'int ', indent1, indent2, iparm_list, iparm_decl)

    rparm_list = []
    gu.extract_strings_1(infile_name, 'rparms:', rparm_list)

    if (len(rparm_list) > 0):
        rparm_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, rparm_list, rparm_decl)

    stparm_list = []
    gu.extract_strings_1(infile_name, 'stparms:', stparm_list)

    if (len(stparm_list) > 0):
        stparm_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, stparm_list, stparm_decl)

    igparm_list = []
    gu.extract_strings_1(infile_name, 'igparms:', igparm_list)

    if (len(igparm_list) > 0):
        igparm_decl = []
        gu.format_string_1a(f_out, nmax, 'double ', indent1, indent2, igparm_list, igparm_decl)

    outparm_list = []
    gu.extract_strings_2(infile_name, 'outparms:', outparm_list)

    if (len(nodes_list) > 0):
        nodes_order = []
        gu.format_string_2a(f_out, 0, 'const int nnd_', indent1, nodes_list, nodes_order)

    if (len(state_vars_list) > 0):
        state_vars_order = []
        gu.format_string_2a(f_out, 0, 'const int nstv_', indent1, state_vars_list, state_vars_order)

    if (len(aux_vars_list) > 0):
        aux_vars_order = []
        gu.format_string_2a(f_out, 0, 'const int na_', indent1, aux_vars_list, aux_vars_order)

    if (len(aux_vars_startup_list) > 0):
        aux_vars_startup_order = []
        gu.format_string_2a(f_out, 0, 'const int nas_', indent1, aux_vars_startup_list,\
          aux_vars_startup_order)

    if (len(x_vars_list) > 0):
        x_vars_order = []
        gu.format_string_2a(f_out, 0, 'const int nx_', indent1, x_vars_list, x_vars_order)

    if (len(iparm_list) > 0):
        iparm_order = []
        gu.format_string_2a(f_out, 0, 'const int ni_', indent1, iparm_list, iparm_order)

    if (len(rparm_list) > 0):
        rparm_order = []
        gu.format_string_2a(f_out, 0, 'const int nr_', indent1, rparm_list, rparm_order)

    if (len(stparm_list) > 0):
        stparm_order = []
        gu.format_string_2a(f_out, 0, 'const int nst_', indent1, stparm_list, stparm_order)

    if (len(igparm_list) > 0):
        igparm_order = []
        gu.format_string_2a(f_out, 0, 'const int nig_', indent1, igparm_list, igparm_order)

    if (len(outparm_list) > 0):
        outparm_order = []
        gu.format_string_2a(f_out, 0, 'const int no_', indent1, outparm_list, outparm_order)

    n_f = gu.extract_int_1(infile_name, 'n_f')
    if (n_f > 0):
        eq_order = []
        gu.format_string_3a(f_out, 'const int nf_', indent1, n_f, eq_order)

    n_g = gu.extract_int_1(infile_name, 'n_g')
    if (n_g > 0):
        eq_order = []
        gu.format_string_3a(f_out, 'const int ng_', indent1, n_g, eq_order)

    n_h = gu.extract_int_1(infile_name, 'n_h')
    if (n_h > 0):
        eq_order = []
        gu.format_string_3a(f_out, 'const int nh_', indent1, n_h, eq_order)

#   source code from xx.ebe:

    gu.extract_lines_1a(f_out, infile_name, 'source:', 'endC')
    f_out.write(ebe_include_3)

f_out.close()

# write getebe.cpp

getebe_include_1 = \
  '#include "global.h"\n' \
+ '#include "ebeusr.h"\n' \
+ '#include "ebejac.h"\n' \
+ '#include <cstdlib>\n' \
+ '#include <iostream>\n' \
+ '#include <string>\n' \
+ 'using namespace std;\n'

getebe_include_2 = \
  ' Global &G,' \
+ ' EbeUsr &X,' \
+ ' EbeJac &J);\n'

getebe_include_3 = \
  'void get_ebe(\n' \
+ ' const int i_ebel,\n' \
+ ' Global &G,\n' \
+ ' EbeUsr &X,\n' \
+ ' EbeJac &J) {\n'

getebe_include_4 = \
  'G,X,J);\n'

getebe_include_5 = \
  '   }\n' \
+ '   return;\n' \
+ '}\n'

outfile_name = aux_dir + '/getebe.cpp'
f_out = open(os.path.expanduser(outfile_name), 'w')
f_out.write(getebe_include_1)

for ebe_name in ebe_list:
    s1 = 'void e_' + ebe_name + '(\n'
    f_out.write(s1)
    f_out.write(getebe_include_2)

f_out.write(getebe_include_3)
f_out.write('   switch (i_ebel) {\n')
for i in range(len(ebe_list)):
    f_out.write('     case ' + str(i) + ':\n')
    f_out.write('       e_' + ebe_list[i] + '(')
    f_out.write(getebe_include_4)
    f_out.write('       break;\n')

f_out.write(getebe_include_5)

f_out.close()
