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

import math

from gi.repository import Gtk, PangoCairo, Pango

from . import colors
from .drawable import Drawable
from grc.gui import Actions, Utils, Constants

from grc.core.utils.descriptors import nop_write
from grc.core.ports import Port as CorePort

class Port(CorePort, Drawable):
    """The graphical port."""

    def __init__(self, parent, direction, **n):
        """
        Port constructor.
        Create list of connector coordinates.
        """
        super(self.__class__, self).__init__(parent, direction, **n)
        Drawable.__init__(self)
        self._connector_coordinate = (0, 0)
        self._hovering = False
        self.force_show_label = False

        self._area = []
        self._bg_color = self._border_color = 0, 0, 0, 0
        self._font_color = list(colors.PORT_NAME_COLOR)

        self._line_width_factor = 1.0
        self._label_layout_offsets = 0, 0

        self.width_with_label = self.height = 0

        self.label_layout = None

        self.port_category = direction

        self.port_index = n['id']

    @property
    def width(self):
#       return self.width_with_label if self._show_label else Constants.PORT_LABEL_HIDDEN_WIDTH
        return Constants.PORT_DIM

    @width.setter
    def width(self, value):
        self.width_with_label = value
        self.label_layout.set_width(value * Pango.SCALE)

    def create_shapes(self):
        """Create new areas and labels for the port."""

#       Note: is_horizontal indicates rotation of the block -> even though
#       the top and bottom nodes are different in orientation than the left
#       and right nodes, we will allocate the same rotation value to all of
#       them, i.e., all of them would be horizontal or all of them would be
#       vertical.

        if self.port_category in ('e_top', 'b_top', 'e_bottom', 'b_bottom'):
            if self.is_horizontal():
                self._area = (0, 0, self.height, self.width)
            elif self.is_vertical():
                self._area = (0, 0, self.width, self.height)
        else:
            if self.parent_block.name.startswith('connector_f'):
                if self.is_source:
                    index1 = self.port_index
                    pos = self.parent_block.params['output' + str(int(index1)+1)].get_value()

                    if pos == 'top' or pos == 'bottom':
                        if self.is_horizontal():
                            self._area = (0, 0, self.height, self.width)
                        elif self.is_vertical():
                            self._area = (0, 0, self.width, self.height)
                    elif pos == 'right':
                        if self.is_horizontal():
                            self._area = (0, 0, self.width, self.height)
                        elif self.is_vertical():
                            self._area = (0, 0, self.height, self.width)
                else:
                    if self.is_horizontal():
                        self._area = (0, 0, self.width, self.height)
                    elif self.is_vertical():
                        self._area = (0, 0, self.height, self.width)
            else:
                if self.is_horizontal():
                    self._area = (0, 0, self.width, self.height)
                elif self.is_vertical():
                    self._area = (0, 0, self.height, self.width)

        if self.parent_block.name.startswith('connector_'):
            l = self._area
        else:
            delta = Constants.PORT_HOVER_DELTA
            l = [x - delta for x in self._area[:2]] + [x + 2*delta for x in self._area[2:]]

        self.bounds_from_area(l)

        self._connector_coordinate = {
            0:   (self.width, self.height / 2),
            90:  (self.height / 2, 0),
            180: (0, self.height / 2),
            270: (self.height / 2, self.width)
        }[self.connector_direction]

    def create_shapes_1(self):

        if self.port_category in ('e_top', 'b_top', 'e_bottom', 'b_bottom'):
            if self.is_horizontal():
                self._area = (0, 0, self.height, self.width)
            elif self.is_vertical():
                self._area = (0, 0, self.width, self.height)
        else:
            if self.is_horizontal():
                self._area = (0, 0, self.width, self.height)
            elif self.is_vertical():
                self._area = (0, 0, self.height, self.width)

        delta = Constants.PORT_HOVER_DELTA
        l = [x - delta for x in self._area[:2]] + [x + 2*delta for x in self._area[2:]]

        self.bounds_from_area(l)

        self._connector_coordinate = {
              0: (self.width, self.height / 2),
             90: (self.height / 2, 0),
            180: (0, self.height / 2),
            270: (self.height / 2, self.width)
        }[self.connector_direction]

    def create_labels(self, cr=None):
        """Create the labels for the socket."""
        self.label_layout = Gtk.DrawingArea().create_pango_layout('')

        if cr:
            PangoCairo.update_layout(cr, self.label_layout)

        self._line_width_factor = 1.0

        layout = self.label_layout

#       self.width = Constants.PORT_LABEL_HIDDEN_WIDTH
        self.width = Constants.PORT_DIM

