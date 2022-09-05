"""
Copyright (C) 2022 - Jeff Wheeler <jeffwheeler@gmail.com>
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

from enum import Enum

from termcolor import cprint


class SolveBlock(object):
    def __init__(self, assignments=None, methods=None, output_blocks=None):
        self.assignments = assignments or {}
        self.methods = methods or []
        self.output_blocks = output_blocks or []

    def dump(self):
        s = "begin_solve\n"
        for k, v in self.assignments.items():
            s += f"   {k}={v}\n"
        for k, v in self.methods:
            s += f"   method: {k}={v}\n"
        for output_block in self.output_blocks:
            s += "   begin_output\n    "
            for k, v in output_block["assignments"].items():
                s += f" {k}={v}"
            for control_block in output_block["control"]:
                s += f"\n     control:"
                for k, v in control_block[1].items():
                    s += f" {k}={v}"
            for v_groups in output_block["variables"]:
                s += "\n     variables:"
                for v in v_groups:
                    s += f" {v}"
            s += "\n   end_output\n"
        s += "end_solve\n"
        return s


class CctFile(object):
    def __init__(self, project_fname=None):
        self.project_fname = project_fname or "undefined"
        self.cct_elems = []
        self.cct_assignments = {}
        self.cct_outvars = {}
        self.solve_blocks = []

    def dump(self):
        s = f"title: {self.project_fname}\n"
        s += "begin_circuit\n"
        for cct_elem_kind, cct_elem_assignments in self.cct_elems:
            line = f"   {cct_elem_kind}"
            for k, v in cct_elem_assignments.items():
                to_add = f" {k}={v}"
                if len(line) + len(to_add) >= 80:
                    s += line + "\n"
                    line = "+    "
                line += to_add
            s += line
            s += "\n"
        for k, v in self.cct_assignments.items():
            s += f"   {k}={v}\n"
        for k, v in self.cct_outvars.items():
            s += f"   outvar: {k}={v}\n"
        s += "end_circuit\n"
        for solve_block in self.solve_blocks:
            s += solve_block.dump()
        s += "end_cf\n"
        return s


class ParmBlock(object):
    def __init__(self, keyword, options, default, force_write, assignments):
        self.keyword = keyword
        self.options = options
        self.default = default
        self.force_write = force_write
        self.assignments = assignments


class ParmsFile(object):
    def __init__(self):
        self.parms = {}

    def add_parm(self, parm):
        self.parms[parm.keyword] = parm

    def dump(self):
        s = "begin_file\n"
        for k, parm_block in self.parms.items():
            s += "  begin_parm"
            if parm_block.force_write:
                s += " force_write"
            for k, v in parm_block.assignments.items():
                s += f" {k}={v}"
            s += f"\n    keyword: {parm_block.keyword}\n"
            if len(parm_block.options) > 1:
                s += "    options:\n"
                for opt in parm_block.options:
                    s += f"+     {opt}\n"
            else:
                s += f"    options: {parm_block.options[0]}\n"
            s += f"    default: {parm_block.default}\n"
            s += "  end_parm\n"
        s += "end_file\n"
        return s
