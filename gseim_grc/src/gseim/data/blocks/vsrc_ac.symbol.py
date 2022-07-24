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

# fractions of the radius:
k1s = 0.6
k2s = 0.4

l_sine = [ \
  ( 0.000,  0.000), \
  ( 0.125,  0.707), \
  ( 0.250,  1.000), \
  ( 0.375,  0.707), \
  ( 0.500,  0.000), \
  ( 0.625, -0.707), \
  ( 0.750, -1.000), \
  ( 0.875, -0.707), \
  ( 1.000, -0.000), \
]

radius = int(round(0.5*delx))
k1x = k1s*radius
k1y = k2s*radius

x0 = delxb2
y0 = delyb2
y1a = y0 - radius
y1b = y0 + radius

box_xmin = x0 - k1x
box_xmax = x0 + k1x
box_ymin = y0 - k1y
box_ymax = y0 + k1y

l_sine_x = [ x[0] for x in l_sine]
l_sine_y = [-x[1] for x in l_sine]

l_x, l_y = Utils.scale_1(l_sine_x, l_sine_y, box_xmin, box_xmax, box_ymin, box_ymax)

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

for x, y in zip(l_x, l_y):
    c_.append((x,y))

a_ = [-360, -180, 0]
s_ = [None, None, None, None]
t_ = [radius]

# end_coord

# begin_draw

cr.move_to(*c_[0])
cr.line_to(*c_[1])

cr.move_to(*c_[2])
cr.line_to(*c_[3])

radius = t_[0]
a1 = math.pi/180
alpha1 = a1*a_[0]
alpha2 = a1*a_[1]
alpha3 = a1*a_[2]

cr.move_to(*c_[5])

# draw the circle in two parts to avoid alpha1 = alpha2 situation
cr.arc(*c_[4], radius, alpha1, alpha2)
cr.arc(*c_[4], radius, alpha2, alpha3)

# draw the sine curve

cr.move_to(*c_[6])
for k in range(7, len(c_)):
    cr.line_to(*c_[k])

cr.stroke()

# end_draw
