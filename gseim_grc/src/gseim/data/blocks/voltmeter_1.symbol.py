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

port_block_x = 6
port_block_y = 5

delxb2 = port_block_x*delta
delyb2 = port_block_y*delta

delx = 2*delxb2
dely = 2*delyb2

k1 = 0.2
k2 = 0.15
k3 = -0.2
k4 = 0.15
k5 = 0.5
k6 = 0.07
beta1 = -140
beta2 = -40

# opposite vertices of the rectangle: 0 and 1

x0 = 0
y0 = 0

x1 = delx
y1 = dely

x2 = x1
y2 = y0

x3 = x0
y3 = y1

# needle:

x4 = delxb2
y4 = dely - k1*dely

x5 = delxb2 + k3*delx
y5 = k1*dely

# centre of the circle:

x6 = x4 + k4*(x5-x4)
y6 = y4 + k4*(y5-y4)

# arc:

radius_arc = k5*dely
radius_circle = k6*dely

a_ = [-360, -180, 0, beta1, beta2]
s_ = [None, None, None, None]
t_ = [radius_circle, radius_arc]

c_ = []
c_.append((x0, y0))  # 0
c_.append((x1, y1))  # 1
c_.append((x2, y2))  # 2
c_.append((x3, y3))  # 3
c_.append((x4, y4))  # 4
c_.append((x5, y5))  # 5
c_.append((x6, y6))  # 6

# end_coord

# begin_draw

w = cr.get_line_width()
cr.set_line_width(0.7)
cr.move_to(*c_[0])
cr.line_to(*c_[2])
cr.line_to(*c_[1])
cr.line_to(*c_[3])
cr.line_to(*c_[0])

cr.stroke()
cr.set_line_width(w)

cr.move_to(*c_[4])
cr.line_to(*c_[5])
cr.stroke()

radius_circle = t_[0]

a1 = math.pi/180
alpha1 = a1*a_[0]
alpha2 = a1*a_[1]
alpha3 = a1*a_[2]

cr.set_line_width(0.5*w)

cr.arc(*c_[6], radius_circle, alpha1, alpha2)
cr.arc(*c_[6], radius_circle, alpha2, alpha3)

cr.set_source_rgb(1.0, 1.0, 1.0)
cr.fill_preserve()
cr.set_source_rgb(0.0, 0.0, 0.0)
cr.stroke()

radius_arc = t_[1]
cr.set_line_width(0.8*w)
cr.set_source_rgb(0.3, 0.3, 0.3)
beta1 = a1*a_[3]
beta2 = a1*a_[4]
cr.arc(*c_[6], radius_arc, beta1, beta2)
cr.stroke()

cr.set_line_width(w)

# end_draw
