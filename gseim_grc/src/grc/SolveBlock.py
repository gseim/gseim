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

class SolveBlock():
    def __init__(self, slvparms_ast=None, s_name='', s_index='', d_solve_block={}):
        self.d_parms = {}
        self.l_out = []

#       Need to take care of two situations:
#       1. The solve block data is read from a file (a saved grc file)
#          and passed as d_solve_block.
#       2. The solve block default parms are read from a file and passed
#          as slvparms_ast. In this case, we also expect s_name and s_index
#          to be specified.
#
#       Notes:
#       - Always pass slvparms_ast when calling SolveBlock.
#       - even if a stored solve block is being treated, first
#         assign the default values and then write over those

        for k, v in slvparms_ast.parms.items():
            self.d_parms[k] = v.default

        if s_name:
            self.name = s_name
        if s_index:
            self.index = s_index
            self.d_parms['block_index'] = s_index

        if len(d_solve_block) != 0:
            self.name = d_solve_block['name']
            self.index = d_solve_block['index']

#           Note: no need to assign self.d_parms['block_index']; it will be
#             assigned from d_parms anyway

            for out in d_solve_block['output_blocks']:
                self.l_out.append(out)

            for k in d_solve_block['d_parms'].keys():
                if k in slvparms_ast.parms.keys():
                    self.d_parms[k] = d_solve_block['d_parms'][k]

    def block_to_dict(self):
        d = {}
        d['name'] = self.name
        d['index'] = self.index
        d['output_blocks'] = self.l_out
        d['d_parms'] = self.d_parms
        return d
