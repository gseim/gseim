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

port_block_x = 4
port_block_y = 4

delxb2 = port_block_x*delta
delyb2 = port_block_y*delta

delx = 2*delxb2
dely = 2*delyb2

x0 = 0
y0 = 0

x1 = delx
y1 = dely

x2 = x1
y2 = y0

x3 = x0
y3 = y1

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

k1 = 0.32
k2 = 0.22
k1x = k1*delx
k1y = k2*dely

box_xmin = delxb2 - k1x
box_xmax = delxb2 + k1x
box_ymin = delyb2 - k1y
box_ymax = delyb2 + k1y

l_sine_x = [ x[0] for x in l_sine]
l_sine_y = [-x[1] for x in l_sine]

l_x, l_y = Utils.scale_1(l_sine_x, l_sine_y, box_xmin, box_xmax, box_ymin, box_ymax)

c_ = []
c_.append((x0, y0))  # 0
c_.append((x1, y1))  # 1
c_.append((x2, y2))  # 2
c_.append((x3, y3))  # 3

for x, y in zip(l_x, l_y):
    c_.append((x,y))
a_ = []
s_ = [None, None, None, None]
t_ = []

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

cr.set_source_rgb(1.0, 0.24, 0.68)
cr.set_line_width(0.7*w)

# draw the sine curve

cr.move_to(*c_[4])
for k in range(5, len(c_)):
    cr.line_to(*c_[k])

cr.stroke()
cr.set_line_width(w)

# end_draw
