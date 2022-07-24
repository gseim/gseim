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

import numpy as np
#delta = 3
delta = Constants.CANVAS_GRID_SIZE

port_block_x = 8
port_block_y = 10

delxb2 = port_block_x*delta
delyb2 = port_block_y*delta

delx = 2*delxb2
dely = 2*delyb2

k1y = 0.16
k2y = 0.10

k3y = 0.08
k4y = 0.15

k1x = 0.25
k2x = 0.05
k3x = 0.5 - (k1x + k2x)

k4x = 0.09
k5x = 0.30

l_arrow = 0.10*delx
gamma = 20.0*np.pi/180.0

# vertical lines (stem):
x1 = (k1x + k2x + k3x)*delx
y1 = 0

x2 = x1
y2 = delyb2 - k1y*dely

x3 = x1
y3 = dely

x4 = x1
y4 = delyb2 + k1y*dely

# horizontal line
x5 = 0
y5 = delyb2

x6 = k1x*delx
y6 = y5

# vertical lines (in the symbol)
x7 = x6
y7 = y2

x8 = x6
y8 = y4

x9 = (k1x + k2x)*delx
y9 = y7

x10 = x9
y10 = y8

# slanted lines

x11 = x9
y11 = delyb2 - k2y*dely

x12 = x9
y12 = delyb2 + k2y*dely

alpha = np.arctan((k1y-k2y)*dely/(k3x*delx))
beta1 = alpha + gamma
beta2 = alpha - gamma

x13 = x4 - l_arrow*np.cos(beta1)
y13 = y4 - l_arrow*np.sin(beta1)

x14 = x4 - l_arrow*np.cos(beta2)
y14 = y4 + l_arrow*np.sin(beta2)

# horizontal lines to diode

x15 = delxb2
y15 = dely - k4y*dely

x16 = delxb2 + k5x*delx
y16 = y15

x17 = x15
y17 = k4y*dely

x18 = x16
y18 = y17

# horizontal lines to diode

x19 = x16
y19 = delyb2 - k3y*dely

x20 = x16
y20 = delyb2 + k3y*dely

# diode symbol: horizontal lines

x21 = x19 - k4x*delx
y21 = y19

x22 = x19 + k4x*delx
y22 = y19

x23 = x21
y23 = y20

x24 = x22
y24 = y20

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
c_.append((x15, y15))  # 14
c_.append((x16, y16))  # 15
c_.append((x17, y17))  # 16
c_.append((x18, y18))  # 17
c_.append((x19, y19))  # 18
c_.append((x20, y20))  # 19
c_.append((x21, y21))  # 20
c_.append((x22, y22))  # 21
c_.append((x23, y23))  # 22
c_.append((x24, y24))  # 23

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

cr.move_to(*c_[8])
cr.line_to(*c_[9])

cr.move_to(*c_[1])
cr.line_to(*c_[10])

cr.move_to(*c_[3])
cr.line_to(*c_[11])

cr.move_to(*c_[3])
cr.line_to(*c_[12])

cr.move_to(*c_[3])
cr.line_to(*c_[13])

cr.move_to(*c_[14])
cr.line_to(*c_[15])

cr.move_to(*c_[16])
cr.line_to(*c_[17])

cr.move_to(*c_[17])
cr.line_to(*c_[18])

cr.move_to(*c_[15])
cr.line_to(*c_[19])

cr.move_to(*c_[20])
cr.line_to(*c_[21])

cr.move_to(*c_[22])
cr.line_to(*c_[23])

cr.move_to(*c_[18])
cr.line_to(*c_[22])

cr.move_to(*c_[18])
cr.line_to(*c_[23])

cr.stroke()

# end_draw
