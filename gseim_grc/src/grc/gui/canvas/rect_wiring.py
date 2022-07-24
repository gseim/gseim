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

import sys

def east_of(p1, p2):
    if p1[0] > p2[0]:
        return p1
    else:
        return p2

def west_of(p1, p2):
    if p1[0] < p2[0]:
        return p1
    else:
        return p2

def north_of(p1, p2):
    if p1[1] < p2[1]:
        return p1
    else:
        return p2

def south_of(p1, p2):
    if p1[1] > p2[1]:
        return p1
    else:
        return p2

def block_rel_pos_x(b1,b2):

#   b1, b2 have the form ((x,y), L, W)
#   (x,y) is the top left position of the block (including the margins).
#   L and W are the horizontal and vertical dimensions, respectively.
#   Note: y-axis direction in the GUI is opp of the normal direction
#     we will follow the same convention here.
#   dely_12: positive if the S edge of b1 is above the N edge of b2.
#   delx_12: positive if the W edge of b1 is on the right of the E edge of b2.

    b1_L = b1[1]
    b2_L = b2[1]

    x_b1_west  = b1[0][0]
    x_b1_east  = x_b1_west + b1_L

    x_b2_west  = b2[0][0]
    x_b2_east  = x_b2_west + b2_L

    delx_b12 = x_b1_west - x_b2_east
    delx_b21 = x_b2_west - x_b1_east

    return delx_b12, delx_b21

def block_rel_pos_y(b1,b2):

    b1_W = b1[2]
    b2_W = b2[2]

    y_b1_north = b1[0][1]
    y_b1_south = y_b1_north + b1_W

    y_b2_north = b2[0][1]
    y_b2_south = y_b2_north + b2_W

    dely_b12 = y_b2_north - y_b1_south
    dely_b21 = y_b1_north - y_b2_south

    return dely_b12, dely_b21

def make_block_1(p0, L0, W0, port_width, delta1):

#   p0 = (x0,y0): top left corner of gseim block
#   L0, W0: horizontal and vertical dimensions of the gseim block
#     (AFTER rotation, if any)
#   delta1: min length of connector starting from the port in the same
#     direction as the port
#   return block in the form ((x,y), L, W)

    delta = port_width + delta1
    x0p = p0[0] - delta
    y0p = p0[1] - delta
    L0p = L0 + 2*delta
    W0p = W0 + 2*delta

    return ((x0p,y0p), L0p, W0p)

def get_wire(b1, cc1, dir1, draw_arrow1, b2, cc2, dir2, draw_arrow2,
    delta1, arrow_h, arrow_w):

    flag_flip = False

    if dir1 in ('N', 'S') and dir2 in ('E', 'W'):
        flag_flip = True
    elif dir1 == 'E' and dir2 == 'W':
        flag_flip = True
    elif dir1 == 'N' and dir2 == 'S':
        flag_flip = True
    elif dir1 == 'E' and dir2 == 'E':
        if west_of(cc1, cc2) == cc2:
            flag_flip = True
    elif dir1 == 'W' and dir2 == 'W':
        if east_of(cc1, cc2) == cc2:
            flag_flip = True
    elif dir1 == 'N' and dir2 == 'N':
        if south_of(cc1, cc2) == cc2:
            flag_flip = True
    elif dir1 == 'S' and dir2 == 'S':
        if north_of(cc1, cc2) == cc2:
            flag_flip = True

    if draw_arrow1 and draw_arrow2:
        print('get_wire: draw_arrow1 and draw_arrow2 cannot both be True. Halting...')
        sys.exit()

    if flag_flip:
        block1, c1, direction1, arrow1 = (b2, cc2, dir2, draw_arrow2)
        block2, c2, direction2, arrow2 = (b1, cc1, dir1, draw_arrow1)
    else:
        block1, c1, direction1, arrow1 = (b1, cc1, dir1, draw_arrow1)
        block2, c2, direction2, arrow2 = (b2, cc2, dir2, draw_arrow2)

    xc1, yc1 = c1
    xc2, yc2 = c2

    block1_N = block1[0][1]
    block1_S = block1_N + block1[2]
    block1_W = block1[0][0]
    block1_E = block1_W + block1[1]

    block2_N = block2[0][1]
    block2_S = block2_N + block2[2]
    block2_W = block2[0][0]
    block2_E = block2_W + block2[1]

    if direction1 == 'E':
        x1p, y1p = c1p = (xc1+delta1, yc1)
    elif direction1 == 'W':
        x1p, y1p = c1p = (xc1-delta1, yc1)
    elif direction1 == 'S':
        x1p, y1p = c1p = (xc1, yc1+delta1)
    elif direction1 == 'N':
        x1p, y1p = c1p = (xc1, yc1-delta1)

    if direction2 == 'E':
        x2p, y2p = c2p = (xc2+delta1, yc2)
    elif direction2 == 'W':
        x2p, y2p = c2p = (xc2-delta1, yc2)
    elif direction2 == 'S':
        x2p, y2p = c2p = (xc2, yc2+delta1)
    elif direction2 == 'N':
        x2p, y2p = c2p = (xc2, yc2-delta1)

    l_middle = []

