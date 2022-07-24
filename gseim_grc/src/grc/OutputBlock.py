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

class OutputBlock():
    def __init__(self, d_outparms={}, s_index_slv='', s_name='', d_output_block={}):
#       index_slv (string): index of the solve block to which this out block belongs

        self.d_parms = {}
        self.l_outvars = []

        if len(d_outparms) != 0:
            for k, v in d_outparms.items():
                self.d_parms[k] = v['default']

        if s_index_slv:
            self.index_slv = s_index_slv
        if s_name:
            self.name = s_name

        if len(d_output_block) != 0:
            self.name = d_output_block['name']
            self.index_slv = d_output_block['index_slv']

            for ov in d_output_block['outvars']:
                self.l_outvars.append(ov)

            for k in d_output_block['d_parms'].keys():
                self.d_parms[k] = d_output_block['d_parms'][k]

    def block_to_dict(self):
        d = {}
        d['name'] = self.name
        d['index_slv'] = self.index_slv
        d['outvars'] = self.l_outvars
        d['d_parms'] = self.d_parms
        return d
