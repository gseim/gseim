"""
Copyright 2007, 2008, 2009 Free Software Foundation, Inc.
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

from __future__ import absolute_import, division

from argparse import Namespace
from math import pi

from . import colors
from .drawable import Drawable
from grc.gui import Utils
from grc.gui.Constants import (
    CONNECTOR_ARROW_BASE,
    CONNECTOR_ARROW_HEIGHT,
    GR_MESSAGE_DOMAIN,
    LINE_SELECT_SENSITIVITY,
    CONNECTOR_LINE_WIDTH,
    CONNECTOR_LINE_WIDTH_FACTOR,
)
from grc.core.Connection import Connection as CoreConnection
from grc.core.utils.descriptors import nop_write

import sys

import cairo
from grc.gui import Constants

from . import rect_wiring as rwire

class Connection(CoreConnection, Drawable):
    """
    A graphical connection for ports.
    The connection has 2 parts, the arrow and the wire.
    The coloring of the arrow and wire exposes the status of 3 states:
        enabled/disabled, valid/invalid, highlighted/non-highlighted.
    The wire coloring exposes the enabled and highlighted states.
    The arrow coloring exposes the enabled and valid states.
    """

    def __init__(self, *args, **kwargs):
        super(self.__class__, self).__init__(*args, **kwargs)
        Drawable.__init__(self)

        self._line = []
        self._line_width_factor = 1.0
        self._color1 = self._color2 = None

        self._current_port_rotations = self._current_coordinates = None

        self._rel_points = None  # connection coordinates relative to sink/source
        self._arrow_rotation = 0.0  # rotation of the arrow in radians
        self._current_cr = None  # for what_is_selected() of curved line
        self._line_path = None

        self._l_wire = []
        self._l_wire_src = []
        self._l_arrow = []
        self._l_arrow_src = []

    @nop_write
    @property
    def coordinate(self):
        return self.source_port.connector_coordinate_absolute

    @nop_write
    @property
    def rotation(self):
        """
        Get the 0 degree rotation.
        Rotations are irrelevant in connection.

        Returns:
            0
        """
        return 0

    def create_shapes(self):
        """Pre-calculate relative coordinates."""

        source = self.source_port
        sink = self.sink_port
        rotate = Utils.get_rotated_coordinate

        # first two components relative to source connector, rest relative to sink connector

        self.wiring_style = source.parent_flowgraph.options_block.params['wiring_style'].get_value()

        source_mirror = source.parent_block.mirror
        source_rotation = source.parent_block.rotation

        if 'Namespace' not in str(type(sink.parent_block)):
            sink_rotation = sink.parent_block.rotation
            sink_mirror = sink.parent_block.mirror
        else:
            sink_rotation = 0
            sink_mirror = 'none'

        if self.wiring_style == 'curved':
            if source.port_type == 'flowgraph':
                if source.parent_block.name.startswith('connector_f'):
                    index1 = source.port_index
                    pos = source.parent_block.params['output' + str(int(index1)+1)].get_value()

                    if pos == 'top':
                        px_source = ((  0,-15), (  0,-50))
                    elif pos == 'bottom':
                        px_source = ((  0, 15), (  0, 50))
                    elif pos == 'right':
                        px_source = (( 15,  0), ( 50,  0))

                    self._rel_points = [
                        rotate(px_source[0], source.rotation),
                        rotate(px_source[1], source.rotation),
                        rotate((-50, 0), sink.rotation),
                        rotate((-15, 0), sink.rotation),
                        rotate((-CONNECTOR_ARROW_HEIGHT, 0), sink.rotation),
                    ]
                    r_sink = sink.rotation
                else:
                    if source_mirror == 'none' and sink_mirror == 'none':
                        self._rel_points = [
                            rotate((15, 0), source.rotation),  # line from 0,0 to here, bezier curve start
                            rotate((50, 0), source.rotation),  # bezier curve control point 1
                            rotate((-50, 0), sink.rotation),  # bezier curve control point 2
                            rotate((-15, 0), sink.rotation),  # bezier curve end
                            rotate((-CONNECTOR_ARROW_HEIGHT, 0), sink.rotation),  # line to arrow head
                        ]
                        r_sink = sink.rotation
                    else:
                        r_source = (180 + source.rotation) % 360 if source_mirror == 'v' else source.rotation
                        r_sink = (180 + sink.rotation) % 360 if sink_mirror == 'v' else sink.rotation
                        self._rel_points = [
                            rotate((15, 0), r_source),
                            rotate((50, 0), r_source),
                            rotate((-50, 0), r_sink),
                            rotate((-15, 0), r_sink),
                            rotate((-CONNECTOR_ARROW_HEIGHT, 0), r_sink),
                        ]
            elif source.port_type == 'electrical':
                if source_mirror == 'none':
                    px_source = {
                       'e_right' :(( 15,  0), ( 50,  0)),
                       'e_left'  :((-15,  0), (-50,  0)),
                       'e_top'   :((  0,-15), (  0,-50)),
                       'e_bottom':((  0, 15), (  0, 50)),
                    }[source.port_subtype]
                elif source_mirror == 'v':
                    px_source = {
                       'e_left'  :(( 15,  0), ( 50,  0)),
                       'e_right' :((-15,  0), (-50,  0)),
                       'e_top'   :((  0,-15), (  0,-50)),
                       'e_bottom':((  0, 15), (  0, 50)),
                    }[source.port_subtype]
                elif source_mirror == 'h':
                    px_source = {
                       'e_right' :(( 15,  0), ( 50,  0)),
                       'e_left'  :((-15,  0), (-50,  0)),
                       'e_bottom':((  0,-15), (  0,-50)),
                       'e_top'   :((  0, 15), (  0, 50)),
                    }[source.port_subtype]

                if sink_mirror == 'none':
                    px_sink = {
                       'e_right' :(( 50,  0), ( 15,  0)),
                       'e_left'  :((-50,  0), (-15,  0)),
                       'e_top'   :((  0,-50), (  0,-15)),
                       'e_bottom':((  0, 50), (  0, 15)),
                    }[sink.port_subtype]
                elif sink_mirror == 'v':
                    px_sink = {
                       'e_left'  :(( 50,  0), ( 15,  0)),
                       'e_right' :((-50,  0), (-15,  0)),
                       'e_top'   :((  0,-50), (  0,-15)),
                       'e_bottom':((  0, 50), (  0, 15)),
                    }[sink.port_subtype]
                elif sink_mirror == 'h':
                    px_sink = {
                       'e_right' :(( 50,  0), ( 15,  0)),
                       'e_left'  :((-50,  0), (-15,  0)),
                       'e_bottom':((  0,-50), (  0,-15)),
                       'e_top'   :((  0, 50), (  0, 15)),
                    }[sink.port_subtype]

                if source_mirror == 'none' and sink_mirror == 'none':
                    self._rel_points = [
                        rotate(px_source[0], source.rotation),
                        rotate(px_source[1], source.rotation),
                        rotate(px_sink[0], sink.rotation),
                        rotate(px_sink[1], sink.rotation),
                        rotate((0, 0), sink.rotation),
                    ]
                else:
                    r_source = (180 + source.rotation) % 360 if source_mirror == 'h' else source.rotation
                    r_sink = (180 + sink.rotation) % 360 if sink_mirror == 'h' else sink.rotation
                    self._rel_points = [
                        rotate(px_source[0], r_source),
                        rotate(px_source[1], r_source),
                        rotate(px_sink[0], r_sink),
                        rotate(px_sink[1], r_sink),
                        rotate((0, 0), r_sink),
                    ]
            elif source.port_type == 'bus':
                if source_mirror == 'none':
                    px_source = {
                       'b_right' :(( 15,  0), ( 50,  0)),
                       'b_left'  :((-15,  0), (-50,  0)),
                       'b_top'   :((  0,-15), (  0,-50)),
                       'b_bottom':((  0, 15), (  0, 50)),
                    }[source.port_subtype]
                elif source_mirror == 'v':
                    px_source = {
                       'b_left'  :(( 15,  0), ( 50,  0)),
                       'b_right' :((-15,  0), (-50,  0)),
                       'b_top'   :((  0,-15), (  0,-50)),
                       'b_bottom':((  0, 15), (  0, 50)),
                    }[source.port_subtype]
                elif source_mirror == 'h':
                    px_source = {
                       'b_right' :(( 15,  0), ( 50,  0)),
                       'b_left'  :((-15,  0), (-50,  0)),
                       'b_bottom':((  0,-15), (  0,-50)),
                       'b_top'   :((  0, 15), (  0, 50)),
                    }[source.port_subtype]

                if sink_mirror == 'none':
                    px_sink = {
                       'b_right' :(( 50,  0), ( 15,  0)),
                       'b_left'  :((-50,  0), (-15,  0)),
                       'b_top'   :((  0,-50), (  0,-15)),
                       'b_bottom':((  0, 50), (  0, 15)),
                    }[sink.port_subtype]
                elif sink_mirror == 'v':
                    px_sink = {
                       'b_left'  :(( 50,  0), ( 15,  0)),
                       'b_right' :((-50,  0), (-15,  0)),
                       'b_top'   :((  0,-50), (  0,-15)),
                       'b_bottom':((  0, 50), (  0, 15)),
                    }[sink.port_subtype]
                elif sink_mirror == 'h':
                    px_sink = {
                       'b_right' :(( 50,  0), ( 15,  0)),
                       'b_left'  :((-50,  0), (-15,  0)),
                       'b_bottom':((  0,-50), (  0,-15)),
                       'b_top'   :((  0, 50), (  0, 15)),
                    }[sink.port_subtype]

                if source_mirror == 'none' and sink_mirror == 'none':
                    self._rel_points = [
                        rotate(px_source[0], source.rotation),
                        rotate(px_source[1], source.rotation),
                        rotate(px_sink[0], sink.rotation),
                        rotate(px_sink[1], sink.rotation),
                        rotate((0, 0), sink.rotation),
                    ]
                else:
                    r_source = (180 + source.rotation) % 360 if source_mirror == 'h' else source.rotation
                    r_sink = (180 + sink.rotation) % 360 if sink_mirror == 'h' else sink.rotation
                    self._rel_points = [
                        rotate(px_source[0], r_source),
                        rotate(px_source[1], r_source),
                        rotate(px_sink[0], r_sink),
                        rotate(px_sink[1], r_sink),
                        rotate((0, 0), r_sink),
                    ]

            if source.port_type == 'flowgraph':
                self._arrow_rotation = -r_sink / 180 * pi

        self._current_coordinates = None  # triggers _make_path()

        self._line_width_factor = 1.0
        self._color1 = colors.CONNECTION_ENABLED_COLOR
        self._color2 = colors.CONNECTION_ENABLED_COLOR

        if not self._bounding_points:
            if self.wiring_style == 'curved':
                self._make_path()  # no cr set --> only sets bounding_points for extent
            elif self.wiring_style == 'rectilinear':
#               self._bounding_points = tuple(self._l_wire_src)
                if self._l_wire_src:
                    self._bounding_points = tuple(self._l_wire_src)
                else:
                    p0 = 0, 0
                    self._bounding_points = p0, p0, p0, p0

    def _make_path(self, cr=None):
        if self.wiring_style == 'curved':
            source = self.source_port
            sink = self.sink_port

            x_pos, y_pos = self.source_port.connector_coordinate_absolute
            # x_start, y_start = self.source_port.get_connector_coordinate()
            x_end, y_end = self.sink_port.connector_coordinate_absolute

            # sink connector relative to sink connector
            x_e, y_e = x_end - x_pos, y_end - y_pos

            # make rel_point all relative to source connector
            p0 = 0, 0  # x_start - x_pos, y_start - y_pos
            p1, p2, (dx_e1, dy_e1), (dx_e2, dy_e2), (dx_e3, dy_e3) = self._rel_points
            p3 = x_e + dx_e1, y_e + dy_e1
            p4 = x_e + dx_e2, y_e + dy_e2
            p5 = x_e + dx_e3, y_e + dy_e3
            self._bounding_points = p0, p1, p4, p5  # ignores curved part =(

            if cr:
                cr.move_to(*p0)
                cr.line_to(*p1)
                cr.curve_to(*(p2 + p3 + p4))

                cr.line_to(*p5)
                self._line_path = cr.copy_path()

        elif self.wiring_style == 'rectilinear':
            source = self.source_port
            sink = self.sink_port

            if 'Namespace' in str(type(sink.parent_block)):
                if not self._bounding_points:
                    p0 = 0, 0
                    self._bounding_points = p0, p0, p0, p0
                return
#           end debug

            _delta1 = Constants.RECT_WIRING_DELTA1
            _wc = source.width

            _b1_coord = source.parent_block.coordinate

            if source.rotation in (0, 180):
                _b1_L = source.parent_block.width
                _b1_W = source.parent_block.height
            else:
                _b1_L = source.parent_block.height
                _b1_W = source.parent_block.width

            _b2_coord = sink.parent_block.coordinate

            if sink.rotation in (0, 180):
                _b2_L = sink.parent_block.width
                _b2_W = sink.parent_block.height
            else:
                _b2_L = sink.parent_block.height
                _b2_W = sink.parent_block.width

            _b1 = rwire.make_block_1(_b1_coord, _b1_L, _b1_W, _wc, _delta1)
            _b2 = rwire.make_block_1(_b2_coord, _b2_L, _b2_W, _wc, _delta1)

            d1 = {
              'sink'    : 0,
              'e_left'  : 0,
              'b_left'  : 0,
              'bottom'  : 1,
              'e_bottom': 1,
              'b_bottom': 1,
              'source'  : 2,
              'right'   : 2,
              'e_right' : 2,
              'b_right' : 2,
              'top'     : 3,
              'e_top'   : 3,
              'b_top'   : 3,
            }

            source_mirror = source.parent_block.mirror
            source_rotation = source.parent_block.rotation

            if 'Namespace' not in str(type(sink.parent_block)):
                sink_rotation = sink.parent_block.rotation
                sink_mirror = sink.parent_block.mirror
            else:
                sink_rotation = 0
                sink_mirror = 'none'

            if source.parent_block.name.startswith('connector_f'):
                index1 = source.port_index
                source_subtype = source.parent_block.params['output' + str(int(index1)+1)].get_value()
            else:
                source_subtype = 'source' if source.port_subtype == '' else source.port_subtype
                if source_mirror != 'none':
                    source_subtype = {
                       ('source'  , 'v'): 'sink',
                       ('sink'    , 'v'): 'source',
                       ('source'  , 'h'): 'source',
                       ('sink'    , 'h'): 'sink',
                       ('e_left'  , 'v'): 'e_right',
                       ('e_right' , 'v'): 'e_left',
                       ('e_left'  , 'h'): 'e_left',
                       ('e_right' , 'h'): 'e_right',
                       ('e_top'   , 'v'): 'e_top',
                       ('e_bottom', 'v'): 'e_bottom',
                       ('e_top'   , 'h'): 'e_bottom',
                       ('e_bottom', 'h'): 'e_top',
                       ('b_left'  , 'v'): 'b_right',
                       ('b_right' , 'v'): 'b_left',
                       ('b_left'  , 'h'): 'b_left',
                       ('b_right' , 'h'): 'b_right',
                       ('b_top'   , 'v'): 'b_top',
                       ('b_bottom', 'v'): 'b_bottom',
                       ('b_top'   , 'h'): 'b_bottom',
                       ('b_bottom', 'h'): 'b_top',
                    }[(source_subtype, source_mirror)]

            sink_subtype = 'sink' if sink.port_subtype == '' else sink.port_subtype
            if sink_mirror != 'none':
                sink_subtype = {
                   ('source'  , 'v'): 'sink',
                   ('sink'    , 'v'): 'source',
                   ('source'  , 'h'): 'source',
                   ('sink'    , 'h'): 'sink',
                   ('e_left'  , 'v'): 'e_right',
                   ('e_right' , 'v'): 'e_left',
                   ('e_left'  , 'h'): 'e_left',
                   ('e_right' , 'h'): 'e_right',
                   ('e_top'   , 'v'): 'e_top',
                   ('e_bottom', 'v'): 'e_bottom',
                   ('e_top'   , 'h'): 'e_bottom',
                   ('e_bottom', 'h'): 'e_top',
                   ('b_left'  , 'v'): 'b_right',
                   ('b_right' , 'v'): 'b_left',
                   ('b_left'  , 'h'): 'b_left',
                   ('b_right' , 'h'): 'b_right',
                   ('b_top'   , 'v'): 'b_top',
                   ('b_bottom', 'v'): 'b_bottom',
                   ('b_top'   , 'h'): 'b_bottom',
                   ('b_bottom', 'h'): 'b_top',
                }[(sink_subtype, sink_mirror)]

            kp_source, kp_sink = [d1[x] for x in (source_subtype, sink_subtype)]

            d2 = { 0:0, 90:1, 180:2, 270:3}
            l_port_dir = ['W', 'S', 'E', 'N']

            k_source = (d2[source.rotation] + kp_source) % 4

            k_sink = (d2[sink.rotation] + kp_sink) % 4

            _dir1 = l_port_dir[k_source]
            _dir2 = l_port_dir[k_sink  ]

            _cc1 = source.connector_coordinate_absolute
            _cc2 = sink.connector_coordinate_absolute

            _arrow_h = 8 
            _arrow_w = 3 

            _draw_arrow1 = False
            _draw_arrow2 = sink.port_type == 'flowgraph'

            self._l_wire, self._l_arrow = rwire.get_wire(
                _b1, _cc1, _dir1, _draw_arrow1,
                _b2, _cc2, _dir2, _draw_arrow2,
                _delta1, _arrow_h, _arrow_w)

            x0, y0 = self._l_wire[0]

            self._l_wire_src = []
            for x, y in self._l_wire:
                self._l_wire_src.append((x-x0, y-y0))

            if sink.port_type == 'flowgraph':
                self._l_arrow_src = []
                for x, y in self._l_arrow:
                    self._l_arrow_src.append((x-x0, y-y0))

#           debug
#           if not self._bounding_points:
#               p0 = 0, 0
#               self._bounding_points = p0, p0, p0, p0
#           end debug

            if cr:
                cr.move_to(*self._l_wire_src[0])
                for k in range(1,len(self._l_wire_src)):
                    cr.line_to(*self._l_wire_src[k])

                self._line_path = cr.copy_path()

    def draw(self, cr):
        """
        Draw the connection.
        """
        self._current_cr = cr
        sink = self.sink_port
        source = self.source_port

        port_rotations = (source.rotation, sink.rotation)
        if self._current_port_rotations != port_rotations:
            self.create_shapes()  # triggers _make_path() call below
            self._current_port_rotations = port_rotations

        new_coordinates = (source.parent_block.coordinate, sink.parent_block.coordinate)
        if self._current_coordinates != new_coordinates:
            self._make_path(cr)
            self._current_coordinates = new_coordinates

        color1, color2 = (
            None if color is None else
            colors.HIGHLIGHT_COLOR if self.highlighted else
            colors.CONNECTION_DISABLED_COLOR if not self.enabled else
            colors.CONNECTION_ERROR_COLOR if not self.is_valid() else
            color
            for color in (self._color1, self._color2)
        )

        if self.wiring_style == 'curved':
            cr.translate(*self.coordinate)
            cr.set_line_width(self._line_width_factor * cr.get_line_width())
            cr.new_path()
            cr.append_path(self._line_path)

            if source.port_type == 'flowgraph':
                arrow_pos = cr.get_current_point()

            if color1:  # not a message connection
                cr.set_source_rgba(*color1)
                cr.stroke_preserve()

            if color1 != color2:
                cr.save()
                cr.set_dash([5.0, 5.0], 5.0 if color1 else 0.0)
                cr.set_source_rgba(*color2)
                cr.stroke()
                cr.restore()
            else:
                cr.new_path()

            if source.port_type == 'flowgraph':
                cr.move_to(*arrow_pos)
                cr.set_source_rgba(*color2)
                cr.rotate(self._arrow_rotation)
                cr.rel_move_to(CONNECTOR_ARROW_HEIGHT, 0)
                cr.rel_line_to(-CONNECTOR_ARROW_HEIGHT, -CONNECTOR_ARROW_BASE/2)
                cr.rel_line_to(0, CONNECTOR_ARROW_BASE)
                cr.close_path()
                cr.fill()

        elif self.wiring_style == 'rectilinear':
            cr.set_line_join(cairo.LineJoin.ROUND)

            cr.translate(*self.coordinate)

#           this is required to ensure that canvas/port.py uses the same line width
#           in drawing connector ports. Not the best perhaps, but will do...

            Constants.temp1 = CONNECTOR_LINE_WIDTH * cr.get_line_width()
            cr.set_line_width(Constants.temp1)
            cr.new_path()

#           debug
            if 'Namespace' not in str(type(sink.parent_block)):
                cr.append_path(self._line_path)
#           cr.append_path(self._line_path)
#           end debug

            if color1:  # not a message connection
                cr.set_source_rgba(*color1)
                cr.stroke_preserve()

            if color1 != color2:
                cr.save()
                cr.set_dash([5.0, 5.0], 5.0 if color1 else 0.0)
                cr.set_source_rgba(*color2)
                cr.stroke()
                cr.restore()
            else:
                cr.new_path()

            if source.port_type == 'flowgraph':

                if 'Namespace' not in str(type(sink.parent_block)):
                    if not sink.parent_block.name.split('$')[0].startswith('connector_f'):
                        cr.move_to(*self._l_arrow_src[0])
                        cr.set_source_rgba(*color2)
                        cr.line_to(*self._l_arrow_src[1])
                        cr.line_to(*self._l_arrow_src[2])
                        cr.close_path()
                        cr.fill()

    def what_is_selected(self, coor, coor_m=None):
        """
        Returns:
            self if one of the areas/lines encompasses coor, else None.
        """

        if coor_m:
            return Drawable.what_is_selected(self, coor, coor_m)

        x, y = [a - b for a, b in zip(coor, self.coordinate)]

        cr = self._current_cr

        if cr is None:
            return
        cr.save()
        cr.new_path()
        cr.append_path(self._line_path)
        cr.set_line_width(cr.get_line_width() * LINE_SELECT_SENSITIVITY)
#       cr.set_line_width(cr.get_line_width())
        hit = cr.in_stroke(x, y)
        cr.restore()

        if hit:
            return self

class DummyCoreConnection(object):
    def __init__(self, source_port, **kwargs):
        self.parent_platform = source_port.parent_platform
        self.source_port = source_port

        self.sink_port = self._dummy_port = Namespace(
            domain=source_port.domain,
            rotation=0,
            coordinate=(0, 0),
            connector_coordinate_absolute=(0, 0),
            connector_direction=0,
            parent_block=Namespace(coordinate=(0, 0)),
            port_subtype='',
        )

        self.enabled = True
        self.highlighted = False,
        self.is_valid = lambda: True
        self.update(**kwargs)

    def update(self, coordinate=None, rotation=None, sink_port=None):
        dp = self._dummy_port
        self.sink_port = sink_port if sink_port else dp
        if coordinate:
            dp.coordinate = coordinate
            dp.connector_coordinate_absolute = coordinate
            dp.parent_block.coordinate = coordinate
        if rotation is not None:
            dp.rotation = rotation
            dp.connector_direction = (180 + rotation) % 360

    @property
    def has_real_sink(self):
        return self.sink_port is not self._dummy_port

DummyConnection = Connection.make_cls_with_base(DummyCoreConnection)
