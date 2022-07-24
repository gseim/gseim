load("@pip_deps//:requirements.bzl", "requirement")

py_binary(
    name = "gseim_gui",
    srcs = glob([
        "**/*.py",
    ]),
    main = "main.py",
    deps = [
        "//gseim:gseim",
        "//gseim_plot:gseim_plot_lib",
        requirement("appdirs"),
        requirement("PyGObject"),
        requirement("pyyaml"),
        requirement("psutil"),
    ],
    data = [
        "//gseim:gseim_parser",
        "//gseim_cpp_lib:gseim_solver",
        'core/default_flow_graph.grc',
        'data/icon.png',
    ],
)
