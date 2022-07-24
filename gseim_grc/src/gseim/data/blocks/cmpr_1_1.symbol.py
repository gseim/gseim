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

port_block_x = 7
port_block_y = 7

delxb2 = delta*port_block_x
delyb2 = delta*port_block_y

delx = 2*delxb2
dely = 2*delyb2

k1x = 0.15
k2x = 0.15

k1y = 0.03

# input horizontal line
x1 = 0
y1 = delyb2

x2 = k1x*delx
y2 = y1

# comprator triangle
x3 = x2
y3 = dely - k1y*dely

x4 = x2
y4 = k1y*dely

x5 = delx - k2x*delx
y5 = delyb2

# output horizontal line (right node)
x6 = delx
y6 = y5

c_ = []
c_.append((x1, y1))  # 0
c_.append((x2, y2))  # 1
c_.append((x3, y3))  # 2
c_.append((x4, y4))  # 3
c_.append((x5, y5))  # 4
c_.append((x6, y6))  # 5

a_ = []
s_ = [None, None, None, None]
t_ = []

# end_coord

# begin_draw

w = cr.get_line_width()
cr.set_line_width(0.7*w)

cr.move_to(*c_[0])
cr.line_to(*c_[1])

cr.move_to(*c_[2])
cr.line_to(*c_[3])
cr.line_to(*c_[4])
cr.line_to(*c_[2])

cr.move_to(*c_[4])
cr.line_to(*c_[5])

cr.stroke()
cr.set_line_width(w)

# end_draw
