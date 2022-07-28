# Copyright 2009-2016 Free Software Foundation, Inc.
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

import argparse, logging, sys

import gi
gi.require_version('Gtk', '3.0')
gi.require_version('PangoCairo', '1.0')
from gi.repository import Gtk

from grc.core.Config import DummyPrefs

VERSION_AND_DISCLAIMER_TEMPLATE = """\
GSEIM %s

This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it.
"""

LOG_LEVELS = {
    'debug': logging.DEBUG,
    'info': logging.INFO,
    'warning': logging.WARNING,
    'error': logging.ERROR,
    'critical': logging.CRITICAL,
}

def main():
    print("GSEIM starting...")

#   parser = argparse.ArgumentParser(
#       description=VERSION_AND_DISCLAIMER_TEMPLATE % '3.8.1.0')
    parser = argparse.ArgumentParser(
        description=VERSION_AND_DISCLAIMER_TEMPLATE % '0.0')
    parser.add_argument('flow_graphs', nargs='*')
    parser.add_argument('--log', choices=['debug', 'info', 'warning', 'error', 'critical'], default='warning')
    args = parser.parse_args()

    # Enable logging
    # Note: All other modules need to use the 'grc.<module>' convention
    log = logging.getLogger('grc')
    log.setLevel(logging.INFO)

    # Console formatting
    console = logging.StreamHandler()
    console.setLevel(LOG_LEVELS[args.log])

    #msg_format = '[%(asctime)s - %(levelname)8s] --- %(message)s (%(filename)s:%(lineno)s)'
    msg_format = '[%(levelname)s] %(message)s (%(filename)s:%(lineno)s)'
    date_format = '%I:%M'
    formatter = logging.Formatter(msg_format, datefmt=date_format)

    #formatter = utils.log.ConsoleFormatter()
    console.setFormatter(formatter)
    log.addHandler(console)

    py_version = sys.version.split()[0]
    log.debug("Starting GSEIM")

    # Delay importing until the logging is setup
    from grc.gui.Platform import Platform
    from grc.gui.Application import Application

    log.debug("Loading platform")
#   platform = Platform(
#       version='3.8.1.0',
#       version_parts=('3', '8', '1'),
#       prefs=DummyPrefs(),
#       install_prefix='/usr'
#   )

    platform = Platform(
        version='0.0',
        version_parts=('0'),
        prefs=DummyPrefs()
    )

    platform.build_library()

    log.debug("Loading application")
    app = Application(args.flow_graphs, platform)
    log.debug("Running")
    sys.exit(app.run())

if __name__ == '__main__':
    main()
