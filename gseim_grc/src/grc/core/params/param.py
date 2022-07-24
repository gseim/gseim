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

import ast
import collections
import textwrap

from .. import Constants
from ..base import Element
from ..utils.descriptors import Evaluated, EvaluatedEnum, setup_names

from . import dtypes

attributed_str = type('attributed_str', (str,), {})

@setup_names
class Param(Element):

    is_param = True

    name = Evaluated(str, default='no name')
    dtype = EvaluatedEnum(Constants.PARAM_TYPE_NAMES, default='raw')

    hide = EvaluatedEnum('none all part')

    # region init
    def __init__(self, parent, id, label='', dtype='raw', default='',
                 options=None, option_labels=None, option_attributes=None,
                 category='', hide='none', **_):
        """Make a new param from nested data"""
        super(Param, self).__init__(parent)
        self.key = id
        self.name = label.strip() or id.title()
        self.category = category or Constants.DEFAULT_PARAM_TAB

        self.dtype = dtype
        self.value = self.default = str(default)

        self.options = self._init_options(options or [], option_labels or [],
                                          option_attributes or {})
        self.hide = hide or 'none'
        # end of args ########################################################

        self._evaluated = None
        self._stringify_flag = False
        self._lisitify_flag = False
        self.hostage_cells = set()
        self._init = False
        self.scale = {
            'E': 1e18,
            'P': 1e15,
            'T': 1e12,
            'G': 1e9,
            'M': 1e6,
            'k': 1e3,
            'm': 1e-3,
            'u': 1e-6,
            'n': 1e-9,
            'p': 1e-12,
            'f': 1e-15,
            'a': 1e-18,
        }
        self.scale_factor = None
        self.number = None

    def _init_options(self, values, labels, attributes):
        """parse option and option attributes"""
        options = collections.OrderedDict()
        options.attributes = collections.defaultdict(dict)

        padding = [''] * max(len(values), len(labels))
        attributes = {key: value + padding for key, value in iter(attributes.items())}

        for i, option in enumerate(values):
            # Test against repeated keys
            if option in options:
                raise KeyError('Value "{}" already exists in options'.format(option))
            # get label
            try:
                label = str(labels[i])
            except IndexError:
                label = str(option)
            # Store the option
            options[option] = label
            options.attributes[option] = {attrib: values[i] for attrib, values in iter(attributes.items())}

        default = next(iter(options)) if options else ''
        if not self.value:
            self.value = self.default = default

        if self.is_enum() and self.value not in options:
            self.value = self.default = default  # TODO: warn
            # raise ValueError('The value {!r} is not in the possible values of {}.'
            #                  ''.format(self.get_value(), ', '.join(self.options)))
        return options
    # endregion

    def __str__(self):
        return 'Param - {}({})'.format(self.name, self.key)

    def __repr__(self):
        return '{!r}.param[{}]'.format(self.parent, self.key)

    def is_enum(self):
        return self.get_raw('dtype') == 'enum'

    def get_value(self):
        value = self.value
        if self.is_enum() and value not in self.options:
            value = self.default
            self.set_value(value)
        return value

    def set_value(self, value):
        # Must be a string
        self.value = str(value)

    def set_default(self, value):
        if self.default == self.value:
            self.set_value(value)
        self.default = str(value)

    def rewrite(self):
        Element.rewrite(self)
        del self.name
        del self.dtype
        del self.hide

        self._evaluated = None
        try:
            self._evaluated = self.evaluate()
        except Exception as e:
            self.add_error_message(str(e))

        rewriter = getattr(dtypes, 'rewrite_' + self.dtype, None)
        if rewriter:
            rewriter(self)

    def validate(self):
        """
        Validate the param.
        The value must be evaluated and type must a possible type.
        """
        Element.validate(self)
        if self.dtype not in Constants.PARAM_TYPE_NAMES:
            self.add_error_message('Type "{}" is not a possible type.'.format(self.dtype))

        validator = dtypes.validators.get(self.dtype, None)
        if self._init and validator:
            try:
                validator(self)
            except dtypes.ValidateError as e:
                self.add_error_message(str(e))

    def get_evaluated(self):
        return self._evaluated

    def is_float(self, num):
        """
        Check if string can be converted to float.

        Returns:
            bool type
        """
        try:
            float(num)
            return True
        except ValueError:
            return False

    def evaluate(self):
        """
        Evaluate the value.

        Returns:
            evaluated type
        """
        self._init = True
        self._lisitify_flag = False
        self._stringify_flag = False
        dtype = self.dtype
        expr = self.get_value()
        scale_factor = self.scale_factor

        # ID and Enum types (not evaled)
        if dtype in ('id', 'name') or self.is_enum():
            if self.options.attributes:
                expr = attributed_str(expr)
                for key, value in self.options.attributes[expr].items():
                    setattr(expr, key, value)
            return expr

        # Numeric Types
        elif dtype in ('raw', 'complex', 'real', 'float', 'int', 'hex', 'bool'):
            if expr:
                try:
                    if isinstance(expr, str) and self.is_float(expr[:-1]):
                            scale_factor = expr[-1:]
                            if scale_factor in self.scale:
                                expr = str(float(expr[:-1])*self.scale[scale_factor])
                    value = self.parent_flowgraph.evaluate(expr)
                except Exception as e:
                    raise Exception('Value "{}" cannot be evaluated:\n{}'.format(expr, e))
            else:
                value = 0
            if dtype == 'hex':
                value = hex(value)
            elif dtype == 'bool':
                value = bool(value)
            return value

        # Numeric Vector Types
        # String Types
        elif dtype in ('string', 'file_open', 'file_save', '_multiline', '_multiline_python_external'):
            # Do not check if file/directory exists, that is a runtime issue
            try:
                # Do not evaluate multiline strings (code snippets or comments)
                if dtype not in ['_multiline','_multiline_python_external']:
                    value = self.parent_flowgraph.evaluate(expr)
                    if not isinstance(value, str):
                        raise Exception()
                else:
                    value = str(expr)
            except Exception:
                self._stringify_flag = True
                value = str(expr)
            if dtype == '_multiline_python_external':
                ast.parse(value)  # Raises SyntaxError
            return value
        # GUI Position/Hint
        # Import Type
        elif dtype == 'import':
            # New namespace
            n = dict()
            try:
                exec(expr, n)
            except ImportError:
                raise Exception('Import "{}" failed.'.format(expr))
            except Exception:
                raise Exception('Bad import syntax: "{}".'.format(expr))
            return [k for k in list(n.keys()) if str(k) != '__builtins__']

        else:
            raise TypeError('Type "{}" not handled'.format(dtype))

    def to_code(self):
        """
        Convert the value to code.
        For string and list types, check the init flag, call evaluate().
        This ensures that evaluate() was called to set the xxxify_flags.

        Returns:
            a string representing the code
        """
        self._init = True
        value = self.get_value()
        # String types
        if self.dtype in ('string', 'file_open', 'file_save', '_multiline', '_multiline_python_external'):
            if not self._init:
                self.evaluate()
            return repr(value) if self._stringify_flag else value

        else:
            return value

    def get_opt(self, item):
        return self.options.attributes[self.get_value()][item]

    # GUI Hint

    def get_all_params(self, dtype, key=None):
        """
        Get all the params from the flowgraph that have the given type and
        optionally a given key

        Args:
            dtype: the specified type
            key: the key to match against

        Returns:
            a list of params
        """
        params = []
        for block in self.parent_flowgraph.iter_enabled_blocks():
            params.extend(
                param for param in block.params.values()
                if param.dtype == dtype and (key is None or key == param.name)
            )
        return params
