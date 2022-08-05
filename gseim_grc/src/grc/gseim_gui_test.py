import os
from pathlib import Path
import shutil
import sys
import tempfile
import types

from importlib_resources import files
import pytest

from grc.core.generator.Generator import Generator
from grc.core.platform import Platform as CorePlatform
from grc.core.Config import DummyPrefs

def test_import():
    "Ensure that we can import the GUI, even if CI can't run it yet"
    from grc.main import main
    assert isinstance(main, types.FunctionType)

def read_flow_graph(grc_file_path):
    platform = CorePlatform(version='0.0', version_parts=('0'), prefs=DummyPrefs())
    platform.build_library()

    flow_graph = platform.make_flow_graph(grc_file_path)
    generator = Generator(flow_graph, grc_file_path)

    return flow_graph, generator

def test_gen_circuit():
    grc_file = files('grc')/'test_data'/'input'/'parent.grc'
    flow_graph, generator = read_flow_graph(str(grc_file))
    assert flow_graph.get_option('title') == 'Sample Parent Circuit'

    # This doesn't appear to actually write anything?
    generator.write()

def test_gen_lib_hiearchical_block():
    # Copy our sample file into the library so we can test generating it
    src_grc_file = files('grc')/'test_data'/'input'/'lib_parameterized_hb.grc'
    lib_grc_file = files('gseim')/'data'/'subckt_grc'/'lib_parameterized_hb.grc'
    shutil.copy(src_grc_file, lib_grc_file)

    flow_graph, generator = read_flow_graph(str(lib_grc_file))
    assert flow_graph.get_option('title') == 'Parameterized Hier Block'

    generator.write(flow_graph)

    assert os.path.exists(
        files('gseim')/'data'/'subckt'/'lib_parameterized_hb.hblock.yml')

def test_gen_user_hiearchical_block_no_env():
    grc_file = files('grc')/'test_data'/'input'/'user_parameterized_hb.grc'
    with pytest.raises(AssertionError):
        flow_graph, generator = read_flow_graph(str(grc_file))

def test_gen_user_hiearchical_block_with_env(monkeypatch):
    temp_dir = tempfile.TemporaryDirectory()
    monkeypatch.setenv('HIER_BLOCK_USER_DIR', temp_dir.name)

    grc_file = files('grc')/'test_data'/'input'/'user_parameterized_hb.grc'
    flow_graph, generator = read_flow_graph(str(grc_file))
    assert flow_graph.get_option('title') == 'Parameterized Hier Block'

    generator.write(flow_graph)

    assert os.path.exists(Path(temp_dir.name)/'user_parameterized_hb.hblock.yml')

if __name__ == '__main__':
    sys.exit(pytest.main(sys.argv))
