# Copyright 2008-2015 Free Software Foundation, Inc.
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

from __future__ import absolute_import, print_function

import collections
import itertools
import sys
import types
import logging

from operator import methodcaller, attrgetter

from . import Messages, blocks
from .base import Element
from .utils import expr_utils

from grc.SolveBlock import SolveBlock
from grc.OutputBlock import OutputBlock

log = logging.getLogger(__name__)

class FlowGraph(Element):

    is_flow_graph = True

    def __init__(self, parent):
        """
        Make a flow graph from the arguments.

        Args:
            parent: a platforms with blocks and element factories

        Returns:
            the flow graph object
        """
        Element.__init__(self, parent)

        self.options_block = self.parent_platform.make_block(self, 'options')
        self.blocks = [self.options_block]

        self.connections = set()

        self.gparms = {}
        self.outvars = {}
        self.l_solve_blocks = []
        self.l_output_blocks = []
        self.d_slvparms = self.parent_platform.d_slvparms

        self._eval_cache = {}
        self.namespace = {}

        self.grc_file_path = ''

    def __str__(self):
        return 'FlowGraph - {}({})'.format(self.get_option('title'), self.get_option('id'))

    def get_variables(self):
        """
        Get a list of all variables (Python) in this flow graph namespace.
        Exclude parameterized variables.

        Returns:
            a sorted list of variable blocks in order of dependency (indep -> dep)
        """
        variables = [block for block in self.iter_enabled_blocks() if block.is_variable]
        return expr_utils.sort_objects(variables, attrgetter('name'), methodcaller('get_var_make'))

    def get_parameters(self):
        """
        Get a list of all parameterized variables in this flow graph namespace.

        Returns:
            a list of parameterized variables
        """
        parameters = [b for b in self.iter_enabled_blocks() if b.key == 'parameter']
        return parameters

    def iter_enabled_blocks(self):
        return (block for block in self.blocks)

    def get_enabled_blocks(self):
        """
        Get a list of all blocks that are enabled and not bypassed.

        Returns:
            a list of blocks
        """
        return list(self.iter_enabled_blocks())

    def get_enabled_connections(self):
        """
        Get a list of all connections that are enabled.

        Returns:
            a list of connections
        """
        return [connection for connection in self.connections]

    def get_option(self, key):
        """
        Get the option for a given key.
        The option comes from the special options block.

        Args:
            key: the param key for the options block

        Returns:
            the value held by that param
        """
        return self.options_block.params[key].get_evaluated()

    # Access Elements
    def get_block(self, name):
        for block in self.blocks:
            if block.name == name:
                return block
        raise KeyError('No block with name {!r}'.format(name))

    def get_elements(self):
        elements = list(self.blocks)
        elements.extend(self.connections)
        return elements

    def children(self):
        return itertools.chain(self.iter_enabled_blocks(), self.connections)

    def rewrite(self):
        """
        Flag the namespace to be renewed.
        """
        self.renew_namespace()
        Element.rewrite(self)

    def renew_namespace(self):
        namespace = {}
        # Load imports

        # Load parameters
        np = {}  # params don't know each other
        for parameter_block in self.get_parameters():
            try:
                value = eval(parameter_block.params['value'].to_code(), namespace)
                np[parameter_block.name] = value
            except Exception:
                log.exception('Failed to evaluate parameter block {0}'.format(parameter_block.name), exc_info=True)
                pass
        namespace.update(np)  # Merge param namespace

        self.namespace.clear()
        self._eval_cache.clear()
        self.namespace.update(namespace)

    def evaluate(self, expr, namespace=None, local_namespace=None):
        """
        Evaluate the expression.
        """
        # Evaluate
        if not expr:
            raise Exception('Cannot evaluate empty statement.')
        if namespace is not None:
            return eval(expr, namespace, local_namespace)
        else:
            return self._eval_cache.setdefault(expr, eval(expr, self.namespace, local_namespace))

    # Add/remove stuff

    def new_block(self, block_id, **kwargs):
        """
        Get a new block of the specified key.
        Add the block to the list of elements.

        Args:
            block_id: the block key

        Returns:
            the new block or None if not found
        """
        if block_id == 'options':
            return self.options_block
        try:
            block = self.parent_platform.make_block(self, block_id, **kwargs)
            self.blocks.append(block)
        except KeyError:
            block = None

        return block

    def connect(self, porta, portb):
        """
        Create a connection between porta and portb.

        Args:
            porta: a port
            portb: another port
        @throw Exception bad connection

        Returns:
            the new connection
        """

        connection = self.parent_platform.Connection(
            parent=self, source=porta, sink=portb)
        self.connections.add(connection)
            
        return connection

    def disconnect(self, *ports):
        to_be_removed = [con for con in self.connections
                         if any(port in con for port in ports)]
        for con in to_be_removed:
            self.remove_element(con)

    def remove_element(self, element):
        """
        Remove the element from the list of elements.
        If the element is a port, remove the whole block.
        If the element is a block, remove its connections.
        If the element is a connection, just remove the connection.
        """
        if element is self.options_block:
            return

        if element.is_port:
            element = element.parent_block  # remove parent block

        if element in self.blocks:
            # Remove block, remove all involved connections
            self.disconnect(*element.ports())
            self.blocks.remove(element)

        elif element in self.connections:
            self.connections.remove(element)

    # Import/Export Methods
    def export_data(self):
        """
        Export this flow graph to nested data.
        Export all block and connection data.

        Returns:
            a nested data odict
        """
        def block_order(b):
            return not b.is_variable, b.name  # todo: vars still first ?!?

        data = collections.OrderedDict()
        data['options'] = self.options_block.export_data()
        data['blocks'] = [b.export_data() for b in sorted(self.blocks, key=block_order)
                          if b is not self.options_block]
        data['connections'] = sorted(c.export_data() for c in self.connections)

        data['gparms'] = self.gparms

