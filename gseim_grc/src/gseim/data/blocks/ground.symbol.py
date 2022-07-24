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

port_block_x = 4
port_block_y = 4

delxb2 = port_block_x*delta
delx = 2*delxb2
dely = 2*port_block_y*delta

rad1 = int(round(0.5*delx))

k1y = 0.6
k1x = 0.3
k2x = 0.1

dely1 = 0.5*(1.0-k1y)*dely

y0 = k1y*dely
y1 = y0 + dely1
y2 = y1 + dely1

x0 = delx/2
x1_l = x0 - k1x*delx
x1_r = x0 + k1x*delx
x2_l = x0 - k2x*delx
x2_r = x0 + k2x*delx

c_ = []
c_.append((delxb2, 0))  # 0
c_.append((delxb2, y0)) # 1
c_.append((0, y0))      # 2
c_.append((delx, y0))   # 3
c_.append((x1_l, y1))   # 4
c_.append((x1_r, y1))   # 5
c_.append((x2_l, y2))   # 6
c_.append((x2_r, y2))   # 7

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
