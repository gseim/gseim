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

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, GObject, Pango
from grc.core.utils import gutils as gu


class SolveBlockEditor(object):
    def __init__(self, title, mark_unsaved_handler, data, schema):
        self.title = title
        self.mark_unsaved_handler = mark_unsaved_handler
        self.data = data
        self.schema = schema
        print("schema", schema)

    class SolveBlockRow(object):
        header = False
        schema = None
        key = ""
        default = ""
        options = []

        def __init__(self, mark_unsaved_handler, solve_block, schema=None):
            self.mark_unsaved_handler = mark_unsaved_handler
            self.solve_block = solve_block

            if schema:
                self.schema = schema

                self.key = schema["parm_name"]
                self.default = schema["default"]
                self.options = schema["options"]

        def get_value(self):
            if self.has_value():
                return self.solve_block.d_parms[self.schema["parm_name"]]
            else:
                return ""

        def set_value(self, value):
            if not self.header:
                self.value = value
                self.solve_block.d_parms[self.schema["parm_name"]] = value
                self.mark_unsaved_handler()

        def remove_value(self):
            del self.solve_block.d_parms[self.schema["parm_name"]]

        def has_value(self):
            if self.schema:
                return self.schema["parm_name"] in self.solve_block.d_parms
            else:
                return False

        @staticmethod
        def set_value_set(cls, iter_, store, mark_unsaved_handler):
            row = store.get(store.get_iter(Gtk.TreePath(iter_)), 0)[0]
            if row.has_value():
                row.remove_value()
            else:
                row.set_value(row.default)
            mark_unsaved_handler()

        @staticmethod
        def cell_data_get_key(column, cell_renderer, model, iter_, data):
            obj = model.get_value(iter_, 0)
            cell_renderer.set_property("text", obj.key)

        @staticmethod
        def cell_data_get_set(column, cell_renderer, model, iter_, data):
            obj = model.get_value(iter_, 0)
            cell_renderer.set_property("active", obj.has_value())
            cell_renderer.set_property("inconsistent", obj.header)

    class SolveBlockValueCellRenderer(Gtk.CellRenderer):
        def __init__(self, store):
            super().__init__()
            self.store = store

            self.renderer_text = Gtk.CellRendererText(ellipsize=Pango.EllipsizeMode.END)
            self.renderer_text.connect("edited", self.text_edited, None)

            self.renderer_combo = Gtk.CellRendererCombo(
                ellipsize=Pango.EllipsizeMode.END
            )
            self.renderer_combo.connect("edited", self.text_edited, None)

            self.models = {}

            self.renderer_combo.set_property("text_column", 0)
            self.renderer_combo.set_property("has_entry", False)

            self.obj_ = None

        @GObject.Property(type=Gtk.CellRenderer)
        def renderer(self):
            if self.obj_.header:
                return self.renderer_text
            elif self.obj_.options[0] != "none":
                return self.renderer_combo
            else:
                return self.renderer_text

        @GObject.Property(type=GObject.TYPE_PYOBJECT)
        def obj(self):
            return self.obj_

        @obj.setter
        def obj(self, new_obj):
            self.obj_ = new_obj
            if new_obj.header:
                self.renderer_text.set_property("text", "")
                self.renderer_text.set_property("editable", False)
            elif new_obj.options[0] != "none":
                self.renderer_combo.set_property("text", new_obj.get_value())
                self.renderer_combo.set_property(
                    "placeholder_text", "Default: " + new_obj.default
                )
                model = Gtk.TreeStore(str)
                for opt in new_obj.options:
                    model.append(None, [opt])
                self.renderer_combo.set_property("model", model)
                self.renderer_combo.set_property("editable", True)
            else:
                self.renderer_text.set_property("text", new_obj.get_value())
                self.renderer_text.set_property(
                    "placeholder_text", "Default: " + new_obj.default
                )
                self.renderer_text.set_property("editable", True)
                self.set_property("mode", Gtk.CellRendererMode.EDITABLE)

        def do_get_preferred_width(self, *args):
            return self.renderer.get_preferred_width(*args)

        def do_get_preferred_height(self, *args):
            return self.renderer.get_preferred_height(*args)

        def do_start_editing(self, *args):
            return self.renderer.start_editing(*args)

        def do_editing_canceled(self, *args):
            return self.renderer.editing_canceled(*args)

        def do_activate(self, *args):
            return self.renderer.activate(*args)

        def do_render(self, cr, widget, background_area, cell_area, flags):
            self.renderer.render(cr, widget, background_area, cell_area, flags)

        def text_edited(self, widget, it, value, unknown_arg):
            row = self.store.get(self.store.get_iter(Gtk.TreePath(it)), 0)[0]
            row.set_value(value)

    def build_tree_store(self):
        store = Gtk.TreeStore(GObject.TYPE_PYOBJECT)

        for solve_block in self.data:
            print(solve_block)
            r = self.SolveBlockRow(self.mark_unsaved_handler, solve_block)
            r.header = True
            r.key = solve_block.name
            parent_iter = store.append(None, [r])

            for cat_name, cat_parms in self.schema.items():
                if cat_name == "none":
                    cat_iter = parent_iter
                else:
                    r = self.SolveBlockRow(self.mark_unsaved_handler, solve_block)
                    r.header = True
                    r.key = cat_name

                    cat_iter = store.append(parent_iter, [r])

                for parm_details in cat_parms:
                    r = self.SolveBlockRow(
                        self.mark_unsaved_handler, solve_block, parm_details
                    )
                    r.header = False
                    store.append(cat_iter, [r])

        return store

    def build_toolbar(self):
        toolbar = Gtk.Toolbar()

        add_icon = Gtk.Image.new_from_stock(
            stock_id=Gtk.STOCK_ADD, size=Gtk.IconSize.SMALL_TOOLBAR
        )
        self.add_btn = Gtk.ToolButton(icon_widget=add_icon, label="Add")
        self.add_btn.connect("clicked", self.add_btn_clicked)

        remove_icon = Gtk.Image.new_from_stock(
            stock_id=Gtk.STOCK_REMOVE, size=Gtk.IconSize.SMALL_TOOLBAR
        )
        self.remove_btn = Gtk.ToolButton(icon_widget=remove_icon, label="Remove")
        self.remove_btn.set_sensitive(False)
        self.remove_btn.connect("clicked", self.remove_btn_clicked)

        toolbar.insert(self.add_btn, 0)
        toolbar.insert(self.remove_btn, 1)

        return toolbar

    def build_tree_view(self):
        store = self.build_tree_store()

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)

        scroll_window = Gtk.ScrolledWindow()
        scroll_window.set_property("hscrollbar_policy", Gtk.PolicyType.NEVER)

        self.tree = Gtk.TreeView(store)
        self.tree.get_selection().connect("changed", self.tree_selection_changed)

        r1 = Gtk.CellRendererText(ellipsize=Pango.EllipsizeMode.END)
        col1 = Gtk.TreeViewColumn("Parameter", r1)
        col1.set_cell_data_func(r1, self.SolveBlockRow.cell_data_get_key, None)
        col1.set_min_width(140)
        col1.set_expand(True)
        self.tree.append_column(col1)

        r2 = self.SolveBlockValueCellRenderer(store)
        col2 = Gtk.TreeViewColumn("Value", r2)
        col2.add_attribute(r2, "obj", 0)
        col2.set_min_width(120)
        col2.set_expand(True)
        self.tree.append_column(col2)

        r3 = Gtk.CellRendererToggle()
        r3.connect(
            "toggled",
            self.SolveBlockRow.set_value_set,
            store,
            self.mark_unsaved_handler,
        )
        col3 = Gtk.TreeViewColumn("Set", r3)
        col3.set_cell_data_func(r3, self.SolveBlockRow.cell_data_get_set, None)
        col3.set_fixed_width(40)
        self.tree.append_column(col3)

        scroll_window.add(self.tree)

        box.pack_start(
            Gtk.Label(label=self.title, xalign=0.05), expand=False, fill=True, padding=5
        )
        box.pack_start(scroll_window, expand=True, fill=True, padding=0)
        box.pack_end(self.build_toolbar(), expand=False, fill=True, padding=0)
        box.show_all()

        return box

    def tree_selection_changed(self, tree_selection):
        model, iter_ = tree_selection.get_selected()

        if iter_:
            row = model.get(iter_, 0)[0]
            self.remove_btn.set_sensitive(row.header)
            return

        self.remove_btn.set_sensitive(False)

    def add_btn_clicked(self, _btn):
        pass

    def remove_btn_clicked(self, _btn):
        model, iter_ = self.tree.get_selection().get_selected()

        if iter_:
            row = model.get(iter_, 0)[0]
            model.remove(iter_)
            pass
