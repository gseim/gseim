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
import sys

from gi.repository import Gtk, Pango, PangoCairo

from . import colors
from .drawable import Drawable
from grc.gui import Actions, Utils, Constants
from grc.gui.Constants import (
    BLOCK_LABEL_PADDING, PORT_SPACING, PORT_SEPARATION, LABEL_SEPARATION,
    PORT_BORDER_SEPARATION, BLOCK_FONT, PARAM_FONT, SHOW_TEXT_LABEL_SEPARATION,
    TEXT_BLOCK_LABEL_PADDING, BLOCK_LABEL_PADDING_TAGS, RECT_WIRING_DELTA1
)
from grc.core import utils
from grc.core.blocks import Block as CoreBlock
from grc.core.utils import gutils as gu

import cairo

class Block(CoreBlock, Drawable):
    """The graphical signal block."""

    def __init__(self, parent, **n):
        """
        Block constructor.
        Add graphics related params to the block.
        """
        super(self.__class__, self).__init__(parent, **n)

        self.states.update(coordinate=(0, 0), rotation=0)
        self.width = self.height = 0
        Drawable.__init__(self)  # needs the states and initial sizes

        self._surface_layouts = [
            None,  # title
            None,  # params
        ]
        self._surface_layouts_offsets = 0, 0
        self._comment_layout = None

        self._area = []
        self._border_color = self._bg_color = colors.BLOCK_ENABLED_COLOR
        self._font_color = list(colors.FONT_COLOR)

        self.drawing_scheme = self.params['drawing_scheme'].get_value()

        self.l_coord = []
        self.l_draw = []
        self.c_ = []
        self.a_ = []
        self.s_ = [None, None, None, None]
        self.t_ = []

        self.c__ = []
        self.a__ = []
        self.s__ = self.s_
        self.has_arc = False

        if self.category:
            if 'Hier' in self.category[0]:
                self.file_symbol = parent.parent.config.hier_block_lib_dir + '/' + self.key + '.symbol.py'
            else:
                self.file_symbol = parent.parent.config.block_lib_dir + '/' + self.key + '.symbol.py'
        else:
            self.file_symbol = parent.parent.config.block_lib_dir + '/' + self.key + '.symbol.py'

        self.port_sep_x = int(self.params['port_sep_x'].get_value())
        self.port_sep_y = int(self.params['port_sep_y'].get_value())
        self.port_block_x = int(self.params['port_block_x'].get_value())
        self.port_block_y = int(self.params['port_block_y'].get_value())

        if self.drawing_scheme == 'symbol':
