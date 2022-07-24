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

import collections
import itertools
import copy

import sys

import re

import ast

from ._flags import Flags

from ..base import Element
from ..utils.descriptors import lazy_property

def _get_elem(iterable, key):
    items = list(iterable)
    for item in items:
        if item.key == key:
            return item
    return ValueError('Key "{}" not found in {}.'.format(key, items))

class Block(Element):

    is_block = True

    key = ''
    label = ''
    category = ''
    flags = Flags('')
    documentation = {'': ''}

    value = None
    asserts = []

    parameters_data = []
    inputs_data = []
    outputs_data = []

    e_left_data = []
    e_right_data = []
    e_top_data = []
    e_bottom_data = []

    b_left_data = []
    b_right_data = []
    b_top_data = []
    b_bottom_data = []

    extra_data = {}
    loaded_from = '(unknown)'

    def __init__(self, parent):
        """Make a new block from nested data."""
        super(Block, self).__init__(parent)
        param_factory = self.parent_platform.make_param
        port_factory = self.parent_platform.make_port

        self.params = collections.OrderedDict(
            (data['id'], param_factory(parent=self, **data))
            for data in self.parameters_data
        )

        if self.key == 'options':
            self.params['id'].hide = 'part'

        self.sinks = [port_factory(parent=self, **params) for params in self.inputs_data]
        self.sources = [port_factory(parent=self, **params) for params in self.outputs_data]

        self.e_lefts = [port_factory(parent=self, **params) for params in self.e_left_data]
        self.e_rights = [port_factory(parent=self, **params) for params in self.e_right_data]
        self.e_tops = [port_factory(parent=self, **params) for params in self.e_top_data]
        self.e_bottoms = [port_factory(parent=self, **params) for params in self.e_bottom_data]

        self.b_lefts = [port_factory(parent=self, **params) for params in self.b_left_data]
        self.b_rights = [port_factory(parent=self, **params) for params in self.b_right_data]
        self.b_tops = [port_factory(parent=self, **params) for params in self.b_top_data]
        self.b_bottoms = [port_factory(parent=self, **params) for params in self.b_bottom_data]

        self.active_sources = []  # on rewrite
        self.active_sinks = []  # on rewrite

        self.n_ttl_l = 0
        self.n_ttl_r = 0
        self.n_ttl_t = 0
        self.n_ttl_b = 0

        self.states = {'state': True}

    # region Rewrite_and_Validation
    def rewrite(self):
        """
        Add and remove ports to adjust for the nports.
        """
        Element.rewrite(self)

        # Adjust nports

        t_ports = (
          self.sources, self.sinks,
          self.e_lefts, self.e_rights, self.e_tops, self.e_bottoms,
          self.b_lefts, self.b_rights, self.b_tops, self.b_bottoms,
        )

        for ports in t_ports:
            self._rewrite_nports(ports)

        # disconnect hidden ports

        self.active_sources = [p for p in self.sources]
        self.active_sinks = [p for p in self.sinks]

        self.n_ttl_l = len(self.active_sinks) + len(self.e_lefts) + len(self.b_lefts)
        self.n_ttl_r = len(self.active_sources) + len(self.e_rights) + len(self.b_rights)
        self.n_ttl_t = len(self.e_tops) + len(self.b_tops)
        self.n_ttl_b = len(self.e_bottoms) + len(self.b_bottoms)

        self.p_off_l = [] if self.n_ttl_l == 0 else self.n_ttl_l*[0]
        self.p_off_r = [] if self.n_ttl_r == 0 else self.n_ttl_r*[0]
        self.p_off_t = [] if self.n_ttl_t == 0 else self.n_ttl_t*[0]
        self.p_off_b = [] if self.n_ttl_b == 0 else self.n_ttl_b*[0]

        if ('port_offset_l' in self.params.keys()):
            l1 = self.params['port_offset_l'].get_value().split(',')
            if len(l1) == 1:
                self.p_off_l = self.n_ttl_l*[int(l1[0])]
            elif len(l1) == self.n_ttl_l:
                self.p_off_l = [int(x) for x in l1]
            else:
                print('blocks/block.py: rewrite:', self.key, 'check port_offset_l:',
                  self.params['port_offset_l'].get_value())
                sys.exit()

        if ('port_offset_r' in self.params.keys()):
            l1 = self.params['port_offset_r'].get_value().split(',')
            if len(l1) == 1:
                self.p_off_r = self.n_ttl_r*[int(l1[0])]
            elif len(l1) == self.n_ttl_r:
                self.p_off_r = [int(x) for x in l1]
            else:
                print('blocks/block.py: rewrite:', self.key, 'check port_offset_r:',
                  self.params['port_offset_r'].get_value())
                sys.exit()

        if ('port_offset_t' in self.params.keys()):
            l1 = self.params['port_offset_t'].get_value().split(',')
            if len(l1) == 1:
                self.p_off_t = self.n_ttl_t*[int(l1[0])]
            elif len(l1) == self.n_ttl_t:
                self.p_off_t = [int(x) for x in l1]
            else:
                print('blocks/block.py: rewrite:', self.key, 'check port_offset_t:',
                  self.params['port_offset_t'].get_value())
                sys.exit()

        if ('port_offset_b' in self.params.keys()):
            l1 = self.params['port_offset_b'].get_value().split(',')
            if len(l1) == 1:
                self.p_off_b = self.n_ttl_b*[int(l1[0])]
            elif len(l1) == self.n_ttl_b:
                self.p_off_b = [int(x) for x in l1]
            else:
                print('blocks/block.py: rewrite:', self.key, 'check port_offset_b:',
                  self.params['port_offset_b'].get_value())
                sys.exit()

