from importlib.resources import files
import os
import subprocess
import sys
import tempfile

import gseim.gutils_gseim as gu

nmax = 80
indent1 = '   '
indent2 = '  '

def main(out_fname, element_fnames):
    headers_dir = files('gseim_cpp_lib')

    with open(out_fname, 'w') as out_f:
        out_f.write("""
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>

#include "gseim_cpp_lib/global.h"
#include "gseim_cpp_lib/xbeusr.h"
#include "gseim_cpp_lib/xbejac.h"
#include "gseim_cpp_lib/user_f.h"
#include "gseim_cpp_lib/utils.h"

using namespace std;
            """)

        for element_fname in element_fnames:
            xbe_name, element_ext = os.path.splitext(os.path.basename(element_fname))
            assert os.path.exists(element_fname)
            assert element_ext == '.xbe'

            out_f.write(f"""
void x_{xbe_name} (
    Global &G,
    XbeUsr &X,
    XbeJac &J) {{
""")

            gu.extract_lines_1a(out_f, element_fname, 'variables:', 'source:')

            in_var_list = []
            gu.extract_strings_2(element_fname, 'input_vars:', in_var_list)

            out_var_list = []
            gu.extract_strings_2(element_fname, 'output_vars:', out_var_list)

            if (len(in_var_list) > 0):
                in_var_decl = []
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, in_var_list, in_var_decl)

            if (len(out_var_list) > 0):
                out_var_decl = []
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, out_var_list, out_var_decl)

            auxvar_list = []
            gu.extract_strings_2(element_fname, 'aux_vars:', auxvar_list)

            if (len(auxvar_list) > 0):
                auxvar_decl = []
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, auxvar_list, auxvar_decl)

            iparm_list = []
            gu.extract_strings_1(element_fname, 'iparms:', iparm_list)

            if (len(iparm_list) > 0):
                iparm_decl = []
                gu.format_string_1a(out_f, nmax, 'int ', indent1, indent2, iparm_list, iparm_decl)

            rparm_list = []
            gu.extract_strings_1(element_fname, 'rparms:', rparm_list)

            if (len(rparm_list) > 0):
                rparm_decl = []
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, rparm_list, rparm_decl)

            stparm_list = []
            gu.extract_strings_1(element_fname, 'stparms:', stparm_list)

            if (len(stparm_list) > 0):
                stparm_decl = []
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, stparm_list, stparm_decl)

            igparm_list = []
            gu.extract_strings_1(element_fname, 'igparms:', igparm_list)

            if (len(igparm_list) > 0):
                igparm_decl = []
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, igparm_list, igparm_decl)

            outparm_list = []
            gu.extract_strings_2(element_fname, 'outparms:', outparm_list)

            if (len(in_var_list) > 0):
                in_var_order = []
                gu.format_string_2a(out_f, 0, 'const int nvr_', indent1, in_var_list, in_var_order)

            if (len(out_var_list) > 0):
                out_var_order = []
                gu.format_string_2a(out_f, len(in_var_list), 'const int nvr_', indent1,\
                  out_var_list, out_var_order)

            if (len(iparm_list) > 0):
                iparm_order = []
                gu.format_string_2a(out_f, 0, 'const int ni_', indent1, iparm_list, iparm_order)

            if (len(rparm_list) > 0):
                rparm_order = []
                gu.format_string_2a(out_f, 0, 'const int nr_', indent1, rparm_list, rparm_order)

            if (len(stparm_list) > 0):
                stparm_order = []
                gu.format_string_2a(out_f, 0, 'const int nst_', indent1, stparm_list, stparm_order)

            if (len(igparm_list) > 0):
                igparm_order = []
                gu.format_string_2a(out_f, 0, 'const int nig_', indent1, igparm_list, igparm_order)

            if (len(outparm_list) > 0):
                outparm_order = []
                gu.format_string_2a(out_f, 0, 'const int no_', indent1, outparm_list, outparm_order)

            if (len(auxvar_list) > 0):
                auxvar_order = []
                gu.format_string_2a(out_f, 0, 'const int na_', indent1, auxvar_list, auxvar_order)

            n_f = gu.extract_int_1(element_fname, 'n_f')
            if (n_f > 0):
                eq_order = []
                gu.format_string_3a(out_f, 'const int nf_', indent1, n_f, eq_order)

            n_g = gu.extract_int_1(element_fname, 'n_g')
            if (n_g > 0):
                eq_order = []
                gu.format_string_3a(out_f, 'const int ng_', indent1, n_g, eq_order)

#   source code from xx.xbe:
            gu.extract_lines_1a(out_f, element_fname, 'source:', 'endC')
            out_f.write("""
    return;
}
""")


        # This implementation uses a map of function pointers. It maps the
        # element's library name (e.g. "thyristor") to a pointer to the
        # function outputted to the file above.
        #
        # The map is built during the first call to `get_xbe`, as maps of
        # strings cannot be statically defined.
        out_f.write("""
typedef void(*xbeFunc)(Global&, XbeUsr&, XbeJac&);
map<string, xbeFunc> xbe_f_ptr_map;

void get_xbe(
        Global &G,
        XbeUsr &X,
        XbeJac &J
) {
    if (xbe_f_ptr_map.empty()) {
        // Initialize map
""")
        for element_fname in element_fnames:
            xbe_name, element_ext = os.path.splitext(os.path.basename(element_fname))
            out_f.write(f"""
        xbe_f_ptr_map["{xbe_name}"] = &x_{xbe_name};
""")
        out_f.write("""
    }

    const auto &xbe_name = X.xbel_name;
    if (xbe_f_ptr_map.count(xbe_name) > 0) {
        (xbe_f_ptr_map.at(xbe_name))(G, X, J);
    } else {
        cout << "get_xbe (alt): did not find " << xbe_name << endl;
        exit(1);
    }
}
""")

if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2:])
