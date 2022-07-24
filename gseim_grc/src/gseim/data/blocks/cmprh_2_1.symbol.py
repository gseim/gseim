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

#port_block_x = 7
#port_block_y = 3
#port_sep_y = 4

port_block_x = 10
port_block_y = 4
port_sep_y = 6

delxb2 = delta*port_block_x
delyb2 = delta*(port_block_y + port_sep_y)

delx = 2*delxb2
dely = 2*delyb2

k1x = 0.15
k2x = 0.15
k3x = 0.04
k4x = 0.08

k1y = 0.03
k2y = 0.04

# input top horizontal line
x1 = 0
y1 = port_block_y*delta

x2 = k1x*delx
y2 = y1

# input bottom horizontal line
x3 = x1
y3 = y1 + 2*port_sep_y*delta

x4 = x2
y4 = y3

# comparator triangle
x5 = x4
y5 = dely - k1y*dely

x6 = x4
y6 = k1y*dely

x7 = delx - k2x*delx
y7 = delyb2

# output horizontal line (right node)
x8 = delx
y8 = y7

# plus sign

x9 = x2 + k3x*delx
y9 = y2 + k2y*delx

x10 = x9 + 2*k4x*delx
y10 = y9

x11 = (x9 + x10)/2
y11 = y9 + k4x*delx

x12 = x11
y12 = y9 - k4x*delx

# minus sign
x13 = x9
y13 = y4 - k2y*dely

x14 = x10
y14 = y4 - k2y*dely

c_ = []
c_.append((x1, y1))  # 0
c_.append((x2, y2))  # 1
c_.append((x3, y3))  # 2
c_.append((x4, y4))  # 3
c_.append((x5, y5))  # 4
c_.append((x6, y6))  # 5
c_.append((x7, y7))  # 6
c_.append((x8, y8))  # 7
c_.append((x9, y9))  # 8
c_.append((x10, y10))  # 9
c_.append((x11, y11))  # 10
c_.append((x12, y12))  # 11
c_.append((x13, y13))  # 12
c_.append((x14, y14))  # 13

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

cr.move_to(*c_[4])
cr.line_to(*c_[5])
cr.line_to(*c_[6])
cr.line_to(*c_[4])

cr.move_to(*c_[6])
cr.line_to(*c_[7])
cr.stroke()

cr.set_source_rgb(1.0, 0.24, 0.68)
cr.move_to(*c_[8])
cr.line_to(*c_[9])

cr.move_to(*c_[10])
cr.line_to(*c_[11])

cr.move_to(*c_[12])
cr.line_to(*c_[13])

cr.stroke()

#cr.set_line_width(0.7*w)
cr.set_line_width(w)

# end_draw
