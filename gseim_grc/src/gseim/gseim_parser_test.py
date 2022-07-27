import filecmp
import os
import sys

from importlib_resources import files
import pytest

from gseim import gseim_parser

def _test_parser(folder, fname):
    gseim_parser.main(
        str(files('gseim').joinpath('test_data', 'input', folder, fname + '.in')),
        str(files('gseim').joinpath('test_data', 'input', folder, fname + '.grc')),
    )

    assert filecmp.cmp(
        str(files('gseim').joinpath('test_data', 'input', folder, fname + '.in')),
        str(files('gseim').joinpath('test_data', 'output', folder, fname + '.in')),
    )


def test_parse_ac_controller_3():
    _test_parser('ac_controller', 'ac_controller_3')

def test_cyclo_converter_1ph_1():
    _test_parser('ac_to_ac', 'cyclo_converter_1ph_1')

def test_parse_boost():
    _test_parser('dc_to_dc', 'boost')

def test_parse_resonant_full_bridge():
    _test_parser('resonant_converter', 'resonant_full_bridge_1')

def test_parse_rectifier_4():
    _test_parser('controlled_rectifier', 'controlled_rectifier_4')

def test_parse_dc_commutation_3():
    _test_parser('dc_commutation', 'dc_commutation_3')

if __name__ == '__main__':
    sys.exit(pytest.main(sys.argv))
