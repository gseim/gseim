# Copyright 2016 Free Software Foundation, Inc.
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

import itertools
import re

from ..Constants import ADVANCED_PARAM_TAB
from ..utils import to_list
from ..Messages import send_warning

from .block import Block
from ._flags import Flags

def build(id, label='', category='', flags='', documentation='',
    parameters=None, inputs=None, outputs=None,
    e_left_nodes=None, e_right_nodes=None, e_top_nodes=None, e_bottom_nodes=None,
    b_left_nodes=None, b_right_nodes=None, b_top_nodes=None, b_bottom_nodes=None,
    **kwargs):

#   print('core/block/_build.py: build: id:',  id)

    block_id = id

    cls = type(str(block_id), (Block,), {})
    cls.key = block_id

    cls.label = label or block_id.title()
    cls.category = [cat.strip() for cat in category.split('/') if cat.strip()]

    cls.flags = Flags(flags)

    cls.documentation = {'': documentation.strip('\n\t ').replace('\\\n', '')}

    cls.inputs_data = build_ports(inputs, 'sink') if inputs else []
    cls.outputs_data = build_ports(outputs, 'source') if outputs else []

    cls.e_left_data = build_ports(e_left_nodes, 'e_left') if e_left_nodes else []
    cls.e_right_data = build_ports(e_right_nodes, 'e_right') if e_right_nodes else []
    cls.e_top_data = build_ports(e_top_nodes, 'e_top') if e_top_nodes else []
    cls.e_bottom_data = build_ports(e_bottom_nodes, 'e_bottom') if e_bottom_nodes else []

    cls.b_left_data = build_ports(b_left_nodes, 'b_left') if b_left_nodes else []
    cls.b_right_data = build_ports(b_right_nodes, 'b_right') if b_right_nodes else []
    cls.b_top_data = build_ports(b_top_nodes, 'b_top') if b_top_nodes else []
    cls.b_bottom_data = build_ports(b_bottom_nodes, 'b_bottom') if b_bottom_nodes else []

    cls.parameters_data = build_params(parameters or [],
         bool(cls.inputs_data), bool(cls.outputs_data), cls.flags, block_id)
    cls.extra_data = kwargs

    return cls

def build_ports(ports_raw, direction):
#   Note: direction is 'source' or 'sink'

    ports = []
    port_ids = set()
    stream_port_ids = itertools.count()

    for i, port_params in enumerate(ports_raw):
        port = port_params.copy()
        port['direction'] = direction

        port_id = port.setdefault('id', str(next(stream_port_ids)))
        if port_id in port_ids:
            raise Exception('Port id "{}" already exists in {}s'.format(port_id, direction))
        port_ids.add(port_id)

        ports.append(port)
    return ports

def build_params(params_raw, have_inputs, have_outputs, flags, block_id):
    params = []

    def add_param(**data):
        params.append(data)

    if flags.SHOW_ID in flags:
        add_param(id='id', name='ID', dtype='id', hide='none')
    else:
        add_param(id='id', name='ID', dtype='id', hide='all')

    base_params_n = {}
    for param_data in params_raw:
        param_id = param_data['id']
        if param_id in params:
            raise Exception('Param id "{}" is not unique'.format(param_id))

        base_key = param_data.get('base_key', None)
        param_data_ext = base_params_n.get(base_key, {}).copy()
        param_data_ext.update(param_data)

        add_param(**param_data_ext)
        base_params_n[param_id] = param_data_ext

    add_param(id='comment', name='Comment', dtype='_multiline', hide='part',
              default='', category=ADVANCED_PARAM_TAB)
    return params