#   split the connection into three parts: l_begin, l_middle, l_end
#   l_begin, l_end are independent of the block locations

    dely_b12, dely_b21 = block_rel_pos_y(block1,block2)
    delx_b12, delx_b21 = block_rel_pos_x(block1,block2)

    if direction1 == 'E':
        if direction2 == 'S':
            if east_of(c1p, c2p) == c2p:
                if north_of(c1p, c2p) == c2p: # NE b1E b2S
                    l_middle.append((x2p, y1p))
                else: # SE b1E b2S
                    if dely_b12 > 0 or delx_b21 > 0:
                        if delx_b21 > 0:
                            xp = block1_E + int(delx_b21/2)
                            l_middle.append((xp, y1p))
                            l_middle.append((xp, y2p))
                        else:
                            yp = block1_S + int(dely_b12/2)
                            l_middle.append((x1p, yp))
                            l_middle.append((block2_W, yp))
                            l_middle.append((block2_W, y2p))
            else:
                if north_of(c1p, c2p) == c2p: # NW b1E b2S
                    if dely_b21 > 0:
                        yp = block2_S + int(dely_b21/2)
                        l_middle.append((x1p, yp))
                        l_middle.append((x2p, yp))
                    else:
                        if delx_b12 > 0:
                            yp = max(block1_S, block2_S)
                            l_middle.append((x1p, yp))
                            l_middle.append((x2p, yp))
                else: # SW b1E b2S
                    if dely_b12 > 0:
                        if block1_E > block2_E:
                            if block2_S > block1_S:
                                l_middle.append((x1p, block2_S))
                            else:
                                l_middle.append((x1p, block1_S))
                                l_middle.append((x2p, block1_S))
                        else:
                            yp = block1_S + int(dely_b12/2)
                            l_middle.append((x1p, yp))
                            l_middle.append((block2_E, yp))
                            l_middle.append((block2_E, y2p))
                    else:
                        if delx_b12 > 0:
                            if block1_S > block2_S:
                                l_middle.append((x1p, block1_S))
                                l_middle.append((x2p, block1_S))
                            else:
                                l_middle.append((x1p, y2p))
        elif direction2 == 'N':
            if east_of(c1p, c2p) == c2p:
                if south_of(c1p, c2p) == c2p: # SE b1E b2N
                    if delx_b21 > 0:
                        l_middle.append((x2p, y1p))
                    elif dely_b12 > 0:
                        l_middle.append((x2p, y1p))
                else: # NE b1E b2N
                    if delx_b21 > 0:
                        xp = block1_E + int(delx_b21/2)
                        l_middle.append((xp, y1p))
                        l_middle.append((xp, y2p))
                    elif dely_b21 > 0:
                        l_middle.append((block2_E, y1p))
                        l_middle.append((block2_E, y2p))
            else:
                if south_of(c1p, c2p) == c2p: # SW b1E b2N
                    if delx_b12 > 0 or dely_b12 > 0:
                        if dely_b12 > 0:
                            yp = block1_S + int(dely_b12/2)
                            l_middle.append((x1p, yp))
                            l_middle.append((x2p, yp))
                        else:
                            yp = min(block1_N, block2_N)
                            l_middle.append((x1p, yp))
                            l_middle.append((x2p, yp))
                else: # NW b1E b2N
                    if delx_b12 > 0 or dely_b21 > 0:
                        if block2_N > block1_N:
                            if block2_E > block1_E:
                                l_middle.append((block2_E, y1p))
                                l_middle.append((block2_E, y2p))
                            else:
                                l_middle.append((x1p, block1_N))
                                l_middle.append((x2p, block1_N))
                        else:
                            if block2_E > block1_E:
                                l_middle.append((block2_E, y1p))
                                l_middle.append((block2_E, y2p))
                            else:
                                l_middle.append((x1p, y2p))
        elif direction2 == 'E': # b1E b2E
            if dely_b12 > 0 or dely_b21 > 0 or delx_b12 > 0 or delx_b21 > 0:
                if block2_S < y1p or block2_N > y1p:
                    l_middle.append((x2p, y1p))
                else:
                    if block2_W < x1p:
                        l_middle.append((x2p, y1p))
                    else:
                        delx = block2_W - x1p
                        xp = x1p + int(delx/2)
                        l_middle.append((xp, y1p))
                        l_middle.append((xp, block2_S))
                        l_middle.append((x2p, block2_S))
    elif direction1 == 'W':
        if direction2 == 'S':
            if delx_b12 > 0 or delx_b21 > 0 or dely_b12 > 0 or dely_b21 > 0:
                if west_of(c1p, c2p) == c2p:
                    if north_of(c1p, c2p) == c2p: # NW b1W b2S
                        l_middle.append((x2p, y1p))
                    else: # SW b1W b2S
                        if delx_b12 > 0:
                            xp = block2_E + int(delx_b12/2)
                            l_middle.append((xp, y1p))
                            l_middle.append((xp, y2p))
                        else:
                            if block2_N > y1p:
                                l_middle.append((block2_W, y1p))
                                l_middle.append((block2_W, y2p))
                            else:
                                l_middle.append((x1p, y2p))
                else:
                    if north_of(c1p, c2p) == c2p: # NE b1W b2S
                        if dely_b21 > 0:
                            yp = block2_S + int(dely_b21/2)
                            l_middle.append((x1p, yp))
                            l_middle.append((x2p, yp))
                        else:
                            if delx_b21 > 0:
                                xp = block1_E + int(delx_b21/2)
                                l_middle.append((x1p, block1_N))
                                l_middle.append((xp, block1_N))
                                l_middle.append((xp, y2p))
                    else: # SE b1W b2S
                        if block2_W < block1_W:
                            if block2_S > block1_S:
                                yp = block1_S + int(dely_b12/2)
                                l_middle.append((x1p, yp))
                                l_middle.append((block2_W, yp))
                                l_middle.append((block2_W, y2p))
                        else:
                            if block2_S > block1_S:
                                l_middle.append((x1p, y2p))
                            else:
                                l_middle.append((x1p, block1_S))
                                l_middle.append((x2p, block1_S))
        elif direction2 == 'N':
            if delx_b12 > 0 or delx_b21 > 0 or dely_b12 > 0 or dely_b21 > 0:
                if west_of(c1p, c2p) == c2p:
                    if south_of(c1p, c2p) == c2p: # SW b1W b2N
                        l_middle.append((x2p, y1p))
                    else: # NW b1W b2N
                        if delx_b12 > 0:
                            xp = block2_E + int(delx_b12/2)
                            l_middle.append((xp, y1p))
                            l_middle.append((xp, y2p))
                        else:
                            l_middle.append((block2_W, y1p))
                            l_middle.append((block2_W, y2p))
                else:
                    if south_of(c1p, c2p) == c2p: # SE b1W b2N
                        if dely_b12 > 0:
                            yp = block1_S + int(dely_b12/2)
                            l_middle.append((x1p, yp))
                            l_middle.append((x2p, yp))
                        else:
                            if delx_b21 > 0:
                                xp = block1_E + int(delx_b21/2)
                                l_middle.append((x1p, block1_S))
                                l_middle.append((xp, block1_S))
                                l_middle.append((xp, y2p))
                            else:
                                yp = max(block1_S, block2_S)
                                l_middle.append((x1p, yp))
                                l_middle.append((block2_E, yp))
                                l_middle.append((block2_E, y2p))
                    else: # NE b1W b2N
                        if block1_N < block2_N:
                            l_middle.append((x1p, block1_N))
                            l_middle.append((x2p, block1_N))
                        else:
                            if block1_W < block2_W:
                                l_middle.append((x1p, y2p))
                            else:
                                l_middle.append((block2_W, y1p))
                                l_middle.append((block2_W, y2p))
        elif direction2 == 'E':
            if delx_b12 > 0 or delx_b21 > 0 or dely_b12 > 0 or dely_b21 > 0:
                if west_of(c1p, c2p) == c2p: # NW and SW b1W b2E
                    delx = x1p - x2p
                    xp = x2p + int(delx/2)
                    l_middle.append((xp, y1p))
                    l_middle.append((xp, y2p))
                else:
                    if north_of(c1p, c2p) == c2p: # NE b1W b2E
                        if dely_b21 > 0:
                            yp = block2_S + int(dely_b21/2)
                            l_middle.append((x1p, yp))
                            l_middle.append((x2p, yp))
                        else:
                            yp = min(block1_N, block2_N)
                            if block2_W < x1p:
                                l_middle.append((block2_W, y1p))
                                l_middle.append((block2_W, yp))
                                l_middle.append((x2p, yp))
                            else:
                                l_middle.append((x1p, yp))
                                l_middle.append((x2p, yp))
                    else: # SE b1W b2E
                        if dely_b12 > 0:
                            yp = block1_S + int(dely_b12/2)
                            l_middle.append((x1p, yp))
                            l_middle.append((x2p, yp))
                        else:
                            yp = max(block1_S, block2_S)
                            if block2_W < x1p:
                                l_middle.append((block2_W, y1p))
                                l_middle.append((block2_W, yp))
                                l_middle.append((x2p, yp))
                            else:
                                l_middle.append((x1p, yp))
                                l_middle.append((x2p, yp))
            else:
                if x1p > x2p:
                    if y2p < y1p:
                        yp = min(block1_N, block2_N)
                        l_middle.append((x1p, yp))
                        l_middle.append((x2p, yp))
                    else:
                        yp = max(block1_S, block2_S)
                        l_middle.append((x1p, yp))
                        l_middle.append((x2p, yp))
        elif direction2 == 'W': # b1W b2W
            if dely_b12 > 0 or dely_b21 > 0 or delx_b12 > 0 or delx_b21 > 0:
                if block2_S < y1p or block2_N > y1p:
                    l_middle.append((x2p, y1p))
                else:
                    if block2_E > x1p:
                        l_middle.append((x2p, y1p))
                    else:
                        delx = x1p - block2_E
                        xp = block2_E + int(delx/2)
                        l_middle.append((xp, y1p))
                        l_middle.append((xp, block2_S))
                        l_middle.append((x2p, block2_S))
    elif direction1 == 'S':
        if direction2 == 'N':
            if dely_b12 > 0 or dely_b21 > 0 or delx_b12 > 0 or delx_b21 > 0:
                if south_of(c1p, c2p) == c2p: # SW and SE b1S b2N
                    dely = y2p - y1p
                    yp = y1p + int(dely/2)
                    l_middle.append((x1p, yp))
                    l_middle.append((x2p, yp))
                else:
                    if east_of(c1p, c2p) == c2p: # NE b1S b2N
                        if delx_b21 > 0:
                            xp = block1_E + int(delx_b21/2)
                            l_middle.append((xp, y1p))
                            l_middle.append((xp, y2p))
                        else:
                            xp = max(block1_E, block2_E)
                            if block2_S > y1p:
                                l_middle.append((x1p, block2_S))
                                l_middle.append((xp, block2_S))
                                l_middle.append((xp, y2p))
                            else:
                                l_middle.append((xp, y1p))
                                l_middle.append((xp, y2p))
                    else: # NW b1S b2N
                        if delx_b12 > 0:
                            xp = block2_E + int(delx_b12/2)
                            l_middle.append((xp, y1p))
                            l_middle.append((xp, y2p))
                        else:
                            xp = min(block1_W, block2_W)
                            if block2_S > y1p:
                                l_middle.append((x1p, block2_S))
                                l_middle.append((xp, block2_S))
                                l_middle.append((xp, y2p))
                            else:
                                l_middle.append((xp, y1p))
                                l_middle.append((xp, y2p))
            else:
                if y1p < y2p:
                    if x2p > x1p:
                        xp = min(block1_W, block2_W)
                        l_middle.append((xp, y1p))
                        l_middle.append((xp, y2p))
                    else:
                        xp = max(block1_E, block2_E)
                        l_middle.append((xp, y1p))
                        l_middle.append((xp, y2p))
        elif direction2 == 'S': # b1S b2S
            if dely_b12 > 0 or dely_b21 > 0 or delx_b12 > 0 or delx_b21 > 0:
                if block2_W > x1p or block2_E < x1p:
                    l_middle.append((x1p, y2p))
                else:
                    if block2_N < y1p:
                        l_middle.append((x1p, y2p))
                    else:
                        dely = block2_N - y1p
                        yp = block2_N - int(dely/2)
                        l_middle.append((x1p, yp))
                        l_middle.append((block2_W, yp))
                        l_middle.append((block2_W, y2p))
    elif direction1 == 'N':
        if direction2 == 'N': # b1N b2N
            if dely_b12 > 0 or dely_b21 > 0 or delx_b12 > 0 or delx_b21 > 0:
                if block2_W > x1p or block2_E < x1p:
                    l_middle.append((x1p, y2p))
                else:
                    if block2_S > y1p:
                        l_middle.append((x1p, y2p))
                    else:
                        dely = y1p - block2_S
                        yp = block2_S + int(dely/2)
                        l_middle.append((x1p, yp))
                        l_middle.append((block2_W, yp))
                        l_middle.append((block2_W, y2p))

    if not l_middle:
        l_corners = [(x1p, y2p), (x2p, y1p)]
        flag_added_corner = False
        for P in l_corners:
            xp, yp = P

            if direction1 in ('E', 'W'):
                p1 = (x1p - xp)*(x1p - xc1)
            else:
                p1 = (y1p - yp)*(y1p - yc1)

            if direction2 in ('E', 'W'):
                p2 = (x2p - xp)*(x2p - xc2)
            else:
                p2 = (y2p - yp)*(y2p - yc2)

            if p1 > 0 or p2 > 0:
                continue
            else:
                if p1 < 0 or p2 < 0:
                    l_middle.append(P)
                    flag_added_corner = True
                    break
                else:
                    continue

        if not flag_added_corner:

            if xc1 == xc2:
                c2p = [x2p, y1p]
            elif yc1 == yc2:
                c2p = [x1p, y2p]

    l_begin = [c1, c1p]
    l_end   = [c2p, c2]

    l_arrow1 = []
    l_arrow2 = []

    if arrow1:
        if direction1 == 'W':
            l_arrow1.append((xc1-arrow_h, yc1-arrow_w))
            l_arrow1.append((xc1        , yc1        ))
            l_arrow1.append((xc1-arrow_h, yc1+arrow_w))
        elif direction1 == 'E':
            l_arrow1.append((xc1+arrow_h, yc1-arrow_w))
            l_arrow1.append((xc1        , yc1        ))
            l_arrow1.append((xc1+arrow_h, yc1+arrow_w))
        elif direction1 == 'S':
            l_arrow1.append((xc1-arrow_w, yc1+arrow_h))
            l_arrow1.append((xc1        , yc1        ))
            l_arrow1.append((xc1+arrow_w, yc1+arrow_h))
        elif direction1 == 'N':
            l_arrow1.append((xc1-arrow_w, yc1-arrow_h))
            l_arrow1.append((xc1        , yc1        ))
            l_arrow1.append((xc1+arrow_w, yc1-arrow_h))

    if arrow2:
        if direction2 == 'W':
            l_arrow1.append((xc2-arrow_h, yc2-arrow_w))
            l_arrow1.append((xc2        , yc2        ))
            l_arrow1.append((xc2-arrow_h, yc2+arrow_w))
        elif direction2 == 'E':
            l_arrow1.append((xc2+arrow_h, yc2-arrow_w))
            l_arrow1.append((xc2        , yc2        ))
            l_arrow1.append((xc2+arrow_h, yc2+arrow_w))
        elif direction2 == 'S':
            l_arrow1.append((xc2-arrow_w, yc2+arrow_h))
            l_arrow1.append((xc2        , yc2        ))
            l_arrow1.append((xc2+arrow_w, yc2+arrow_h))
        elif direction2 == 'N':
            l_arrow1.append((xc2-arrow_w, yc2-arrow_h))
            l_arrow1.append((xc2        , yc2        ))
            l_arrow1.append((xc2+arrow_w, yc2-arrow_h))

    l = l_begin + l_middle + l_end

    if flag_flip:
        l.reverse()

    if l_arrow1:
        return l, l_arrow1
    elif l_arrow2:
        return l, l_arrow2
    else:
        return l, None
