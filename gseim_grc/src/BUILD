load("@rules_python//python:packaging.bzl", "py_package", "py_wheel")

py_package(
    name = "gseim_pkg",
    deps = [
        "//grc:gseim_gui_lib",
        "//gseim:gseim",
        "//gseim_plot:gseim_plot_lib",
    ],
    packages = [
        "grc",
        "gseim",
        "gseim_plot",
    ],
)

py_wheel(
    name = "gseim_wheel",
    distribution = "gseim",
    python_tag = "py3",
    version = "0.0.1",
    deps = [
        ":gseim_pkg",
    ],
    requires = [
        "appdirs",
        "importlib_resources",
        "matplotlib",
        "numpy",
        "psutil",
        "PyGObject",
        "pyqt6",
        "pyyaml",
    ],
    entry_points = {
        'gui_scripts': [
            'gseim-gui = grc.main:main',
        ],
        'console_scripts': [
            'gseim_parser = gseim.gseim_parser:console_entry',
        ],
    }
)
