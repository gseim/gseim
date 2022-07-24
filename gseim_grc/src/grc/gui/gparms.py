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

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk

class AddGparm(Gtk.Dialog):
    def __init__(self, parent):
        Gtk.Dialog.__init__(self, title="Add Gparm", transient_for=parent, flags=0)
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

        label_name = Gtk.Label(label = "  Name ", xalign=1)
        label_value = Gtk.Label(label = "  Value ", xalign=1)

        self.entry_name = Gtk.Entry(text='')
        self.entry_value = Gtk.Entry(text='0')

        grid.add(label_name)
        grid.attach(self.entry_name, 1, 0, 1, 1)
        grid.attach_next_to(label_value, label_name, Gtk.PositionType.BOTTOM, 1, 1)
        grid.attach_next_to(self.entry_value, label_value, Gtk.PositionType.RIGHT, 1, 1)

        box.add(grid)
        self.show_all()

class DelGparm(Gtk.Dialog):
    def __init__(self, parent, parm_dict):

        Gtk.Dialog.__init__(self, title="Delete Gparms", transient_for=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL, Gtk.STOCK_OK, Gtk.ResponseType.OK
        )
        self.set_default_size(150, 200)
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

        l_names = list(parm_dict.keys())
        l_values = list(parm_dict.values())
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

        self.show_all()

class EditGparm(Gtk.Dialog):
    def __init__(self, parent, parm_dict):

        Gtk.Dialog.__init__(self, title="Edit Gparms", transient_for=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL, Gtk.STOCK_OK, Gtk.ResponseType.OK
        )
        self.set_default_size(350, 200)
        self.set_position(Gtk.WindowPosition.CENTER)

        grid = Gtk.Grid()
        grid.set_row_spacing(10)
        grid.set_column_spacing(10)

        scrolled_window = Gtk.ScrolledWindow()
        scrolled_window.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrolled_window.show()
        self.vbox.pack_start(scrolled_window, True, True, 0)
        scrolled_window.add(grid)

        l_names = list(parm_dict.keys())
        l_values = list(parm_dict.values())
        self.name = []
        self.value = []

        for i in range(len(l_names)):
            self.name.append(Gtk.Label(label=l_names[i], xalign=0.5))
            self.value.append(Gtk.Entry(text=l_values[i]))

        grid.add(self.name[0])
        grid.attach(self.value[0], 1, 0, 1, 1)

        for i in range(1, len(l_names)):
            grid.attach_next_to(self.name[i], self.name[i-1], Gtk.PositionType.BOTTOM, 1, 1)
            grid.attach_next_to(self.value[i], self.name[i], Gtk.PositionType.RIGHT, 1, 1)

        self.show_all()

