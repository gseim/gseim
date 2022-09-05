import filecmp
import sys
import tempfile

from importlib_resources import files
import pytest

from gseim.cct_parser import parser
from test_support.compare_files import diff


def _test_parser(f, path, expected_output_path=None):
    if expected_output_path is None:
        expected_output_path = path

    ast = f(path)

    with tempfile.NamedTemporaryFile(mode="w") as fout:
        fout.write(ast.dump())
        fout.flush()

        diff(expected_output_path, fout.name)


def _test_cct_parser(path, expected_output_path=None):
    _test_parser(parser.parse_cct_file, path, expected_output_path)


def _test_parm_parser(path, expected_output_path=None):
    _test_parser(parser.parse_parms_file, path, expected_output_path)


def test_ac_controllerer_3():
    _test_cct_parser(
        files("gseim") / "test_data" / "output" / "ac_controller" / "ac_controller_3.in"
    )


def test_cyclo_converter_1ph_1():
    _test_cct_parser(
        files("gseim")
        / "test_data"
        / "output"
        / "ac_to_ac"
        / "cyclo_converter_1ph_1.in"
    )


def test_controlled_rectifier_4():
    _test_cct_parser(
        files("gseim")
        / "test_data"
        / "output"
        / "controlled_rectifier"
        / "controlled_rectifier_4.in"
    )


def test_dc_commutation_3():
    _test_cct_parser(
        files("gseim")
        / "test_data"
        / "output"
        / "dc_commutation"
        / "dc_commutation_3.in"
    )


def test_boost():
    _test_cct_parser(files("gseim") / "test_data" / "output" / "dc_to_dc" / "boost.in")


def test_dual_active_bridge_converter():
    _test_cct_parser(
        files("gseim")
        / "test_data"
        / "output"
        / "dc_to_dc"
        / "dual_active_bridge_converter.in"
    )


def test_resonant_full_bridge_1():
    _test_cct_parser(
        files("gseim")
        / "test_data"
        / "output"
        / "resonant_converter"
        / "resonant_full_bridge_1.in"
    )


def test_filter_1():
    _test_cct_parser(
        files("gseim") / "test_data" / "output" / "test" / "test_filter_1.in"
    )


def test_filter_2():
    _test_cct_parser(
        files("gseim") / "test_data" / "output" / "test" / "test_filter_2.in"
    )


def test_parms():
    _test_parm_parser(
        files("gseim") / "cct_parser" / "test_data" / "input" / "parms.in",
        files("gseim") / "cct_parser" / "test_data" / "output" / "parms.in",
    )


if __name__ == "__main__":
    sys.exit(pytest.main(sys.argv))
