"""Copyright 2016 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
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

import os
from os.path import expanduser, normpath, expandvars, exists

from . import Constants

class Config(object):
#   name = 'GNU Radio Companion (no gui)'
    name = 'GSEIM'
    license = __doc__.strip()
    website = 'https://gseim.github.io/'

    home_dir = "."

    hier_block_lib_dir = home_dir + '/gseim_grc/subckt'
    block_lib_dir      = home_dir + '/gseim_grc/blocks'
    gseim_output_dir   = home_dir + '/gseim_grc/gseim/output'
    gseim_xbe_dir      = home_dir + '/gseim_grc/gseim/xbe'
    gseim_ebe_dir      = home_dir + '/gseim_grc/gseim/ebe'
    gseim_bbe_dir      = home_dir + '/gseim_grc/gseim/bbe'
    gseim_exec_dir     = home_dir + '/gseim_grc/gseim/exec'
    gseim_cpp_dir      = home_dir + '/gseim_grc/gseim/cppsrc'

    def __init__(self, version, version_parts=None, name=None, prefs=None):
        self._gr_prefs = DummyPrefs()
        self.version = version
        self.version_parts = version_parts or version[1:].split('-', 1)[0].split('.')[:3]
        if name:
            self.name = name

    @property
    def block_paths(self):

        valid_paths = [self.hier_block_lib_dir, self.block_lib_dir]

        return valid_paths

    @property
    def default_flow_graph(self):
        user_default = (
            os.environ.get('GRC_DEFAULT_FLOW_GRAPH') or
            self._gr_prefs.get_string('grc', 'default_flow_graph', '') or
            os.path.join(self.hier_block_lib_dir, 'default_flow_graph.grc')
        )
        return user_default if exists(user_default) else Constants.DEFAULT_FLOW_GRAPH

class DummyPrefs(object):

    def get_string(self, category, item, default):
        return str(default)

    def set_string(self, category, item, value):
        pass

    def get_long(self, category, item, default):
        return int(default)

    def save(self):
        pass
