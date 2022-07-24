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
import os
import time
from subprocess import Popen, PIPE, STDOUT
from gi.repository import GLib
from grc.core.utils import gutils as gu

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Pango

class RunProcess(Gtk.Dialog):
    def __init__(self, parent, cmd, s_max_time):
        Gtk.Dialog.__init__(self, title="Run Process", transient_for=parent, flags=0)

        self.add_button("Stop program", Gtk.ResponseType.CANCEL)
        self.add_button("Close window", Gtk.ResponseType.OK)

        self.set_default_size(600, 600)
        self.set_position(Gtk.WindowPosition.CENTER)

        grid = Gtk.Grid()
        grid.set_row_spacing(10)
        grid.set_column_spacing(10)

        self.s1 = ''
        self.label1 = Gtk.Label(label = self.s1, xalign=0)
        self.label1.modify_font(Pango.font_description_from_string('DejaVu Sans Mono 9'))

        grid.add(self.label1)

        self.scrolled_window = Gtk.ScrolledWindow()
        self.scrolled_window.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.ALWAYS)
        self.scrolled_window.show()
        self.vbox.pack_start(self.scrolled_window, True, True, 0)

        self.scrolled_window.add(grid)

        self.t_end = time.time() + gu.get_sec(s_max_time)
        self.t_1 = 0.0
        self.flag_normal = False
        self.flag_error = False
        self.flag_timelimit = False
        self.flag_killed = False
        self.sub_proc = Popen(cmd, stdout=PIPE, shell=True)

#       first argument is in milliseconds
        GLib.timeout_add(100, self.update_terminal)

        self.show_all()

    def update_terminal(self):
        NMAX = 100
        self.s1 = self.label1.get_text() + gu.non_block_read(self.sub_proc.stdout)
        n_lines = self.s1.count('\n')
        m = n_lines - NMAX
        if m > 0:
            n1 = gu.find_nth(self.s1, '\n', m)
            self.s1 = self.s1[n1 + 1:]

        self.label1.set_text(self.s1)

        position = self.scrolled_window.get_vadjustment()
        position.set_value(position.get_upper() - position.get_page_size())
        self.scrolled_window.set_vadjustment(position)

        self.t_1 = time.time()

        if self.t_1 > self.t_end:
            print('update_terminal: time exceeded, killing process')

            while self.sub_proc.poll() is None:
                os.kill(self.sub_proc.pid, 9)

            self.flag_timelimit = True
            return False

        if not self.sub_proc.poll() is None:
            if 'program completed' in self.s1.lower():
                print('update_terminal: program completed found.')
                self.flag_normal = True
            else:
                print('update_terminal: program completed not found.')
                self.flag_error = True
            return False

        return True
