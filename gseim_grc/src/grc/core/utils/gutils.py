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

import sys
import os
import fcntl

def next_line_1(f, pos_in):
    string_list = []
    f.seek(pos_in)
    for i in range(500):
        pos = f.tell()
        line = f.readline()
        s2 = line.replace("\n","")
        s3 = s2.replace("="," ")
        s1 = s3.split()
        if (len(s1) == 0):
            continue
        if (s1[0] == '#'):
            continue
        if (len(string_list) == 0):
            for k in s1:
                string_list.append(k)
        else:
            if (s1[0] == '+'):
                for j in range(1,len(s1)):
                    string_list.append(s1[j])
            else:
                break
    return pos, string_list

def get_parms(filename, d_parms):

    f = open(os.path.expanduser(filename), 'r')
    pos_in = f.tell()
    pos_in, l_strings = next_line_1(f, pos_in)
    if l_strings[0] != 'begin_file':
        print('get_parms: expected begin_file. Halting...')
        sys.exit()
    while True:
        pos_in, l_strings = next_line_1(f, pos_in)
        if l_strings[0] == 'end_file':
            f.close()
            break
        if l_strings[0] != 'begin_parm':
            print('get_parms: expected begin_parm. Halting...')
            sys.exit()

        parm_category = 'none'
        if 'category' in l_strings:
            parm_category = l_strings[l_strings.index('category') + 1]

        pos_in, l_strings = next_line_1(f, pos_in)
        if l_strings[0] != 'keyword:':
            print('get_parms: expected keyword. Halting...')
            sys.exit()
        parm_name = l_strings[1]

        pos_in, l_strings = next_line_1(f, pos_in)
        if l_strings[0] != 'options:':
            print('get_parms: expected options. Halting...')
            sys.exit()
        l_options = l_strings[1:]

        pos_in, l_strings = next_line_1(f, pos_in)
        if l_strings[0] != 'default:':
            print('get_parms: expected default. Halting...')
            sys.exit()
        parm_default = l_strings[1]

        pos_in, l_strings = next_line_1(f, pos_in)
        if l_strings[0] != 'end_parm':
            print('get_parms: expected end_parm. Halting...')
            sys.exit()

        d_parms[parm_name] = {
            'options': l_options,
            'default': parm_default,
            'category': parm_category
        }    

def assign_parms_1(d_dialog, d_parms):
    for k, v in d_dialog.items():
        w = v['widget']

        if v['type_widget'] == 'entry':
            s = w.get_text().replace(' ', '')
            if s:
                d_parms[k] = s
            else:
                d_parms[k] = 'none'
        elif v['type_widget'] == 'checkbutton':
            d_parms[k] = 'yes' if w.get_active() else 'no'
        elif v['type_widget'] == 'combo':
            d_parms[k] = w.get_active_text()

def prepare_dict_1(d_parms, d_categories):

    for parm, d in d_parms.items():
        category = d['category']

        d_new = {'parm_name': parm, 'options': d['options'], 'default': d['default']}

        if category in d_categories.keys():
            d_categories[category].append(d_new)
        else:
            d_categories[category] = [d_new]

def find_nth(s_in, c, n):
#   https://stackoverflow.com/questions/30833409/python-deleting-the-first-2-lines-of-a-string

    start = s_in.find(c)
    while start >= 0 and n > 1:
        start = s_in.find(c, start+len(c))
        n -= 1
    return start

def is_running(pid):
    if os.path.isdir('/proc/{}'.format(pid)):
        return True
    return False

def get_sec(time_str):
#   https://stackoverflow.com/questions/6402812/how-to-convert-an-hmmss-time-string-to-seconds-in-python/6402934
    """Get Seconds from time."""
    h, m, s = time_str.split(':')
    return int(h)*3600 + int(m)*60 + int(s)

def non_block_read(output):
    ''' even in a thread, a normal read with block until the buffer is full '''
    fd = output.fileno()
    fl = fcntl.fcntl(fd, fcntl.F_GETFL)
    fcntl.fcntl(fd, fcntl.F_SETFL, fl | os.O_NONBLOCK)

    op = output.read()

    if op == None:
        return ''
    return op.decode('utf-8')

def python_code_to_list(filename, keyword1=None, keyword2=None):

    l1 = []

#   we will assume that there are no tab characters

    f = open(os.path.expanduser(filename), 'r')
    kw1_found = False

    for i in range(2000):
        x = f.readline()
        if not x: break

        if (keyword1 and kw1_found) or not keyword1:
            if keyword1 and keyword2 in x:
                break
            line = x.replace("\n","")
            if len(line.split()) == 0: continue
            line1 = line.split('#')[0]
            if len(line1.split()) == 0: continue
            l1.append(line1)
        else:
            if keyword1 and keyword1 in x:
                kw1_found = True
            continue

    f.close()

    if not kw1_found:
        print('python_code_to_list:', keyword1, 'not found. Halting...')
        sys.exit()

    if not l1:
        print('python_code_to_list: l1 is empty. Halting...')
        sys.exit()

    l2 = []
    flag_new = True

    for s in l1:
        if flag_new:
#           we will assume that \ appears at the end
#           (i.e., the user meant continuation)
            if '\\' in s:
                s_temp = s.replace('\\', ' ')
                flag_new = False
            else:
                l2.append(s.rstrip())
        else:
            if '\\' in s:
                s_temp += s.replace('\\', ' ')
            else:
                s_temp += s
                l2.append(s_temp.rstrip())
                flag_new = True

    n_indent = []

    for s in l2:
        if s.startswith(' '):
            n_sp = len(s) - len(s.lstrip())
            if n_sp % 4 != 0:
                print('python_code_to_list: indent must have 4, 8, .. spaces.')
                print('Check this statement:')
                print(s)
                sys.exit()
            n_indent.append(int(round(n_sp/4)))
        else:
            n_indent.append(0)

    if n_indent[0] != 0:
        print('python_code_to_list: indent must be 0 for the first line.')
        print('Check this statement:')
        print(l2[0])
        sys.exit()

    n1 = len(l2)
    if n1 == 1: return l2

    string_list = []
    flag_new = True
    s_temp = ''

    for i, s in enumerate(l2):
        if i == (n1-1):
            if flag_new:
                s_temp = s
            else:
                s_temp += '\n' + s
            string_list.append(s_temp.rstrip())
        else:
            if flag_new:
                s_temp = s
                if n_indent[i+1] > 0:
                    flag_new = False
                else:
                    string_list.append(s_temp.rstrip())
            else:
                s_temp += '\n' + s
                if n_indent[i+1] == 0:
                    flag_new = True
                    string_list.append(s_temp.rstrip())

    return string_list