#           if the block name is b_1, assume that the python code for the symbol
#           is in b_1.symbol.py (in the blocks/ directory)

            self.l_coord = gu.python_code_to_list(
                filename=self.file_symbol,
                keyword1='begin_coord',
                keyword2='end_coord')

            for s in self.l_coord:
                exec(s, None, globals())
            self.c_ = c_
            self.a_ = a_
            self.s_ = s_
            self.t_ = t_

            self.l_draw = gu.python_code_to_list(
                filename=self.file_symbol,
                keyword1='begin_draw',
                keyword2='end_draw')

            self.has_arc = any('arc' in x for x in self.l_draw)

        self._hovering = False
        self.label_layout = None

        self.parent0 = parent

    @property
    def coordinate(self):
        """
        Get the coordinate from the position param.

        Returns:
            the coordinate tuple (x, y) or (0, 0) if failure
        """
        return Utils.scale(self.states['coordinate'])

    @coordinate.setter
    def coordinate(self, coor):
        """
        Set the coordinate into the position param.

        Args:
            coor: the coordinate tuple (x, y)
        """
        coor = Utils.scale(coor, reverse=True)
        if Actions.TOGGLE_SNAP_TO_GRID.get_active():

            coor = Utils.align_to_grid(coor)

        self.states['coordinate'] = coor

    @property
    def rotation(self):
        """
        Get the rotation from the position param.

        Returns:
            the rotation in degrees or 0 if failure
        """
        return self.states['rotation']

    @rotation.setter
    def rotation(self, rot):
        """
        Set the rotation into the position param.

        Args:
            rot: the rotation in degrees
        """
        self.states['rotation'] = rot

    def _update_colors(self):

        block_type = type(self).__name__
        if block_type.startswith('dummy'):
            color1 = colors.BLOCK_DUMMY_COLOR
        elif block_type.startswith('pad'):
            color1 = colors.BLOCK_PAD_COLOR
        elif block_type.startswith('bus'):
            color1 = colors.BLOCK_BUS_COLOR
        else:
            color1 = colors.BLOCK_ENABLED_COLOR

        if self.name.startswith('connector_f'):
            self._bg_color = colors.F_PORT_COLOR
        elif self.name.startswith('connector_e'):
            self._bg_color = colors.E_PORT_COLOR
        elif self.name.startswith('connector_b'):
            self._bg_color = colors.B_PORT_COLOR
        else:
            self._bg_color = color1

        self._font_color[-1] = 1.0 if self.state == 'enabled' else 0.4

        self._border_color = (
            colors.MISSING_BLOCK_BORDER_COLOR if self.is_dummy_block else
            colors.BORDER_COLOR_DISABLED if not self.state == 'enabled' else colors.BORDER_COLOR
        )

    def get_offset(self, _rotate_strict, side, theta, port_offset, U, N, h1, k):

        if _rotate_strict == 'no':
            _flag1 = 'normal'
        elif theta == 0:
            _flag1 = 'normal'
        else:
            _flag1 = {
               (  'vertical',  90):'normal',
               (  'vertical', 180):'reverse',
               (  'vertical', 270):'reverse',
               ('horizontal',  90):'reverse',
               ('horizontal', 180):'reverse',
               ('horizontal', 270):'normal',
            }[(side, theta)]

        del1 = Constants.CANVAS_GRID_SIZE

        port_offs = del1*port_offset

        if theta == 0:
          port_offset_1 = port_offs
        elif theta == 180:
          port_offset_1 = -port_offs
        elif theta == 90:
          if side == 'horizontal':
            port_offset_1 = -port_offs
          else:
            port_offset_1 = port_offs
        elif theta == 270:
          if side == 'horizontal':
            port_offset_1 = port_offs
          else:
            port_offset_1 = -port_offs

        if self.drawing_scheme in ('none', 'name', 'symbol'):

            if side == 'vertical':
                del_port = 2*del1*self.port_sep_y
            elif side == 'horizontal':
                del_port = 2*del1*self.port_sep_x
            if _flag1 == 'normal':
                _offset = (U - (N-1)*del_port - h1)/2 + k*del_port + port_offset_1
            elif _flag1 == 'reverse':
                _offset = (U - (N-1)*del_port - h1)/2 + (N-k-1)*del_port + port_offset_1
        else:
            if _flag1 == 'normal':
                _offset = (U - (N-1)*PORT_SEPARATION - h1)/2 + k*PORT_SEPARATION
            elif _flag1 == 'reverse':
                _offset = (U - (N-1)*PORT_SEPARATION - h1)/2 + (N-k-1)*PORT_SEPARATION

        return _offset

    def get_offset_1(self, side, port_offset, U, N, h1, k):

        _flag1 = 'normal'

        del1 = Constants.CANVAS_GRID_SIZE

        if side == 'vertical':
            del_port = 2*del1*self.port_sep_y
        elif side == 'horizontal':
            del_port = 2*del1*self.port_sep_x

        _offset = (U - (N-1)*del_port - h1)/2 + k*del_port + del1*port_offset

        return _offset

    def create_block_label(self, cr=None):
        """Create label for the block"""

        self.label_layout = Gtk.DrawingArea().create_pango_layout('')
        if cr:
            PangoCairo.update_layout(cr, self.label_layout)

    def create_shapes(self):
        """Update the block, parameters, and ports when a change occurs."""

        self.mirror = self.params['mirror'].get_value()
        self.rotate_strict = self.params['rotate_strict'].get_value()

        flag_ignore_mirror = False
        if not self.mirror == 'none':
            if self.rotate_strict == 'no':
                print('canvas/block.py: rotate_strict must be yes for mirroring.')
                print('   element:', self.name)
                print('   treating rotate_strict to be yes')
                self.rotate_strict = 'yes'
            if self.name.startswith('connector_'):
                print('canvas/block.py: mirroring not available for connector elements.')
                print('   element:', self.name)
                print('   mirror will be ignored.')
                flag_ignore_mirror = True

        if flag_ignore_mirror:
            self.mirror = 'none'

        if self.is_horizontal():
            self._area = (0, 0, self.width, self.height)
        elif self.is_vertical():
            self._area = (0, 0, self.height, self.width)

        self.bounds_from_area(self._area)

        if self.name.startswith('connector_'): self.mirror = 'none'

        if self.name.startswith('connector_f'):
            n_outputs = len(self.outputs_data)
            l_output_pos = []
            for k in range(n_outputs):
                pos = self.params['output' + str(k+1)].get_value()
                l_output_pos.append(pos)
            h1 = self.active_sources[0].height

            for k, port in enumerate(self.active_sources):
                port.create_shapes()
                pos = l_output_pos[k]

                if pos == 'right':
                    offset = self.get_offset('no', 'vertical', self.rotation, 0,
                       self.height, 1, h1, 0)

                    port.coordinate = {
                        0: (+self.width, offset),
                        90: (offset, -port.width),
                        180: (-port.width, offset),
                        270: (offset, +self.width),
                    }[port.connector_direction]
                else:
                    offset = self.get_offset(
                       'no', 'horizontal', self.rotation, 0,
                       self.width, 1, h1, 0)
                    port.coordinate = {
                        0: (+self.height, offset),
                        90: (offset, -port.width),
                        180: (-port.width, offset),
                        270: (offset, +self.height),
                    }[port.connector_direction]

        if self.mirror == 'none':
            if not self.name.startswith('connector_f'):
                if self.active_sources or self.e_rights or self.b_rights:
                    if self.active_sources:
                        h1 = self.active_sources[0].height
                    elif self.e_rights:
                        h1 = self.e_rights[0].height
                    elif self.b_rights:
                        h1 = self.b_rights[0].height

                    _N = len(self.active_sources) + len(self.e_rights) + len(self.b_rights)
                    _k = 0
                    for ports in (self.active_sources, self.e_rights, self.b_rights):
                        for port in ports:
                            port.create_shapes()

                            offset = self.get_offset(
                               self.params['rotate_strict'].get_value(),
                               'vertical', self.rotation, self.p_off_r[_k],
                               self.height, _N, h1, _k)

                            port.coordinate = {
                                0: (+self.width, offset),
                                90: (offset, -port.width),
                                180: (-port.width, offset),
                                270: (offset, +self.width),
                            }[port.connector_direction]
                            _k += 1

            if self.active_sinks or self.e_lefts or self.b_lefts:
                if self.active_sinks:
                    h1 = self.active_sinks[0].height
                elif self.e_lefts:
                    h1 = self.e_lefts[0].height
                elif self.b_lefts:
                    h1 = self.b_lefts[0].height

                _N = len(self.active_sinks) + len(self.e_lefts) + len(self.b_lefts)
                _k = 0
                for ports in (self.active_sinks, self.e_lefts, self.b_lefts):
                    for port in ports:
                        port.create_shapes()
                        offset = self.get_offset(
                           self.params['rotate_strict'].get_value(),
                           'vertical', self.rotation, self.p_off_l[_k],
                           self.height, _N, h1, _k)
                        port.coordinate = {
                            0: (+self.width, offset),
                            90: (offset, -port.width),
                            180: (-port.width, offset),
                            270: (offset, +self.width),
                        }[port.connector_direction]
                        _k += 1

            if self.e_tops or self.b_tops:
                if self.e_tops:
                    h1 = self.e_tops[0].height
                elif self.b_tops:
                    h1 = self.b_tops[0].height

                _N = len(self.e_tops) + len(self.b_tops)
                _k = 0
                for ports in (self.e_tops, self.b_tops):
                    for port in ports:
                        port.create_shapes()
                        offset = self.get_offset(
                           self.params['rotate_strict'].get_value(),
                           'horizontal', self.rotation, self.p_off_t[_k],
                           self.width, _N, h1, _k)
                        port.coordinate = {
                            0: (+self.height, offset),
                            90: (offset, -port.width),
                            180: (-port.width, offset),
                            270: (offset, +self.height),
                        }[port.connector_direction]
                        _k += 1

            if self.e_bottoms or self.b_bottoms:
                if self.e_bottoms:
                    h1 = self.e_bottoms[0].height
                elif self.b_bottoms:
                    h1 = self.b_bottoms[0].height

                _N = len(self.e_bottoms) + len(self.b_bottoms)
                _k = 0
                for ports in (self.e_bottoms, self.b_bottoms):
                    for port in ports:
                        port.create_shapes()
                        offset = self.get_offset(
                           self.params['rotate_strict'].get_value(),
                           'horizontal', self.rotation, self.p_off_b[_k],
                           self.width, _N, h1, _k)
                        port.coordinate = {
                            0: (+self.height, offset),
                            90: (offset, -port.width),
                            180: (-port.width, offset),
                            270: (offset, +self.height),
                        }[port.connector_direction]
                        _k += 1

            if self.drawing_scheme == 'symbol':
                rotate1 = Utils.get_rotated_coordinate_1

                self.c__ = []
                for c1 in self.c_:
                    self.c__.append(rotate1(c1, self.width, self.height, self.rotation))

                self.a__ = []
                for theta in self.a_: 
                    self.a__.append(Utils.get_rotated_angle_1(theta, self.rotation))

                if self.s_[0]:
                    self.s__ = Utils.get_scaling_factors(self.s_, self.rotation)

        else:

            flag_v = True if self.mirror == 'v' else False
            flag_h = True if self.mirror == 'h' else False

            Hb, Wb = self.height, self.width

            if self.active_sources or self.e_rights or self.b_rights:
                if self.active_sources:
                    h1 = self.active_sources[0].height
                elif self.e_rights:
                    h1 = self.e_rights[0].height
                elif self.b_rights:
                    h1 = self.b_rights[0].height

                _N = len(self.active_sources) + len(self.e_rights) + len(self.b_rights)
                _k = 0
                for ports in (self.active_sources, self.e_rights, self.b_rights):
                    for port in ports:
                        port.create_shapes_1()
                        Hp, Wp = port.height, port.width

                        offset = self.get_offset_1('vertical',
                            self.p_off_r[_k], Hb, _N, h1, _k)

                        port.coordinate = {
                            (  0, 'v'): (-Wp         , offset      ),
                            (180, 'h'): (-Wp         , offset      ),
                            ( 90, 'v'): (offset      , Wb          ),
                            (270, 'h'): (offset      , Wb          ),
                            (180, 'v'): (Wb          , Hb-offset-Hp),
                            (  0, 'h'): (Wb          , Hb-offset-Hp),
                            (270, 'v'): (Hb-offset-Hp, -Wp         ),
                            ( 90, 'h'): (Hb-offset-Hp, -Wp         ),
                        }[(self.rotation, self.mirror)]

                        _k += 1
            if self.active_sinks or self.e_lefts or self.b_lefts:
                if self.active_sinks:
                    h1 = self.active_sinks[0].height
                elif self.e_lefts:
                    h1 = self.e_lefts[0].height
                elif self.b_lefts:
                    h1 = self.b_lefts[0].height

                _N = len(self.active_sinks) + len(self.e_lefts) + len(self.b_lefts)
                _k = 0
                for ports in (self.active_sinks, self.e_lefts, self.b_lefts):
                    for port in ports:
                        port.create_shapes_1()
                        Hp, Wp = port.height, port.width

                        offset = self.get_offset_1('vertical',
                            self.p_off_l[_k], Hb, _N, h1, _k)

                        port.coordinate = {
                            (  0, 'v'): (Wb          , offset      ),
                            (180, 'h'): (Wb          , offset      ),
                            ( 90, 'v'): (offset      , -Wp         ),
                            (270, 'h'): (offset      , -Wp         ),
                            (180, 'v'): (-Wp         , Hb-offset-Hp),
                            (  0, 'h'): (-Wp         , Hb-offset-Hp),
                            (270, 'v'): (Hb-offset-Hp, Wb          ),
                            ( 90, 'h'): (Hb-offset-Hp, Wb          ),
                        }[(self.rotation, self.mirror)]

                        _k += 1

            if self.e_tops or self.b_tops:
                if self.e_tops:
                    h1 = self.e_tops[0].height
                elif self.b_tops:
                    h1 = self.b_tops[0].height

                _N = len(self.e_tops) + len(self.b_tops)
                _k = 0
                for ports in (self.e_tops, self.b_tops):
                    for port in ports:
                        port.create_shapes_1()
                        Hp, Wp = port.height, port.width

                        offset = self.get_offset_1('horizontal',
                            self.p_off_t[_k], Wb, _N, h1, _k)
                        port.coordinate = {
                            (  0, 'v'): (Wb-offset-Hp, -Wp         ),
                            (180, 'h'): (Wb-offset-Hp, -Wp         ),
                            ( 90, 'v'): (-Wp         , offset      ),
                            (270, 'h'): (-Wp         , offset      ),
                            (180, 'v'): (offset      , Hb          ),
                            (  0, 'h'): (offset      , Hb          ),
                            (270, 'v'): (Hb          , Wb-offset-Hp),
                            ( 90, 'h'): (Hb          , Wb-offset-Hp),
                        }[(self.rotation, self.mirror)]
                        _k += 1

            if self.e_bottoms or self.b_bottoms:
                if self.e_bottoms:
                    h1 = self.e_bottoms[0].height
                elif self.b_bottoms:
                    h1 = self.b_bottoms[0].height

                _N = len(self.e_bottoms) + len(self.b_bottoms)
                _k = 0
                for ports in (self.e_bottoms, self.b_bottoms):
                    for port in ports:
                        port.create_shapes_1()
                        Hp, Wp = port.height, port.width

                        offset = self.get_offset_1('horizontal',
                            self.p_off_b[_k], Wb, _N, h1, _k)
                        port.coordinate = {
                            (  0, 'v'): (Wb-offset-Hp, Hb          ),
                            (180, 'h'): (Wb-offset-Hp, Hb          ),
                            ( 90, 'v'): (Hb          , offset      ),
                            (270, 'h'): (Hb          , offset      ),
                            (180, 'v'): (offset      , -Wp         ),
                            (  0, 'h'): (offset      , -Wp         ),
                            (270, 'v'): (-Wp         , Wb-offset-Hp),
                            ( 90, 'h'): (-Wp         , Wb-offset-Hp),
                        }[(self.rotation, self.mirror)]
                        _k += 1

            if self.drawing_scheme == 'symbol':
                rotate2 = Utils.get_rotated_coordinate_2

                self.c__ = []
                for c1 in self.c_:
                    self.c__.append(rotate2(c1, self.width, self.height, self.rotation, self.mirror))

                self.a__ = []
                for theta in self.a_: 
                    self.a__.append(Utils.get_rotated_angle_2(theta, self.rotation, self.mirror))

                if self.s_[0]:
                    self.s__ = Utils.get_scaling_factors(self.s_, self.rotation)

    def create_labels(self, cr=None):
        """Create the labels for the signal block."""

        self.create_block_label()

        def options_create_markup(s1, s2):
            s_font = Constants.PARAM_FONT
            s3 = '<span font_desc=' + '"' + s_font + '">' \
               + '<b>{label}:</b> {value}</span>'.format(label=Utils.encode(s1), value=s2)
            return s3

        def create_markup_1(s1):
            s_font = Constants.BLOCK_FONT
            s3 = '<span font_desc=' + '"' + s_font + '">' \
               + '<b>{label}</b></span>'.format(label=Utils.encode(s1))
            return s3

        def get_min_dim(ports):

            n_ports = len(ports)
            if n_ports > 1:
                min_dim = 2*PORT_BORDER_SEPARATION + (n_ports-1)*PORT_SEPARATION
            else:
                min_dim = 2*PORT_BORDER_SEPARATION

            return min_dim

        if self.drawing_scheme == 'name':
            # (Re-)creating layouts here, because layout.context_changed() doesn't seems to work (after zoom)
            title_layout, params_layout = self._surface_layouts = [
                Gtk.DrawingArea().create_pango_layout(''),  # title
                Gtk.DrawingArea().create_pango_layout(''),  # params
            ]

            if cr:  # to fix up extents after zooming
                PangoCairo.update_layout(cr, title_layout)
                PangoCairo.update_layout(cr, params_layout)

            foreground='foreground="red"' if not self.is_valid() else ''
            label=Utils.encode(self.label)

            block_type = type(self).__name__
            if block_type == 'dummy_source':
                s1 = self.params['n'].get_value() + ' >'
            elif block_type == 'dummy_sink':
                s1 = '> ' + self.params['n'].get_value()
            elif block_type.startswith('dummy_e'):
                s1 = self.params['n'].get_value()
            elif block_type.startswith('dummy_b'):
                s1 = self.params['n'].get_value()
            elif block_type.startswith('pad_source'):
                s1 = self.params['label'].get_value() + ' >'
            elif block_type.startswith('pad_sink'):
                s1 = '> ' + self.params['label'].get_value()
            elif block_type.startswith('pad_e_'):
                s1 = self.params['label'].get_value()
            elif block_type.startswith('pad_b_'):
                s1 = self.params['label'].get_value()
            else:
                s2 = self.label
                s1 = s2.split('\\n')[0] if '\\n' in s2 else s2

            if block_type.startswith('dummy_'):
                title_layout.set_markup(
                    '<span {foreground} font_desc="{font}">{label}</span>'.format(
                        foreground='foreground="red"' if not self.is_valid() else '', font=BLOCK_FONT,
                        label=Utils.encode(s1)
                    )
                )
            else:
                title_layout.set_markup(
                    '<span {foreground} font_desc="{font}"><b>{label}</b></span>'.format(
                        foreground='foreground="red"' if not self.is_valid() else '', font=BLOCK_FONT,
                        label=Utils.encode(s1)
                    )
                )

            title_width, title_height = title_layout.get_size()
 
            force_show_id = False

            if type(self).__name__ == 'options':

                markups = []
                l = [
                   'title',
                   'wiring_style',
                   'engine_output',
                   'generate_options',
                ]
                for param in self.params.values():
                    if param.key in l:
                        s1 = param.name
                        s2 = self.params[param.key].get_value()
                        s3 = options_create_markup(s1, s2)
                        markups.append(s3)

            else:
                markups = []
                s1 = self.label
                if '\\n' in s1:
                    l = s1.split('\\n')
                    markups = []
                    for k in range(1, len(l)):
                        s3 = create_markup_1(l[k])
                        markups.append(s3)

            params_layout.set_spacing(LABEL_SEPARATION * Pango.SCALE)
            params_layout.set_markup('\n'.join(markups))
            params_width, params_height = params_layout.get_size() if markups else (0, 0)

            label_width = max(title_width, params_width) / Pango.SCALE
            label_height = title_height / Pango.SCALE
            if markups:
                label_height += LABEL_SEPARATION + params_height / Pango.SCALE

            if self.name.startswith('dummy_'):
                width = label_width + 2 * BLOCK_LABEL_PADDING_TAGS
                height = label_height + 2 * BLOCK_LABEL_PADDING_TAGS
            else:
                width = label_width + 2 * BLOCK_LABEL_PADDING
                height = label_height + 2 * BLOCK_LABEL_PADDING

            self._update_colors()
            self.create_port_labels()

