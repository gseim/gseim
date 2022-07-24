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

k1x = 0.08
k2x = 0.12

# rectangle:

x0 = 0
y0 = 0

x1 = delx
y1 = 0

x2 = delx
y2 = dely

x3 = 0
y3 = dely

# plus sign:

x4 = k1x*delx
y4 = port_block_y*delta

x5 = x4 + 2*k2x*delx
y5 = y4

x6 = x4 + k2x*delx
y6 = y4 + k2x*delx

x7 = x6
y7 = y4 - k2x*delx

# minus sign:

x8 = x4
y8 = y4 + 2*port_sep_y*delta

x9 = x5
y9 = y8

c_ = []
c_.append((x0, y0))  # 0
c_.append((x1, y1))  # 1
c_.append((x2, y2))  # 2
c_.append((x3, y3))  # 3
c_.append((x4, y4))  # 4
c_.append((x5, y5))  # 5
c_.append((x6, y6))  # 6
c_.append((x7, y7))  # 7
c_.append((x8, y8))  # 8
c_.append((x9, y9))  # 9

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

cr.move_to(*c_[6])
cr.line_to(*c_[7])

cr.move_to(*c_[8])
cr.line_to(*c_[9])

cr.stroke()
cr.set_line_width(w)

# end_draw