#       self.height = 13
        self.height = Constants.PORT_DIM

        self.height += self.height % 2  # uneven height

    def draw(self, cr):
        """
        Draw the socket with a label.
        """

        border_color = colors.BORDER_COLOR

        cr.set_line_width(0.5)

        cr.translate(*self.coordinate)

#       this draws only the rectangles (fill is done below)
        if not self.parent_block.name.startswith('connector_'):
            cr.rectangle(*self._area)

        if self.parent_block.name.startswith('connector_'):
            cr.set_source_rgba(*colors.FLOWGRAPH_BACKGROUND_COLOR)

            _color1 = colors.CONNECTION_ENABLED_COLOR
            cr.move_to(*self._connector_coordinate)
            x, y = self._connector_coordinate

            if self.connector_direction == 0:
                x -= self.width
            elif self.connector_direction == 180:
                x += self.width
            elif self.connector_direction == 90:
                y += self.width
            elif self.connector_direction == 270:
                y -= self.width

            cr.set_line_width(Constants.temp1)
            cr.line_to(x, y)
        else:
            if self.port_category in ('e_left', 'e_right', 'e_top', 'e_bottom'):
                cr.set_source_rgba(*colors.E_PORT_COLOR)
            elif self.port_category in ('b_left', 'b_right', 'b_top', 'b_bottom'):
                cr.set_source_rgba(*colors.B_PORT_COLOR)
            else:
                cr.set_source_rgba(*colors.F_PORT_COLOR)

        cr.fill_preserve()
        cr.set_source_rgba(*border_color)

#       commenting this out removes the fill (but the ports are still drawn)
        cr.stroke()

