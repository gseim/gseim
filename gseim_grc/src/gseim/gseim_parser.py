"""
Copyright (C) 2022 - Mahesh Patil <mbpatil@ee.iitb.ac.in>
This file is part of GSEIM.

GSEIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""

import importlib
from itertools import islice
import os
import subprocess
import sys

from importlib_resources import files
import yaml

import gseim.gutils_gseim as gu
from gseim.file_parsers import parse_parms_file, CctFile, SolveBlock
from gseim import parse_filters

flag_read_yml_once = True
d_yml = {}

def lookup_subckt(subckt_name):
    dir_sub_user = os.environ.get('HIER_BLOCK_USER_DIR', None)
    yml_base_fname = '{}.hblock.yml'.format(subckt_name)

    if dir_sub_user:
        yml_user_fname = os.path.join(dir_sub_user, yml_base_fname)
        if os.path.exists(yml_user_fname):
            return yml_user_fname

    dir_sub_lib  = files('gseim').joinpath('data', 'subckt')
    return os.path.join(dir_sub_lib, yml_base_fname)

def treat_text_blocks(d):
    lnew = []
    for i in d['blocks']:
        if not i['name'].startswith('show'):
            lnew.append(i)
    d['blocks'] = lnew

    return d

def treat_connector_e_1(l_in, connector_name, d_connector):

    l_out = []
    lp = []

    for l_connector in l_in:
        if not connector_name in l_connector:
            l_out.append(l_connector)
        else:
            if l_connector[0] == connector_name:
                l1 = [l_connector[2], l_connector[3]]
            elif l_connector[2] == connector_name:
                l1 = [l_connector[0], l_connector[1]]
            lp.append(l1)

    if len(lp) < 2:
        print('treat_connector_e_1: expect lp to have at least 2 elements. Halting...')
        sys.exit()

    for k in range(1,len(lp)):
        l_out.append(lp[0] + lp[k])

    d_connector[connector_name] = lp[0] + lp[1]

    return l_out, d_connector

def treat_connectors_e(d, keyword):

    _l_connections = d['connections']

    l_connector_names = []

    for l1 in _l_connections:
        for x in l1:
            if x.startswith(keyword):
                l_connector_names.append(x)

    if not _l_connections: return d

    s_connector_names = gu.list_unique_elements(l_connector_names)

    d_connector = {}

    for connector_name in s_connector_names:

        _l_connections, d_connector = \
          treat_connector_e_1(_l_connections, connector_name, d_connector)

    for k in range(len(s_connector_names)):
        flag_break = True
        for connector_name in s_connector_names:
            l = d_connector[connector_name]
            if any(x.startswith(keyword) for x in l):
                flag_break = False
                if l[0].startswith(keyword):
                    if not any(x.startswith(keyword) for x in d_connector[l[0]]):
                        d_connector[connector_name] = d_connector[l[0]]
                elif l[2].startswith(keyword):
                    if not any(x.startswith(keyword) for x in d_connector[l[2]]):
                        d_connector[connector_name] = d_connector[l[2]]
        if flag_break: break

    d['connections'] = _l_connections

#   delete connector blocks:

    lnew = []
    for i in d['blocks']:
        if not i['name'].startswith(keyword):
            lnew.append(i)
    d['blocks'] = lnew

#   treat outvars

    for k, v in d['outvars'].items():
        if v[0] == 'connection':
            if v[1][0].startswith(keyword):
                connector_name = v[1][0]
                d['outvars'][k] = ['connection', d_connector[connector_name]]
            elif v[1][2].startswith(keyword):
                connector_name = v[1][2]
                d['outvars'][k] = ['connection', d_connector[connector_name]]

    return d

def treat_connector_f_1(l_in, connector_name, d_connector):

    l_out = []
    lp_1 = []
    lp_2 = []

    for l_connector in l_in:
        if not connector_name in l_connector:
            l_out.append(l_connector)
        else:
            if l_connector[2] == connector_name:
                lp_1.append([l_connector[0], l_connector[1]])
            elif l_connector[0] == connector_name:
                lp_2.append([l_connector[2], l_connector[3]])

    if len(lp_1) != 1:
        print('treat_connector_f_1: expect lp_1 to have exactly 1 element. Halting...')
        print('  connector_name:', connector_name)
        sys.exit()

    if len(lp_2) < 1:
        print('treat_connector_f_1: expect lp_2 to have at least 2 elements. Halting...')
        print('  connector_name:', connector_name)
        sys.exit()

    for k in range(0,len(lp_2)):
        l_out.append(lp_1[0] + lp_2[k])

    d_connector[connector_name] = lp_1[0] + lp_2[0]

    return l_out, d_connector

def treat_connectors_f(d):

    _l_connections = d['connections']

    l_connector_names = []

    for l1 in _l_connections:
        for x in l1:
            if x.startswith('connector_f'):
                l_connector_names.append(x)

    if not _l_connections: return d

    s_connector_names = gu.list_unique_elements(l_connector_names)
    d_connector = {}

    for connector_name in s_connector_names:
        _l_connections, d_connector = \
          treat_connector_f_1(_l_connections, connector_name, d_connector)

    for k in range(len(s_connector_names)):
        flag_break = True
        for connector_name in s_connector_names:
            l = d_connector[connector_name]
            if any(x.startswith('connector_f') for x in l):
                flag_break = False
                if l[0].startswith('connector_f'):
                    if not any(x.startswith('connector_f') for x in d_connector[l[0]]):
                        d_connector[connector_name] = d_connector[l[0]]
                elif l[2].startswith('connector_f'):
                    if not any(x.startswith('connector_f') for x in d_connector[l[2]]):
                        d_connector[connector_name] = d_connector[l[2]]
        if flag_break: break

    d['connections'] = _l_connections

#   delete connector blocks:

    lnew = []
    for i in d['blocks']:
        if not i['name'].startswith('connector_f'):
            lnew.append(i)
    d['blocks'] = lnew

#   treat outvars

    for k, v in d['outvars'].items():
        if v[0] == 'connection':
            if v[1][0].startswith('connector_f'):
                connector_name = v[1][0]
                d['outvars'][k] = ['connection', d_connector[connector_name]]
            elif v[1][2].startswith('connector_f'):
                connector_name = v[1][2]
                d['outvars'][k] = ['connection', d_connector[connector_name]]

    return d

def treat_virtual(d):

    for block in d['blocks']:
        if 'name' in block.keys():
            old_name = block['name']
            if old_name.startswith('dummy'):
                if 'parameters' in block.keys():
                    node_name = block['parameters']['n']
                    if node_name == 'none':
                        print('treat_virtual: node_name: none not allowed. Halting...',
                          flush=True)
                        sys.exit()
                else:
                    print('treat_virtual: parameters not found. Halting...',
                      flush=True)
                    sys.exit()

                l_1 = old_name.split('$')
                new_name = l_1[0] + '$' + node_name

                block['name'] = new_name

                for c in d['connections']:

                    if c[0] == old_name: c[0] = new_name
                    if c[2] == old_name: c[2] = new_name

                for k, v in d['outvars'].items():
                    if v[0] == 'connection':

                        if v[1][0] == old_name:
                            d['outvars'][k] = ['connection', [new_name, v[1][1], v[1][2], v[1][3]]]
                        if v[1][2] == old_name:
                            d['outvars'][k] = ['connection', [v[1][0], v[1][1], new_name, v[1][3]]]

    check_dummy_1(d)
    return d

def check_dummy_1(d):

    s_blocks = []

    for block in d['blocks']:
        if 'name' in block.keys():

            if block['name'].startswith('dummy_s'):
                if block['name'] not in s_blocks:
                    s_blocks.append(block['name'])

    for s1 in s_blocks:
        if s1.startswith('dummy_source'):
            s2 = s1.replace('source', 'sink')
            if s2 not in s_blocks:
                print('check_dummy_1: complement of',
                  s1, ' not found. Halting...', flush=True)
                sys.exit()

        if s1.startswith('dummy_sink'):
            s2 = s1.replace('sink', 'source')
            if s2 not in s_blocks:
                print('check_dummy_1: complement of',
                  s1, ' not found. Halting...', flush=True)
                sys.exit()

#   check that there is only one dummy_sink for any node name:

    l_dummy_sinks = []

    for block in d['blocks']:
        if 'name' in block.keys():
            if block['name'].startswith('dummy_sink'):
                l_dummy_sinks.append(block['name'])

    for k in l_dummy_sinks:
        if l_dummy_sinks.count(k) != 1:
            print('check_dummy_1:', k, 'appears more than once.',
              'Halting...', flush=True)
            sys.exit()

def get_unique_names_1(l_in):

    l_out = []

    for l0 in l_in:
        s1 = l0[0]
        if s1 not in l_out:
            l_out.append(s1)
        s1 = l0[2]
        if s1 not in l_out:
            l_out.append(s1)
    return l_out

class lib_entity:
    def __init__(self, name, filename):
        self.name = name

        self.pad_in_names = []
        self.pad_out_names = []

        self.pad_e_left_names = []
        self.pad_e_right_names = []
        self.pad_e_top_names = []
        self.pad_e_bottom_names = []

        self.pad_b_left_names = []
        self.pad_b_right_names = []
        self.pad_b_top_names = []
        self.pad_b_bottom_names = []

        self.gseim_in_names = []
        self.gseim_out_names = []

        self.gseim_e_left_names = []
        self.gseim_e_right_names = []
        self.gseim_e_top_names = []
        self.gseim_e_bottom_names = []

        self.gseim_b_left_names = []
        self.gseim_b_right_names = []
        self.gseim_b_top_names = []
        self.gseim_b_bottom_names = []

        self.l_outvars = []
        self.d_outvars = {}
        self.l_connections = []

        self.gseim_prm = {}

#       for assigning pad_in_names, pad_out_names, we need to open the grc
#       file for the subckt.
#       category: '[GRC Hier Blocks]' indicates subckt

        if flag_read_yml_once:
            if filename in d_yml.keys():
                data = d_yml[filename]
            else:
                with open(os.path.expanduser(filename)) as f:
                    data = yaml.safe_load(f)
                d_yml[filename] = data
        else:
            with open(os.path.expanduser(filename)) as f:
                data = yaml.safe_load(f)

        self.flag_subckt = False
        self.flag_block = False

        if 'category' in data.keys():
            if 'Hier' in data['category']:
                self.flag_subckt = True
            else:
                self.flag_block = True
        else:
            self.flag_block = True

        if not data['id'].startswith('pad'):
            if 'parameters' in data.keys():
                for d in data['parameters']:
                    self.gseim_prm[d['id']] = d['default']

        self.n_in       = len(data['inputs'        ]) if 'inputs'         in data.keys() else 0
        self.n_out      = len(data['outputs'       ]) if 'outputs'        in data.keys() else 0

        self.n_e_left   = len(data['e_left_nodes'  ]) if 'e_left_nodes'   in data.keys() else 0
        self.n_e_right  = len(data['e_right_nodes' ]) if 'e_right_nodes'  in data.keys() else 0
        self.n_e_top    = len(data['e_top_nodes'   ]) if 'e_top_nodes'    in data.keys() else 0
        self.n_e_bottom = len(data['e_bottom_nodes']) if 'e_bottom_nodes' in data.keys() else 0

        self.n_b_left   = len(data['b_left_nodes'  ]) if 'b_left_nodes'   in data.keys() else 0
        self.n_b_right  = len(data['b_right_nodes' ]) if 'b_right_nodes'  in data.keys() else 0
        self.n_b_top    = len(data['b_top_nodes'   ]) if 'b_top_nodes'    in data.keys() else 0
        self.n_b_bottom = len(data['b_bottom_nodes']) if 'b_bottom_nodes' in data.keys() else 0

        self.has_flow_nodes = any([
          self.n_in,
          self.n_out,
        ])
        self.has_e_nodes = any([
          self.n_e_left,
          self.n_e_right,
          self.n_e_top,
          self.n_e_bottom,
        ])
        self.has_b_nodes = any([
          self.n_b_left,
          self.n_b_right,
          self.n_b_top,
          self.n_b_bottom,
        ])

        if self.flag_subckt:
#           assign pad_in_names, pad_out_names
            grc_filename = os.path.normpath(
                os.path.join(os.path.dirname(filename), '..', data['grc_source']),
            )

            if flag_read_yml_once:
                if grc_filename in d_yml.keys():
                    data = d_yml[grc_filename]
                else:
                    with open(os.path.expanduser(grc_filename)) as f:
                        data = yaml.safe_load(f)
                    d_yml[grc_filename] = data
            else:
                with open(os.path.expanduser(grc_filename)) as f:
                    data = yaml.safe_load(f)

            data = treat_connectors_e(data, 'connector_e')
            data = treat_connectors_e(data, 'connector_b')
            data = treat_connectors_f(data)
            data = treat_virtual(data)

            for block in data['blocks']:
                if 'name' in block.keys():
                    if block['name'].startswith('pad_source'):
                        self.pad_in_names.append(block['name'])
                    elif block['name'].startswith('pad_sink'):
                        self.pad_out_names.append(block['name'])

                    elif block['name'].startswith('pad_e_left'):
                        self.pad_e_left_names.append(block['name'])
                    elif block['name'].startswith('pad_e_right'):
                        self.pad_e_right_names.append(block['name'])
                    elif block['name'].startswith('pad_e_top'):
                        self.pad_e_top_names.append(block['name'])
                    elif block['name'].startswith('pad_e_bottom'):
                        self.pad_e_bottom_names.append(block['name'])

                    elif block['name'].startswith('pad_b_left'):
                        self.pad_b_left_names.append(block['name'])
                    elif block['name'].startswith('pad_b_right'):
                        self.pad_b_right_names.append(block['name'])
                    elif block['name'].startswith('pad_b_top'):
                        self.pad_b_top_names.append(block['name'])
                    elif block['name'].startswith('pad_b_bottom'):
                        self.pad_b_bottom_names.append(block['name'])

            self.l_connections = data['connections']
            self.d_outvars = data['outvars']

        if 'outvars' in data.keys():
            self.l_outvars = data['outvars']

class cct:
    def __init__(self, name, flag_top):
        self.name = name
        self.netlist_name = ''
        self.flag_top = False
        self.flag_subckt = False
        self.flag_block = True
        self.children = []

        if (flag_top):
            self.flag_top = True
            self.pos = []
            self.pos.append(0)
            self.pos.append(0)
        else:
            self.pos = []
        self.parent = None
        self.lib_map = -1

        self.l_in_names = []
        self.l_out_names = []

        self.l_e_left_names = []
        self.l_e_right_names = []
        self.l_e_top_names = []
        self.l_e_bottom_names = []

        self.l_b_left_names = []
        self.l_b_right_names = []
        self.l_b_top_names = []
        self.l_b_bottom_names = []

        self.l_connections = []
        self.d_outvars = {}
        self.index_sub = -1
        self.flag_prm_file = False
        self.prm_file_name = ''
        self.grc_file_name = ''

        self.node_map = []
        self.node_type = []
        self.gseim_prm = {}

    def add_child(self, c):

        c.pos.append(self.pos[0] + 1)
        c.pos.append(len(self.children))

        self.children.append(c)

        c.parent = self
        self.flag_block = False

def parse_1(parent, child_name, dir_block, n_sub):

    name1 = child_name.split('$')[0]
    filename = os.path.join(dir_block, name1 + '.block.yml')

    if (os.path.exists(filename)):
        cct0 = cct(child_name, False)
        parent.add_child(cct0)
        return
    else:
        filename = lookup_subckt(name1)
        if (os.path.exists(filename)):
            cct0 = cct(child_name, False)
            parent.add_child(cct0)
            cct0.flag_subckt = True
            cct0.index_sub = n_sub[0]
            n_sub[0] += 1
            if cct0.pos[0] > n_sub[1]:
                n_sub[1] = cct0.pos[0]
#           get elements called by this subckt:

            if flag_read_yml_once:
                if filename in d_yml.keys():
                    data = d_yml[filename]
                else:
                    with open(os.path.expanduser(filename)) as f:
                        data = yaml.safe_load(f)
                    d_yml[filename] = data
            else:
                with open(os.path.expanduser(filename)) as f:
                    data = yaml.safe_load(f)

            grc_filename = os.path.normpath(
                os.path.join(os.path.dirname(filename), '..', data['grc_source']),
            )
            cct0.grc_file_name = grc_filename

            if flag_read_yml_once:
                if grc_filename in d_yml.keys():
                    data = d_yml[grc_filename]
                else:
                    with open(os.path.expanduser(grc_filename)) as f:
                        data = yaml.safe_load(f)
                    d_yml[grc_filename] = data
            else:
                with open(os.path.expanduser(grc_filename)) as f:
                    data = yaml.safe_load(f)

            data = treat_text_blocks(data)
            data = treat_connectors_e(data, 'connector_e')
            data = treat_connectors_e(data, 'connector_b')
            data = treat_connectors_f(data)
            data = treat_virtual(data)
            l_connections = data['connections']
            l_unique = get_unique_names_1(l_connections)
            for i in l_unique:
                parse_1(cct0, i, dir_block, n_sub)
        else:
            print('parse_1:', child_name, 'is not block or subckt.', flush=True)
            print('   Halting...', flush=True)
            sys.exit()

def make_lib(input_entity, l_lib, dir_block):
#
#   We will assume that block names and subckt names are different.

    name1 = input_entity.name.split('$')[0]
    l_names = list(map(lambda x : x.name, l_lib))

    if input_entity.flag_subckt:
        if name1 in l_names:
            map0 = l_names.index(name1)
            input_entity.lib_map = map0
        else:
            filename = lookup_subckt(name1)
            lib0 = lib_entity(name1, filename)
            map0 = len(l_lib)
            l_lib.append(lib0)
            input_entity.lib_map = map0

        n_nodes = len(l_lib[map0].l_connections)
        for i in range(n_nodes):
            input_entity.node_map.append(None)
            input_entity.node_type.append(None)
        for i in input_entity.children:
            make_lib(i, l_lib, dir_block)
    else:
        if name1 in l_names:
            map0 = l_names.index(name1)
        else:
            filename = os.path.join(dir_block, name1 + '.block.yml')
            lib0 = lib_entity(name1, filename)
            map0 = len(l_lib)
            l_lib.append(lib0)
        input_entity.lib_map = map0

def make_dict_1(input_entity, d1):
    if input_entity.flag_subckt:
        n_next = max(y for (x,y) in d1.items()) + 1
        d1[input_entity.name] = n_next
        for j in input_entity.children:
            make_dict_1(j, d1)
    else:
        return

def add_in_out(l_connections, l_c):
    for c in l_connections:
        if c[1].startswith('e'):
            l_c1 = [c[0], c[1], 'e']
            l_c2 = [c[2], c[3], 'e']
        elif c[1].startswith('b'):
            l_c1 = [c[0], c[1], 'b']
            l_c2 = [c[2], c[3], 'b']
        else:
            l_c1 = [c[0], c[1], 'out']
            l_c2 = [c[2], c[3], 'in']

        l = [l_c1, l_c2]
        l_c.append(l)

def get_unique_wires_1(l_c):
    wires = l_c
    for k in range(len(l_c)):
        flag_done, wires = merge_wires(wires)
        if flag_done: break
    return wires

def merge_wires(wires):
    wires_new = []
    wires_new.append(wires[0])
    flag_done = True

    for i in range(1, len(wires)):
        l1 = wires[i]
        flag_found = False

        flag_found = False
        for i_w in range(0, len(wires_new)):
            wire = wires_new[i_w]

            for k in range(len(l1)):
                if l1[k] in wire:
                    for k1 in range(len(l1)):
                        if k != k1:
                            if l1[k1] not in wire:
                                wires_new[i_w].append(l1[k1])
                    flag_found = True
                    break
                if l1[k][0].startswith('dummy_source'):
                    wire_sink = [l1[k][0].replace('source', 'sink'), '0', 'in']
                    if wire_sink in wire:
                        for k1 in range(len(l1)):
                            if l1[k1] not in wire:
                                wires_new[i_w].append(l1[k1])
                        flag_found = True
                        break
                if l1[k][0].startswith('dummy_sink'):
                    wire_sink = [l1[k][0].replace('sink', 'source'), '0', 'out']
                    if wire_sink in wire:
                        for k1 in range(len(l1)):
                            if l1[k1] not in wire:
                                wires_new[i_w].append(l1[k1])
                        flag_found = True
                        break
        if flag_found:
            flag_done = False
            continue
        else:
            wires_new.append(l1) # new wire

    return flag_done, wires_new

def assign_node_names(input_entity, l_lib, cct_file,
    dir_block, d_names, node_counter):
    if input_entity.flag_top:
        l_connections = input_entity.l_connections
    else:
        l_connections = l_lib[input_entity.lib_map].l_connections

    l_c = []
    add_in_out(l_connections, l_c)

    wires = get_unique_wires_1(l_c)

    for i in input_entity.children:
        for i_in in range(l_lib[i.lib_map].n_in):
            node0 = [i.name, str(i_in), 'in']
            flag_found = False
            for i_wire, wire in enumerate(wires):
                if node0 in wire:
                    flag_found = True
                    node_name = 'n' \
                      + str(d_names[i.parent.name]) \
                      + '_' + str(i_wire)
                    i.l_in_names.append(node_name)
            if not flag_found:
                i.l_in_names.append('n' + str(node_counter))
                node_counter += 1

        for i_out in range(l_lib[i.lib_map].n_out):
            node0 = [i.name, str(i_out), 'out']
            flag_found = False
            for i_wire, wire in enumerate(wires):
                if node0 in wire:
                    flag_found = True
                    node_name = 'n' \
                      + str(d_names[i.parent.name]) \
                      + '_' + str(i_wire)
                    i.l_out_names.append(node_name)
            if not flag_found:
                i.l_out_names.append('n' + str(node_counter))
                node_counter += 1

        for i1 in range(l_lib[i.lib_map].n_e_left):
            x2 = 'el' + str(i1)
            node0 = [i.name, x2, 'e']
            flag_found = False
            for i_wire, wire in enumerate(wires):
                if node0 in wire:
                    flag_found = True
                    node_name = 'n' \
                      + str(d_names[i.parent.name]) \
                      + '_' + str(i_wire)
                    i.l_e_left_names.append(node_name)
            if not flag_found:
                i.l_e_left_names.append('n' + str(node_counter))
                node_counter += 1

        for i1 in range(l_lib[i.lib_map].n_e_right):
            x2 = 'er' + str(i1)
            node0 = [i.name, x2, 'e']
            flag_found = False
            for i_wire, wire in enumerate(wires):
                if node0 in wire:
                    flag_found = True
                    node_name = 'n' \
                      + str(d_names[i.parent.name]) \
                      + '_' + str(i_wire)
                    i.l_e_right_names.append(node_name)
            if not flag_found:
                i.l_e_right_names.append('n' + str(node_counter))
                node_counter += 1

        for i1 in range(l_lib[i.lib_map].n_e_top):
            x2 = 'et' + str(i1)
            node0 = [i.name, x2, 'e']
            flag_found = False
            for i_wire, wire in enumerate(wires):
                if node0 in wire:
                    flag_found = True
                    node_name = 'n' \
                      + str(d_names[i.parent.name]) \
                      + '_' + str(i_wire)
                    i.l_e_top_names.append(node_name)
            if not flag_found:
                i.l_e_top_names.append('n' + str(node_counter))
                node_counter += 1

        for i1 in range(l_lib[i.lib_map].n_e_bottom):
            x2 = 'eb' + str(i1)
            node0 = [i.name, x2, 'e']
            flag_found = False
            for i_wire, wire in enumerate(wires):
                if node0 in wire:
                    flag_found = True
                    node_name = 'n' \
                      + str(d_names[i.parent.name]) \
                      + '_' + str(i_wire)
                    i.l_e_bottom_names.append(node_name)
            if not flag_found:
                i.l_e_bottom_names.append('n' + str(node_counter))
                node_counter += 1

        for i1 in range(l_lib[i.lib_map].n_b_left):
            x2 = 'bl' + str(i1)
            node0 = [i.name, x2, 'b']
            flag_found = False
            for i_wire, wire in enumerate(wires):
                if node0 in wire:
                    flag_found = True
                    node_name = 'n' \
                      + str(d_names[i.parent.name]) \
                      + '_' + str(i_wire)
                    i.l_b_left_names.append(node_name)
            if not flag_found:
                i.l_b_left_names.append('n' + str(node_counter))
                node_counter += 1

        for i1 in range(l_lib[i.lib_map].n_b_right):
            x2 = 'br' + str(i1)
            node0 = [i.name, x2, 'b']
            flag_found = False
            for i_wire, wire in enumerate(wires):
                if node0 in wire:
                    flag_found = True
                    node_name = 'n' \
                      + str(d_names[i.parent.name]) \
                      + '_' + str(i_wire)
                    i.l_b_right_names.append(node_name)
            if not flag_found:
                i.l_b_right_names.append('n' + str(node_counter))
                node_counter += 1

        for i1 in range(l_lib[i.lib_map].n_b_top):
            x2 = 'bt' + str(i1)
            node0 = [i.name, x2, 'b']
            flag_found = False
            for i_wire, wire in enumerate(wires):
                if node0 in wire:
                    flag_found = True
                    node_name = 'n' \
                      + str(d_names[i.parent.name]) \
                      + '_' + str(i_wire)
                    i.l_b_top_names.append(node_name)
            if not flag_found:
                i.l_b_top_names.append('n' + str(node_counter))
                node_counter += 1

        for i1 in range(l_lib[i.lib_map].n_b_bottom):
            x2 = 'bb' + str(i1)
            node0 = [i.name, x2, 'b']
            flag_found = False
            for i_wire, wire in enumerate(wires):
                if node0 in wire:
                    flag_found = True
                    node_name = 'n' \
                      + str(d_names[i.parent.name]) \
                      + '_' + str(i_wire)
                    i.l_b_bottom_names.append(node_name)
            if not flag_found:
                i.l_b_bottom_names.append('n' + str(node_counter))
                node_counter += 1

    for i in input_entity.children:
        if i.flag_subckt:
            assign_node_names(i, l_lib, cct_file,
               dir_block, d_names, node_counter)

def assign_gseim_prm(input_entity, l_lib, cct_file, dir_block):
    if input_entity.flag_top:
#       main cct: take parms from grc file of the cct:
        filename = cct_file

        if flag_read_yml_once:
            if filename in d_yml.keys():
                data = d_yml[filename]
            else:
                with open(os.path.expanduser(filename)) as f:
                    data = yaml.safe_load(f)
                d_yml[filename] = data
        else:
            with open(os.path.expanduser(filename)) as f:
                data = yaml.safe_load(f)

        input_entity.gseim_prm = data['gparms']
    else:
#       subckt: take parms from grc file of the parent:
        lib_name = l_lib[input_entity.lib_map].name
        filename = input_entity.parent.grc_file_name

        if flag_read_yml_once:
            if filename in d_yml.keys():
                data = d_yml[filename]
            else:
                with open(os.path.expanduser(filename)) as f:
                    data = yaml.safe_load(f)
                d_yml[filename] = data
        else:
            with open(os.path.expanduser(filename)) as f:
                data = yaml.safe_load(f)

        for d in data['blocks']:
            if d['name'] == input_entity.name:
                if 'parameters' in d.keys():
                    for k, v in d['parameters'].items():
                        if k in l_lib[input_entity.lib_map].gseim_prm.keys():
                            input_entity.gseim_prm[k] = v

    if input_entity.flag_subckt or input_entity.flag_top:
        for i in input_entity.children:
            assign_gseim_prm(i, l_lib, cct_file, dir_block)

def get_pad_in_name(subckt, input_pad_name, l_lib):

    index1 = sorted(l_lib[subckt.lib_map].pad_out_names, key = lambda x: int(x.split('$')[-1])).index(input_pad_name)

    node_name = subckt.l_out_names[index1]
    return node_name

def get_pad_out_name(subckt, output_pad_name, l_lib):

    index1 = sorted(l_lib[subckt.lib_map].pad_in_names, key = lambda x: int(x.split('$')[-1])).index(output_pad_name)

    node_name = subckt.l_in_names[index1]
    return node_name

def get_pad_e_left_name(subckt, pad_name, l_lib):
    index1 = sorted(l_lib[subckt.lib_map].pad_e_left_names, key = lambda x: int(x.split('$')[-1])).index(pad_name)

    node_name = subckt.l_e_left_names[index1]
    return node_name

def get_pad_e_right_name(subckt, pad_name, l_lib):

    index1 = sorted(l_lib[subckt.lib_map].pad_e_right_names, key = lambda x: int(x.split('$')[-1])).index(pad_name)

    node_name = subckt.l_e_right_names[index1]
    return node_name

def get_pad_e_top_name(subckt, pad_name, l_lib):
    index1 = sorted(l_lib[subckt.lib_map].pad_e_top_names, key = lambda x: int(x.split('$')[-1])).index(pad_name)

    node_name = subckt.l_e_top_names[index1]
    return node_name

def get_pad_e_bottom_name(subckt, pad_name, l_lib):
    index1 = sorted(l_lib[subckt.lib_map].pad_e_bottom_names, key = lambda x: int(x.split('$')[-1])).index(pad_name)

    node_name = subckt.l_e_bottom_names[index1]
    return node_name

def get_pad_b_left_name(subckt, pad_name, l_lib):
    index1 = sorted(l_lib[subckt.lib_map].pad_b_left_names, key = lambda x: int(x.split('$')[-1])).index(pad_name)
    node_name = subckt.l_b_left_names[index1]
    return node_name

def get_pad_b_right_name(subckt, pad_name, l_lib):
    index1 = sorted(l_lib[subckt.lib_map].pad_b_right_names, key = lambda x: int(x.split('$')[-1])).index(pad_name)
    node_name = subckt.l_b_right_names[index1]
    return node_name

def get_pad_b_top_name(subckt, pad_name, l_lib):
    index1 = sorted(l_lib[subckt.lib_map].pad_b_top_names, key = lambda x: int(x.split('$')[-1])).index(pad_name)
    node_name = subckt.l_b_top_names[index1]
    return node_name

def get_pad_b_bottom_name(subckt, pad_name, l_lib):
    index1 = sorted(l_lib[subckt.lib_map].pad_b_bottom_names, key = lambda x: int(x.split('$')[-1])).index(pad_name)
    node_name = subckt.l_b_bottom_names[index1]
    return node_name

def replace_node_names_1(input_entity, node_name, name_to_replace):
    for i in input_entity.parent.children:
        i.l_in_names = [node_name if x==name_to_replace else x
           for x in i.l_in_names]
        i.l_out_names = [node_name if x==name_to_replace else x
           for x in i.l_out_names]

        i.l_e_left_names = [node_name if x==name_to_replace else x
           for x in i.l_e_left_names]
        i.l_e_right_names = [node_name if x==name_to_replace else x
           for x in i.l_e_right_names]
        i.l_e_top_names = [node_name if x==name_to_replace else x
           for x in i.l_e_top_names]
        i.l_e_bottom_names = [node_name if x==name_to_replace else x
           for x in i.l_e_bottom_names]

        i.l_b_left_names = [node_name if x==name_to_replace else x
           for x in i.l_b_left_names]
        i.l_b_right_names = [node_name if x==name_to_replace else x
           for x in i.l_b_right_names]
        i.l_b_top_names = [node_name if x==name_to_replace else x
           for x in i.l_b_top_names]
        i.l_b_bottom_names = [node_name if x==name_to_replace else x
           for x in i.l_b_bottom_names]

def replace_nodes_1(input_entity, l_lib):
    if input_entity.flag_subckt:
        for i in input_entity.children:
            replace_nodes_1(i, l_lib)
    else:
        if input_entity.name.startswith('pad_source'):
            node_name = get_pad_out_name(input_entity.parent,input_entity.name, l_lib)
            name_to_replace = input_entity.l_out_names[0]
            replace_node_names_1(input_entity, node_name, name_to_replace)
        elif input_entity.name.startswith('pad_sink'):
            node_name = get_pad_in_name(input_entity.parent,input_entity.name, l_lib)
            name_to_replace = input_entity.l_in_names[0]
            replace_node_names_1(input_entity, node_name, name_to_replace)
        elif input_entity.name.startswith('pad_e_left'):
            node_name = get_pad_e_left_name(input_entity.parent,input_entity.name, l_lib)
            name_to_replace = input_entity.l_e_right_names[0]
            replace_node_names_1(input_entity, node_name, name_to_replace)
        elif input_entity.name.startswith('pad_e_right'):
            node_name = get_pad_e_right_name(input_entity.parent,input_entity.name, l_lib)
            name_to_replace = input_entity.l_e_left_names[0]
            replace_node_names_1(input_entity, node_name, name_to_replace)
        elif input_entity.name.startswith('pad_e_top'):
            node_name = get_pad_e_top_name(input_entity.parent,input_entity.name, l_lib)
            name_to_replace = input_entity.l_e_bottom_names[0]
            replace_node_names_1(input_entity, node_name, name_to_replace)
        elif input_entity.name.startswith('pad_e_bottom'):
            node_name = get_pad_e_bottom_name(input_entity.parent,input_entity.name, l_lib)
            name_to_replace = input_entity.l_e_top_names[0]
            replace_node_names_1(input_entity, node_name, name_to_replace)
        elif input_entity.name.startswith('pad_b_left'):
            node_name = get_pad_b_left_name(input_entity.parent,input_entity.name, l_lib)
            name_to_replace = input_entity.l_b_right_names[0]
            replace_node_names_1(input_entity, node_name, name_to_replace)
        elif input_entity.name.startswith('pad_b_right'):
            node_name = get_pad_b_right_name(input_entity.parent,input_entity.name, l_lib)
            name_to_replace = input_entity.l_b_left_names[0]
            replace_node_names_1(input_entity, node_name, name_to_replace)
        elif input_entity.name.startswith('pad_b_top'):
            node_name = get_pad_b_top_name(input_entity.parent,input_entity.name, l_lib)
            name_to_replace = input_entity.l_b_bottom_names[0]
            replace_node_names_1(input_entity, node_name, name_to_replace)
        elif input_entity.name.startswith('pad_b_bottom'):
            node_name = get_pad_b_bottom_name(input_entity.parent,input_entity.name, l_lib)
            name_to_replace = input_entity.l_b_top_names[0]
            replace_node_names_1(input_entity, node_name, name_to_replace)

def make_netlist_name(input_entity, l_lib):
    if input_entity.flag_subckt:
        for i in input_entity.children:
            make_netlist_name(i, l_lib)
    else:
        if not input_entity.name.startswith('pad'):
            if input_entity.parent.flag_subckt:
                name1 = 's' + str(input_entity.parent.index_sub) \
                         + '#' + input_entity.name
            else:
                name1 = input_entity.name
            input_entity.netlist_name = name1

def build_cct_elements(input_entity, l_lib):
    """Returns a list of circuit elements of type (kind, assignments_dict)

    [
        ('xelement', {'name': 'xxx', 'type': 'xxx', 'x1': 'xxx'}),
        ('eelement', {'name': 'xxx', 'type': 'xxx', 'x1': 'xxx'}),
        ('belement', {'name': 'xxx', 'type': 'xxx', 'x1': 'xxx'}),
    ]
    """

    cct_elems = []

    if input_entity.flag_subckt:
        for i in input_entity.children:
            cct_elems.extend(build_cct_elements(i, l_lib))
    else:
        assignments = {}

        if l_lib[input_entity.lib_map].has_b_nodes:
            kind = 'belement'
        elif l_lib[input_entity.lib_map].has_e_nodes:
            kind = 'eelement'
        else:
            kind = 'xelement'

        if not input_entity.name.startswith('pad'):
            assignments['name'] = input_entity.netlist_name
            assignments['type'] = input_entity.name.split('$')[0]

            for i_node, node_name in enumerate(input_entity.l_in_names):
                lib_node_name = l_lib[input_entity.lib_map].gseim_in_names[i_node]
                assignments[lib_node_name] = node_name
            for i_node, node_name in enumerate(input_entity.l_out_names):
                lib_node_name = l_lib[input_entity.lib_map].gseim_out_names[i_node]
                assignments[lib_node_name] = node_name

            for i_node, node_name in enumerate(input_entity.l_e_left_names):
                lib_node_name = l_lib[input_entity.lib_map].gseim_e_left_names[i_node]
                assignments[lib_node_name] = node_name
            for i_node, node_name in enumerate(input_entity.l_e_right_names):
                lib_node_name = l_lib[input_entity.lib_map].gseim_e_right_names[i_node]
                assignments[lib_node_name] = node_name
            for i_node, node_name in enumerate(input_entity.l_e_top_names):
                lib_node_name = l_lib[input_entity.lib_map].gseim_e_top_names[i_node]
                assignments[lib_node_name] = node_name
            for i_node, node_name in enumerate(input_entity.l_e_bottom_names):
                lib_node_name = l_lib[input_entity.lib_map].gseim_e_bottom_names[i_node]
                assignments[lib_node_name] = node_name

            for i_node, node_name in enumerate(input_entity.l_b_left_names):
                lib_node_name = l_lib[input_entity.lib_map].gseim_b_left_names[i_node]
                assignments[lib_node_name] = node_name
            for i_node, node_name in enumerate(input_entity.l_b_right_names):
                lib_node_name = l_lib[input_entity.lib_map].gseim_b_right_names[i_node]
                assignments[lib_node_name] = node_name
            for i_node, node_name in enumerate(input_entity.l_b_top_names):
                lib_node_name = l_lib[input_entity.lib_map].gseim_b_top_names[i_node]
                assignments[lib_node_name] = node_name
            for i_node, node_name in enumerate(input_entity.l_b_bottom_names):
                lib_node_name = l_lib[input_entity.lib_map].gseim_b_bottom_names[i_node]
                assignments[lib_node_name] = node_name

            for k, v in input_entity.gseim_prm.items():
                v1 = v.strip()

#               We will reserve some keywords for GUI use only. Skip them here.
                l_exclude = [
                   'name',
                   'drawing_scheme',
                   'port_block_x',
                   'port_block_y',
                   'port_sep_x',
                   'port_sep_y',
                   'port_offset_top',
                   'port_offset_bottom',
                   'port_offset_left',
                   'port_offset_right',
                   'rotate_strict',
                   'mirror',
                ]
                if k not in l_exclude:
                    if v1 != l_lib[input_entity.lib_map].gseim_prm[k]:
                        assignments[k] = v1

            cct_elems.append((kind, assignments))

    return cct_elems

def make_prm_files(input_entity, l_lib, dir_block, l_files):
    if input_entity.flag_subckt:
        name1 = input_entity.name.split('$')[0]
        hblock_fname = lookup_subckt(name1)
        if os.path.exists(hblock_fname):
            parm_fname = os.path.join(os.path.dirname(hblock_fname), name1 + '_parm.py')
            if os.path.exists(parm_fname):
                input_entity.flag_prm_file = True
                input_entity.prm_file_name = parm_fname
                l_files.append(parm_fname)

            for i in input_entity.children:
                make_prm_files(i, l_lib, dir_block, l_files)
    else:
        return

def compute_prm_0(prm_filename, f, d_prm):
    importlib.invalidate_caches()
    import compute_prm as comp
    importlib.reload(comp)

    method_to_call = getattr(comp, f)
    method_to_call(d_prm)

def compute_prm_1(input_entity, d_prm):
    if input_entity.flag_top:
        name1 = os.path.splitext(os.path.basename(input_entity.prm_file_name))[0]
        f = name1.split('/')[-1]
    else:
        name1 = input_entity.name.split('$')[0]
        f = name1.split('/')[-1] + '_parm'

#   need to remove path from name1, keeping only the last part:
    compute_prm_0(input_entity.prm_file_name, f, d_prm)

    for prm in d_prm.values():
        if not isinstance(prm, str):
            print('compute_prm_1: prm is expected to be type str', flush=True)
            print('  check this:' + prm, flush=True)
            sys.exit()

def compute_prm_2(input_entity):
    if input_entity.flag_subckt:
#       assign parameters coming from parent:
        for k, v in input_entity.gseim_prm.items():
            if v in input_entity.parent.gseim_prm.keys():
                input_entity.gseim_prm[k] = input_entity.parent.gseim_prm[v]

#       evaluate parameters for this entity:
        if input_entity.flag_prm_file:
            compute_prm_1(input_entity, input_entity.gseim_prm)

#       check if any parameter = 'computed'
        for k, v in input_entity.gseim_prm.items():
            if v == 'computed':
                print('compute_prm_2: parameter', k, 'has not been computed.',
                  'Halting...', flush=True)
                sys.exit()

#       treat children:
        for i in input_entity.children:
            compute_prm_2(i)
    else:
#       assign parameters coming from parent:
        for k, v in input_entity.gseim_prm.items():
            if v in input_entity.parent.gseim_prm.keys():
                input_entity.gseim_prm[k] = input_entity.parent.gseim_prm[v]

def assign_node_map(input_entity, l_lib):

    if input_entity.flag_top:
        l_connections = input_entity.l_connections
    else:
        l_connections = l_lib[input_entity.lib_map].l_connections

    if input_entity.flag_top or input_entity.flag_subckt:
        for i_c, connection in enumerate(l_connections):
            if connection[1].startswith('e'):
                input_entity.node_type[i_c] = 'elec'
                name1 = connection[0]
                node1 = int(connection[1][2:])
                if connection[1].startswith('el'):
                    for i in input_entity.children:
                        if i.name == name1:
                            input_entity.node_map[i_c] = i.l_e_left_names[node1]
                elif connection[1].startswith('er'):
                    for i in input_entity.children:
                        if i.name == name1:
                            input_entity.node_map[i_c] = i.l_e_right_names[node1]
                elif connection[1].startswith('et'):
                    for i in input_entity.children:
                        if i.name == name1:
                            input_entity.node_map[i_c] = i.l_e_top_names[node1]
                elif connection[1].startswith('eb'):
                    for i in input_entity.children:
                        if i.name == name1:
                            input_entity.node_map[i_c] = i.l_e_bottom_names[node1]
                else:
                    print ('assign_node_map: error in e nodes. Halting...')
                    sys.exit()

            elif connection[1].startswith('b'):
                input_entity.node_type[i_c] = 'bus'
                name1 = connection[0]
                node1 = int(connection[1][2:])
                if connection[1].startswith('bl'):
                    for i in input_entity.children:
                        if i.name == name1:
                            input_entity.node_map[i_c] = i.l_b_left_names[node1]
                elif connection[1].startswith('br'):
                    for i in input_entity.children:
                        if i.name == name1:
                            input_entity.node_map[i_c] = i.l_b_right_names[node1]
                elif connection[1].startswith('bt'):
                    for i in input_entity.children:
                        if i.name == name1:
                            input_entity.node_map[i_c] = i.l_b_top_names[node1]
                elif connection[1].startswith('bb'):
                    for i in input_entity.children:
                        if i.name == name1:
                            input_entity.node_map[i_c] = i.l_b_bottom_names[node1]
                else:
                    print ('assign_node_map: error in b nodes. Halting...')
                    sys.exit()

            else:
                input_entity.node_type[i_c] = 'flow'
                name1 = connection[0]
                in_node = int(connection[1])

                for i in input_entity.children:
                    if i.name == name1:
                        input_entity.node_map[i_c] = i.l_out_names[in_node]

        for i in input_entity.children:
            if i.flag_subckt:
                assign_node_map(i, l_lib)

def resolve_outvar(ov_main, ov_temp, input_entity, d_ov, l_lib,
    d_flag_resolved, d_resolved_outvars):

    ov_type = d_ov[ov_temp][0]

    if ov_type == 'connection':
        connection = d_ov[ov_temp][1]

        if input_entity.flag_top:
            index1 = input_entity.l_connections.index(connection)
        elif input_entity.flag_subckt:
            index1 = l_lib[input_entity.lib_map].l_connections.index(connection)
        else:
            print('resolve_outvar: connection not resolved. Halting...',
              flush=True)
            sys.exit()

        d_flag_resolved[ov_main] = True

        if connection[1].startswith('e'):
            d_resolved_outvars[ov_main] = 'nodev_of_' + input_entity.node_map[index1]
        else:
            d_resolved_outvars[ov_main] = 'xvar_of_' + input_entity.node_map[index1]
        return
    elif ov_type == 'outvar':
        e_name, ov_name = d_ov[ov_temp][1]

        if input_entity.flag_top:
            for child in input_entity.children:
                if child.name == e_name:
                    if child.flag_block:
                        d_flag_resolved[ov_main] = True
                        d_resolved_outvars[ov_main] = ov_name + '_of_' + e_name
                        return
                    else:
                        d0 = l_lib[child.lib_map].d_outvars
                        resolve_outvar(ov_main, ov_name, child, d0, l_lib,
                           d_flag_resolved, d_resolved_outvars)
        elif input_entity.flag_subckt:
            for child in input_entity.children:
                if child.name == e_name:
                    if child.flag_block:
                        d_flag_resolved[ov_main] = True
                        d_resolved_outvars[ov_main] = ov_name + '_of_' + child.netlist_name
                        return
                    else:
                        d0 = l_lib[child.lib_map].d_outvars
                        resolve_outvar(ov_main, ov_name, child, d0, l_lib,
                           d_flag_resolved, d_resolved_outvars)
        else:
            print('resolve_outvar: outvar not resolved (2). Halting...',
              flush=True)
            sys.exit()

# main program:
def main(gseim_file, cct_file):
    dir_block = files('gseim').joinpath('data', 'blocks')
    dir_xbe   = files('gseim').joinpath('data', 'xbe')
    dir_ebe   = files('gseim').joinpath('data', 'ebe')
    dir_bbe   = files('gseim').joinpath('data', 'bbe')

    dir_cct = os.path.dirname(cct_file)

    print('Parser started. Wait for Program Completed message...', flush=True) 

    if flag_read_yml_once:
        if cct_file in d_yml.keys():
            data = d_yml[cct_file]
        else:
            with open(os.path.expanduser(cct_file)) as f:
                data = yaml.safe_load(f)
            d_yml[cct_file] = data
    else:
        with open(os.path.expanduser(cct_file)) as f:
            data = yaml.safe_load(f)

    data = treat_text_blocks(data)

    data = treat_connectors_e(data, 'connector_e')
    data = treat_connectors_e(data, 'connector_b')
    data = treat_connectors_f(data)
    data = treat_virtual(data)

    cct1 = cct('top_cct', True)
    cct1.grc_file_name = cct_file

    cct1.l_connections = data['connections']
    cct1.d_outvars = data['outvars']

    if len(cct1.l_connections) > 0:
        l_top_elements = get_unique_names_1(cct1.l_connections)
    else:
        l_top_elements = []
        for i in data['blocks']:
            for k,v in i.items():
                if k == 'name':
                    l_top_elements.append(v)

    n_main_nodes = len(cct1.l_connections)
    for i in range(n_main_nodes):
        cct1.node_map.append(None)
        cct1.node_type.append(None)

# n_sub[0]: number of subckts in the circuit
# n_sub[1]: depth of the circuit
    n_sub = [0, 0]

    for i in l_top_elements:
        parse_1(cct1, i, dir_block, n_sub)

# prepare list of library entities (which includes basic
# elements and subcircuits)

    l_lib = []
    for i in cct1.children:
        make_lib(i, l_lib, dir_block)

# prepare dict (to be used in circuit node names):
    d_names = {}
    d_names[cct1.name] = 0
    for i in cct1.children:
        make_dict_1(i, d_names)

    node_counter = 0
    assign_node_names(cct1, l_lib, cct_file,
        dir_block, d_names, node_counter)

# extract input/output nodes names from GSEIM templates.
# Assume the template name to be xxx.xbe/xxx.ebe where
# xxx comes from l_lib.

    for i in l_lib:
        if not i.flag_subckt:
            name1 = i.name
            if not name1.startswith('pad'):
                if i.has_b_nodes:
                    filename = os.path.join(dir_bbe, name1 + '.bbe')
                elif i.has_e_nodes:
                    filename = os.path.join(dir_ebe, name1 + '.ebe')
                else:
                    filename = os.path.join(dir_xbe, name1 + '.xbe')

                if i.has_b_nodes:

                    gu.extract_strings_2(filename, 'in_nodes:', i.gseim_in_names)
                    gu.extract_strings_2(filename, 'out_nodes:', i.gseim_out_names)

                    l_e_nodes = []
                    gu.extract_strings_2(filename, 'e_nodes:', l_e_nodes)
                    l1 = list([i.n_e_left, i.n_e_right, i.n_e_top, i.n_e_bottom])
                    l_e_nodes_it = iter(l_e_nodes)
                    i.gseim_e_left_names, i.gseim_e_right_names, \
                    i.gseim_e_top_names, i.gseim_e_bottom_names = \
                      [list(islice(l_e_nodes_it,elem)) for elem in l1]

                    l_b_nodes = []
                    gu.extract_strings_2(filename, 'b_nodes:', l_b_nodes)
                    l1 = list([i.n_b_left, i.n_b_right, i.n_b_top, i.n_b_bottom])
                    l_b_nodes_it = iter(l_b_nodes)
                    i.gseim_b_left_names, i.gseim_b_right_names, \
                    i.gseim_b_top_names, i.gseim_b_bottom_names = \
                      [list(islice(l_b_nodes_it,elem)) for elem in l1]
                else:
                    if i.has_flow_nodes:
                        if i.has_e_nodes:
                            l1 = list([i.n_in, i.n_out])
                            l_xvars = []
                            gu.extract_strings_2(filename, 'x_vars:', l_xvars)
                            l_xvars_it = iter(l_xvars)
                            i.gseim_in_names, i.gseim_out_names = \
                              [list(islice(l_xvars_it,elem)) for elem in l1]
                        else:
                            gu.extract_strings_2(filename, 'input_vars:', i.gseim_in_names)
                            gu.extract_strings_2(filename, 'output_vars:', i.gseim_out_names)

                    if i.has_e_nodes:
                        l_e_nodes = []
                        gu.extract_strings_2(filename, 'nodes:', l_e_nodes)

                        l1 = list([i.n_e_left, i.n_e_right, i.n_e_top, i.n_e_bottom])
                        l_e_nodes_it = iter(l_e_nodes)
                        i.gseim_e_left_names, i.gseim_e_right_names, \
                        i.gseim_e_top_names, i.gseim_e_bottom_names = \
                          [list(islice(l_e_nodes_it,elem)) for elem in l1]

                        if i.n_in:
                            gu.extract_strings_2(filename, 'x_vars:', i.gseim_in_names)
                        if i.n_out:
                            gu.extract_strings_2(filename, 'x_vars:', i.gseim_out_names)

                if not i.has_b_nodes:
                    gu.extract_dict_1(filename, 'rparms:', i.gseim_prm)

# replace node names (subckt parsing):
    for depth in range(n_sub[1]):
        for i in cct1.children:
            replace_nodes_1(i, l_lib)

# make a list of files to be called for prm computation:
    l_prm_files = []

# main circuit file:
    filename = cct_file.replace('.grc', '_parm.py')
    if os.path.exists(filename):
        cct1.flag_prm_file = True
        cct1.prm_file_name = filename
        l_prm_files.append(filename)

    for i in cct1.children:
        make_prm_files(i, l_lib, dir_block, l_prm_files)

# concatenate the prm computation files:
# for subckt s_1, expect file s_1_parm.py

    filename = os.path.join(dir_cct, 'compute_prm.py')
    sys.path.insert(0, dir_cct)
    with open(os.path.expanduser(filename), 'w') as f_out:
        for infile in l_prm_files:
            with open(os.path.expanduser(infile), 'r') as f_in:
                f_out.write(f_in.read())

    assign_gseim_prm(cct1, l_lib, cct_file, dir_block)

# evaluate main cct prm expression, if file exisits:

    if cct1.flag_prm_file:
        compute_prm_1(cct1, cct1.gseim_prm)

    for k, v in cct1.gseim_prm.items():
        if v == 'computed':
            print('main: parameter', k, 'has not been computed.',
              'Halting...', flush=True)
            sys.exit()

# evaluate prm expression for rest of the tree:

    for i in cct1.children:
        compute_prm_2(i)

    sys.path.pop(0)

    assign_node_map(cct1, l_lib)

# prepare names of leaf blocks as they would appear in the netlist
# (and also in outvars)

    for i in cct1.children:
        make_netlist_name(i, l_lib)

    d_resolved_outvars = {}
    d_flag_resolved = {}
    for k in cct1.d_outvars.keys():
        d_flag_resolved[k] = False

    for k in cct1.d_outvars.keys():
        resolve_outvar(k, k, cct1, cct1.d_outvars, l_lib,
           d_flag_resolved, d_resolved_outvars)

    for k in cct1.d_outvars.keys():
        if not d_flag_resolved[k]:
            print('main: outvar', k, ' not resolved. Halting...', flush=True)
            sys.exit()

    cct_ast = CctFile(os.path.basename(cct_file))
    for i in cct1.children:
        cct_ast.cct_elems.extend(build_cct_elements(i, l_lib))

    cct_ast.cct_elems = [
        (orig_cct_kind, orig_cct_assignments)
        for orig_cct_kind, orig_cct_assignments in cct_ast.cct_elems
        if orig_cct_assignments['type'] != 'dummy_b'
    ]

    for i_pass, _ in enumerate(cct_ast.cct_elems):
        flag_belement = False
        for i_line, cct_elem in enumerate(cct_ast.cct_elems):
            cct_elem_kind, cct_elem_assignments = cct_elem
            l_bus_line_no = []

            if cct_elem_kind == 'belement':
                flag_belement = True

                bus_type = cct_elem_assignments['type']
                l_bus_line_no.append(i_line)
                bus_cct_node = cct_elem_assignments[bus_type]

                if bus_type.startswith('bus_e_'):
                     bus_type_to_search = bus_type
                elif bus_type.startswith('bus_f_i_'):
                     bus_type_to_search = bus_type.replace('_i_', '_o_')
                elif bus_type.startswith('bus_f_o_'):
                     bus_type_to_search = bus_type.replace('_o_', '_i_')

                for i1_line in range(i_line+1, len(cct_ast.cct_elems)):
                    l1_cct_elem_kind, l1_cct_elem_assignments = cct_ast.cct_elems[i1_line]
                    if l1_cct_elem_kind == 'belement':
                        bus1_type = l1_cct_elem_assignments['type']
                        if bus1_type == bus_type_to_search:
                            bus1_cct_node = l1_cct_elem_assignments[bus1_type]
                            if bus1_cct_node == bus_cct_node:
                                l_bus_line_no.append(i1_line)

                d_nodemap = {}
                s1 = 'b' + str(i_pass + 1) + '_'
                for i1_line in l_bus_line_no:
                    l1_cct_elem_kind, l1_cct_elem_assignments = cct_ast.cct_elems[i1_line]
                    bus1_type = l1_cct_elem_assignments['type']

                    if bus1_type.startswith('bus_e_'):
                        n_nodes = int(bus1_type.split('bus_e_')[-1])
                        for i_node in range(n_nodes):
                            cct_node = l1_cct_elem_assignments['e' + str(i_node + 1)]
                            if cct_node not in d_nodemap.keys():
                                d_nodemap[cct_node] = s1 + str(i_node + 1)
                    elif bus1_type.startswith('bus_f_i_'):
                        n_nodes = int(bus1_type.split('bus_f_i_')[-1])
                        for i_node in range(n_nodes):
                            cct_node = l1_cct_elem_assignments['out' + str(i_node + 1)]
                            if cct_node not in d_nodemap.keys():
                                d_nodemap[cct_node] = s1 + str(i_node + 1)
                    elif bus1_type.startswith('bus_f_o_'):
                        n_nodes = int(bus1_type.split('bus_f_o_')[-1])
                        for i_node in range(n_nodes):
                            cct_node = l1_cct_elem_assignments['in' + str(i_node + 1)]
                            if cct_node not in d_nodemap.keys():
                                d_nodemap[cct_node] = s1 + str(i_node + 1)

                for i1_line, cct_elem in enumerate(cct_ast.cct_elems):
                    l1_cct_elem_kind, l1_cct_elem_assignments = cct_elem
                    if i1_line not in l_bus_line_no:
                        for k, v in l1_cct_elem_assignments.items():
                            if v in d_nodemap:
                                new_node = d_nodemap[v]
                                l1_cct_elem_assignments[k] = new_node

                for k, v in d_resolved_outvars.items():
                    if v.startswith('nodev_of_'):
                        old_node = v.split('nodev_of_')[-1]
                        if old_node in d_nodemap.keys():
                            d_resolved_outvars[k] = 'nodev_of_' + d_nodemap[old_node]
                    elif v.startswith('xvar_of_'):
                        old_node = v.split('xvar_of_')[-1]
                        if old_node in d_nodemap.keys():
                            d_resolved_outvars[k] = 'xvar_of_' + d_nodemap[old_node]

                cct_ast.cct_elems = [
                    cct_elem for idx, cct_elem in enumerate(cct_ast.cct_elems)
                    if idx not in l_bus_line_no
                ]

                break

        if not flag_belement:
            break;

    filename = str(files('gseim.data').joinpath('gseim_slvparms.in'))
    slvlib_ast = parse_parms_file(filename)
    l_solve_blocks = sorted(data['solve_blocks'], key=lambda x: int(x['index']))
    l_output_blocks = data['output_blocks']

# take care of ground elements/ref_node statement

    n_grounds = 0

    for _ in cct_ast.cct_elems:
        flag_ground = False
        for k, cct_elem in enumerate(cct_ast.cct_elems):
            cct_elem_kind, cct_elem_assignments = cct_elem
            if cct_elem_kind == 'eelement':
                if cct_elem_assignments['type'] == 'ground':
                    flag_ground = True
                    k_ground = k
                    n_grounds += 1
                    node_ground = cct_elem_assignments['g']
                    break
        if not flag_ground:
            break
        del cct_ast.cct_elems[k_ground]
        for j, cct_elem1 in enumerate(cct_ast.cct_elems):
            cct_elem_kind, cct_elem_assignments = cct_elem1
            if cct_elem_kind == 'eelement':
                for k, v in cct_elem_assignments.items():
                    if v == node_ground:
                        cct_elem_assignments[k] = '0'

    cct_ast.cct_elems = [
        (cct_elem_kind, cct_elem_assignments)
        for cct_elem_kind, cct_elem_assignments in cct_ast.cct_elems
        if 'dummy_' not in cct_elem_assignments['name']
    ]

    if n_grounds > 0:
        cct_ast.cct_assignments['ref_node'] = '0'

    n_ebe = n_xbe = 0
    for cct_elem_kind, cct_elem_assignments in cct_ast.cct_elems:
        if cct_elem_kind == 'eelement': n_ebe += 1
        if cct_elem_kind == 'xelement': n_xbe += 1

    if n_ebe > 0:
        if n_grounds == 0:
            print('main: no ref node in the circuit? Halting...')
            sys.exit()

    for k, v in d_resolved_outvars.items():
        cct_ast.cct_outvars[k] = v

    for slv in l_solve_blocks:
        slv_block = SolveBlock()
        cct_ast.solve_blocks.append(slv_block)

        slv_block.assignments['solve_type'] = slv['d_parms']['solve_type']
        slv_block.assignments['initial_sol'] = slv['d_parms']['initial_sol']
        if slv['d_parms']['initial_sol'] == 'read_from_file':
            slv_block.assignments['initial_sol_file'] = slv['d_parms']['initial_sol_file']

        l_skip = [
          'solve_type',
          'initial_sol',
          'initial_sol_file',
          'output_solution_file',
          'block_index',
        ]

        for k, v in slv['d_parms'].items():
            if k not in l_skip:
                if v in [slvlib_ast.parms[k].default, 'none']:
                    if slvlib_ast.parms[k].force_write:
                        v1 = cct1.gseim_prm[v] if v in cct1.gseim_prm.keys() else v
                        slv_block.methods.append((k, v1))
                else:
                    v1 = cct1.gseim_prm[v] if v in cct1.gseim_prm.keys() else v
                    slv_block.methods.append((k, v1))

        for out_name in slv['output_blocks']:
            l_out_1 = list(filter(lambda x: x['name'] == out_name, l_output_blocks))
            if len(l_out_1) != 1:
                print('did not find output block', out_name,
                  'in circuit file. Halting...', flush=True)
                sys.exit()
            out = l_out_1[0]
            output_block = {'assignments': {}, 'control': [], 'variables': [[]]}
            slv_block.output_blocks.append(output_block)
            if out['d_parms']['filename'] == 'none':
                print('filename not specified in output block', out['name'], flush=True)
                print('Halting...', flush=True)
                sys.exit()

            output_block['assignments']['filename'] = out['d_parms']['filename']
            if out['d_parms']['append'] == 'yes':
                output_block['assignments']['append'] = 'yes'

            output_block['assignments']['limit_lines'] = out['d_parms']['limit_lines']

            control_block = {}
            if out['d_parms']['fixed_interval'] != 'none':
                control_block['fixed_interval'] = out['d_parms']['fixed_interval']
            if out['d_parms']['t_start'] != 'none':
                control_block['out_tstart'] = out['d_parms']['t_start']
            if out['d_parms']['t_end'] != 'none':
                control_block['out_tend'] = out['d_parms']['t_end']

            if control_block:
                output_block['control'].append([None, control_block])

            if not out['outvars']:
                print('no outvars specified in output block',
                  out.name, 'Halting...', flush=True)
                sys.exit()

            l_ov1 = list(cct1.d_outvars.keys())
            for ov in sorted(out['outvars'], key=l_ov1.index):
                output_block['variables'][0].append(ov)

    # check if there are xfer_fn elements in the circuit
    flag_filter_1 = False
    for cct_elem_kind, cct_elem_assignments in cct_ast.cct_elems:
        if cct_elem_kind == 'xelement' and cct_elem_assignments['type'] == 'xfer_fn':
            flag_filter_1 = True
            break

    if flag_filter_1:
        parse_filters.process_xfer_fns(cct_ast)

    with open(gseim_file, 'w') as fout:
        fout.write(cct_ast.dump())

    print('Program completed.', flush=True) 

def console_entry():
    if len(sys.argv) != 3:
        print('gseim-parser: need 2 arguments. Halting...')
        sys.exit()

    gseim_file = sys.argv[1]
    cct_file   = sys.argv[2]

    main(
        gseim_file,
        cct_file,
    )

if __name__ == '__main__':
    sys.exit(console_entry())
