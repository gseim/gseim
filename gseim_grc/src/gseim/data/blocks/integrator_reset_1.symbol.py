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
port_block_y = 3
port_sep_y = 4

delxb2 = port_block_x*delta
delyb2 = delta*(port_block_y + port_sep_y)

delx = 2*delxb2
dely = 2*delyb2

kc = 0.5

k1x = 0.1
k2x = 0.05
k3x = 0.05

k1y = 0.6
k2y = 0.1
k3y = 0.05

r1 = delx/dely
k1y = k1y*r1
k2y = k2y*r1
k3y = k3y*r1

x1 = (kc - 0.5*k1x - k2x - k3x)*delx
y1 = (kc + 0.5*k1y + k3y)*dely

x2 = x1 + k3x*delx
y2 = (kc + 0.5*k1y + k2y)*dely

x3 = (kc - 0.5*k1x)*delx
y3 = (kc + 0.5*k1y)*dely

x6 = delx - x1
y6 = dely - y1

x5 = delx - x2
y5 = dely - y2

x4 = delx - x3
y4 = dely - y3

# rectangle:

x7 = 0
y7 = 0

x8 = delx
y8 = 0

x9 = delx
y9 = dely

x10 = 0
y10 = dely

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

a_ = []
s_ = [None, None, None, None]
t_ = []

# end_coord

# begin_draw

# rectangle
w = cr.get_line_width()
cr.set_line_width(0.7)

cr.move_to(*c_[6])
cr.line_to(*c_[7])
cr.line_to(*c_[8])
cr.line_to(*c_[9])
cr.line_to(*c_[6])
cr.stroke()

cr.set_line_width(w)

# integral symbol
cr.set_source_rgb(1.0, 0.24, 0.68)
cr.move_to(*c_[0])

for k in range(1, 6):
    cr.line_to(*c_[k])

cr.stroke()
# end_draw
