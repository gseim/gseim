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
port_block_y = 5

delxb2 = port_block_x*delta
delyb2 = port_block_y*delta

delx = 2*delxb2
dely = 2*delyb2

k1x = 0.1
k2x = 0.35
k1y = 0.22

# axes:
x1 = k1x*delx
y1 = delyb2

x2 = (1.0 - k1x)*delx
y2 = delyb2

x3 = delxb2
y3 = k1x*dely

x4 = delxb2
y4 = (1.0 - k1x)*dely

# graph:
x5 = delxb2 - k2x*delx
y5 = delyb2 + k1y*dely

x6 = delxb2
y6 = y5

x7 = delxb2
y7 = delyb2 - k1y*dely

x8 = delxb2 + k2x*delx
y8 = delyb2 - k1y*dely

# rectangle:

x9 = 0
y9 = 0

x10 = delx
y10 = 0

x11 = delx
y11 = dely

x12 = 0
y12 = dely

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

a_ = []
s_ = [None, None, None, None]
t_ = []

# end_coord

# begin_draw

# rectangle
w = cr.get_line_width()
cr.set_line_width(0.7)

cr.move_to(*c_[8])
cr.line_to(*c_[9])
cr.line_to(*c_[10])
cr.line_to(*c_[11])
cr.line_to(*c_[8])
cr.stroke()

cr.move_to(*c_[0])
cr.line_to(*c_[1])

cr.set_source_rgb(0.5, 0.5, 0.5)

cr.move_to(*c_[2])
cr.line_to(*c_[3])
cr.stroke()

# graph:
cr.set_line_width(w)
cr.set_source_rgb(1.0, 0.24, 0.68)

cr.move_to(*c_[4])
cr.line_to(*c_[5])
cr.line_to(*c_[6])
cr.line_to(*c_[7])

cr.stroke()

# end_draw
