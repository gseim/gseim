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

delyb2 = port_block_y*delta

delx = 2*port_block_x*delta
dely = 2*delyb2

k1 = 0.25
k2 = 0.4

x0 = 0
y0 = delyb2

x1 = int(0.5*(1.0-k1)*delx)
y1 = delyb2

x2 = delx - x1
y2 = delyb2

x3 = delx
y3 = delyb2

x4 = x1
y4 = delyb2 - int(k2*dely)

x5 = x1
y5 = delyb2 + int(k2*dely)

x6 = x2
y6 = delyb2 - int(k2*dely)

x7 = x2
y7 = delyb2 + int(k2*dely)

c_ = []
c_.append((x0, y0))  # 0
c_.append((x1, y1))  # 1
c_.append((x2, y2))  # 2
c_.append((x3, y3))  # 3
c_.append((x4, y4))  # 4
c_.append((x5, y5))  # 5
c_.append((x6, y6))  # 6
c_.append((x7, y7))  # 7

a_ = []
s_ = [None, None, None, None]
t_ = []

# end_coord

# begin_draw

cr.move_to(*c_[0])
cr.line_to(*c_[1])

cr.move_to(*c_[2])
cr.line_to(*c_[3])

cr.move_to(*c_[1])
cr.line_to(*c_[4])
cr.line_to(*c_[2])
cr.line_to(*c_[5])
cr.line_to(*c_[1])

cr.move_to(*c_[6])
cr.line_to(*c_[7])

cr.stroke()

# end_draw
