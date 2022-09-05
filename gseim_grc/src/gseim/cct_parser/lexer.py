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
    Comment = 0
    KeywordTitle = 1
    KeywordBeginFile = 2
    KeywordEndFile = 3
    KeywordBeginCircuit = 4
    KeywordEndCircuit = 5
    KeywordOutVar = 6
    KeywordMethod = 7
    KeywordControl = 8
    KeywordVariables = 9
    KeywordBeginOutput = 10
    KeywordEndOutput = 11
    KeywordBeginSolve = 12
    KeywordEndSolve = 13
    KeywordEndCircuitFile = 14
    KeywordBeginParm = 15
    KeywordEndParm = 16
    KeywordKeyword = 17
    KeywordOptions = 18
    KeywordDefault = 19
    KeywordForceWrite = 20
    Ident = 21
    Equals = 22
    Number = 23
    Plus = 24
    Newline = 25
    Whitespace = 26


TOKEN_TYPES = [
    # (TokenKind, regex, emit token?)
    (TokenKind.Comment, re.compile(r"#[^\n\r]+[\n\r]+"), False),
    (TokenKind.KeywordTitle, re.compile(r"title:"), True),
    (TokenKind.KeywordEndFile, re.compile(r"end_file"), True),
    (TokenKind.KeywordBeginFile, re.compile(r"begin_file"), True),
    (TokenKind.KeywordEndCircuitFile, re.compile(r"end_cf"), True),
    (TokenKind.KeywordBeginCircuit, re.compile(r"begin_circuit"), True),
    (TokenKind.KeywordEndCircuit, re.compile(r"end_circuit"), True),
    (TokenKind.KeywordBeginSolve, re.compile(r"begin_solve"), True),
    (TokenKind.KeywordEndSolve, re.compile(r"end_solve"), True),
    (TokenKind.KeywordBeginOutput, re.compile(r"begin_output"), True),
    (TokenKind.KeywordEndOutput, re.compile(r"end_output"), True),
    (TokenKind.KeywordOutVar, re.compile(r"outvar:"), True),
    (TokenKind.KeywordMethod, re.compile(r"method:"), True),
    (TokenKind.KeywordVariables, re.compile(r"variables:"), True),
    (TokenKind.KeywordControl, re.compile(r"control:"), True),
    (TokenKind.KeywordBeginParm, re.compile(r"begin_parm"), True),
    (TokenKind.KeywordEndParm, re.compile(r"end_parm"), True),
    (TokenKind.KeywordKeyword, re.compile(r"keyword:"), True),
    (TokenKind.KeywordOptions, re.compile(r"options:"), True),
    (TokenKind.KeywordDefault, re.compile(r"default:"), True),
    (TokenKind.KeywordForceWrite, re.compile(r"force_write"), True),
    (TokenKind.Equals, re.compile(r"="), True),
    (TokenKind.Ident, re.compile(r"[A-Za-z][A-Za-z0-9_.$#]*"), True),
    (TokenKind.Number, re.compile(r"-?\d[\d.e+-]*[umMpk]?"), True),
    (TokenKind.Plus, re.compile(r"\+"), True),
    (TokenKind.Newline, re.compile(r"[\n\r]+"), True),
    # The whitespace token explicitly avoids capturing newlines. This is
    # important because the regex matches greedily, so a whitespace followed
    # by a newline would otherwise be accidentally matched.
    (TokenKind.Whitespace, re.compile(r"[^\S\n\r]+"), False),
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
                        yield Token(token_kind, m[0], line_no + 1, pos + 1)
                    pos += m.end()
                    break
            else:
                cprint(
                    f"Did not find any matching token at line {line_no}: {line[pos:]}",
                    "red",
                )
                return
