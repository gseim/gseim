import os
import subprocess
import sys
import tempfile

from importlib_resources import files

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
#include "gseim_cpp_lib/ebeusr.h"
#include "gseim_cpp_lib/ebejac.h"
#include "gseim_cpp_lib/utils.h"

using namespace std;
            """)

        for element_fname in element_fnames:
            ebe_name, element_ext = os.path.splitext(os.path.basename(element_fname))
            assert os.path.exists(element_fname)
            assert element_ext == '.ebe'

            out_f.write(f"""
void e_{ebe_name} (
    Global &G,
    EbeUsr &X,
    EbeJac &J) {{
""")

            gu.extract_lines_1a(out_f, element_fname, 'variables:', 'source:')

            nodes_list = []
            gu.extract_strings_2(element_fname, 'nodes:', nodes_list)

            state_vars_list = []
            gu.extract_strings_2(element_fname, 'state_vars:', state_vars_list)

            aux_vars_list = []
            gu.extract_strings_2(element_fname, 'aux_vars:', aux_vars_list)

            aux_vars_startup_list = []
            gu.extract_strings_2(element_fname, 'aux_vars_startup:', aux_vars_startup_list)

            x_vars_list = []
            gu.extract_strings_2(element_fname, 'x_vars:', x_vars_list)

            if (len(state_vars_list) > 0):
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, state_vars_list, [])

            if (len(aux_vars_list) > 0):
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, aux_vars_list, [])

            if (len(aux_vars_startup_list) > 0):
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, aux_vars_startup_list, [])

            if (len(x_vars_list) > 0):
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, x_vars_list, [])

            iparm_list = []
            gu.extract_strings_1(element_fname, 'iparms:', iparm_list)

            if (len(iparm_list) > 0):
                gu.format_string_1a(out_f, nmax, 'int ', indent1, indent2, iparm_list, [])

            rparm_list = []
            gu.extract_strings_1(element_fname, 'rparms:', rparm_list)

            if (len(rparm_list) > 0):
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, rparm_list, [])

            stparm_list = []
            gu.extract_strings_1(element_fname, 'stparms:', stparm_list)

            if (len(stparm_list) > 0):
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, stparm_list, [])

            igparm_list = []
            gu.extract_strings_1(element_fname, 'igparms:', igparm_list)

            if (len(igparm_list) > 0):
                gu.format_string_1a(out_f, nmax, 'double ', indent1, indent2, igparm_list, [])

            outparm_list = []
            gu.extract_strings_2(element_fname, 'outparms:', outparm_list)

            if (len(nodes_list) > 0):
                gu.format_string_2a(out_f, 0, 'const int nnd_', indent1, nodes_list, [])

            if (len(state_vars_list) > 0):
                gu.format_string_2a(out_f, 0, 'const int nstv_', indent1, state_vars_list, [])

            if (len(aux_vars_list) > 0):
                gu.format_string_2a(out_f, 0, 'const int na_', indent1, aux_vars_list, [])

            if (len(aux_vars_startup_list) > 0):
                gu.format_string_2a(out_f, 0, 'const int nas_', indent1, aux_vars_startup_list, [])

            if (len(x_vars_list) > 0):
                gu.format_string_2a(out_f, 0, 'const int nx_', indent1, x_vars_list, [])

            if (len(iparm_list) > 0):
                gu.format_string_2a(out_f, 0, 'const int ni_', indent1, iparm_list, [])

            if (len(rparm_list) > 0):
                gu.format_string_2a(out_f, 0, 'const int nr_', indent1, rparm_list, [])

            if (len(stparm_list) > 0):
                gu.format_string_2a(out_f, 0, 'const int nst_', indent1, stparm_list, [])

            if (len(igparm_list) > 0):
                gu.format_string_2a(out_f, 0, 'const int nig_', indent1, igparm_list, [])

            if (len(outparm_list) > 0):
                gu.format_string_2a(out_f, 0, 'const int no_', indent1, outparm_list, [])

            n_f = gu.extract_int_1(element_fname, 'n_f')
            if (n_f > 0):
                gu.format_string_3a(out_f, 'const int nf_', indent1, n_f, [])

            n_g = gu.extract_int_1(element_fname, 'n_g')
            if (n_g > 0):
                gu.format_string_3a(out_f, 'const int ng_', indent1, n_g, [])

            n_h = gu.extract_int_1(element_fname, 'n_h')
            if (n_h > 0):
                gu.format_string_3a(out_f, 'const int nh_', indent1, n_h, [])

#   source code from xx.ebe:

            gu.extract_lines_1a(out_f, element_fname, 'source:', 'endC')
            out_f.write("""
    return;
}
""")


        # This implementation uses a map of function pointers. It maps the
        # element's library name (e.g. "thyristor") to a pointer to the
        # function outputted to the file above.
        #
        # The map is built during the first call to `get_ebe`, as maps of
        # strings cannot be statically defined.
        out_f.write("""
typedef void(*ebeFunc)(Global&, EbeUsr&, EbeJac&);
map<string, ebeFunc> ebe_f_ptr_map;

void get_ebe(
        Global &G,
        EbeUsr &X,
        EbeJac &J
) {
    if (ebe_f_ptr_map.empty()) {
        // Initialize map
""")
        for element_fname in element_fnames:
            ebe_name, element_ext = os.path.splitext(os.path.basename(element_fname))
            out_f.write(f"""
        ebe_f_ptr_map["{ebe_name}"] = &e_{ebe_name};
""")
        out_f.write("""
    }

    const auto &ebe_name = X.ebel_name;
    if (ebe_f_ptr_map.count(ebe_name) > 0) {
        (ebe_f_ptr_map.at(ebe_name))(G, X, J);
    } else {
        cout << "get_ebe: did not find \\"" << ebe_name << "\\"" << endl;
        exit(1);
    }
}
""")

if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2:])
