# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import

class FlowGraphProxy(object):  # TODO: move this in a refactored Generator

    def __init__(self, fg):
        self.orignal_flowgraph = fg

    def __getattr__(self, item):
        return getattr(self.orignal_flowgraph, item)

    def get_pad_sources(self):
        """
        Get a list of pad source blocks sorted by id order.

        Returns:
            a list of pad source blocks in this flow graph
        """
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_source']
        return sorted(pads, key=lambda x: int(x.name.split('$')[-1]))

    def get_pad_sinks(self):
        """
        Get a list of pad sink blocks sorted by id order.

        Returns:
            a list of pad sink blocks in this flow graph
        """
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_sink']
        return sorted(pads, key=lambda x: int(x.name.split('$')[-1]))

    def get_pad_e_lefts(self):
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_e_left']
        return sorted(pads, key=lambda x: int(x.name.split('$')[-1]))

    def get_pad_e_rights(self):
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_e_right']
        return sorted(pads, key=lambda x: int(x.name.split('$')[-1]))

    def get_pad_e_tops(self):
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_e_top']
        return sorted(pads, key=lambda x: int(x.name.split('$')[-1]))

    def get_pad_e_bottoms(self):
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_e_bottom']
        return sorted(pads, key=lambda x: int(x.name.split('$')[-1]))

    def get_pad_b_lefts(self):
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_b_left']
        return sorted(pads, key=lambda x: int(x.name.split('$')[-1]))

    def get_pad_b_rights(self):
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_b_right']
        return sorted(pads, key=lambda x: int(x.name.split('$')[-1]))

    def get_pad_b_tops(self):
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_b_top']
        return sorted(pads, key=lambda x: int(x.name.split('$')[-1]))

    def get_pad_b_bottoms(self):
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_b_bottom']
        return sorted(pads, key=lambda x: int(x.name.split('$')[-1]))

