"""
Copyright 2008-2011,2015 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

from __future__ import absolute_import

import numbers

from gi.repository import GLib
import cairo

from grc.gui.canvas.colors import FLOWGRAPH_BACKGROUND_COLOR
from . import Constants

def scale_1(x, y, xmin, xmax, ymin, ymax):

    xmin0 = min(x)
    xmax0 = max(x)
    ymin0 = min(y)
    ymax0 = max(y)

    kx = (xmax - xmin)/(xmax0 - xmin0)
    ky = (ymax - ymin)/(ymax0 - ymin0)

    cx = xmax - kx*xmax0
    cy = ymax - ky*ymax0

    lx = []; ly = []
    for x0 in x:
        x1 = kx*x0 + cx
        lx.append(x1)
    for y0 in y:
        y1 = ky*y0 + cy
        ly.append(y1)

    return lx, ly

def get_rotated_coordinate(coor, rotation):
    """
    Rotate the coordinate by the given rotation.

    Args:
        coor: the coordinate x, y tuple
        rotation: the angle in degrees

    Returns:
        the rotated coordinates
    """
    # handles negative angles
    rotation = (rotation + 360) % 360
    if rotation not in Constants.POSSIBLE_ROTATIONS:
        raise ValueError('unusable rotation angle "%s"'%str(rotation))

    x, y = coor

    if rotation == 0:
        return x , y
    elif rotation == 90:
       return y, -x
    elif rotation == 180:
       return -x, -y
    elif rotation == 270:
       return -y, x

def get_rotated_coordinate_1(coor, delx, dely, rotation):

    rotation = (rotation + 360) % 360
    x, y = coor

    if rotation == 0:
        return x , y
    elif rotation == 90:
       return y, -x + delx
    elif rotation == 180:
       return -x + delx, -y + dely
    elif rotation == 270:
       return -y + dely, x

def get_rotated_coordinate_2(coor, delx, dely, rotation, mirror):

    x, y = coor
    if mirror == 'v':
        coor = (delx-x, y)
    elif mirror == 'h':
        coor = (x, dely-y)

    rotation = (rotation + 360) % 360
    x, y = coor

    if rotation == 0:
        return x , y
    elif rotation == 90:
       return y, -x + delx
    elif rotation == 180:
       return -x + delx, -y + dely
    elif rotation == 270:
       return -y + dely, x

def get_scaling_factors(t, rotation):

    k = {0:0, 90:1, 180:2, 270:3}[rotation]
    return t[k]

def get_rotated_angle_1(theta, rotation):
    alpha = int(theta - rotation)
    if alpha > 360: alpha -= 360
    if alpha < -360: alpha += 360
    return alpha

def get_rotated_angle_2(theta, rotation, mirror):

    if mirror == 'none':
        theta1 = theta
    elif mirror == 'v':
        if theta >= 0:
           theta1 = 180 - theta
        else:
           theta1 = -(180 + theta)
    elif mirror == 'h':
        theta1 = -theta

    alpha = int(theta1 - rotation)

    if alpha > 180: alpha -= 360
    if alpha < -180: alpha += 360

    return alpha

def get_angle_from_coordinates(p1, p2):
    """
    Given two points, calculate the vector direction from point1 to point2, directions are multiples of 90 degrees.

    Args:
        (x1,y1): the coordinate of point 1
        (x2,y2): the coordinate of point 2

    Returns:
        the direction in degrees
    """
    (x1, y1) = p1
    (x2, y2) = p2
    if y1 == y2:  # 0 or 180
        return 0 if x2 > x1 else 180
    else:  # 90 or 270
        return 270 if y2 > y1 else 90

def align_to_grid(coor, mode=round):

    def align(value):
        return int(mode(value / (1.0 * Constants.CANVAS_GRID_SIZE)) * Constants.CANVAS_GRID_SIZE)
    try:
        return [align(c) for c in coor]
    except TypeError:
        x = coor
        return align(coor)

def align_to_grid_1(coor, mode=round):

    def align(value):
        return int(mode(value / (2.0 * Constants.CANVAS_GRID_SIZE)) * 2*Constants.CANVAS_GRID_SIZE)
    try:
        return [align(c) for c in coor]
    except TypeError:
        x = coor
        return align(coor)

def num_to_str(num):
    """ Display logic for numbers """
    def eng_notation(value, fmt='g'):
        """Convert a number to a string in engineering notation.  E.g., 5e-9 -> 5n"""
        template = '{:' + fmt + '}{}'
        magnitude = abs(value)
        for exp, symbol in zip(range(9, -15-1, -3), 'GMk munpf'):
            factor = 10 ** exp
            if magnitude >= factor:
                return template.format(value / factor, symbol.strip())
        return template.format(value, '')

    if isinstance(num, numbers.Complex):
        num = complex(num)  # Cast to python complex
        if num == 0:
            return '0'
        output = eng_notation(num.real) if num.real else ''
        output += eng_notation(num.imag, '+g' if output else 'g') + 'j' if num.imag else ''
        return output
    else:
        return str(num)

def encode(value):
    """Make sure that we pass only valid utf-8 strings into markup_escape_text.

    Older versions of glib seg fault if the last byte starts a multi-byte
    character.
    """

    valid_utf8 = value

    return GLib.markup_escape_text(valid_utf8)

def make_screenshot(flow_graph, file_path, transparent_bg=False):
    if not file_path:
        return

    x_min, y_min, x_max, y_max = flow_graph.get_extents_1()

    padding = Constants.CANVAS_GRID_SIZE
    width = x_max - x_min + 2 * padding
    height = y_max - y_min + 2 * padding

    if file_path.endswith('.png'):
        psurf = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
    elif file_path.endswith('.pdf'):
        psurf = cairo.PDFSurface(file_path, width, height)
    elif file_path.endswith('.svg'):
        psurf = cairo.SVGSurface(file_path, width, height)
    else:
        raise ValueError('Unknown file format')

    cr = cairo.Context(psurf)

#   cr.set_line_join(cairo.LineJoin.ROUND)

    if not transparent_bg:
        cr.set_source_rgba(*FLOWGRAPH_BACKGROUND_COLOR)
        cr.rectangle(0, 0, width, height)
        cr.fill()

    cr.translate(padding - x_min, padding - y_min)

    flow_graph.create_labels(cr)
    flow_graph.create_shapes()

    flow_graph.draw_1(cr)

    if file_path.endswith('.png'):
        psurf.write_to_png(file_path)
    if file_path.endswith('.pdf') or file_path.endswith('.svg'):
        cr.show_page()
    psurf.finish()

def scale(coor, reverse=False):
    factor = Constants.DPI_SCALING if not reverse else 1 / Constants.DPI_SCALING
    return tuple(int(x * factor) for x in coor)

def scale_scalar(coor, reverse=False):
    factor = Constants.DPI_SCALING if not reverse else 1 / Constants.DPI_SCALING
    return int(coor * factor)
