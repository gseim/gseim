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
port_block_y = 10

delxb2 = port_block_x*delta
delyb2 = port_block_y*delta

delx = 2*delxb2
dely = 2*delyb2

radius = int(round(0.5*delx))

x0 = delxb2
y0 = delyb2
y1a = y0 - radius
y1b = y0 + radius

# V symbol:

k1v = 0.5*radius
k2v = 0.9*radius

c_ = []

# vertical lines (begin and end)
c_.append((x0, 0))    # 0
c_.append((x0, y1a))  # 1
c_.append((x0, y1b))  # 2
c_.append((x0, dely)) # 3

# circle
c_.append((x0, y0))  # 4
# Note: radius can be computed from 1 and 4

# point to move to before drawing the circle
c_.append((x0 + radius, y0))  # 5

# top left of V:
x2 = x0 - k1v
y2 = y0 - (k2v/2)

# top right of V:
x3 = x0 + k1v
y3 = y2

# bottom of V:
x4 = x0
y4 = y0 + (k2v/2)

c_.append((x2, y2))  # 6
c_.append((x3, y3))  # 7
c_.append((x4, y4))  # 8

radius_ = c_[4][1] - c_[1][1]

a_ = [-360, -180, 0]
s_ = [None, None, None, None]
t_ = [radius]

# end_coord

# begin_draw

cr.move_to(*c_[0])
cr.line_to(*c_[1])

cr.move_to(*c_[2])
cr.line_to(*c_[3])

cr.move_to(*c_[6])
cr.line_to(*c_[8])
cr.line_to(*c_[7])

radius = t_[0]
a1 = math.pi/180
alpha1 = a1*a_[0]
alpha2 = a1*a_[1]
alpha3 = a1*a_[2]

cr.move_to(*c_[5])

# cr.arc(*c_[4], radius, alpha1, alpha2)
cr.arc(*c_[4], radius, alpha1, alpha2)
cr.arc(*c_[4], radius, alpha2, alpha3)

cr.stroke()

# end_draw
