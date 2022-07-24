import sys

import pytest
from pytestqt import qtbot

def test_gseim_plot(qtbot):
    from gseim_plot import main as gsplot

    window = gsplot.ApplicationWindow('.')
    qtbot.addWidget(window)

    assert window.windowTitle() == 'Plot Data'

if __name__ == '__main__':
    sys.exit(pytest.main(sys.argv))
