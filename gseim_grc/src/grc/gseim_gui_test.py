import sys
import types

import pytest

def test_import():
    "Ensure that we can import the GUI, even if CI can't run it yet"
    from grc.main import main
    assert isinstance(main, types.FunctionType)

if __name__ == '__main__':
    sys.exit(pytest.main(sys.argv))
