"""
Copyright 2008-2015 Free Software Foundation, Inc.
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

from .base import Element
from .utils.descriptors import lazy_property

class Connection(Element):

    is_connection = True

    def __init__(self, parent, source, sink):
        """
        Make a new connection given the parent and 2 ports.

        Args:
            flow_graph: the parent of this element
            source: a port (any direction)
            sink: a port (any direction)
        @throws Error cannot make connection

        Returns:
            a new connection
        """
        Element.__init__(self, parent)

#       edited by mbp

        if source.port_type != sink.port_type:
            raise ValueError('Connection: source is', source.port_type, ', sink is', sink.port_type)
      
        if source.port_type == 'flowgraph':
            if not source.is_source:
                source, sink = sink, source
            if not source.is_source:
                raise ValueError('Connection could not isolate source')
            if not sink.is_sink:
                raise ValueError('Connection could not isolate sink')
            self.source_port = source
            self.sink_port = sink
        elif source.port_type in ('electrical', 'bus'):
            self.source_port = source
            self.sink_port = sink

#       end edited by mbp

    def __str__(self):
        return 'Connection (\n\t{}\n\t\t{}\n\t{}\n\t\t{}\n)'.format(
            self.source_block, self.source_port, self.sink_block, self.sink_port,
        )

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return self.source_port == other.source_port and self.sink_port == other.sink_port

    def __hash__(self):
        return hash((self.source_port, self.sink_port))

    def __iter__(self):
        return iter((self.source_port, self.sink_port))

    @lazy_property
    def source_block(self):
        return self.source_port.parent_block

    @lazy_property
    def sink_block(self):
        return self.sink_port.parent_block

    @property
    def enabled(self):
        """
        Get the enabled state of this connection.

        Returns:
            true if source and sink blocks are enabled
        """
        return self.source_block.enabled and self.sink_block.enabled

    def validate(self):
        """
        Validate the connections.
        The ports must match in io size.
        """
        Element.validate(self)
        platform = self.parent_platform

    # Import/Export Methods
    def export_data(self):
        """
        Export this connection's info.

        Returns:
            a nested data odict
        """

        if self.source_port.port_type == 'flowgraph':
            return (
                self.source_block.name, self.source_port.key,
                self.sink_block.name, self.sink_port.key
            )
        elif self.source_port.port_type == 'electrical':
            source_key = {
               'e_left'  : 'el',
               'e_right' : 'er',
               'e_top'   : 'et',
               'e_bottom': 'eb',
            }[self.source_port.port_subtype]

            sink_key = {
               'e_left'  : 'el',
               'e_right' : 'er',
               'e_top'   : 'et',
               'e_bottom': 'eb',
            }[self.sink_port.port_subtype]

            return (
                self.source_block.name, source_key + self.source_port.key,
                self.sink_block.name, sink_key + self.sink_port.key
            )
        elif self.source_port.port_type == 'bus':
            source_key = {
               'b_left'  : 'bl',
               'b_right' : 'br',
               'b_top'   : 'bt',
               'b_bottom': 'bb',
            }[self.source_port.port_subtype]

            sink_key = {
               'b_left'  : 'bl',
               'b_right' : 'br',
               'b_top'   : 'bt',
               'b_bottom': 'bb',
            }[self.sink_port.port_subtype]

            return (
                self.source_block.name, source_key + self.source_port.key,
                self.sink_block.name, sink_key + self.sink_port.key
            )
