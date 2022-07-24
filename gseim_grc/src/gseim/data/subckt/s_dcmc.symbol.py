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

#port_block_x = 6
#port_block_y = 10

port_block_x = 8
port_block_y = 12

delxb2 = delta*port_block_x
delyb2 = delta*port_block_y

delx = 2*delxb2
dely = 2*delyb2

k1x = 0.2
#k2x = 0.8
k2x = 0.7
k3x = 0.3
k4x = 0.15

k1y = 0.6
k2y = 0.15

x1 = delxb2 - 0.5*k1x*delx
y1 = delyb2 - 0.5*k1y*dely

x1a = delxb2 + 0.5*k1x*delx
y1a = delyb2 + 0.5*k1y*dely

radius = 0.5*k2x*delx

# centre:

x2 = delxb2
y2 = delyb2

# point to move to before drawing the circle
x3 = x2 + radius
y3 = y2

x4 = k3x*delx - 0.5*k4x*delx
y4 = k2y*dely

x5 = k3x*delx + 0.5*k4x*delx
y5 = y4

x6 = k3x*delx
y6 = y4 - 0.5*k4x*delx

x7 = x6
y7 = y4 + 0.5*k4x*delx

x8 = x4
y8 = dely - k2y*dely

x9 = x5
y9 = y8

x10 = delxb2
y10 = 0

x11 = x10
y11 = 0.5*(1.0-k1y)*dely

x12 = delxb2
y12 = dely

x13 = x12
y13 = dely - 0.5*(1.0-k1y)*dely

x14 = 0
y14 = 0

x15 = delx
y15 = dely

c_ = []
c_.append((x1, y1))  # 0
#c_.append((dx, dy))  # 1
c_.append((x1a, y1a))  # 1
c_.append((x2, y2))  # 2
c_.append((x3, y3))  # 3
c_.append((x4, y4))  # 4
c_.append((x5, y5))  # 5
c_.append((x6, y6))  # 6
c_.append((x7, y7))  # 7
c_.append((x8, y8))  # 8
c_.append((x9, y9))  # 9
c_.append((x10, y10))  # 10
c_.append((x11, y11))  # 11
c_.append((x12, y12))  # 12
c_.append((x13, y13))  # 13
c_.append((x14, y14))  # 14
c_.append((x15, y15))  # 15

a_ = [-360, -180, 0]
t_ = [radius]
s_ = [None, None, None, None]

# end_coord

# begin_draw

dx = c_[1][0] - c_[0][0]
dy = c_[1][1] - c_[0][1]
cr.rectangle(*c_[0], dx, dy)
cr.fill()
cr.stroke()

radius = t_[0]
a1 = math.pi/180
alpha1 = a1*a_[0]
alpha2 = a1*a_[1]
alpha3 = a1*a_[2]

cr.set_source_rgb(1.0, 1.0, 1.0)
cr.move_to(*c_[3])
cr.arc(*c_[2], radius, alpha1, alpha2)
cr.arc(*c_[2], radius, alpha2, alpha3)
cr.fill()
cr.stroke()

cr.set_source_rgb(0.0, 0.0, 0.0)
cr.move_to(*c_[3])
cr.arc(*c_[2], radius, alpha1, alpha2)
cr.arc(*c_[2], radius, alpha2, alpha3)
cr.stroke()

cr.move_to(*c_[4])
cr.line_to(*c_[5])

cr.move_to(*c_[6])
cr.line_to(*c_[7])

cr.move_to(*c_[8])
cr.line_to(*c_[9])

cr.move_to(*c_[10])
cr.line_to(*c_[11])

cr.move_to(*c_[12])
cr.line_to(*c_[13])

cr.stroke()

w = cr.get_line_width()
cr.set_line_width(0.5)

dx = c_[15][0] - c_[14][0]
dy = c_[15][1] - c_[14][1]
cr.rectangle(*c_[14], dx, dy)
cr.stroke()

cr.set_line_width(w)
# end_draw
