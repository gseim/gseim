"""
Copyright 2008-2016 Free Software Foundation, Inc.
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

import os
import numbers
import stat

from importlib_resources import files
import numpy

# Data files
DATA_DIR = os.path.dirname(__file__)

DEFAULT_FLOW_GRAPH = str(files('grc.core').joinpath('default_flow_graph.grc'))

# Param tabs
DEFAULT_PARAM_TAB = "General"
ADVANCED_PARAM_TAB = "Advanced"
DEFAULT_BLOCK_MODULE_NAME = '(no module specified)'

# Port domains
GR_STREAM_DOMAIN = "stream"
GR_MESSAGE_DOMAIN = "message"
DEFAULT_DOMAIN = GR_STREAM_DOMAIN

# File creation modes
HIER_BLOCK_FILE_MODE = stat.S_IRUSR | stat.S_IWUSR | stat.S_IRGRP | stat.S_IWGRP | stat.S_IROTH

PARAM_TYPE_NAMES = {
    'raw', 'enum',
    'complex', 'real', 'float', 'int',
    'complex_vector', 'real_vector', 'float_vector', 'int_vector',
    'hex', 'string', 'bool',
    'file_open', 'file_save', '_multiline', '_multiline_python_external',
    'id', 'stream_id','name',
    'gui_hint',
    'import',
}

# Updating colors. Using the standard color palette from:
# http://www.google.com/design/spec/style/color.html#color-color-palette
# Most are based on the main, primary color standard. Some are within
# that color's spectrum when it was deemed necessary.

GRC_COLOR_BROWN = '#795548'
GRC_COLOR_BLUE = '#2196F3'
GRC_COLOR_LIGHT_GREEN = '#8BC34A'
GRC_COLOR_GREEN = '#4CAF50'
GRC_COLOR_AMBER = '#FFC107'
GRC_COLOR_PURPLE = '#9C27B0'
GRC_COLOR_CYAN = '#00BCD4'
GRC_COLOR_GR_ORANGE = '#FF6905'

GRC_COLOR_ORANGE = '#fcb772'

GRC_COLOR_LIME = '#CDDC39'
GRC_COLOR_TEAL = '#009688'
GRC_COLOR_YELLOW = '#FFEB3B'
GRC_COLOR_PINK = '#F50057'

GRC_COLOR_PURPLE_A100 = '#EA80FC'
GRC_COLOR_PURPLE_A400 = '#D500F9'

GRC_COLOR_DARK_GREY = '#72706F'
GRC_COLOR_GREY = '#BDBDBD'
GRC_COLOR_WHITE = '#FFFFFF'

CORE_TYPES = (  # name, key, sizeof, color
    ('Complex Float 64',    'fc64', 16, GRC_COLOR_BROWN),
    ('Complex Float 32',    'fc32',  8, GRC_COLOR_BLUE),
    ('Complex Integer 64',  'sc64', 16, GRC_COLOR_LIGHT_GREEN),
    ('Complex Integer 32',  'sc32',  8, GRC_COLOR_GREEN),
    ('Complex Integer 16',  'sc16',  4, GRC_COLOR_AMBER),
    ('Complex Integer 8',    'sc8',  2, GRC_COLOR_PURPLE),
    ('Float 64',             'f64',  8, GRC_COLOR_CYAN),
    ('Float 32',             'f32',  4, GRC_COLOR_ORANGE),
    ('Integer 64',           's64',  8, GRC_COLOR_LIME),
    ('Integer 32',           's32',  4, GRC_COLOR_TEAL),
    ('Integer 16',           's16',  2, GRC_COLOR_YELLOW),
    ('Integer 8',             's8',  1, GRC_COLOR_PURPLE_A400),
    ('Bits (unpacked byte)', 'bit',  1, GRC_COLOR_PURPLE_A100),
    ('Async Message',    'message',  0, GRC_COLOR_GREY),
    ('Bus Connection',       'bus',  0, GRC_COLOR_WHITE),
    ('Wildcard',                '',  0, GRC_COLOR_WHITE),
)

ALIAS_TYPES = {
    'complex': (8, GRC_COLOR_BLUE),
    'float':   (4, GRC_COLOR_ORANGE),
    'int':     (4, GRC_COLOR_TEAL),
    'short':   (2, GRC_COLOR_YELLOW),
    'byte':    (1, GRC_COLOR_PURPLE_A400),
    'bits':    (1, GRC_COLOR_PURPLE_A100),
}

TYPE_TO_SIZEOF = {key: sizeof for name, key, sizeof, color in CORE_TYPES}

TYPE_TO_SIZEOF.update((key, sizeof) for key, (sizeof, _) in ALIAS_TYPES.items())
