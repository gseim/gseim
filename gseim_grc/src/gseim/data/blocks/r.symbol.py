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

delx = 2*port_block_x*delta
dely = 2*port_block_y*delta

delx1 = int(round(0.25*delx))
dely1 = int(round(0.2*dely))
dely0 = int(round(0.5*dely))

delx2 = int(round((delx-2*delx1)/6))
delx3_1 = delx1 + int(round(delx2/2))
delx3_2 = delx3_1 + delx2
delx3_3 = delx3_2 + delx2
delx3_4 = delx3_3 + delx2
delx3_5 = delx3_4 + delx2
delx3_6 = delx3_5 + delx2
delx3_7 = delx3_6 + int(round(delx2/2))

c_ = []
c_.append((0, dely0))
c_.append((delx1, dely0))
c_.append((delx3_1, dely0 - dely1))
c_.append((delx3_2, dely0 + dely1))
c_.append((delx3_3, dely0 - dely1))
c_.append((delx3_4, dely0 + dely1))
c_.append((delx3_5, dely0 - dely1))
c_.append((delx3_6, dely0 + dely1))
c_.append((delx3_7, dely0))
c_.append((delx, dely0))

a_ = []
s_ = [None, None, None, None]
t_ = []

# end_coord

# begin_draw

x, y = c_[0]
cr.move_to(x, y)

for k in range(1, len(c_)):
    x, y = c_[k]
    cr.line_to(x, y)

cr.stroke()

# end_draw
