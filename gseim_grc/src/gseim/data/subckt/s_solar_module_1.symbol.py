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

port_block_x = 5
port_block_y = 4

port_sep_x = 0
port_sep_y = 3

delxb2 = delta*port_block_x
delyb2 = delta*(port_block_y + port_sep_y)

delx = 2*delxb2
dely = 2*delyb2

k1y = 0.3

# opposite vertices of the rectangle: 0 and 1

x0 = 0
y0 = 0

x1 = delx
y1 = dely

x2 = x1
y2 = y0

x3 = x0
y3 = y1

x4 = delxb2
y4 = k1y*dely

# arc:

a_ = []
s_ = [None, None, None, None]
t_ = []

c_ = []
c_.append((x0, y0))  # 0
c_.append((x1, y1))  # 1
c_.append((x2, y2))  # 2
c_.append((x3, y3))  # 3
c_.append((x4, y4))  # 4

# end_coord

# begin_draw

w = cr.get_line_width()
cr.set_line_width(0.7)
cr.move_to(*c_[0])
cr.line_to(*c_[2])
cr.line_to(*c_[1])
cr.line_to(*c_[3])
cr.line_to(*c_[0])

cr.move_to(*c_[0])
cr.line_to(*c_[4])
cr.line_to(*c_[2])
cr.stroke()

cr.set_line_width(w)

# end_draw
