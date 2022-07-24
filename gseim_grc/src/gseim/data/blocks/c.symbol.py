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


# begin_coord

#delta = 3
delta = Constants.CANVAS_GRID_SIZE

port_block_x = 10
port_block_y = 4

delx = 2*port_block_x*delta
dely = 2*port_block_y*delta

k_width = 0.06
k_height = 0.5

dely0 = int(round(0.5*dely))
dely1 = int(round(k_height*dely))
dely2a = dely0 - dely1
dely2b = dely0 + dely1

delx1 = int(round(0.5*delx))
delx2 = int(round(k_width*delx))
delx2a = delx1 - delx2
delx2b = delx1 + delx2

c_ = []
c_.append((0, dely0))        # 0
c_.append((delx2a, dely0))   # 1
c_.append((delx2a, dely2a))  # 2
c_.append((delx2a, dely2b))  # 3
c_.append((delx2b, dely2a))  # 4
c_.append((delx2b, dely2b))  # 5
c_.append((delx2b, dely0))   # 6
c_.append((delx, dely0))     # 7

a_ = []
s_ = [None, None, None, None]
t_ = []

# end_coord

# begin_draw

cr.move_to(*c_[0])
cr.line_to(*c_[1])

cr.move_to(*c_[2])
cr.line_to(*c_[3])

cr.move_to(*c_[4])
cr.line_to(*c_[5])

cr.move_to(*c_[6])
cr.line_to(*c_[7])

cr.stroke()

# end_draw
