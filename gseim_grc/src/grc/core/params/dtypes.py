# Copyright 2008-2017 Free Software Foundation, Inc.
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

import re

from .. import blocks
from .. import Constants

# Blacklist certain ids, its not complete, but should help

ID_BLACKLIST = ['self', 'options', 'gr', 'math', 'firdes']

validators = {}

def validates(*dtypes):
    def decorator(func):
        for dtype in dtypes:
            assert dtype in Constants.PARAM_TYPE_NAMES
            validators[dtype] = func
        return func
    return decorator

class ValidateError(Exception):
    """Raised by validate functions"""

@validates('id')
def validate_block_id(param):
    value = param.value
    # Can python use this as a variable?
    if not re.match(r'^[a-z|A-Z|_]*[\$0-9]*', value):
        raise ValidateError('ID "{}" must begin with a letter and may contain letters, numbers, '
                            'and underscores.'.format(value))
    if value in ID_BLACKLIST:
        raise ValidateError('ID "{}" is blacklisted.'.format(value))
    block_names = [block.name for block in param.parent_flowgraph.iter_enabled_blocks()]
    # Id should only appear once, or zero times if block is disabled
    if param.key == 'id' and block_names.count(value) > 1:
        raise ValidateError('ID "{}" is not unique.'.format(value))
    elif value not in block_names:
        raise ValidateError('ID "{}" does not exist.'.format(value))
    return value

@validates('name')
def validate_name(param):
    # Name of a function that will be generated literally not as a string
    value = param.value
    # Can python use this as a variable?
    if not re.match(r'^[a-z|A-Z|_]*[\$0-9]*', value):
        raise ValidateError('ID "{}" must begin with a letter and may contain letters, numbers, '
                            'and underscores.'.format(value))
    return value

