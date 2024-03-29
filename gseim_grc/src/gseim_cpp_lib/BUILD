load("@pip_deps//:requirements.bzl", "requirement")

# Must include `__init__.py` so importlib's files function can point us to this
# directory.
filegroup(
    name = "headers",
    srcs = glob(["*.h"]) + ['__init__.py'],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gseim_solver_lib",
    srcs = [
        "gseim_solver.cpp",
        "cctfile.cpp",
        "global.cpp",
        "utils.cpp",
        "routines1.cpp",
        "routines2.cpp",
        "xbelib.cpp",
        "xbeusr.cpp",
        "xbejac.cpp",
        "ebelib.cpp",
        "ebeusr.cpp",
        "ebejac.cpp",
        "circuit.cpp",
        "solveblocks.cpp",
        "solveparm.cpp",
        "sysmat.cpp",
        "ijmat.cpp",
        "knuthmat.cpp",
        "matop.cpp",
        "matgnrl1.cpp",
        "matrix_2.cpp",
        "user_f.cpp",
        "//gseim:subebe_cpp",
        "//gseim:subxbe_cpp",
    ],
    hdrs = [
        "gseim_solver.h",
        "cctfile.h",
        "global.h",
        "utils.h",
        "routines1.h",
        "routines2.h",
        "xbelib.h",
        "xbeusr.h",
        "xbejac.h",
        "ebelib.h",
        "ebeusr.h",
        "ebejac.h",
        "circuit.h",
        "solveblocks.h",
        "solveparm.h",
        "sysmat.h",
        "ijmat.h",
        "knuthmat.h",
        "matop.h",
        "matgnrl1.h",
        "matrix_2.h",
        "user_f.h",
        "get_yyy.h",
    ],
    copts = ["-std=c++17"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "gseim_solver",
    srcs = ["main.cpp"],
    deps = [
        ":gseim_solver_lib"
    ],
    data = [
        "//gseim:element_data",
    ],
    copts = ["-std=c++17"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "gseim_solver_test_cpp",
    srcs = ["gseim_solver_test.cpp"],
    deps = [
        ":gseim_solver_lib",
    ],
    data = [
        "//gseim:element_data",
    ] + glob([
        "test_data/**",
    ]),
    copts = ["-std=c++17"],
)

py_test(
    name = "gseim_solver_test",
    srcs = ["gseim_solver_test.py"],
    deps = [
        requirement("importlib-resources"),
        requirement("pytest"),
    ],
    data = [
        ":gseim_solver",
    ] + glob([
        "test_data/**",
    ]),
)

cc_binary(
    name = "libfilter.so",
    srcs = [
        "utils.cpp",
        "matrix_2.cpp",
        "ijmat.cpp",
        "pic_dummy.cpp",
        "global.h",
        "knuthmat.h",
        "utils.h",
        "matrix_2.h",
        "ijmat.h",
    ],
    # linkstatic = False,
    linkshared = True,
    copts = ["-std=c++17", "-fPIC"],
    visibility = ["//visibility:public"],
)
