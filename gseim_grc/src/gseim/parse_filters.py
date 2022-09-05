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

# Parsing of xfer_fn elements.
# Read the circuit file and rewrite after parsing xfer_fn elements.
#

import numpy as np
from ctypes import cdll

from importlib_resources import files

XFER_FN_PARMS = [
    "a0",
    "a1",
    "a2",
    "a3",
    "a4",
    "a5",
    "b0",
    "b1",
    "b2",
    "b3",
    "b4",
    "b5",
    "scale_coef",
    "f0",
]


class PICDummy(object):
    def __init__(self, lib):
        self.lib = lib
        self.obj = lib.PICDummy_new()

    def filter_compute_coef(self):
        self.lib.PICDummy_filter_compute_coef(self.obj)


def compute_params(d, dummy):
    flag_scale = False

    if d["scale_coef"] != "0":
        k_scale = 2.0 * np.pi * float(d["f0"])
        flag_scale = True

    f_temp = open("temp_coef_1.dat", "w")
    for k in XFER_FN_PARMS:
        f_temp.write(d[k] + "  -- " + k + "\n")
    f_temp.close()

    # call C++ program to compute partial fractions
    dummy.filter_compute_coef()

    # read partial fraction info from file
    f_temp = open("temp_coef_2.dat", "r")

    line = f_temp.readline()
    l = line.split()
    n_roots = int(l[0])
    gain = float(l[1])
    a0p = float(l[2])

    flag_real = []
    root_power = []
    alpha = []
    beta = []
    a = []
    b = []

    for i in range(n_roots):
        line = f_temp.readline()
        l = line.split()
        flag_real.append(int(l[0]))
        power_dummy = int(l[1])
        root_power.append(power_dummy)

        if flag_scale:
            alpha_scale = k_scale
            if power_dummy == 1:
                a_scale = k_scale
            elif power_dummy == 2:
                a_scale = k_scale * k_scale
        else:
            alpha_scale = 1.0
            a_scale = 1.0

        line = f_temp.readline()
        l = line.split()
        alpha_dummy = float(l[0])
        beta_dummy = float(l[1])
        alpha.append(alpha_scale * alpha_dummy)
        beta.append(alpha_scale * beta_dummy)

        line = f_temp.readline()
        l = line.split()
        a_dummy = float(l[0])
        b_dummy = float(l[1])
        a.append(a_scale * a_dummy)
        b.append(a_scale * b_dummy)

    f_temp.close()

    for i in range(n_roots):
        a[i] *= gain
        b[i] *= gain

    n_elements_ttl = 0

    for i in range(n_roots):
        if a[i] != 0.0 or b[i] != 0.0:
            n_elements_ttl += 1
    if a0p != 0.0:
        n_elements_ttl += 1

    return {
        "a": a,
        "b": b,
        "a0p": a0p,
        "alpha": alpha,
        "beta": beta,
        "gain": gain,
        "n_roots": n_roots,
        "root_power": root_power,
        "flag_real": flag_real,
        "n_elements_ttl": n_elements_ttl,
    }


def process(dummy, ast_1):
    new_elems = []
    n_filters = 1
    for cct_elem_kind, cct_elem_assignments in ast_1.cct_elems:
        if cct_elem_kind == "xelement" and cct_elem_assignments["type"] == "xfer_fn":
            element_name = cct_elem_assignments["name"]

            x_node = cct_elem_assignments["x"]
            y_node = cct_elem_assignments["y"]

            d = {k: cct_elem_assignments.get(k, "0") for k in XFER_FN_PARMS}

            r = compute_params(d, dummy)
            a = r["a"]
            b = r["b"]
            a0p = r["a0p"]
            alpha = r["alpha"]
            beta = r["beta"]
            gain = r["gain"]
            n_roots = r["n_roots"]
            root_power = r["root_power"]
            flag_real = r["flag_real"]
            n_elements_ttl = r["n_elements_ttl"]

            n_elements = 0

            for i in range(n_roots):
                if a[i] != 0.0 or b[i] != 0.0:
                    n_elements += 1
                    kind = "xelement"
                    assgns = {}

                    if n_elements_ttl > 1:
                        y_nd = x_node + "_f_" + str(n_filters) + "_" + str(n_elements)
                    else:
                        y_nd = y_node

                    if flag_real[i] == 1:
                        assgns["type"] = "pole_real_order_" + str(root_power[i])
                        assgns["a"] = ("%14.7e" % a[i]).replace(" ", "")
                        assgns["alpha"] = ("%14.7e" % alpha[i]).replace(" ", "")
                    else:
                        assgns["type"] = "pole_complex_order_" + str(root_power[i])
                        assgns["a"] = ("%14.7e" % a[i]).replace(" ", "")
                        assgns["b"] = ("%14.7e" % b[i]).replace(" ", "")
                        assgns["alpha"] = ("%14.7e" % alpha[i]).replace(" ", "")
                        assgns["beta"] = ("%14.7e" % beta[i]).replace(" ", "")

                    assgns["x"] = x_node
                    assgns["y"] = y_nd

                    if n_elements == 1:
                        y_st = float(cct_elem_assignments.get("y_st", "0"))
                        assgns["y_st"] = ("%14.7e" % y_st).replace(" ", "")

                    new_elems.append((kind, assgns))

            if a0p != 0.0:
                n_elements += 1
                kind = "xelement"
                assgns = {"type": "multscl"}

                y_nd = x_node + "_f_" + str(n_filters) + "_" + str(n_elements)
                assgns["x"] = x_node
                assgns["y"] = y_nd
                assgns["k"] = ("%14.7e" % (gain * a0p)).replace(" ", "")

                new_elems.append((kind, assgns))

            if n_elements > 1:
                kind = "xelement"
                assgns = {"type": "sum_" + str(n_elements)}

                for i in range(n_elements):
                    x_nd = x_node + "_f_" + str(n_filters) + "_" + str(i + 1)
                    assgns["x" + str(i + 1)] = str(x_nd)
                assgns["y"] = y_node

                new_elems.append((kind, assgns))

            for k1, v1 in ast_1.cct_outvars.items():
                if v1.split("_of_")[-1] == element_name:
                    if v1.split("_of_")[0] == "x":
                        ast_1.cct_outvars[k1] = "xvar_of_" + x_node
                    elif v1.split("_of_")[0] == "y":
                        ast_1.cct_outvars[k1] = "xvar_of_" + y_node
        else:
            new_elems.append((cct_elem_kind, cct_elem_assignments))

    ast_1.cct_elems = new_elems


def process_xfer_fns(cct_ast):
    # load the library
    lib_filename = files("gseim_cpp_lib") / "libfilter.so"
    lib = cdll.LoadLibrary(lib_filename)
    dummy = PICDummy(lib)

    process(dummy, cct_ast)
