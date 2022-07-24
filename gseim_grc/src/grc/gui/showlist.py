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
from gi.repository import Gtk, Pango

class ShowList(Gtk.Window):
    def __init__(self, l, W, H, s_title):
        Gtk.Window.__init__(self, title=s_title)

#       self.set_default_size(-1, 350)
        self.set_default_size(W, H)
        self.set_position(Gtk.WindowPosition.CENTER)

        self.grid = Gtk.Grid()
        self.add(self.grid)

        self.create_textview(l)

    def create_textview(self, l):
        scrolledwindow = Gtk.ScrolledWindow()
        scrolledwindow.set_hexpand(True)
        scrolledwindow.set_vexpand(True)
        self.grid.attach(scrolledwindow, 0, 1, 3, 1)

        self.textview = Gtk.TextView()
        self.textview.override_font(
          Pango.font_description_from_string('DejaVu Sans Mono 10')
        )
        self.textbuffer = self.textview.get_buffer()

        text1 = ''
        for l1 in l:
            text1 += l1[0] + l1[1] + '\n'

        self.textbuffer.set_text(text1)

        scrolledwindow.add(self.textview)
