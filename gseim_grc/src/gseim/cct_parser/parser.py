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

from collections import defaultdict, OrderedDict
import sys

from more_itertools import peekable
from termcolor import cprint

from gseim.cct_parser.lexer import lex, TokenKind
from gseim.cct_parser.syntax_tree import SolveBlock, CctFile


def expect_token(tok_gen, kind):
    t = next(tok_gen)
    if not isinstance(kind, list):
        kind = [kind]
    assert t.kind in kind, f"Expecting {kind} but saw {t}"
    return t


def parse_optional_newline(tok_gen):
    if tok_gen and tok_gen.peek().kind == TokenKind.Newline:
        next(tok_gen)


def parse_assignment(tok_gen):
    lvalue = expect_token(tok_gen, TokenKind.Ident)
    expect_token(tok_gen, TokenKind.Equals)
    rvalue = expect_token(tok_gen, [TokenKind.Number, TokenKind.Ident])
    return (lvalue.s, rvalue.s)


def parse_title(tok_gen):
    expect_token(tok_gen, TokenKind.KeywordTitle)
    title = expect_token(tok_gen, TokenKind.Ident)
    expect_token(tok_gen, TokenKind.Newline)
    return title.s


def parse_element(tok_gen):
    elem_tok = next(tok_gen)
    elem_kind = elem_tok.s

    assignments = OrderedDict()

    while tok_gen.peek().kind != TokenKind.Newline:
        k, v = parse_assignment(tok_gen)
        assignments[k] = v

    next(tok_gen)  # Already know it is a newline

    if tok_gen.peek().kind == TokenKind.Plus:
        # This is a line continuation. Skip the '+' then continue.
        next(tok_gen)

        tok_gen.prepend(elem_tok)
        assignments.update(parse_element(tok_gen)[1])

    return (elem_kind, assignments)


def parse_outvar_method(tok_gen):
    next(tok_gen)
    assignment = parse_assignment(tok_gen)
    expect_token(tok_gen, TokenKind.Newline)
    return assignment


def parse_variables(tok_gen):
    next(tok_gen)
    r = []
    for tok in tok_gen:
        if tok.kind == TokenKind.Newline:
            break
        r.append(tok.s)
    return r


BLOCK_PARSERS = {
    TokenKind.KeywordBeginCircuit: {
        "end": TokenKind.KeywordEndCircuit,
        "parsers": {
            TokenKind.KeywordOutVar: ("outvars", parse_outvar_method),
        },
    },
    TokenKind.KeywordBeginSolve: {
        "end": TokenKind.KeywordEndSolve,
        "parsers": {
            TokenKind.KeywordMethod: ("methods", parse_outvar_method),
            TokenKind.KeywordBeginOutput: (
                "output_blocks",
                lambda tok_gen: parse_block(tok_gen, TokenKind.KeywordBeginOutput),
            ),
        },
    },
    TokenKind.KeywordBeginOutput: {
        "end": TokenKind.KeywordEndOutput,
        "parsers": {
            TokenKind.KeywordControl: ("control", parse_element),
            TokenKind.KeywordVariables: ("variables", parse_variables),
        },
    },
}


def parse_block(tok_gen, start_tok_kind):
    expect_token(tok_gen, start_tok_kind)
    expect_token(tok_gen, TokenKind.Newline)

    ret = defaultdict(lambda: [])
    ret["assignments"] = OrderedDict()

    parser_details = BLOCK_PARSERS[start_tok_kind]

    for tok in tok_gen:
        # Try parsing this line
        if tok.kind == TokenKind.Ident:
            if tok_gen.peek().kind == TokenKind.Equals:
                tok_gen.prepend(tok)
                k, v = parse_assignment(tok_gen)
                ret["assignments"][k] = v

                # There can be repeated assignments on the same line, so
                # don't automatically consume it.
                parse_optional_newline(tok_gen)
            else:
                # This should be an element description
                tok_gen.prepend(tok)
                ret["elements"].append(parse_element(tok_gen))
        elif tok.kind in parser_details["parsers"]:
            tok_gen.prepend(tok)
            output_var, parser = parser_details["parsers"][tok.kind]
            ret[output_var].append(parser(tok_gen))
        elif tok.kind == parser_details["end"]:
            expect_token(tok_gen, TokenKind.Newline)
            break
        else:
            print(f"Unexpected token when parsing circuit: {tok}")
            return

    return ret


def parse_file(fname):
    with open(fname, "r", encoding="utf-8") as cct_file:
        tok_gen = peekable(lex(cct_file))

        title = parse_title(tok_gen)
        cct_block = parse_block(tok_gen, TokenKind.KeywordBeginCircuit)
        solve_blocks = []
        while tok_gen.peek().kind == TokenKind.KeywordBeginSolve:
            solve_blocks.append(parse_block(tok_gen, TokenKind.KeywordBeginSolve))
        expect_token(tok_gen, TokenKind.KeywordEndCircuitFile)
        parse_optional_newline(tok_gen)

        for tok in tok_gen:
            cprint(f"Remaining tokens: {tok}", "blue", file=sys.stderr)

        cct_file = CctFile(title)
        cct_file.cct_elems = cct_block["elements"]
        cct_file.cct_assignments = cct_block["assignments"]
        cct_file.cct_outvars = dict(cct_block["outvars"])
        for solve_block in solve_blocks:
            cct_file.solve_blocks.append(
                SolveBlock(
                    solve_block["assignments"],
                    solve_block["methods"],
                    solve_block["output_blocks"],
                )
            )

        return cct_file
