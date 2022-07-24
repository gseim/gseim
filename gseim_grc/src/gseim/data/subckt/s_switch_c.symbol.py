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

port_block_y = 10
port_block_x = 7

delyb2 = port_block_y*delta
delxb2 = port_block_x*delta

dely = 2*delyb2
delx = 2*delxb2

k1 = 0.25
k2 = 0.26
k3 = 0.26
k4 = 0.40

y0 = 0
x0 = delxb2

y1 = int(0.5*(1.0-k1)*dely)
x1 = delxb2

y2 = y1 + int(k1*dely)
x2 = delxb2

y3 = dely
x3 = delxb2

y4 = y2 - int(k2*dely)
x4 = delxb2 - int(k3*delx)

y5 = delyb2
x5 = 0

y6 = delyb2
x6 = int(k4*delx)

c_ = []
c_.append((x0, y0))  # 0
c_.append((x1, y1))  # 1
c_.append((x2, y2))  # 2
c_.append((x3, y3))  # 3
c_.append((x4, y4))  # 4
c_.append((x5, y5))  # 5
c_.append((x6, y6))  # 6

radius = 2

a_ = [-360, -180, 0]
s_ = [None, None, None, None]
t_ = [radius]

# end_coord

# begin_draw

cr.move_to(*c_[0])
cr.line_to(*c_[1])

cr.move_to(*c_[2])
cr.line_to(*c_[3])

cr.move_to(*c_[2])
cr.line_to(*c_[4])

cr.stroke()

radius = t_[0]
a1 = math.pi/180
alpha1 = a1*a_[0]
alpha2 = a1*a_[1]
alpha3 = a1*a_[2]

w = cr.get_line_width()
cr.set_line_width(0.5*w)

cr.arc(*c_[1], radius, alpha1, alpha2)
cr.arc(*c_[1], radius, alpha2, alpha3)

cr.set_source_rgb(1.0, 1.0, 1.0)
cr.fill_preserve()
cr.set_source_rgb(0.0, 0.0, 0.0)
cr.stroke()

cr.arc(*c_[2], radius, alpha1, alpha2)
cr.arc(*c_[2], radius, alpha2, alpha3)

cr.set_source_rgb(1.0, 1.0, 1.0)
cr.fill_preserve()
cr.set_source_rgb(0.0, 0.0, 0.0)
cr.stroke()

cr.set_line_width(0.6*w)
cr.set_dash([5.0, 2.0])

cr.move_to(*c_[5])
cr.line_to(*c_[6])
cr.stroke()

cr.set_line_width(w)

cr.set_dash([1.0, 0.0])

cr.stroke()

# end_draw
