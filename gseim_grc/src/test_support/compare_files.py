"""
Copyright (C) 2022 - Jeff Wheeler <jeffwheeler@gmail.com>
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

import os
import subprocess
import sys

def diff(fname_a, fname_b, f=None):
    f = f or sys.stdout

    print('Comparing files', file=f)
    print(' ', fname_a, file=f)
    print(' ', fname_b, file=f)

    assert os.path.exists(fname_a), 'Cannot diff because first file doesn\'t exist'
    assert os.path.exists(fname_b), 'Cannot diff because second file doesn\'t exist'

    p = subprocess.run(['diff', '-u', fname_a, fname_b], capture_output=True, encoding='utf-8')
    print('args', p.args, file=f)
    print('diff stdout:', file=f)
    print(p.stdout, file=f)
    print('diff stderr:', file=f)
    print(p.stderr, file=f)

    p.check_returncode()
