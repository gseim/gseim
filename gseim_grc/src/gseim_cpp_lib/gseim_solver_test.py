import filecmp
import os
import subprocess
import sys

from importlib_resources import files
import pytest

def _test_solver(fname):
    r = subprocess.run(
        [
            str(files('gseim_cpp_lib').joinpath('gseim_solver')),
            str(files('gseim_cpp_lib').joinpath('test_data', 'input', fname)),
        ],
        capture_output=True,
        text=True,
        env={'HOME': 'not set'},
    )

    assert 'Program completed.' in r.stdout
    r.check_returncode()

    print(r.stdout)

    out_fname = os.path.splitext(fname)[0] + '.dat'

    assert filecmp.cmp(
        str(files('gseim_cpp_lib').joinpath('test_data', 'input', out_fname)),
        str(files('gseim_cpp_lib').joinpath('test_data', 'output', out_fname)),
    )

def test_ac_controllerer_1():
    _test_solver('ac_controller_1.in')

def test_buck():
    _test_solver('buck.in')

def test_controlled_rectifier_2():
    _test_solver('controlled_rectifier_2.in')

if __name__ == '__main__':
    sys.exit(pytest.main(sys.argv))