#       debug
#       return
        if not self._show_label:
            return  # this port is folded (no label)

        if self.parent_block.name.startswith('connector_f'):
            if self.is_source:
                index1 = self.port_index
                pos = self.parent_block.params['output' + str(int(index1)+1)].get_value()
                if pos == 'top' or pos == 'bottom':
                    if self.is_horizontal():
                        cr.rotate(-math.pi / 2)
                        cr.translate(-self.width, 0)
                elif pos == 'right':
                    if self.is_vertical():
                        cr.rotate(-math.pi / 2)
                        cr.translate(-self.width, 0)
            else:
                if self.is_vertical():
                    cr.rotate(-math.pi / 2)
                    cr.translate(-self.width, 0)
        else:
            if self.port_category in ('sink', 'source', 'e_left', 'e_right', 'b_left', 'b_right'):
                if self.is_vertical():
                    cr.rotate(-math.pi / 2)
                    cr.translate(-self.width, 0)

        if self.port_category in ('e_top', 'e_bottom', 'b_top', 'b_bottom'):
            if self.is_horizontal():
                cr.rotate(-math.pi / 2)
                cr.translate(-self.width, 0)

        if self.parent_block.name.startswith('connector_f'):
            if self.is_source:
                index1 = self.port_index
                port_pos = self.parent_block.params['output' + str(int(index1)+1)].get_value()
            else:
                port_pos = 'left'
        else:
            _mirror = self.parent_block.mirror
            if _mirror == 'none':
                port_pos = {
                  'sink'    : 'left',
                  'source'  : 'right',
                  'e_left'  : 'left',
                  'e_right' : 'right',
                  'e_top'   : 'top',
                  'e_bottom': 'bottom',
                  'b_left'  : 'left',
                  'b_right' : 'right',
                  'b_top'   : 'top',
                  'b_bottom': 'bottom',
                }[self.port_category]
            elif _mirror == 'v':
                port_pos = {
                  'sink'    : 'right',
                  'source'  : 'left',
                  'e_left'  : 'right',
                  'e_right' : 'left',
                  'e_top'   : 'top',
                  'e_bottom': 'bottom',
                  'b_left'  : 'right',
                  'b_right' : 'left',
                  'b_top'   : 'top',
                  'b_bottom': 'bottom',
                }[self.port_category]
            elif _mirror == 'h':
                port_pos = {
                  'sink'    : 'left',
                  'source'  : 'right',
                  'e_left'  : 'left',
                  'e_right' : 'right',
                  'e_top'   : 'bottom',
                  'e_bottom': 'top',
                  'b_left'  : 'left',
                  'b_right' : 'right',
                  'b_top'   : 'bottom',
                  'b_bottom': 'top',
                }[self.port_category]

        _mirror = self.parent_block.mirror

        d1 = {
            ('top'   ,   0) : 'L',
            ('top'   ,  90) : 'R',
            ('top'   , 180) : 'R',
            ('top'   , 270) : 'L',
            ('bottom',   0) : 'R',
            ('bottom',  90) : 'L',
            ('bottom', 180) : 'L',
            ('bottom', 270) : 'R',
            ('left'  ,   0) : 'R',
            ('left'  ,  90) : 'R',
            ('left'  , 180) : 'L',
            ('left'  , 270) : 'L',
            ('right' ,   0) : 'L',
            ('right' ,  90) : 'L',
            ('right' , 180) : 'R',
            ('right' , 270) : 'R',
        }

        align = d1[(port_pos, self.rotation)]

        s_font = "Sans " + str(Constants.PORT_LABEL_FONTSIZE)
        self.label_layout.set_markup('<span font_desc="{font}">{name}</span>'.format(
            name=Utils.encode(self.name), font=s_font))

        if align == 'L':
            self.label_layout.set_alignment(Pango.Alignment.LEFT)
            self._label_layout_offsets = [5, -Constants.PORT_LABEL_OFFSET]
        elif align == 'R':
            self.label_layout.set_alignment(Pango.Alignment.RIGHT)
            self._label_layout_offsets = [-5, -Constants.PORT_LABEL_OFFSET]

        cr.translate(*self._label_layout_offsets)

        cr.set_source_rgba(*self._font_color)

        PangoCairo.update_layout(cr, self.label_layout)
        PangoCairo.show_layout(cr, self.label_layout)

    @property
    def connector_coordinate_absolute(self):
        """the coordinate where connections may attach to"""

        return [sum(c) for c in zip(
            self._connector_coordinate,   # relative to port
            self.coordinate,              # relative to block
            self.parent_block.coordinate  # abs
        )]

    @property
    def connector_direction(self):
        """Get the direction that the socket points: 0,90,180,270."""

        if self.parent_block.name.startswith('connector_f'):
            if self.is_source:
                index1 = self.port_index
                pos = self.parent_block.params['output' + str(int(index1)+1)].get_value()
                if pos == 'top':
                    return (self.rotation + 90) % 360
                elif pos == 'bottom':
                    return (self.rotation + 270) % 360
                elif pos == 'right':
                    return self.rotation
            else:
                return (self.rotation + 180) % 360
        else:
            _mirror = self.parent_block.mirror
            r1 = self.rotation

            if _mirror == 'none':
                if self.is_source or self.is_e_right or self.is_b_right:
                    r = r1
                elif self.is_sink or self.is_e_left or self.is_b_left:
                    r = (r1 + 180) % 360
                elif self.is_e_top or self.is_b_top:
                    r = (r1 + 90) % 360
                elif self.is_e_bottom or self.is_b_bottom:
                    r = (r1 + 270) % 360
            elif _mirror == 'v':
                if self.is_source or self.is_e_right or self.is_b_right:
                    r = (r1 + 180) % 360
                elif self.is_sink or self.is_e_left or self.is_b_left:
                    r = r1
                elif self.is_e_top or self.is_b_top:
                    r = (r1 + 90) % 360
                elif self.is_e_bottom or self.is_b_bottom:
                    r = (r1 + 270) % 360
            elif _mirror == 'h':
                if self.is_source or self.is_e_right or self.is_b_right:
                    r = r1
                elif self.is_sink or self.is_e_left or self.is_b_left:
                    r = (r1 + 180) % 360
                elif self.is_e_top or self.is_b_top:
                    r = (r1 + 270) % 360
                elif self.is_e_bottom or self.is_b_bottom:
                    r = (r1 + 90) % 360

            return r

    @nop_write
    @property
    def rotation(self):
        return self.parent_block.rotation

    def rotate(self, direction):
        """
        Rotate the parent rather than self.

        Args:
            direction: degrees to rotate
        """
        self.parent_block.rotate(direction)

    def move(self, delta_coor):
        """Move the parent rather than self."""
        self.parent_block.move(delta_coor)

    @property
    def highlighted(self):
        return self.parent_block.highlighted

    @highlighted.setter
    def highlighted(self, value):
        self.parent_block.highlighted = value

    @property
    def _show_label(self):
        """
        Figure out if the label should be hidden

        Returns:
            true if the label should not be shown
        """
#       if self._hovering:
#           print('self._hovering:', self._hovering)
#           print('self.force_show_label:', self.force_show_label)
#           print('Actions.TOGGLE_AUTO_HIDE_PORT_LABELS.get_active():',
#             Actions.TOGGLE_AUTO_HIDE_PORT_LABELS.get_active())
#       return self._hovering
#       return self._hovering or self.force_show_label
        return self._hovering or self.force_show_label or not Actions.TOGGLE_AUTO_HIDE_PORT_LABELS.get_active()

    def mouse_over(self):
        """
        Called from flow graph on mouse-over
        """
#       print('port.py: mouse_over:')
        changed = not self._show_label
        self._hovering = True
        return changed

    def mouse_out(self):
        """
        Called from flow graph on mouse-out
        """
        label_was_shown = self._show_label
        self._hovering = False
        return label_was_shown != self._show_label
