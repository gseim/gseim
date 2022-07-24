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

class DisplayName(Gtk.Dialog):
    def __init__(self, parent, s_name, l1):
        Gtk.Dialog.__init__(self, title="Element Name", transient_for=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_OK, Gtk.ResponseType.OK
        )
        self.set_default_size(250, 100)
        self.set_position(Gtk.WindowPosition.CENTER)

        box = self.get_content_area()
        box.set_spacing(30)
        box.set_margin_top(5)

        label = Gtk.Label(s_name, xalign=0.5)
        box.add(label)

        if l1:
            for l2 in l1:
               label = Gtk.Label(l2[0] + ': ' + l2[1], xalign=0)
               box.add(label)

        self.show_all()