#           Note: for xbe/ebe blocks, we will not have both x and e nodes on the
#              same edge, but we need to keep this for subckts:

            if self.name.startswith('dummy_'):
                min_h_1 = get_min_dim(self.active_sinks + self.e_lefts)
                min_h_2 = get_min_dim(self.active_sources + self.e_rights)
                height = max(height, min_h_1, min_h_2)
                min_w_1 = get_min_dim(self.e_tops)
                min_w_2 = get_min_dim(self.e_bottoms)
                width = max(width, min_w_1, min_w_2)
            else:
                if self.name.startswith('connector_f'):
                    n_ports_W = n_ports_E = n_ports_S = n_ports_N = 1
                else:

                    n_ports_W = len(self.active_sinks) + len(self.e_lefts) + len(self.b_lefts)
                    n_ports_E = len(self.active_sources) + len(self.e_rights) + len(self.b_rights)
                    n_ports_N = len(self.e_tops) + len(self.b_tops)
                    n_ports_S = len(self.e_bottoms) + len(self.b_bottoms)

                n_ports_WE = max(n_ports_W, n_ports_E)
                n_ports_SN = max(n_ports_S, n_ports_N)

                del1 = Constants.CANVAS_GRID_SIZE

                if n_ports_WE > 0:
                    height1 = 2*del1*(self.port_sep_y*(n_ports_WE-1) + self.port_block_y)
                else:
                    height1 = 2*del1*self.port_block_y

                if n_ports_SN > 0:
                    width1 = 2*del1*(self.port_sep_x*(n_ports_SN-1) + self.port_block_x)
                else:
                    width1 = 2*del1*self.port_block_x

                height = max(height, height1)
                width = max(width, width1)

            self.width, self.height = width, height = Utils.align_to_grid_1((width, height))

            self._surface_layouts_offsets = [
                (0, (height - label_height) / 2.0),
                (0, (height - label_height) / 2.0 + LABEL_SEPARATION + title_height / Pango.SCALE)
            ]

            title_layout.set_width(width * Pango.SCALE)
            title_layout.set_alignment(Pango.Alignment.CENTER)
            params_layout.set_indent((width - label_width) / 2.0 * Pango.SCALE)

        elif self.drawing_scheme == 'text':

            title_layout, params_layout = self._surface_layouts = [
                Gtk.DrawingArea().create_pango_layout(''),
                Gtk.DrawingArea().create_pango_layout(''),
            ]
            if cr:
                PangoCairo.update_layout(cr, title_layout)
                PangoCairo.update_layout(cr, params_layout)

            foreground = ''
            label=Utils.encode(self.label)

            if self.key == 'show_text':

                font_size = self.params['font_size'].get_value()
                s_font = "Sans " + font_size

                s1 = self.params['text'].get_value()
                l1 = [x.strip() for x in s1.split('\\n')]
                s_title = l1[0] if l1 else 'dummy'
                markups = []
                for i, s in enumerate(l1):
                    if i > 0:
                        s2 = '<span font_desc=' + '"' + s_font + '">' + s + '</span>'
                        markups.append(s2)

            elif self.key == 'show_parameter':
                s1 = self.params['element_name'].get_value()
                s2 = self.params['parameter_name'].get_value()

                l3 = [x for x in self.parent0.blocks if x.name == s1]
                flag_found = False

                if l3:
                    b1 = l3[0]

                    if s2 in b1.params.keys():
                        s_param = b1.params[s2].get_value()
                        flag_found = True
                        flag_show_name = self.params['show_name'].get_value() == 'show'

                if flag_found:
                    if flag_show_name:
                        s_title = s2 + '=' + s_param
                    else:
                        s_title = s_param
                else:
                    s_title = 'error'

                font_size = self.params['font_size'].get_value()
                s_font = "Sans " + font_size
                markups = []

            title_layout.set_markup('<span font_desc="{font}">{name}</span>'.format(
                name=Utils.encode(s_title), font=s_font))

            title_width, title_height = title_layout.get_size()

            params_layout.set_spacing(SHOW_TEXT_LABEL_SEPARATION * Pango.SCALE)
            params_layout.set_markup('\n'.join(markups))
            params_width, params_height = params_layout.get_size() if markups else (0, 0)

            label_width = max(title_width, params_width) / Pango.SCALE
            label_height = title_height / Pango.SCALE
            if markups:
                label_height += SHOW_TEXT_LABEL_SEPARATION + params_height / Pango.SCALE

            width = label_width + 2 * TEXT_BLOCK_LABEL_PADDING
            height = label_height + 2 * TEXT_BLOCK_LABEL_PADDING

            self._update_colors()

            self.width, self.height = width, height = Utils.align_to_grid_1((width, height))

            self._surface_layouts_offsets = [
                (0, (height - label_height) / 2.0),
                (0, (height - label_height) / 2.0 + SHOW_TEXT_LABEL_SEPARATION + title_height / Pango.SCALE)
            ]

            title_layout.set_width(width * Pango.SCALE)
            title_layout.set_alignment(Pango.Alignment.LEFT)
            params_layout.set_alignment(Pango.Alignment.LEFT)
            params_layout.set_indent((width - label_width) / 2.0 * Pango.SCALE)

        elif self.drawing_scheme in ('none', 'symbol'):
            self._update_colors()
            self.create_port_labels()

            if self.name.startswith('connector_f'):
                n_ports_W = n_ports_E = n_ports_S = n_ports_N = 1
            else:

                n_ports_W = len(self.active_sinks) + len(self.e_lefts) + len(self.b_lefts)
                n_ports_E = len(self.active_sources) + len(self.e_rights) + len(self.b_rights)
                n_ports_N = len(self.e_tops) + len(self.b_tops)
                n_ports_S = len(self.e_bottoms) + len(self.b_bottoms)

            n_ports_WE = max(n_ports_W, n_ports_E)
            n_ports_SN = max(n_ports_S, n_ports_N)

            del1 = Constants.CANVAS_GRID_SIZE

            if n_ports_WE > 0:
                self.height = 2*del1*(self.port_sep_y*(n_ports_WE-1) + self.port_block_y)
            else:
                self.height = 2*del1*self.port_block_y

            if n_ports_SN > 0:
                self.width = 2*del1*(self.port_sep_x*(n_ports_SN-1) + self.port_block_x)
            else:
                self.width = 2*del1*self.port_block_x

    def create_port_labels(self):
        for ports in (self.active_sinks, self.active_sources,
            self.e_lefts, self.e_rights, self.e_tops, self.e_bottoms,
            self.b_lefts, self.b_rights, self.b_tops, self.b_bottoms):
            max_width = 0
            for port in ports:
                port.create_labels()
                max_width = max(max_width, port.width_with_label)
            for port in ports:
                port.width = max_width

    def draw(self, cr):
        """
        Draw the signal block with label and inputs/outputs.
        """

        if self.key == 'options':

            delta1 = int(self.params['delta_show_grid'].get_value())

            if delta1 != 0:

                w0a, h0a = self.parent._main_window.get_size()

                w0 = max(w0a, self.parent.max_w_all + 50)
                h0 = max(h0a, self.parent.max_h_all + 50)

                d1_grid = delta1*RECT_WIRING_DELTA1

                x_l = 0
                x_r = w0
                y_t = 0
                y_b = h0

                cr.save()
                cr.set_line_width(0.5)

                nttlx = 0
                httl = 0

                while httl < h0:
                    if (nttlx % 5) == 0:
                        cr.set_source_rgba(*colors.GRID_DARK_COLOR)
                    else:
                        cr.set_source_rgba(*colors.GRID_LIGHT_COLOR)
                    cr.move_to(x_l, httl)
                    cr.line_to(x_r, httl)
                    cr.stroke()
                    httl += d1_grid
                    nttlx += 1

                nttly = 0
                wttl = 0

                while wttl < w0:
                    if (nttly % 5) == 0:
                        cr.set_source_rgba(*colors.GRID_DARK_COLOR)
                    else:
                        cr.set_source_rgba(*colors.GRID_LIGHT_COLOR)
                    cr.move_to(wttl, y_t)
                    cr.line_to(wttl, y_b)
                    cr.stroke()
                    wttl += d1_grid
                    nttly += 1

                cr.restore()

        cr.translate(*self.coordinate)

        if self._show_label:

            s_font = "Sans " + str(Constants.BLOCK_LABEL_FONTSIZE)

            self.label_layout.set_markup('<span font_desc="{font}">{name}</span>'.format(
                name=Utils.encode(self.name), font=s_font))

            self.label_layout.set_alignment(Pango.Alignment.LEFT)

            cr.translate(0, -Constants.BLOCK_LABEL_OFFSET)
            cr.set_source_rgba(*colors.BLOCK_LABEL_COLOR)

            PangoCairo.update_layout(cr, self.label_layout)
            PangoCairo.show_layout(cr, self.label_layout)

            cr.translate(0, Constants.BLOCK_LABEL_OFFSET)

        if self.drawing_scheme == 'name':
            border_color = colors.HIGHLIGHT_COLOR if self.highlighted else self._border_color

            for port in self.active_ports():  # ports first
                cr.save()
                port.draw(cr)
                cr.restore()

            cr.rectangle(*self._area)
            cr.set_source_rgba(*self._bg_color)
            cr.fill_preserve()
            cr.set_source_rgba(*border_color)

            cr.set_line_width(0.5)

            cr.stroke()

            # title and params label
            if self.is_vertical():
                cr.rotate(-math.pi / 2)
                cr.translate(-self.width, 0)
            cr.set_source_rgba(*self._font_color)

            for layout, offset in zip(self._surface_layouts, self._surface_layouts_offsets):
                cr.save()
                cr.translate(*offset)
                PangoCairo.update_layout(cr, layout)
                PangoCairo.show_layout(cr, layout)
                cr.restore()

        elif self.drawing_scheme == 'text':
            border_color = colors.HIGHLIGHT_COLOR if self.highlighted else colors.FLOWGRAPH_BACKGROUND_COLOR

            cr.rectangle(*self._area)
            cr.set_source_rgba(*colors.FLOWGRAPH_BACKGROUND_COLOR)
            cr.fill_preserve()
            cr.set_source_rgba(*border_color)

            cr.set_line_width(0.5)
            cr.stroke()

            if self.is_vertical():
                cr.rotate(-math.pi / 2)
                cr.translate(-self.width, 0)
            cr.set_source_rgba(*colors.SHOW_TEXT_COLOR)

            for layout, offset in zip(self._surface_layouts, self._surface_layouts_offsets):
                cr.save()
                cr.translate(*offset)
                PangoCairo.update_layout(cr, layout)
                PangoCairo.show_layout(cr, layout)
                cr.restore()

        elif self.drawing_scheme == 'none':
            border_color = colors.HIGHLIGHT_COLOR if self.highlighted else self._border_color

            for port in self.active_ports():  # ports first
                cr.save()
                port.draw(cr)
                cr.restore()

            cr.rectangle(*self._area)
            cr.set_source_rgba(*self._bg_color)
            cr.fill_preserve()
            cr.set_source_rgba(*border_color)

            cr.set_line_width(0.5)

            cr.stroke()
        elif self.drawing_scheme == 'symbol':

            for port in self.active_ports():  # ports first
                cr.save()
                port.draw(cr)
                cr.restore()

