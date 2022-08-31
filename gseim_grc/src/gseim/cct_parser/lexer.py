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

from enum import Enum
import re
from typing import NamedTuple

from termcolor import cprint

class TokenKind(Enum):
    KeywordTitle = 0
    KeywordBeginCircuit = 1
    KeywordEndCircuit = 2
    KeywordOutVar = 3
    KeywordMethod = 4
    KeywordControl = 5
    KeywordVariables = 6
    KeywordBeginOutput = 7
    KeywordEndOutput = 8
    KeywordBeginSolve = 9
    KeywordEndSolve = 10
    KeywordEndCircuitFile = 11
    Ident = 12
    Equals = 13
    Number = 14
    Plus = 15
    Newline = 16
    Whitespace = 17

TOKEN_TYPES = [
    # (TokenKind, regex, emit token?)
    (TokenKind.KeywordTitle, re.compile(r'title:'), True),
    (TokenKind.KeywordEndCircuitFile, re.compile(r'end_cf'), True),
    (TokenKind.KeywordBeginCircuit, re.compile(r'begin_circuit'), True),
    (TokenKind.KeywordEndCircuit, re.compile(r'end_circuit'), True),
    (TokenKind.KeywordBeginSolve, re.compile(r'begin_solve'), True),
    (TokenKind.KeywordEndSolve, re.compile(r'end_solve'), True),
    (TokenKind.KeywordBeginOutput, re.compile(r'begin_output'), True),
    (TokenKind.KeywordEndOutput, re.compile(r'end_output'), True),
    (TokenKind.KeywordOutVar, re.compile(r'outvar:'), True),
    (TokenKind.KeywordMethod, re.compile(r'method:'), True),
    (TokenKind.KeywordVariables, re.compile(r'variables:'), True),
    (TokenKind.KeywordControl, re.compile(r'control:'), True),
    (TokenKind.Equals, re.compile(r'='), True),
    (TokenKind.Ident, re.compile(r'[A-Za-z][A-Za-z0-9_.$#]*'), True),
    (TokenKind.Number, re.compile(r'-?\d[\d.e+-]*[umMpk]?'), True),
    (TokenKind.Plus, re.compile(r'\+'), True),
    (TokenKind.Newline, re.compile(r'\n+'), True),
    (TokenKind.Whitespace, re.compile(r'\s+'), False),
]

class Token(NamedTuple):
    kind: TokenKind
    s: str
    line_no: int
    pos: int

def lex(stream):
    """Lexer for circuit file yields instances of (TokenKind, string)"""
    for line_no, line in enumerate(stream):
        pos = 0
        while pos < len(line):
            for (token_kind, token_re, emit) in TOKEN_TYPES:
                m = token_re.match(line[pos:])
                if m:
                    if emit:
                        # Line numbers and character positions usually start at 1
                        yield Token(token_kind, m[0], line_no+1, pos+1)
                    pos += m.end()
                    break
            else:
                cprint(f'Did not find any matching token at line {line_no}: {line[pos:]}', 'red')
                return
