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

c_ = []
c_.append((0    , delta  ))
c_.append((delta, delta  ))
c_.append((delta, 2*delta))

a_ = []
s_ = [None, None, None, None]
t_ = []

# end_coord

# begin_draw

cr.move_to(*c_[0])
cr.line_to(*c_[1])
cr.line_to(*c_[2])

cr.stroke()

# end_draw
