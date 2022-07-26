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

import os
from os.path import expanduser, normpath, expandvars, exists

import appdirs
from importlib_resources import files

from . import Constants

class Config(object):
#   name = 'GNU Radio Companion (no gui)'
    name = 'GSEIM'
    license = __doc__.strip()
    website = 'https://gseim.github.io/'

    hier_block_lib_dir = str(files('gseim').joinpath('data', 'subckt'))
    block_lib_dir      = str(files('gseim').joinpath('data', 'blocks'))
    gseim_xbe_dir      = str(files('gseim').joinpath('data', 'xbe'))
    gseim_ebe_dir      = str(files('gseim').joinpath('data', 'ebe'))
    gseim_bbe_dir      = str(files('gseim').joinpath('data', 'bbe'))
    gseim_cache_dir    = appdirs.user_cache_dir('gseim')

    def __init__(self, version, version_parts=None, name=None, prefs=None):
        self._gr_prefs = DummyPrefs()
        self.version = version
        self.version_parts = version_parts or version[1:].split('-', 1)[0].split('.')[:3]
        if name:
            self.name = name

        if not os.path.exists(self.gseim_cache_dir):
            os.makedirs(self.gseim_cache_dir)

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