#           if self.highlighted:
#               border_color = colors.HIGHLIGHT_COLOR
#               cr.rectangle(*self._area)
#           else:
#               border_color = colors.FLOWGRAPH_BACKGROUND_COLOR
#               x1, y1, x2, y2 = self._area
#               cr.rectangle(x1+2, y1+2, x2-5, y2-5)
#           cr.set_source_rgba(*colors.FLOWGRAPH_BACKGROUND_COLOR)
#           cr.fill_preserve()
#           cr.set_line_width(1.0)
#           cr.set_source_rgba(*border_color)
#           cr.stroke()

            border_color = colors.FLOWGRAPH_BACKGROUND_COLOR
            x1, y1, x2, y2 = self._area
            cr.rectangle(x1+2, y1+2, x2-5, y2-5)
            cr.set_source_rgba(*colors.FLOWGRAPH_BACKGROUND_COLOR)
            cr.fill_preserve()
            cr.set_line_width(1.0)
            cr.set_source_rgba(*border_color)
            cr.stroke()

            cr.set_source_rgb(0, 0, 0)

            if self.key in('connector_e_2A', 'connector_b_2A', 'connector_f_2x', 'connector_f_2y'):
                cr.set_line_width(Constants.temp1)
            else:
                cr.set_line_width(1.5*Constants.temp1)

            cr.set_line_join(cairo.LineJoin.ROUND)

            c_ = self.c__
            a_ = self.a__
            s_ = self.s__
            t_ = self.t_

            if self.has_arc:
                if self.mirror == 'none':
                    for s in self.l_draw: exec(s)
                else:
                    l_draw_new = []
                    for s in self.l_draw:
                        if 'cr.arc' in s:
                            s1 = s.replace('cr.arc(', 'cr.dummy1')
                            s2 = s1.replace('cr.arc_negative(', 'cr.arc(')
                            s3 = s2.replace('cr.dummy1', 'cr.arc_negative(')
                        else:
                            s3 = s
                        l_draw_new.append(s3)

                    for s in l_draw_new: exec(s)
            else:
                for s in self.l_draw: exec(s)

            if self.highlighted:
                border_color = colors.HIGHLIGHT_COLOR
                cr.rectangle(*self._area)
                cr.set_line_width(1.0)
                cr.set_source_rgba(*border_color)
                cr.stroke()

    def what_is_selected(self, coor, coor_m=None):
        """
        Get the element that is selected.

        Args:
            coor: the (x,y) tuple
            coor_m: the (x_m, y_m) tuple

        Returns:
            this block, a port, or None
        """
        for port in self.active_ports():
            port_selected = port.what_is_selected(
                coor=[a - b for a, b in zip(coor, self.coordinate)],
                coor_m=[a - b for a, b in zip(coor, self.coordinate)] if coor_m is not None else None
            )
            if port_selected:
                return port_selected
        return Drawable.what_is_selected(self, coor, coor_m)

    def draw_comment(self, cr):
        if not self._comment_layout:
            return
        x, y = self.coordinate

        if self.is_horizontal():
            y += self.height + BLOCK_LABEL_PADDING
        else:
            x += self.height + BLOCK_LABEL_PADDING

        cr.save()
        cr.translate(x, y)
        PangoCairo.update_layout(cr, self._comment_layout)
        PangoCairo.show_layout(cr, self._comment_layout)
        cr.restore()

    def get_extents(self):
        extent = Drawable.get_extents(self)
        x, y = self.coordinate
        for port in self.active_ports():
            extent = (min_or_max(xy, offset + p_xy) for offset, min_or_max, xy, p_xy in zip(
                (x, y, x, y), (min, min, max, max), extent, port.get_extents()
            ))
        return tuple(extent)

    def get_extents_comment(self):
        x, y = self.coordinate
        if not self._comment_layout:
            return x, y, x, y
        if self.is_horizontal():
            y += self.height + BLOCK_LABEL_PADDING
        else:
            x += self.height + BLOCK_LABEL_PADDING
        w, h = self._comment_layout.get_pixel_size()
        return x, y, x + w, y + h

    def mouse_over(self):
        changed = not self._show_label
        self._hovering = True
        return changed

    def mouse_out(self):
        label_was_shown = self._show_label
        self._hovering = False
        return label_was_shown != self._show_label

    @property
    def _show_label(self):
        return self._hovering

