import gi
from gi.repository import GLib
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Pango

from grc.core.utils import gutils as gu

class RunPythonProcessDialog(Gtk.Dialog):
    def __init__(self, parent, timeout_sec):
        super().__init__(title="Run Python Process", transient_for=parent, flags=0)

        self.add_button("Stop program", Gtk.ResponseType.CANCEL)
        self.add_button("Close window", Gtk.ResponseType.OK)

        self.set_default_size(600, 600)
        self.set_position(Gtk.WindowPosition.CENTER)

        grid = Gtk.Grid()
        grid.set_row_spacing(10)
        grid.set_column_spacing(10)

        self.s1 = ''
        self.label1 = Gtk.Label(label=self.s1, xalign=0)
        self.label1.modify_font(Pango.font_description_from_string('DejaVu Sans Mono 9'))

        grid.add(self.label1)

        self.scrolled_window = Gtk.ScrolledWindow()
        self.scrolled_window.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.ALWAYS)
        self.scrolled_window.show()
        self.vbox.pack_start(self.scrolled_window, True, True, 0)

        self.scrolled_window.add(grid)

        GLib.timeout_add(100, self.update_terminal)

        self.show_all()

    def update_terminal(self):
        pass
