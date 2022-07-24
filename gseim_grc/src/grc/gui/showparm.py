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

class AddShowParm(Gtk.Dialog):
    def __init__(self, parent, l_parm_names):
        Gtk.Dialog.__init__(self, title="Show Parameter", transient_for=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL, Gtk.STOCK_OK, Gtk.ResponseType.OK
        )
        self.set_default_size(400, 500)
        self.set_position(Gtk.WindowPosition.CENTER)

        box = self.get_content_area()
        box.set_spacing(30)
        box.set_margin_top(5)

        grid = Gtk.Grid()
        grid.set_row_spacing(10)
        grid.set_column_spacing(10)

        scrolled_window = Gtk.ScrolledWindow()
        scrolled_window.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrolled_window.show()
        self.vbox.pack_start(scrolled_window, True, True, 0)
        scrolled_window.add(grid)

        self.l_buttons = []

        for i, parm_name in enumerate(l_parm_names):
            if i == 0:
                button1 = Gtk.RadioButton(label=parm_name)
                button1.set_active(False)
                self.l_buttons.append(button1)
            else:
                button1 = Gtk.RadioButton.new_from_widget(self.l_buttons[i-1])
                button1.set_label(parm_name)
                self.l_buttons.append(button1)

        label_1 = Gtk.Label(label = ' Select parameter ', xalign=0.5)

        grid.add(label_1)

        grid.attach_next_to(self.l_buttons[0],
           label_1, Gtk.PositionType.BOTTOM, 1, 1)

        for i in range(1, len(l_parm_names)):
            grid.attach_next_to(self.l_buttons[i], self.l_buttons[i-1],
               Gtk.PositionType.BOTTOM, 1, 1)

        box.add(grid)
        self.show_all()
