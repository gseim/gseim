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

import os

import sys

from .hier_block import HierBlockGenerator
from .top_block import TopBlockGenerator

DATA_DIR = os.path.dirname(__file__)

class Generator(object):
    """Adaptor for various generators (uses generate_options)"""

    def __init__(self, flow_graph, file_path):
        """
        Initialize the generator object.
        Determine the file to generate.

        Args:
            flow_graph: the flow graph object
            file_path: the path to the grc file
        """
        print('/core/generator/Generator.py: file_path:', file_path)
        self.generate_options = flow_graph.get_option('generate_options')

        if self.generate_options == 'hb':
            generator_cls = HierBlockGenerator
        else:
            generator_cls = TopBlockGenerator

        self._generator = generator_cls(flow_graph, file_path)

    def __getattr__(self, item):
        """get all other attrib from actual generator object"""
        return getattr(self._generator, item)
