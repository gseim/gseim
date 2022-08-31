import sys
import tempfile

from importlib_resources import files
import pytest
from pytestqt import qtbot

from gseim_plot import main as gsplot

def test_gseim_plot(qtbot):
    with tempfile.NamedTemporaryFile() as tfile:
        window = gsplot.ApplicationWindow(tfile.name)
        qtbot.addWidget(window)

        assert window.windowTitle() == 'Plot Data'

def _test_open_file(qtbot, fname):
    with tempfile.NamedTemporaryFile() as tfile:
        window = gsplot.ApplicationWindow(tfile.name)
        qtbot.addWidget(window)

        window.ProjectfileName = str(files('gseim_plot')/'test_data'/'input'/fname)
        window.openFile1()

        return window

def test_dc_commutation_5_startup(qtbot):
    window = _test_open_file(qtbot, 'dc_commutation_5_startup.in')
    assert window.OPFiles.count() == 2

if __name__ == '__main__':
    sys.exit(pytest.main(sys.argv))
