# Copyright 2008-2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import

from .. import Constants
from ..base import Element
from ..utils.descriptors import (
    EvaluatedFlag, EvaluatedEnum, EvaluatedPInt,
    setup_names, lazy_property
)

@setup_names
class Port(Element):

    is_port = True

    dtype = EvaluatedEnum(list(Constants.TYPE_TO_SIZEOF.keys()), default='')

    def __init__(self, parent, direction, id, label='', domain=Constants.DEFAULT_DOMAIN, dtype='',
                 bus_struct=None, **_):
        """Make a new port from nested data."""
        Element.__init__(self, parent)

        self._dir = direction

        self.port_subtype = direction if direction != 'source' else ''

        if direction in ('source', 'sink'):
            self.port_type = 'flowgraph'
        elif direction in ('e_left', 'e_right', 'e_top', 'e_bottom'):
            self.port_type = 'electrical'
        elif direction in ('b_left', 'b_right', 'b_top', 'b_bottom'):
            self.port_type = 'bus'

        self.key = id
        if not label:

            label = id if not id.isdigit() else {
                'sink': 'in',
                'source': 'out',
                'e_left': 'e',
                'e_right': 'e',
                'e_top': 'e',
                'e_bottom': 'e',
                'b_left': 'b',
                'b_right': 'b',
                'b_top': 'b',
                'b_bottom': 'b',
            }[direction]

        self.name = self._base_name = label

        self.domain = domain
        self.dtype = dtype

        self.stored_hidden_state = None
        self.bus_structure = bus_struct

        # end of args ########################################################

    def __str__(self):
        if self.is_source:
            return 'Source - {}({})'.format(self.name, self.key)
        if self.is_sink:
            return 'Sink - {}({})'.format(self.name, self.key)
        if self.is_e_left:
            return 'e_left - {}({})'.format(self.name, self.key)
        if self.is_e_right:
            return 'e_right - {}({})'.format(self.name, self.key)
        if self.is_e_top:
            return 'e_top - {}({})'.format(self.name, self.key)
        if self.is_e_bottom:
            return 'e_bottom - {}({})'.format(self.name, self.key)
        if self.is_b_left:
            return 'b_left - {}({})'.format(self.name, self.key)
        if self.is_b_right:
            return 'b_right - {}({})'.format(self.name, self.key)
        if self.is_b_top:
            return 'b_top - {}({})'.format(self.name, self.key)
        if self.is_b_bottom:
            return 'b_bottom - {}({})'.format(self.name, self.key)

    def __repr__(self):
        if self.is_source:
            s1 = 'sources'
        elif self.is_sink:
            s1 = 'sinks'
        elif self.is_e_left:
            s1 = 'e_lefts'
        elif self.is_e_right:
            s1 = 'e_rights'
        elif self.is_e_top:
            s1 = 'e_tops'
        elif self.is_e_bottom:
            s1 = 'e_bottoms'
        elif self.is_b_left:
            s1 = 'b_lefts'
        elif self.is_b_right:
            s1 = 'b_rights'
        elif self.is_b_top:
            s1 = 'b_tops'
        elif self.is_b_bottom:
            s1 = 'b_bottoms'

        return '{!r}.{}[{}]'.format(self.parent, s1, self.key)

    @lazy_property
    def is_sink(self):
        return self._dir == 'sink'

    @lazy_property
    def is_source(self):
        return self._dir == 'source'

    @lazy_property
    def is_e_left(self):
        return self._dir == 'e_left'

    @lazy_property
    def is_e_right(self):
        return self._dir == 'e_right'

    @lazy_property
    def is_e_top(self):
        return self._dir == 'e_top'

    @lazy_property
    def is_e_bottom(self):
        return self._dir == 'e_bottom'

    @lazy_property
    def is_b_left(self):
        return self._dir == 'b_left'

    @lazy_property
    def is_b_right(self):
        return self._dir == 'b_right'

    @lazy_property
    def is_b_top(self):
        return self._dir == 'b_top'

    @lazy_property
    def is_b_bottom(self):
        return self._dir == 'b_bottom'

    def validate(self):
        Element.validate(self)
        platform = self.parent_platform

        num_connections = len(list(self.connections(enabled=True)))

        if self.dtype not in Constants.TYPE_TO_SIZEOF.keys():
            self.add_error_message('Type "{}" is not a possible type.'.format(self.dtype))

        if self.is_sink and num_connections > 1:
            self.add_error_message('sink can have only one connection.')

    def rewrite(self):
        del self.dtype

        Element.rewrite(self)

    def connections(self, enabled=None):
        """Iterator over all connections to/from this port

        enabled: None for all, True for enabled only, False for disabled only
        """
        for con in self.parent_flowgraph.connections:
            #TODO clean this up - but how to get past this validation
            # things don't compare simply with an x in y because
            # bus ports are created differently.  
            port_in_con = False

            if self in con and (enabled is None or enabled == con.enabled):
                yield con

