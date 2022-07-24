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

port_sep_y = 4
port_block_x = 5
port_block_y = 3

delxb2 = delta*port_block_x
delyb2 = delta*(port_block_y + port_sep_y)

delx = 2*delxb2
dely = 2*delyb2

k1 = 0.4

x0 = 0
y0 = 0

x1 = delx
y1 = 0

x2 = delx
y2 = dely

x3 = 0
y3 = dely

w1 = k1*delx/2

x4 = delxb2 - w1
y4 = delyb2

x5 = delxb2 + w1
y5 = delyb2

c_ = []
c_.append((x0, y0))  # 0
c_.append((x1, y1))  # 1
c_.append((x2, y2))  # 2
c_.append((x3, y3))  # 3
c_.append((x4, y4))  # 4
c_.append((x5, y5))  # 5

a_ = []
s_ = [None, None, None, None]
t_ = []

# end_coord

# begin_draw

w = cr.get_line_width()
cr.set_line_width(0.7)
cr.move_to(*c_[0])
cr.line_to(*c_[1])
cr.line_to(*c_[2])
cr.line_to(*c_[3])
cr.line_to(*c_[0])
cr.stroke()

cr.set_source_rgb(1.0, 0.24, 0.68)
cr.set_line_width(0.7*w)

cr.move_to(*c_[4])
cr.line_to(*c_[5])

cr.stroke()
cr.set_line_width(w)

# end_draw
