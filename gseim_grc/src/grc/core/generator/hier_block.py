import collections
import os
from pathlib import Path

import codecs
from importlib_resources import files

from .top_block import TopBlockGenerator

from .. import Constants
from ..io import yaml

UNKNOWN_OUTPUT_DIR = \
    'Do not know where to write user hierarchical block. Please set ' \
    'the environment variable HIER_BLOCK_USER_DIR to the intended ' \
    'output directory.'

class HierBlockGenerator(TopBlockGenerator):
    """Extends the top block generator to also generate a block YML file"""

    def __init__(self, flow_graph, file_path):
        """
        Initialize the hier block generator object.

        Args:
            flow_graph: the flow graph object
            file_path: where to write the py file (the yml goes into HIER_BLOCK_LIB_DIR)
        """
        TopBlockGenerator.__init__(self, flow_graph, file_path)
        platform = flow_graph.parent

        subckt_grc_dir = files('gseim')/'data'/'subckt_grc'

        if subckt_grc_dir in Path(file_path).parents:
            # This hierarchical block is part of the application library. We
            # should generate into the library directory.
            hier_block_dir = platform.config.hier_block_lib_dir
        else:
            # This hierarchical block was created by a user and is not a part
            # of the application library. It should be written to the directory
            # specified by the environment variable HIER_BLOCK_USER_DIR. If
            # that variable is not set, we do not know where to write the
            # output.
            hier_block_dir = platform.config.hier_block_user_dir
            assert hier_block_dir is not None, UNKNOWN_OUTPUT_DIR

        self._mode = Constants.HIER_BLOCK_FILE_MODE
        print('HierBlockGenerator: hier_block_lib_dir:', hier_block_dir)
        yml_filename = self._flow_graph.get_option('id') + '.hblock.yml'
        self.file_path_yml = Path(hier_block_dir, yml_filename)
        print('HierBlockGenerator: self.file_path_yml:', self.file_path_yml)

    def write(self, flow_graph):
        """generate output and write it to files"""
        print('HierBlockGenerator: calling TopBlockGenerator.write')
        TopBlockGenerator.write(self)
        print('HierBlockGenerator: TopBlockGenerator.write over')

        data = yaml.dump(self._build_block_n_from_flow_graph_io(flow_graph))

        replace = [
            ('parameters:', '\nparameters:'),
            ('inputs:', '\ninputs:'),
            ('outputs:', '\noutputs:'),
            ('e_left_nodes:', '\ne_left_nodes:'),
            ('e_right_nodes:', '\ne_right_nodes:'),
            ('e_top_nodes:', '\ne_top_nodes:'),
            ('e_bottom_nodes:', '\ne_bottom_nodes:'),
            ('b_left_nodes:', '\nb_left_nodes:'),
            ('b_right_nodes:', '\nb_right_nodes:'),
            ('b_top_nodes:', '\nb_top_nodes:'),
            ('b_bottom_nodes:', '\nb_bottom_nodes:'),
            ('asserts:', '\nasserts:'),
            ('templates:', '\ntemplates:'),
            ('documentation:', '\ndocumentation:'),
        ]

        for r in replace:
            data = data.replace(*r)

        print('HierBlockGenerator: self.file_path_yml:', self.file_path_yml)
        with codecs.open(os.path.expanduser(self.file_path_yml), 'w', encoding='utf-8') as fp:
            fp.write(data)

        # Windows only supports S_IREAD and S_IWRITE, other flags are ignored
        os.chmod(self.file_path_yml, self._mode)

    def _build_block_n_from_flow_graph_io(self, flow_graph):
        """
        Generate a block YML nested data from the flow graph IO

        Returns:
            a yml node tree
        """
        # Extract info from the flow graph
        block_id = self._flow_graph.get_option('id')
        parameters = self._flow_graph.get_parameters()

        def var_or_value(name):
            if name in (p.name for p in parameters):
                return "${" + name + " }"
            return name

        # Build the nested data
        data = collections.OrderedDict()
        data['id'] = block_id
        data['label'] = (
            self._flow_graph.get_option('title') or
            self._flow_graph.get_option('id').replace('_', ' ').title()
        )
        data['category'] = self._flow_graph.get_option('category')

