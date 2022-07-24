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

import gi
import sys
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk
from grc.core.utils import gutils as gu

class AddSolveBlock(Gtk.Dialog):
    def __init__(self, parent):
        Gtk.Dialog.__init__(self, title='Add Solve Block', transient_for=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL, Gtk.STOCK_OK, Gtk.ResponseType.OK
        )
        self.set_default_size(150, 100)
        self.set_position(Gtk.WindowPosition.CENTER)

        box = self.get_content_area()

        box.set_spacing(30)
        box.set_margin_top(5)

        grid = Gtk.Grid()

        grid.set_row_spacing(10)
        grid.set_column_spacing(10)

        label_name = Gtk.Label(label = '  Name ', xalign=1)
        label_index = Gtk.Label(label = '  Index ', xalign=1)

        self.entry_name = Gtk.Entry(text='')
        self.entry_value = Gtk.Entry(text='0')

        grid.add(label_name)
        grid.attach(self.entry_name, 1, 0, 1, 1)
        grid.attach_next_to(label_index, label_name, Gtk.PositionType.BOTTOM, 1, 1)
        grid.attach_next_to(self.entry_value, label_index, Gtk.PositionType.RIGHT, 1, 1)

        box.add(grid)
        self.show_all()

class DelSolveBlock(Gtk.Dialog):
    def __init__(self, parent, l_solve_blocks):
#       l_solve_blocks is a list of solve blocks, each solve block is a dict
#       Note: do not need to add warning if l_solve_blocks is empty; that would be
#         done in the calling function

        Gtk.Dialog.__init__(self, title='Delete Solve Blocks', transient_for=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL, Gtk.STOCK_OK, Gtk.ResponseType.OK
        )
        self.set_default_size(150, 100)
        self.set_position(Gtk.WindowPosition.CENTER)

        box = self.get_content_area()
        box.set_spacing(30)
        box.set_margin_top(5)

        grid = Gtk.Grid()
        grid.set_row_spacing(10)
        grid.set_column_spacing(10)
        grid.props.halign = Gtk.Align.CENTER

        l_names = []

#       if the solve block name is slv_1 and index is 0, we will display
#       'slv_1 (0)' as the label
#       We will treat index as a string, not int.

        for slv in l_solve_blocks:
            l_names.append(slv.name + ' (' + slv.index + ')')

        self.name = []
        self.tick = []

        for i in range(len(l_names)):
            self.name.append(Gtk.Label(label=l_names[i], xalign=0.5))
            self.tick.append(Gtk.CheckButton())

        grid.add(self.name[0])
        grid.attach(self.tick[0], 1, 0, 1, 1)

        for i in range(1, len(l_names)):
            grid.attach_next_to(self.name[i], self.name[i-1], Gtk.PositionType.BOTTOM, 1, 1)
            grid.attach_next_to(self.tick[i], self.name[i], Gtk.PositionType.RIGHT, 1, 1)

        box.add(grid)
        self.show_all()

class PickSolveBlock(Gtk.Dialog):
    def __init__(self, parent, l_solve_blocks):
#       Note: do not need to add warning if l_solve_blocks is empty; that would be
#         done in the calling function

        Gtk.Dialog.__init__(self, title='Pick Solve Block for Editing',
            transient_for=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL, Gtk.STOCK_OK, Gtk.ResponseType.OK
        )
        self.set_default_size(150, 100)
        self.set_position(Gtk.WindowPosition.CENTER)

        box = self.get_content_area()
        box.set_spacing(30)
        box.set_margin_top(5)

        grid = Gtk.Grid()
        grid.set_row_spacing(10)
        grid.set_column_spacing(10)
        grid.props.halign = Gtk.Align.CENTER

        l_names = []
        self.l_buttons = []

        for slv in l_solve_blocks:
            l_names.append(slv.name + ' (' + slv.index + ')')

        self.l_buttons.append(Gtk.RadioButton(label=l_names[0]))

        for i in range(1, len(l_names)):
            button1 = Gtk.RadioButton.new_from_widget(self.l_buttons[i-1])
            button1.set_label(l_names[i])
            self.l_buttons.append(button1)

        grid.add(self.l_buttons[0])

        for i in range(1, len(l_names)):
            grid.attach_next_to(self.l_buttons[i], self.l_buttons[i-1],
               Gtk.PositionType.BOTTOM, 1, 1)

        box.add(grid)
        self.show_all()

