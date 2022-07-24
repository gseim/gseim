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

# w.r.t. vsrc_dc, make port_block_x larger, call it port_block_x_1
port_block_x_1 = 7
port_block_x = 5
port_block_y = 10

delxb2 = port_block_x*delta
delyb2 = port_block_y*delta

delx = 2*delxb2
dely = 2*delyb2

# this is the "extra" delx (as compared to vsrc_dc):
delx_1 = 2*(port_block_x_1-port_block_x)*delta

radius = int(round(0.5*delx))

# add delx_1 to all x values, no change in y is required

x0 = delxb2 + delx_1
y0 = delyb2
y1a = y0 - radius
y1b = y0 + radius

c_ = []

# vertical lines (begin and end)
c_.append((x0, 0))    # 0
c_.append((x0, y1a))  # 1
c_.append((x0, y1b))  # 2
c_.append((x0, dely)) # 3

# circle
c_.append((x0, y0))  # 4
# Note: radius can be computed from 1 and 4

# these are w.r.t. radius:
k1y = 0.3*radius
k2y = 0.35*radius
k3y = 0.7*radius
k1x = 0.25*radius

# arrow stem
c_.append((x0, y0+k3y))      # 5
c_.append((x0, y0-k1y-k2y))  # 6

# arrow tip
c_.append((x0-k1x, y0-k2y))  # 7
c_.append((x0, y0-k1y-k2y))  # 8
c_.append((x0+k1x, y0-k2y))  # 9

# point to move to before drawing the circle
c_.append((x0 + radius, y0)) # 10

# straight line from x to circle:
c_.append((0, y0))           # 11
c_.append((delx_1, y0))      # 12

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

cr.move_to(*c_[5])
cr.line_to(*c_[6])

cr.move_to(*c_[7])
cr.line_to(*c_[8])
cr.line_to(*c_[9])

# syntax: arc(cx, cy, radius, start_angle, stop_angle)
radius = t_[0]
a1 = math.pi/180
alpha1 = a1*a_[0]
alpha2 = a1*a_[1]
alpha3 = a1*a_[2]

#print('vsrc_dc symbol: radius:', radius, 'alpha1:', alpha1, 'alpha2:', alpha2)

cr.move_to(*c_[10])

# draw the circle in two parts to avoid alpha1 = alpha2 situation
# cr.arc(*c_[4], radius, alpha1, alpha2)
cr.arc(*c_[4], radius, alpha1, alpha2)
cr.arc(*c_[4], radius, alpha2, alpha3)

# straight line from x to circle
cr.move_to(*c_[11])
cr.line_to(*c_[12])

cr.stroke()

# end_draw