#       Parameters
        data['parameters'] = []

        if 'name' not in [dx['id'] for dx in data['parameters']]:
            p = collections.OrderedDict()
            p['id'] = 'name'
            p['label'] = 'name'
            p['dtype'] = 'string'
            p['default'] = 'none'
            data['parameters'].append(p)

        if 'rotate_strict' not in flow_graph.gparms.keys():
            flow_graph.gparms['rotate_strict'] = 'no'

        if 'mirror' not in flow_graph.gparms.keys():
            flow_graph.gparms['mirror'] = 'none'

        if 'port_sep_x' not in flow_graph.gparms.keys():
            flow_graph.gparms['port_sep_x'] = '4'
        if 'port_sep_y' not in flow_graph.gparms.keys():
            flow_graph.gparms['port_sep_y'] = '4'
        if 'port_block_x' not in flow_graph.gparms.keys():
            flow_graph.gparms['port_block_x'] = '4'
        if 'port_block_y' not in flow_graph.gparms.keys():
            flow_graph.gparms['port_block_y'] = '4'

        if 'drawing_scheme' in flow_graph.gparms.keys():
            if flow_graph.gparms['drawing_scheme'] == 'symbol':
                if 'port_offset_l' not in flow_graph.gparms.keys():
                    flow_graph.gparms['port_offset_l'] = '0'
                if 'port_offset_r' not in flow_graph.gparms.keys():
                    flow_graph.gparms['port_offset_r'] = '0'
                if 'port_offset_t' not in flow_graph.gparms.keys():
                    flow_graph.gparms['port_offset_t'] = '0'
                if 'port_offset_b' not in flow_graph.gparms.keys():
                    flow_graph.gparms['port_offset_b'] = '0'
        else:
            flow_graph.gparms['drawing_scheme'] = 'name'

#       write gparms which the user would be interested in editing (when the sub ckt is
#       called), then followed by gparms such as port_sep_x which the user would not
#       be editing.

        l1 = [
          'rotate_strict',
          'mirror',
          'port_sep_x',
          'port_sep_y',
          'port_block_x',
          'port_block_y',
          'drawing_scheme',
          'port_offset_l',
          'port_offset_r',
          'port_offset_t',
          'port_offset_b',
        ]

        for k, v in flow_graph.gparms.items():
            if k not in l1:
                p = collections.OrderedDict()
                p['id'] = k
                p['label'] = k
                p['default'] = v
                data['parameters'].append(p)

        for k, v in flow_graph.gparms.items():
            if k in l1:
                p = collections.OrderedDict()
                p['id'] = k
                p['label'] = k
                p['default'] = v
                data['parameters'].append(p)

#       outvars
        data['outvars'] = []

        for k in flow_graph.outvars.keys():
            data['outvars'].append(k)

        # Ports
        t_dir = (
          'inputs', 'outputs',
          'e_left_nodes', 'e_right_nodes', 'e_top_nodes', 'e_bottom_nodes',
          'b_left_nodes', 'b_right_nodes', 'b_top_nodes', 'b_bottom_nodes',
        )
        for direction in t_dir:
            data[direction] = []
            for port in get_hier_block_io(self._flow_graph, direction):
                p = collections.OrderedDict()
                p['label'] = port.parent.params['label'].value
                p['dtype'] = port.dtype

                print('HierBlockGenerator: _build_block_n_from_flow_graph_io: p[label]:', p['label'])

                data[direction].append(p)

        data['grc_source'] = str(self._flow_graph.grc_file_path)

        return data

def get_hier_block_io(flow_graph, direction, domain=None):
    """
    Get a list of io ports for this flow graph.

    Returns a list of blocks
    """

    if direction == 'inputs':
        pads = flow_graph.get_pad_sources()
    elif direction == 'outputs':
        pads = flow_graph.get_pad_sinks()
    elif direction == 'e_left_nodes':
        pads = flow_graph.get_pad_e_lefts()
    elif direction == 'e_right_nodes':
        pads = flow_graph.get_pad_e_rights()
    elif direction == 'e_top_nodes':
        pads = flow_graph.get_pad_e_tops()
    elif direction == 'e_bottom_nodes':
        pads = flow_graph.get_pad_e_bottoms()
    elif direction == 'b_left_nodes':
        pads = flow_graph.get_pad_b_lefts()
    elif direction == 'b_right_nodes':
        pads = flow_graph.get_pad_b_rights()
    elif direction == 'b_top_nodes':
        pads = flow_graph.get_pad_b_tops()
    elif direction == 'b_bottom_nodes':
        pads = flow_graph.get_pad_b_bottoms()

    for pad in pads:
        if direction == 'inputs':
            for port in pad.sources:
                yield port
        elif direction == 'outputs':
            for port in pad.sinks:
                yield port
        elif direction == 'e_left_nodes':
            for port in pad.e_rights:
                yield port
        elif direction == 'e_right_nodes':
            for port in pad.e_lefts:
                yield port
        elif direction == 'e_top_nodes':
            for port in pad.e_bottoms:
                yield port
        elif direction == 'e_bottom_nodes':
            for port in pad.e_tops:
                yield port
        elif direction == 'b_left_nodes':
            for port in pad.b_rights:
                yield port
        elif direction == 'b_right_nodes':
            for port in pad.b_lefts:
                yield port
        elif direction == 'b_top_nodes':
            for port in pad.b_bottoms:
                yield port
        elif direction == 'b_bottom_nodes':
            for port in pad.b_tops:
                yield port