class EditSolveBlock(Gtk.Dialog):
#   Present the selected solve block for editing:

    def __init__(self, parent, blk, d_slv_categories):
#       blk is the selected solve block
        Gtk.Dialog.__init__(self,
            title=blk.name,
            transient_for=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL, Gtk.STOCK_OK, Gtk.ResponseType.OK
        )
        self.set_default_size(400, 800)
        self.set_position(Gtk.WindowPosition.CENTER)

        grid = Gtk.Grid()
        grid.set_row_spacing(0)
        grid.set_column_spacing(0)

        scrolled_window = Gtk.ScrolledWindow()
        scrolled_window.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrolled_window.show()
        self.vbox.pack_start(scrolled_window, True, True, 0)
        scrolled_window.add(grid)

        d_widgets = {}
        set1 = {'yes', 'no'}

        self.d_widgets_1 = {}

        for d in d_slv_categories['none']:
            parm_name = d['parm_name']
            l_options = d['options']

            if parm_name in blk.d_parms.keys():
                s_value = blk.d_parms[parm_name]
            else:
                s_value = d['default']

            l_widgets = []

#           if key is initial_sol_file, then we need special treatment
#           button/entry with button connected to a function which will
#           use Gtk.FileChooserDialog

            if parm_name == 'initial_sol_file':
                button1 = Gtk.Button(label="init sol file", xalign=0)
                button1.connect("clicked", self.on_file_clicked)
                l_widgets.append([button1, [0, 0, 1, 1]])
            else:
                label_1 = Gtk.Label(label = '  ' + parm_name + '  ', xalign=0)
                l_widgets.append([label_1, [0, 0, 1, 1]])

            if l_options[0] == 'none':
                if s_value == 'none':
                    entry_1 = Gtk.Entry(text='')
                else:
                    entry_1 = Gtk.Entry(text=s_value)

                self.d_widgets_1[parm_name] = {'type_widget': 'entry', 'widget':entry_1}
                l_widgets.append([entry_1, [1, 0, 1, 1]])
            else:
                if set(l_options) == set1:
                    button_1 = Gtk.CheckButton()
                    button_1.set_active(s_value == 'yes')
                    button_1.set_label('yes/no')
                    l_widgets.append([button_1, [1, 0, 1, 1]])
                    self.d_widgets_1[parm_name] = {'type_widget': 'checkbutton', 'widget':button_1}
                else:
                    combo_1 = Gtk.ComboBoxText()
                    combo_1.set_entry_text_column(0)
                    for x in l_options:
                        combo_1.append_text(x)
                    combo_1.set_active(l_options.index(s_value))
                    self.d_widgets_1[parm_name] = {'type_widget': 'combo', 'widget':combo_1}

                    l_widgets.append([combo_1, [1, 0, 1, 1]])

            d_widgets[parm_name] = l_widgets

        l_keys = []
        for d in d_slv_categories['none']:
            parm_name = d['parm_name']
            l_keys.append(parm_name)

        for k,l in d_slv_categories.items():
            if k != 'none':
                button1 = Gtk.Button(label=k)
                button1.connect('clicked', self.solve_options_1, k, l, blk)
                self.d_widgets_1[k] = {'type_widget': 'button', 'widget':button1}
                l_widgets = [[button1, [0, 0, 2, 1]]]
                d_widgets[k] = l_widgets
                l_keys.append(k)

        disp = 0

        for k in l_keys:
            for l0 in d_widgets[k]:
                l1 = l0[1]
                grid.attach(l0[0], l1[0], l1[1] + disp, l1[2], l1[3])
            d0 = max(map(lambda x: x[1][1], d_widgets[k])) + 1
                    
            disp += d0

        self.show_all()

    def on_file_clicked(self, widget):
        _dialog = Gtk.FileChooserDialog(
            title="Please choose a file", parent=self,
            action=Gtk.FileChooserAction.OPEN
        )
        _dialog.add_buttons(
            Gtk.STOCK_CANCEL,
            Gtk.ResponseType.CANCEL,
            Gtk.STOCK_OPEN,
            Gtk.ResponseType.OK,
        )

        _filter_gsol = Gtk.FileFilter()
        _filter_gsol.set_name("gsol files")
        _filter_gsol.add_pattern("*.gsol")
        _dialog.add_filter(_filter_gsol)

        response = _dialog.run()
        if response == Gtk.ResponseType.OK:
            print("Open clicked")
            filename = _dialog.get_filename()
            print("File selected: " + _dialog.get_filename())
            self.d_widgets_1['initial_sol_file']['widget'].set_text(filename)
        elif response == Gtk.ResponseType.CANCEL:
            print("Cancel clicked")

        _dialog.destroy()

    def solve_options_1(self, widget, category, l_dict, blk):

        dialog2 = EditSolveGroup(self, category, l_dict, blk)

        response = dialog2.run()
        if response == Gtk.ResponseType.OK:
            print('solve_options_1: dialog2: The OK button was clicked')
            gu.assign_parms_1(dialog2.d_widgets_1, blk.d_parms)
        elif response == Gtk.ResponseType.CANCEL:
            print('solve_options_1: dialog2: The Cancel button was clicked')

        dialog2.destroy()
        return

