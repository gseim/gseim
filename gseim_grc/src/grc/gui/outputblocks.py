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
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

class AddOutputBlock(Gtk.Dialog):
    def __init__(self, parent, l_solve_blocks):
        Gtk.Dialog.__init__(self, title='Add Output Block', transient_for=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL, Gtk.STOCK_OK, Gtk.ResponseType.OK
        )
        self.set_default_size(250, 100)
        self.set_position(Gtk.WindowPosition.CENTER)

        box = self.get_content_area()

        box.set_spacing(30)
        box.set_margin_top(5)

        grid = Gtk.Grid()

        grid.set_row_spacing(10)
        grid.set_column_spacing(10)

        label_1 = Gtk.Label(label = '  Choose parent solve block  ', xalign=0.5)

        self.combo_1 = Gtk.ComboBoxText()
        self.combo_1.set_entry_text_column(0)
        for slv in l_solve_blocks:
            self.combo_1.append_text(slv.name + ' (' + slv.index + ')')
        self.combo_1.set_active(0)

        grid.add(label_1)
        grid.attach_next_to(self.combo_1, label_1, Gtk.PositionType.BOTTOM, 1, 1)

        box.add(grid)
        self.show_all()

class PickOutputBlock(Gtk.Dialog):
    def __init__(self, parent, l_output_blocks, l_solve_blocks):
        Gtk.Dialog.__init__(self, title='Pick Output Block', transient_for=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL, Gtk.STOCK_OK, Gtk.ResponseType.OK
        )
        self.set_default_size(250, 200)
        self.set_position(Gtk.WindowPosition.CENTER)

        grid = Gtk.Grid()
        grid.set_row_spacing(10)
        grid.set_column_spacing(10)
        grid.props.halign = Gtk.Align.CENTER

        scrolled_window = Gtk.ScrolledWindow()
        scrolled_window.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrolled_window.show()
        self.vbox.pack_start(scrolled_window, True, True, 0)
        scrolled_window.add(grid)

        self.l_names = []
        self.l_buttons = []

        for slv in l_solve_blocks:
            for out in slv.l_out:
                self.l_names.append(slv.name + ': ' + out)

        print('PickOutputBlock: self.l_names:', self.l_names)

        self.l_buttons.append(Gtk.RadioButton(label=self.l_names[0]))

        for i in range(1, len(self.l_names)):
            button1 = Gtk.RadioButton.new_from_widget(self.l_buttons[i-1])
            button1.set_label(self.l_names[i])
            self.l_buttons.append(button1)

        grid.add(self.l_buttons[0])

        for i in range(1, len(self.l_names)):
            grid.attach_next_to(self.l_buttons[i], self.l_buttons[i-1],
               Gtk.PositionType.BOTTOM, 1, 1)

        self.show_all()

class EditOutputBlock(Gtk.Dialog):
#   Present the selected output block for editing:

    def __init__(self, parent, blk, d_lib, l_outvars, l_solve_blocks):
#       blk is the selected output block

        i_slv = int(blk.index_slv)
        s1 = 'Block ' + blk.name + ' in Solve Block ' \
           + l_solve_blocks[i_slv].name + '(' \
           + l_solve_blocks[i_slv].index + ')'

        Gtk.Dialog.__init__(self,
            title=s1,
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

        for k, v in d_lib.items():
            l_options = v['options']
            s_value = blk.d_parms[k]

            label_1 = Gtk.Label(label = '  ' + k + '  ', xalign=0)
            l_widgets = []
            l_widgets.append([label_1, [0, 0, 1, 1]])

            if l_options[0] == 'none':
                if s_value == 'none':
                    entry_1 = Gtk.Entry(text='')
                else:
                    entry_1 = Gtk.Entry(text=s_value)

                self.d_widgets_1[k] = {'type_widget': 'entry', 'widget':entry_1}
                l_widgets.append([entry_1, [1, 0, 1, 1]])
            else:
                if set(l_options) == set1:
                    button_1 = Gtk.CheckButton()
                    button_1.set_active(s_value == 'yes')
                    button_1.set_label('yes/no')
                    l_widgets.append([button_1, [1, 0, 1, 1]])
                    self.d_widgets_1[k] = {'type_widget': 'checkbutton', 'widget':button_1}
                else:
                    combo_1 = Gtk.ComboBoxText()
                    combo_1.set_entry_text_column(0)
                    for x in l_options:
                        combo_1.append_text(x)
                    combo_1.set_active(l_options.index(s_value))
                    self.d_widgets_1[k] = {'type_widget': 'combo', 'widget':combo_1}

                    l_widgets.append([combo_1, [1, 0, 1, 1]])

            d_widgets[k] = l_widgets

        l_keys = []
        for k in d_lib.keys():
            l_keys.append(k)

        disp = 0

        for k in l_keys:
            for l0 in d_widgets[k]:
                l1 = l0[1]
                grid.attach(l0[0], l1[0], l1[1] + disp, l1[2], l1[3])
            d0 = max(map(lambda x: x[1][1], d_widgets[k])) + 1
                    
            disp += d0

        n_ov = len(l_outvars)
        label_ov = Gtk.Label(label = "  Output Variables  ", xalign=0.5)
        grid.attach(label_ov, 0, disp, 1, n_ov)

        self.l_ov_buttons = []

        for i, ov_name in enumerate(l_outvars):
            button_1 = Gtk.CheckButton()
            button_1.set_active(ov_name in blk.l_outvars)
            button_1.set_label(ov_name)
            self.l_ov_buttons.append(button_1)
            grid.attach(button_1, 1, disp + i, 1, 1)

        disp += n_ov

        self.show_all()
