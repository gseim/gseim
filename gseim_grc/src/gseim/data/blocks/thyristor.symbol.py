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

port_block_x = 8
port_block_y = 10

delxb2 = port_block_x*delta
delyb2 = port_block_y*delta

delx = 2*delxb2
dely = 2*delyb2

k1 = 0.35
k2 = 0.25
k3a = 6

k4 = 0.2
k5 = 0.08

# 0,1,2,3,..,7: diode symbol part

x0 = delxb2
y0 = 0

x1 = delxb2
y1 = k1*dely

x2 = delxb2
y2 = y1 + k2*dely

x3 = delxb2
y3 = dely

x4 = delxb2 - int(k4*delx)
y4 = y1

x5 = delxb2 + int(k4*delx)
y5 = y1

x6 = delxb2 - int(k4*delx)
y6 = y2

x7 = delxb2 + int(k4*delx)
y7 = y2

# the thyristor leg

x8 = x4 - k5*delx
y8 = delyb2 + k3a*delta

x9 = 0
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

cr.move_to(*c_[2])
cr.line_to(*c_[8])
cr.line_to(*c_[9])

cr.stroke()

# end_draw
