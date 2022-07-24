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

port_sep_y = 20
port_block_x = 13
port_block_y = 5

delxb2 = delta*(port_block_x)
delyb2 = delta*(port_block_y + port_sep_y)

delx = 2*delxb2
dely = 2*delyb2

k1x = 0.09
k2x = 1.4*k1x
k3x = k1x
k4x = 0.15
k5x = 0.08

k1y = 0.3
k3y = 0.08
k2y = (1.0-2*k1y-k3y)/2
k4y = k1y + k2y - k3y

#print('delx:', delx)
#print('dely:', dely)
#
#print('k1y:', k1y)
#print('k2y:', k2y)
#print('k3y:', k3y)
#print('k4y:', k4y)
#
#print('k1x:', k1x)
#print('k2x:', k2x)
#print('k3x:', k3x)
#print('k4x:', k4x)
#print('k5x:', k5x)

# vertical lines (in the centre)

x1 = delxb2
y1 = k1y*dely

x2 = x1
y2 = (k1y + k2y)*dely

x3 = x1
y3 = (1.0 - k1y)*dely

x4 = x1
y4 = (1.0 - k1y - k2y)*dely

# diode symbol

x5 = delxb2 - k1x*delx
y5 = (k1y + k2y + k3y)*dely

x6 = delxb2 + k1x*delx
y6 = y5

x7 = delxb2 - k1x*delx
y7 = y2

x8 = delxb2 + k1x*delx
y8 = y2

# plus sign

x9 = delx - k5x*delx - k4x*delx
y9 = delyb2 - delta*port_sep_y

x10 = delx - k5x*delx
y10 = y9

x11 = delx - k5x*delx - 0.5*k4x*delx
y11 = y9 - 0.5*k4x*delx

x12 = x11
y12 = y9 + 0.5*k4x*delx

# minus sign

x13 = x9
y13 = delyb2 + delta*port_sep_y

x14 = x10
y14 = y13

# leg

x15 = delxb2 - k2x*delx
y15 = k4y*dely

x16 = x15 - k3x*delx
y16 = y15

# rectangle

x17 = 0
y17 = 0

x18 = delx
y18 = dely

x19 = x18
y19 = y17

x20 = x17
y20 = y18

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

a_ = []
s_ = [None, None, None, None]
t_ = []

# end_coord

# begin_draw

w = cr.get_line_width()
cr.set_line_width(0.7)
cr.move_to(*c_[16])
cr.line_to(*c_[18])
cr.line_to(*c_[17])
cr.line_to(*c_[19])
cr.line_to(*c_[16])

cr.stroke()
cr.set_line_width(w)

cr.move_to(*c_[0])
cr.line_to(*c_[1])

cr.move_to(*c_[2])
cr.line_to(*c_[3])

cr.move_to(*c_[4])
cr.line_to(*c_[5])

cr.move_to(*c_[1])
cr.line_to(*c_[4])

cr.move_to(*c_[1])
cr.line_to(*c_[5])

cr.move_to(*c_[6])
cr.line_to(*c_[7])

cr.move_to(*c_[8])
cr.line_to(*c_[9])

cr.move_to(*c_[10])
cr.line_to(*c_[11])

cr.move_to(*c_[12])
cr.line_to(*c_[13])

cr.move_to(*c_[1])
cr.line_to(*c_[14])

cr.move_to(*c_[14])
cr.line_to(*c_[15])

cr.stroke()

# end_draw