class EditSolveGroup(Gtk.Dialog):
#   Present the statements from a solve block group (such as x_nr)

    def __init__(self, parent, category, l_dict, blk):
        Gtk.Dialog.__init__(self,
            title=blk.name + ': ' + category,
            transient_for=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL, Gtk.STOCK_OK, Gtk.ResponseType.OK
        )
        self.set_default_size(400, 800)
        self.set_position(Gtk.WindowPosition.CENTER)

        grid = Gtk.Grid()
        grid.set_row_spacing(0)
        grid.set_column_spacing(0)

        scrolled_window = Gtk.ScrolledWindow()
        scrolled_window.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrolled_window.show()
        self.vbox.pack_start(scrolled_window, True, True, 0)

        scrolled_window.add(grid)

        d_widgets = {}
        set1 = {'yes', 'no'}

        self.d_widgets_1 = {}

        for d in l_dict:
            parm_name = d['parm_name']
            l_options = d['options']
            if parm_name not in blk.d_parms.keys():
                print('EditSolveGroup:', parm_name, 'not found in blk.d_parms.keys(). Halting...')
                sys.exit()

            s_value = blk.d_parms[parm_name]

            l_widgets = []

            label_1 = Gtk.Label(label = '  ' + parm_name + '  ', xalign=0)
            l_widgets.append([label_1, [0, 0, 1, 1]])

            if l_options[0] == 'none':
                if s_value == 'none':
                    entry_1 = Gtk.Entry(text='')
                else:
                    entry_1 = Gtk.Entry(text=s_value)

                self.d_widgets_1[parm_name] = {'type_widget': 'entry', 'widget':entry_1}
                l_widgets.append([entry_1, [1, 0, 1, 1]])
            else:
                if set(l_options) == set1:
                    button_1 = Gtk.CheckButton()
                    button_1.set_active(s_value == 'yes')
                    button_1.set_label('yes/no')
                    l_widgets.append([button_1, [1, 0, 1, 1]])
                    self.d_widgets_1[parm_name] = {'type_widget': 'checkbutton', 'widget':button_1}
                else:
                    combo_1 = Gtk.ComboBoxText()
                    combo_1.set_entry_text_column(0)
                    for x in l_options:
                        combo_1.append_text(x)
                    combo_1.set_active(l_options.index(s_value))
                    self.d_widgets_1[parm_name] = {'type_widget': 'combo', 'widget':combo_1}

                    l_widgets.append([combo_1, [1, 0, 1, 1]])

            d_widgets[parm_name] = l_widgets

        l_keys = []

        for d in l_dict:
            parm_name = d['parm_name']
            l_keys.append(parm_name)

        disp = 0

        for k in l_keys:
            for l0 in d_widgets[k]:
                l1 = l0[1]
                grid.attach(l0[0], l1[0], l1[1] + disp, l1[2], l1[3])
            d0 = max(map(lambda x: x[1][1], d_widgets[k])) + 1
                    
            disp += d0

        self.show_all()