#       Assume the following form for data['outvars']
#       - for connection: { name: ['wire', [xx, xx, xx, xx]], ...}
#         (where the list inside describes the connection
#       - for block parm: { name: ['outvar', [xx, yy]], ...}
#         (xx: block/subckt name, yy: which outvar of that block/subckt)

        data['outvars'] = self.outvars

        l_data_solve = []
        l_data_output = []

        for slv in self.l_solve_blocks:
            l_data_solve.append(slv.block_to_dict())

        for out in self.l_output_blocks:
            l_data_output.append(out.block_to_dict())

        data['solve_blocks'] = l_data_solve
        data['output_blocks'] = l_data_output

        return data

    def import_data(self, data):
        """
        Import blocks and connections into this flow graph.
        Clear this flow graph of all previous blocks and connections.
        Any blocks or connections in error will be ignored.

        Args:
            data: the nested data odict
        """
        # Remove previous elements
        del self.blocks[:]
        self.connections.clear()

        if 'gparms' in data.keys():
            self.gparms = data['gparms']
        else:
            self.gparms = {}

        if 'outvars' in data.keys():
            self.outvars = data['outvars']
        else:
            self.outvars = {}

        self.l_solve_blocks = []
        self.l_output_blocks = []

        if 'solve_blocks' in data.keys():
            if len(data['solve_blocks']) != 0:
                for d in data['solve_blocks']:

                    slv = SolveBlock(self.d_slvparms, '', '', d)
                    self.l_solve_blocks.append(slv)

        if 'output_blocks' in data.keys():
            if len(data['output_blocks']) != 0:
                for d in data['output_blocks']:
                    out = OutputBlock({}, '', '', d)
                    self.l_output_blocks.append(out)

        # build the blocks
        self.options_block.import_data(name='', **data.get('options', {}))
        self.blocks.append(self.options_block)

        for block_data in data.get('blocks', []):
            block_id = block_data['id']
            block = (
                self.new_block(block_id) or
                self.new_block(block_id='_dummy', missing_block_id=block_id, **block_data)
            )

            block.import_data(**block_data)

        self.rewrite() 

        # build the connections
        def verify_and_get_port(key, block, dir):

            ports = {
              'sink'    : block.sinks,
              'source'  : block.sources,
              'e_left'  : block.e_lefts,
              'e_right' : block.e_rights,
              'e_top'   : block.e_tops,
              'e_bottom': block.e_bottoms,
              'b_left'  : block.b_lefts,
              'b_right' : block.b_rights,
              'b_top'   : block.b_tops,
              'b_bottom': block.b_bottoms,
            }[dir]

            for port in ports:

                if key == port.key:
                    break

            else:
                if block.is_dummy_block:
                    port = block.add_missing_port(key, dir)
                else:
                    raise LookupError('%s key %r not in %s block keys' % (dir, key, dir))

            return port

        had_connect_errors = False
        _blocks = {block.name: block for block in self.blocks}

        try:
            # TODO: Add better error handling if no connections exist in the flowgraph file.
            for src_blk_id, src_port_id, snk_blk_id, snk_port_id in data.get('connections', []):

                source_block = _blocks[src_blk_id]
                sink_block = _blocks[snk_blk_id]

                # fix old, numeric message ports keys

                # build the connection

                if src_port_id.startswith('e'):

                    src_port_id_1 = src_port_id[2:].strip()
                    snk_port_id_1 = snk_port_id[2:].strip()

                    src_prefix = src_port_id[0:2]
                    snk_prefix = snk_port_id[0:2]

                    src_port_type = {
                       'el' : 'e_left',
                       'er' : 'e_right',
                       'et' : 'e_top',
                       'eb' : 'e_bottom',
                    }[src_prefix]

                    snk_port_type = {
                       'el' : 'e_left',
                       'er' : 'e_right',
                       'et' : 'e_top',
                       'eb' : 'e_bottom',
                    }[snk_prefix]

                    source_port = verify_and_get_port(src_port_id_1, source_block, src_port_type)
                    sink_port = verify_and_get_port(snk_port_id_1, sink_block, snk_port_type)
                elif src_port_id.startswith('b'):

                    src_port_id_1 = src_port_id[2:].strip()
                    snk_port_id_1 = snk_port_id[2:].strip()

                    src_prefix = src_port_id[0:2]
                    snk_prefix = snk_port_id[0:2]

                    src_port_type = {
                       'bl' : 'b_left',
                       'br' : 'b_right',
                       'bt' : 'b_top',
                       'bb' : 'b_bottom',
                    }[src_prefix]

                    snk_port_type = {
                       'bl' : 'b_left',
                       'br' : 'b_right',
                       'bt' : 'b_top',
                       'bb' : 'b_bottom',
                    }[snk_prefix]

                    source_port = verify_and_get_port(src_port_id_1, source_block, src_port_type)
                    sink_port = verify_and_get_port(snk_port_id_1, sink_block, snk_port_type)
                else:
                    source_port = verify_and_get_port(src_port_id, source_block, 'source')
                    sink_port = verify_and_get_port(snk_port_id, sink_block, 'sink')

                self.connect(source_port, sink_port)

        except (KeyError, LookupError) as e:
            Messages.send_error_load(
                'Connection between {}({}) and {}({}) could not be made.\n\t{}'.format(
                    src_blk_id, src_port_id, snk_blk_id, snk_port_id, e))
            had_connect_errors = True

        for block in self.blocks:
            if block.is_dummy_block :
                block.rewrite()      # Make ports visible
                # Flowgraph errors depending on disabled blocks are not displayed
                # in the error dialog box
                # So put a message into the Property window of the dummy block
                block.add_error_message('Block id "{}" not found.'.format(block.key))

        self.rewrite()  # global rewrite
        return had_connect_errors

# edited by mbp