#       e nodes: don't bother with "active" e nodes since we don't have active
#       and hidden in gseim -> use e_lefts rather than active_e_lefts (etc)

    def _rewrite_nports(self, ports):
        for port in ports:
            if hasattr(port, 'master_port'):  # Not a master port and no left-over clones
                port.dtype = port.master_port.dtype
                continue

    def validate(self):
        """
        Validate this block.
        Call the base class validate.
        Evaluate the checks: each check must evaluate to True.
        """
        Element.validate(self)
        self._run_asserts()
        self._validate_var_value()

    def _run_asserts(self):
        """Evaluate the checks"""
        for expr in self.asserts:
            try:
                if not self.evaluate(expr):
                    self.add_error_message('Assertion "{}" failed.'.format(expr))
            except Exception:
                self.add_error_message('Assertion "{}" did not evaluate.'.format(expr))

    def _validate_var_value(self):
        """or variables check the value (only if var_value is used)"""
        if self.is_variable and self.value != 'value':
            try:
                self.parent_flowgraph.evaluate(self.value, local_namespace=self.namespace)
            except Exception as err:
                self.add_error_message('Value "{}" cannot be evaluated:\n{}'.format(self.value, err))
    # endregion

    # region Properties

    def __str__(self):
        return 'Block - {} - {}({})'.format(self.name, self.label, self.key)

    def __repr__(self):
        try:
            name = self.name
        except Exception:
            name = self.key
        return 'block[' + name + ']'

    @property
    def name(self):
        return self.params['id'].value

    @lazy_property
    def is_variable(self):
        return bool(self.value)

    @lazy_property
    def is_import(self):
        return self.key == 'import'

    @lazy_property
    def is_snippet(self):
        return self.key == 'snippet'

    @property
    def comment(self):
        return self.params['comment'].value

    @property
    def state(self):
        return 'enabled'

    @state.setter
    def state(self, value):
        """Sets the state for the block."""
        self.states['state'] = value

    # Enable/Disable Aliases
    @property
    def enabled(self):
        """Get the enabled state of the block"""
        return self.state != 'disabled'

    def ports(self):
        return itertools.chain(self.sources, self.sinks,
           self.e_lefts, self.e_rights, self.e_tops, self.e_bottoms,
           self.b_lefts, self.b_rights, self.b_tops, self.b_bottoms)

    def active_ports(self):
        return itertools.chain(
            self.active_sources,
            self.active_sinks,
            self.e_lefts,
            self.e_rights,
            self.e_tops,
            self.e_bottoms,
            self.b_lefts,
            self.b_rights,
            self.b_tops,
            self.b_bottoms,
        )

    def children(self):
        return itertools.chain(self.params.values(), self.ports())

    # Access

    def get_sink(self, key):
        return _get_elem(self.sinks, key)

    def get_source(self, key):
        return _get_elem(self.sources, key)

    def get_e_left(self, key):
        return _get_elem(self.e_lefts, key)

    def get_e_right(self, key):
        return _get_elem(self.e_rights, key)

    def get_e_top(self, key):
        return _get_elem(self.e_tops, key)

    def get_e_bottom(self, key):
        return _get_elem(self.e_bottoms, key)

    def get_b_left(self, key):
        return _get_elem(self.b_lefts, key)

    def get_b_right(self, key):
        return _get_elem(self.b_rights, key)

    def get_b_top(self, key):
        return _get_elem(self.b_tops, key)

    def get_b_bottom(self, key):
        return _get_elem(self.b_bottoms, key)

    # Resolve
    @property
    def namespace(self):
        return {key: param.get_evaluated() for key, param in iter(self.params.items())}

    def evaluate(self, expr):
        return self.parent_flowgraph.evaluate(expr, self.namespace)

    # Import/Export Methods
    def export_data(self):
        """
        Export this block's params to nested data.

        Returns:
            a nested data odict
        """
        data = collections.OrderedDict()
        if self.key != 'options':
            data['name'] = self.name
            data['id'] = self.key
        data['parameters'] = collections.OrderedDict(sorted(
            (param_id, param.value) for param_id, param in self.params.items()
            if (param_id != 'id' or self.key == 'options')
        ))
        data['states'] = collections.OrderedDict(sorted(self.states.items()))

        return data

    def import_data(self, name, states, parameters, **_):
        """
        Import this block's params from nested data.
        Any param keys that do not exist will be ignored.
        Since params can be dynamically created based another param,
        call rewrite, and repeat the load until the params stick.
        """
        self.params['id'].value = name
        self.states.update(states)

        def get_hash():
            return hash(tuple(hash(v) for v in self.params.values()))

        pre_rewrite_hash = -1
        while pre_rewrite_hash != get_hash():
            for key, value in iter(parameters.items()):
                try:
                    self.params[key].set_value(value)
                except KeyError:
                    continue
            # Store hash and call rewrite
            pre_rewrite_hash = get_hash()
            self.rewrite()
            
    # Controller Modify

