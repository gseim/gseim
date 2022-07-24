"""
Copyright 2007-2011 Free Software Foundation, Inc.
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

import logging

from gi.repository import Gtk, Gdk, Gio, GLib, GObject

log = logging.getLogger(__name__)

def filter_from_dict(vars):
    return filter(lambda x: isinstance(x[1], Action), vars.items())

class Namespace(object):

    def __init__(self):
        self._actions = {}

    def add(self, action):
        key = action.props.name
        self._actions[key] = action

    def connect(self, name, handler):
        #log.debug("Connecting action <{}> to handler <{}>".format(name, handler.__name__))
        self._actions[name].connect('activate', handler)

    def register(self, name, parameter=None, handler=None, label=None, tooltip=None,
                 icon_name=None, keypresses=None, preference_name=None, default=None):
        # Check types
        if not isinstance(name, str):
            raise TypeError("Cannot register function: 'name' must be a str")
        if parameter and not isinstance(parameter, str):
            raise TypeError("Cannot register function: 'parameter' must be a str")
        if handler and not callable(handler):
            raise TypeError("Cannot register function: 'handler' must be callable")

        # Check if the name has a prefix.
        prefix = None
        if name.startswith("app.") or name.startswith("win."):
            # Set a prefix for later and remove it
            prefix = name[0:3]
            name = name[4:]

        if handler:
            log.debug("Register action [{}, prefix={}, param={}, handler={}]".format(
                  name, prefix, parameter, handler.__name__))
        else:
            log.debug("Register action [{}, prefix={}, param={}, handler=None]".format(
                  name, prefix, parameter))

        action = Action(name, parameter, label=label, tooltip=tooltip,
                        icon_name=icon_name, keypresses=keypresses, prefix=prefix,
                        preference_name=preference_name, default=default)
        if handler:
            action.connect('activate', handler)

        key = name
        if prefix:
            key = "{}.{}".format(prefix, name)
            if prefix == "app":
                pass
                #self.app.add_action(action)
            elif prefix == "win":
                pass
                #self.win.add_action(action)

        #log.debug("Registering action as '{}'".format(key))
        self._actions[key] = action
        return action

    # If the actions namespace is called, trigger an action
    def __call__(self, name):
        # Try to parse the action string.
        valid, action_name, target_value = Action.parse_detailed_name(name)
        if not valid:
            raise Exception("Invalid action string: '{}'".format(name))
        if action_name not in self._actions:
            raise Exception("Action '{}' is not registered!".format(action_name))

        if target_value:
            self._actions[action_name].activate(target_value)
        else:
            self._actions[action_name].activate()

    def __getitem__(self, key):
        return self._actions[key]

    def __iter__(self):
        return self._actions.itervalues()

    def __repr__(self):
        return str(self)

    def get_actions(self):
        return self._actions

    def __str__(self):
        s = "{Actions:"
        for key in self._actions:
            s += " {},".format(key)
        s = s.rstrip(",") + "}"
        return s

class Action(Gio.SimpleAction):

    # Change these to normal python properties.
    #prefs_name

    def __init__(self, name, parameter=None, label=None, tooltip=None,
                 icon_name=None, keypresses=None, prefix=None,
                 preference_name=None, default=None):
        self.name = name
        self.label = label
        self.tooltip = tooltip
        self.icon_name = icon_name
        self.keypresses = keypresses
        self.prefix = prefix
        self.preference_name = preference_name
        self.default = default

        # Don't worry about checking types here, since it's done in register()
        # Save the parameter type to use for converting in __call__
        self.type = None

        variant = None
        state = None
        if parameter:
            variant = GLib.VariantType.new(parameter)
        if preference_name:
            state = GLib.Variant.new_boolean(True)
        Gio.SimpleAction.__init__(self, name=name, parameter_type=variant, state=state)

    def enable(self):
        self.props.enabled = True

    def disable(self):
        self.props.enabled = False

    def set_enabled(self, state):
        if not isinstance(state, bool):
            raise TypeError("State must be True/False.")
        self.props.enabled = state

    def __str__(self):
        return self.props.name

    def __repr__(self):
        return str(self)

    def get_active(self):
        if self.props.state:
            return self.props.state.get_boolean()
        return False

    def set_active(self, state):
        if not isinstance(state, bool):
            raise TypeError("State must be True/False.")
        self.change_state(GLib.Variant.new_boolean(state))

    # Allows actions to be directly called.
    def __call__(self, parameter=None):
        if self.type and parameter:
            # Try to convert it to the correct type.
            try:
                param = GLib.Variant(self.type, parameter)
                self.activate(param)
            except TypeError:
                raise TypeError("Invalid parameter type for action '{}'. Expected: '{}'".format(self.get_name(), self.type))
        else:
            self.activate()

    def load_from_preferences(self, *args):
        log.debug("load_from_preferences({})".format(args))
        if self.preference_name is not None:
            config = Gtk.Application.get_default().config
            self.set_active(config.entry(self.preference_name, default=bool(self.default)))

    def save_to_preferences(self, *args):
        log.debug("save_to_preferences({})".format(args))
        if self.preference_name is not None:
            config = Gtk.Application.get_default().config
            config.entry(self.preference_name, value=self.get_active())

actions = Namespace()

def get_actions():
    return actions.get_actions()

def connect(action, handler=None):
    return actions.connect(action, handler=handler)

# Old Actions
PAGE_CHANGE = actions.register("win.page_change")
EXTERNAL_UPDATE = actions.register("app.external_update")
VARIABLE_EDITOR_UPDATE = actions.register("app.variable_editor_update")
FLOW_GRAPH_NEW = actions.register("app.flowgraph.new",
    label='_New',
    tooltip='Create a new flow graph',
    icon_name='document-new',
    keypresses=["<Ctrl>n"],
)
FLOW_GRAPH_NEW_TYPE = actions.register("app.flowgraph.new_type",
    parameter="s",
)
FLOW_GRAPH_OPEN = actions.register("app.flowgraph.open",
    label='_Open',
    tooltip='Open an existing flow graph',
    icon_name='document-open',
    keypresses=["<Ctrl>o"],
)
FLOW_GRAPH_OPEN_RECENT = actions.register("app.flowgraph.open_recent",
    label='Open _Recent',
    tooltip='Open a recently used flow graph',
    icon_name='document-open-recent',
    parameter="s",
)
FLOW_GRAPH_CLEAR_RECENT = actions.register("app.flowgraph.clear_recent")
FLOW_GRAPH_SAVE = actions.register("app.flowgraph.save",
    label='_Save',
    tooltip='Save the current flow graph',
    icon_name='document-save',
    keypresses=["<Ctrl>s"],
)
FLOW_GRAPH_SAVE_AS = actions.register("app.flowgraph.save_as",
    label='Save _As',
    tooltip='Save the current flow graph as...',
    icon_name='document-save-as',
    keypresses=["<Ctrl><Shift>s"],
)
FLOW_GRAPH_SAVE_COPY = actions.register("app.flowgraph.save_copy",
    label='Save Copy',
    tooltip='Save a copy of current flow graph',
)
FLOW_GRAPH_DUPLICATE = actions.register("app.flowgraph.duplicate",
    label='_Duplicate',
    tooltip='Create a duplicate of current flow graph',
    #stock_id=Gtk.STOCK_COPY,
    keypresses=["<Ctrl><Shift>d"],
)
FLOW_GRAPH_CLOSE = actions.register("app.flowgraph.close",
    label='_Close',
    tooltip='Close the current flow graph',
    icon_name='window-close',
    keypresses=["<Ctrl>w"],
)
APPLICATION_INITIALIZE = actions.register("app.initialize")
APPLICATION_QUIT = actions.register("app.quit",
    label='_Quit',
    tooltip='Quit program',
    icon_name='application-exit',
    keypresses=["<Ctrl>q"],
)
FLOW_GRAPH_UNDO = actions.register("win.undo",
    label='_Undo',
    tooltip='Undo a change to the flow graph',
    icon_name='edit-undo',
    keypresses=["<Ctrl>z"],
)
FLOW_GRAPH_REDO = actions.register("win.redo",
    label='_Redo',
    tooltip='Redo a change to the flow graph',
    icon_name='edit-redo',
    keypresses=["<Ctrl>y"],
)
NOTHING_SELECT = actions.register("win.unselect")
SELECT_ALL = actions.register("win.select_all",
    label='Select _All',
    tooltip='Select all blocks and connections in the flow graph',
    icon_name='edit-select-all',
    keypresses=["<Ctrl>a"],
)
ELEMENT_SELECT = actions.register("win.select")
ELEMENT_CREATE = actions.register("win.add")
ELEMENT_DELETE = actions.register("win.delete",
    label='_Delete',
    tooltip='Delete the selected blocks',
    icon_name='edit-delete',
    keypresses=["Delete"],
)
BLOCK_MOVE = actions.register("win.block_move")
BLOCK_ROTATE_CCW = actions.register("win.block_rotate_ccw",
    label='Rotate Counterclockwise',
    tooltip='Rotate CCW 90 deg',
    icon_name='object-rotate-left',
    keypresses=["Left"],
)
BLOCK_ROTATE_CW = actions.register("win.block_rotate",
    label='Rotate Clockwise',
    tooltip='Rotate CW 90 deg',
    icon_name='object-rotate-right',
    keypresses=["Right"],
)
BLOCK_VALIGN_TOP = actions.register("win.block_align_top",
    label='Vertical Align Top',
    tooltip='Align tops of selected blocks',
    keypresses=["<Shift>t"],
)
BLOCK_VALIGN_MIDDLE = actions.register("win.block_align_middle",
    label='Vertical Align Middle',
    tooltip='Align centers of selected blocks vertically',
    keypresses=["<Shift>m"],
)
BLOCK_VALIGN_BOTTOM = actions.register("win.block_align_bottom",
    label='Vertical Align Bottom',
    tooltip='Align bottoms of selected blocks',
    keypresses=["<Shift>b"],
)
BLOCK_HALIGN_LEFT = actions.register("win.block_align_left",
    label='Horizontal Align Left',
    tooltip='Align left edges of blocks selected blocks',
    keypresses=["<Shift>l"],
)
BLOCK_HALIGN_CENTER = actions.register("win.block_align_center",
    label='Horizontal Align Center',
    tooltip='Align centers of selected blocks horizontally',
    keypresses=["<Shift>c"],
)
BLOCK_HALIGN_RIGHT = actions.register("win.block_align_right",
    label='Horizontal Align Right',
    tooltip='Align right edges of selected blocks',
    keypresses=["<Shift>r"],
)
BLOCK_ALIGNMENTS = [
    BLOCK_VALIGN_TOP,
    BLOCK_VALIGN_MIDDLE,
    BLOCK_VALIGN_BOTTOM,
    None,
    BLOCK_HALIGN_LEFT,
    BLOCK_HALIGN_CENTER,
    BLOCK_HALIGN_RIGHT,
]
GPARM_ADD = actions.register("win.gparm_add",
    label='Add gparm',
    tooltip='Add Global Parameter',
    keypresses=[],
)
GPARM_DEL = actions.register("win.gparm_del",
    label='Delete gparm',
    tooltip='Delete Global Parameter',
    keypresses=[],
)
GPARM_EDIT = actions.register("win.gparm_edit",
    label='Edit gparm',
    tooltip='Edit Global Parameter',
    keypresses=[],
)
OUTVAR_ADD = actions.register("win.outvar_add",
    label='Add to outvars',
    tooltip='Add to outvars',
    keypresses=[],
)
OUTVAR_DEL = actions.register("win.outvar_del",
    label='Delete outvar',
    tooltip='Delete Output Variable',
    keypresses=[],
)
OUTVAR_EDIT = actions.register("win.outvar_edit",
    label='Edit outvar',
    tooltip='Edit Output Variable',
    keypresses=[],
)
SOLVEBLOCK_ADD = actions.register("win.solveblock_add",
    label='Add Solve Block',
    tooltip='Add Solve Block',
    keypresses=[],
)
SOLVEBLOCK_DEL = actions.register("win.solveblock_del",
    label='Delete Solve Block',
    tooltip='Delete Solve Block',
    keypresses=[],
)
SOLVEBLOCK_EDIT = actions.register("win.solveblock_edit",
    label='Edit Solve Block',
    tooltip='Edit Solve Block',
    keypresses=[],
)
SOLVEBLOCK_RESET = actions.register("win.solveblock_reset",
    label='Reset Solve Block',
    tooltip='Reset Solve Block',
    keypresses=[],
)
SOLVEBLOCK_DISP = actions.register("win.solveblock_disp",
    label='Show Solve Block',
    tooltip='Show Solve Block',
    keypresses=[],
)
OUTPUTBLOCK_ADD = actions.register("win.outputblock_add",
    label='Add Output Block',
    tooltip='Add Output Block',
    keypresses=[],
)
OUTPUTBLOCK_DEL = actions.register("win.outputblock_del",
    label='Delete Output Block',
    tooltip='Delete Output Block',
    keypresses=[],
)
OUTPUTBLOCK_EDIT = actions.register("win.outputblock_edit",
    label='Edit Output Block',
    tooltip='Edit Output Block',
    keypresses=[],
)
OUTPUTBLOCK_OPS = [
    OUTPUTBLOCK_ADD,
    OUTPUTBLOCK_DEL,
    OUTPUTBLOCK_EDIT,
]
ELEMENT_DISPLAY = actions.register("win.element_display",
    label='Show Element Name',
    tooltip='Show Element Name',
    keypresses=[],
)
DOC_DISPLAY = actions.register("win.doc_display",
    label='Show Document',
    tooltip='Show Document',
    keypresses=[],
)
SHOW_PARAM = actions.register("win.show_param",
    label='Show parameter',
    tooltip='Show parameter',
    keypresses=[],
)
PASTE_SELECTED = actions.register("win.paste_selected",
    label='Paste selected',
    tooltip='Paste selected',
    keypresses=[],
)
BLOCK_PARAM_MODIFY = actions.register("win.block_modify",
    label='_Properties',
    tooltip='Modify params for the selected block',
    icon_name='document-properties',
    keypresses=["Return"],
)
TOGGLE_SNAP_TO_GRID = actions.register("win.snap_to_grid",
    label='_Snap to grid',
    tooltip='Snap blocks to a grid for an easier connection alignment',
    preference_name='snap_to_grid',
)
TOGGLE_AUTO_HIDE_PORT_LABELS = actions.register("win.auto_hide_port_labels",
    label='Auto-Hide _Port Labels',
    tooltip='Automatically hide port labels',
    preference_name='auto_hide_port_labels'
)
BLOCK_CUT = actions.register("win.block_cut",
    label='Cu_t',
    tooltip='Cut',
    icon_name='edit-cut',
    keypresses=["<Ctrl>x"],
)
BLOCK_COPY = actions.register("win.block_copy",
    label='_Copy',
    tooltip='Copy',
    icon_name='edit-copy',
    keypresses=["<Ctrl>c"],
)
BLOCK_PASTE = actions.register("win.block_paste",
    label='_Paste',
    tooltip='Paste',
    icon_name='edit-paste',
    keypresses=["<Ctrl>v"],
)
TOGGLE_CONSOLE_WINDOW = actions.register("win.toggle_console_window",
    label='Show _Console Panel',
    tooltip='Toggle visibility of the console',
    keypresses=["<Ctrl>r"],
    preference_name='console_window_visible',
    default=True
)
# TODO: Might be able to convert this to a Gio.PropertyAction eventually.
#       actions would need to be defined in the correct class and not globally
TOGGLE_BLOCKS_WINDOW = actions.register("win.toggle_blocks_window",
    label='Show _Block Tree Panel',
    tooltip='Toggle visibility of the block tree widget',
    keypresses=["<Ctrl>b"],
    preference_name='blocks_window_visible',
    default=True
)
TOGGLE_SCROLL_LOCK = actions.register("win.console.scroll_lock",
    label='Console Scroll _Lock',
    tooltip='Toggle scroll lock for the console window',
    preference_name='scroll_lock'
)
ABOUT_WINDOW_DISPLAY = actions.register("app.about",
    label='_About',
    tooltip='About this program',
    icon_name='help-about',
)
HELP_WINDOW_DISPLAY = actions.register("app.help",
    label='_Help',
    tooltip='Usage tips',
    icon_name='help-contents',
    keypresses=["F1"],
)
TYPES_WINDOW_DISPLAY = actions.register("app.types",
    label='_Types',
    tooltip='Types color mapping',
    icon_name='dialog-information',
)
KEYBOARD_SHORTCUTS_WINDOW_DISPLAY = actions.register("app.keys",
    label='_Keys',
    tooltip='Keyboard - Shortcuts',
    icon_name='dialog-information',
    keypresses=["<Ctrl>K"],
)
SHOWDOC_WINDOW_DISPLAY = actions.register("app.showdoc",
    label='_Showdoc',
    tooltip='Show doc for project',
    icon_name='dialog-information',
)
FLOW_GRAPH_GEN = actions.register("app.flowgraph.generate",
    label='_Generate',
    tooltip='Generate circuit file',
    icon_name='insert-object',
    keypresses=["F5"],
)
FLOW_GRAPH_PLOT = actions.register("app.flowgraph.plot",
    label='_Plot',
    tooltip='View results',
    icon_name='image-x-generic',
    keypresses=[],
)
FLOW_GRAPH_EXEC = actions.register("app.flowgraph.execute",
    label='_Execute',
    tooltip='Run simulation',
    icon_name='media-playback-start',
    keypresses=["F6"],
)
FLOW_GRAPH_KILL = actions.register("app.flowgraph.kill",
    label='_Kill',
    tooltip='Kill the flow graph',
    icon_name='media-playback-stop',
    keypresses=["F7"],
)
FLOW_GRAPH_SCREEN_CAPTURE = actions.register("app.flowgraph.screen_capture",
    label='Screen Ca_pture',
    tooltip='Create a screen capture of the flow graph',
    icon_name='printer',
    keypresses=["<Ctrl>p"],
)
FIND_BLOCKS = actions.register("win.find_blocks",
    label='_Find Blocks',
    tooltip='Search for a block by name',
    icon_name='edit-find',
    keypresses=["<Ctrl>f", "slash"],
)
CLEAR_CONSOLE = actions.register("win.console.clear",
    label='_Clear Console',
    tooltip='Clear Console',
    icon_name='edit-clear',
)
SAVE_CONSOLE = actions.register("win.console.save",
    label='_Save Console',
    tooltip='Save Console',
    icon_name='edit-save',
)
OPEN_HIER = actions.register("win.open_hier",
    label='Open H_ier',
    tooltip='Open selected hierarchical block',
    icon_name='go-jump',
)
POST_HANDLER = actions.register("app.post_handler")
READY = actions.register("app.ready")
