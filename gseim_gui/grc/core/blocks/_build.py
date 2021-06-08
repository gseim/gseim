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
          value=None, asserts=None,
          parameters=None, inputs=None, outputs=None, templates=None, cpp_templates=None, **kwargs):
    block_id = id

    cls = type(str(block_id), (Block,), {})
    cls.key = block_id

    cls.label = label or block_id.title()
    cls.category = [cat.strip() for cat in category.split('/') if cat.strip()]

    cls.flags = Flags(flags)

    cls.documentation = {'': documentation.strip('\n\t ').replace('\\\n', '')}

    cls.inputs_data = build_ports(inputs, 'sink') if inputs else []
    cls.outputs_data = build_ports(outputs, 'source') if outputs else []
    cls.parameters_data = build_params(parameters or [],
                                       bool(cls.inputs_data), bool(cls.outputs_data), cls.flags, block_id)
    cls.extra_data = kwargs

    return cls

def build_ports(ports_raw, direction):
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

