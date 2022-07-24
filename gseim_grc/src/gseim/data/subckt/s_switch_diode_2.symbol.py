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

delta = 3

port_block_x = 6
port_block_y = 10

delxb2 = port_block_x*delta
delyb2 = port_block_y*delta

delx = 2*delxb2
dely = 2*delyb2

k1y = 0.19
k2y = 0.22
k4y = 0.22
k5y = 0.19

k3y = 1.0 - (k1y + k2y + k4y + k5y)

k6y = 0.35
k7y = (1.0 - 2*k6y)
k8y = 0.28
k9y = 0.15

#k1x = 0.2
#k2x = 0.6
#k3x = 0.15
#k4x = 0.18
#k5x = 0.09

#k1x = 0.4
k1x = 0.417
k2x = 0.45
k3x = 0.15
k4x = 0.18
k5x = 0.09

# diode horizontal lines:

x1 = k1x*delx
y1 = k1y*dely

x2 = (k1x + k2x)*delx
y2 = y1

x3 = x1
y3 = (1.0 - k5y)*dely

x4 = x2
y4 = y3

# diode vertical lines:

x5 = x2
y5 = (k1y + k2y)*dely

x6 = x2
y6 = (k1y + k2y + k3y)*dely

# diode symbol:

x7 = x5 - k3x*delx
y7 = y5

x8 = x5 + k3x*delx
y8 = y5

x9 = x7
y9 = y6

x10 = x8
y10 = y6

# switch vertical lines:

x11 = k1x*delx
y11 = 0

x12 = x11
y12 = k6y*dely

x13 = x11
y13 = y12 + k7y*dely

x14 = x11
y14 = dely

# switch throw:

x15 = x11 - k4x*delx
y15 = y12 + k8y*dely

x16 = 0.0
y16 = y12 + k9y*dely

x17 = x15 + k5x*delx
y17 = y16

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

radius = 2

a_ = [-360, -180, 0]
s_ = [None, None, None, None]
t_ = [radius]

# end_coord

# begin_draw

# diode:

cr.move_to(*c_[0])
cr.line_to(*c_[1])

cr.move_to(*c_[2])
cr.line_to(*c_[3])

cr.move_to(*c_[1])
cr.line_to(*c_[4])

cr.move_to(*c_[5])
cr.line_to(*c_[3])

cr.move_to(*c_[6])
cr.line_to(*c_[7])

cr.move_to(*c_[8])
cr.line_to(*c_[9])

cr.move_to(*c_[4])
cr.line_to(*c_[8])

cr.move_to(*c_[4])
cr.line_to(*c_[9])

# switch:

cr.move_to(*c_[10])
cr.line_to(*c_[11])

cr.move_to(*c_[12])
cr.line_to(*c_[13])

cr.move_to(*c_[11])
cr.line_to(*c_[14])

cr.stroke()

# circles:

radius = t_[0]
a1 = math.pi/180
alpha1 = a1*a_[0]
alpha2 = a1*a_[1]
alpha3 = a1*a_[2]

w = cr.get_line_width()
cr.set_line_width(0.5*w)

cr.arc(*c_[11], radius, alpha1, alpha2)
cr.arc(*c_[11], radius, alpha2, alpha3)

cr.set_source_rgb(1.0, 1.0, 1.0)
cr.fill_preserve()
cr.set_source_rgb(0.0, 0.0, 0.0)
cr.stroke()

cr.arc(*c_[12], radius, alpha1, alpha2)
cr.arc(*c_[12], radius, alpha2, alpha3)

cr.set_source_rgb(1.0, 1.0, 1.0)
cr.fill_preserve()
cr.set_source_rgb(0.0, 0.0, 0.0)
cr.stroke()

cr.set_line_width(0.6*w)
cr.set_dash([5.0, 2.0])

cr.move_to(*c_[15])
cr.line_to(*c_[16])
cr.stroke()

cr.set_line_width(w)

cr.set_dash([1.0, 0.0])

# end_draw
