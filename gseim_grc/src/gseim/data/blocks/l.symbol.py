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

port_block_x = 10
port_block_y = 4

delyb2 = port_block_y*delta

delx = 2*port_block_x*delta
dely = 2*delyb2

k1 = 0.5
k3 = 0.5
k4 = 0.3

delx1 = int(k3*delx/(4.0-3.0*k4))
delx2 = int(k4*delx1)

x0 = 0
x1 = int(0.5*(1.0-k3)*delx)
x2 = x1 + delx1
x3 = x2 - delx2
x4 = x3 + delx1
x5 = x4 - delx2
x6 = x5 + delx1
x7 = x6 - delx2
x8 = x7 + delx1
x9 = delx

c_ = []
c_.append((x0, delyb2))  # 0
c_.append((x1, delyb2))  # 1
c_.append((x2, delyb2))  # 2
c_.append((x3, delyb2))  # 3
c_.append((x4, delyb2))  # 4
c_.append((x5, delyb2))  # 5
c_.append((x6, delyb2))  # 6
c_.append((x7, delyb2))  # 7
c_.append((x8, delyb2))  # 8
c_.append((x9, delyb2))  # 9

scl1 = 1.5
scl2 = 2.5

cy = c_[1][1]

k = 1
for i in range(4):
    c_.append((c_[k][0], cy))
    cx = (c_[k][0] + c_[k+1][0])/2
    c_.append((cx, cy))

    if i == 3: break
    c_.append((c_[k+2][0], cy))
    cx = (c_[k+2][0] + c_[k+1][0])/2
    c_.append((cx, cy))

    k += 2

radius1 = 0.5*(c_[2][0] - c_[1][0])
radius2 = 0.5*(c_[2][0] - c_[3][0])

t_ = [radius1, radius2]

a_ = [180, 0]

s_ = [ \
  [( 1.0, scl1), (   1.0, 1/scl1), ( 1.0, scl2), (   1.0, 1/scl2)], \
  [(scl1,  1.0), (1/scl1,    1.0), (scl2,  1.0), (1/scl2,    1.0)], \
  [( 1.0, scl1), (   1.0, 1/scl1), ( 1.0, scl2), (   1.0, 1/scl2)], \
  [(scl1,  1.0), (1/scl1,    1.0), (scl2,  1.0), (1/scl2,    1.0)], \
]

# end_coord

# begin_draw

radius1, radius2 = t_

cr.move_to(*c_[0])
cr.line_to(*c_[1])

cr.move_to(*c_[8])
cr.line_to(*c_[9])

cr.stroke()

k = 1
a1 = math.pi/180
b1 = a1*a_[0]
b2 = a1*a_[1]

k1 = 10

for i in range(4):
    cr.scale(*s_[0])
    cr.move_to(c_[k1][0]/s_[0][0], c_[k1][1]/s_[0][1])
    k1 += 1
    cr.arc(c_[k1][0]/s_[0][0], c_[k1][1]/s_[0][1], radius1, b1, b2)
    k1 += 1
    cr.scale(*s_[1])
    cr.stroke()

    if i == 3: break
    cr.scale(*s_[2])
    cr.move_to(c_[k1][0]/s_[2][0], c_[k1][1]/s_[2][1])
    k1 += 1
    cr.arc_negative(c_[k1][0]/s_[2][0], c_[k1][1]/s_[2][1], radius2, b1, b2)
    k1 += 1
    cr.scale(*s_[3])
    cr.stroke()

    k += 2

#cr.stroke()

# end_draw
