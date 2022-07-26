load("@rules_python//python:defs.bzl", "py_binary")
load("@pip_deps//:requirements.bzl", "requirement")

filegroup(
    name = "element_data",
    srcs = glob(["data/**"]),
    visibility = ["//visibility:public"],
)

py_library(
    name = "gseim",
    srcs = glob(["*.py"]),
    deps = [
        requirement("numpy"),
        requirement("pyyaml"),
        requirement("importlib_resources"),
    ],
    data = [
        "//gseim_cpp_lib:libfilter.so",
        ":element_data",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "gseim_parser",
    srcs = ["gseim_parser.py"],
    deps = [
        ":gseim",
        ":parse_filters",
    ],
    visibility = ["//visibility:public"],
)

py_test(
    name = "gseim_parser_test",
    srcs = ["gseim_parser_test.py"],
    deps = [
        ":gseim",
        requirement("pytest"),
    ],
    data = glob([
        "test_data/**",
    ]),
)

py_binary(
    name = "parse_filters",
    srcs = ["parse_filters.py"],
    deps = [":gseim"],
)

py_binary(
    name = "gen_ebe",
    srcs = ["gen_ebe.py"],
    deps = [":gseim"],
    data = [
        "//gseim_cpp_lib:headers",
        requirement("importlib-resources"),
    ] + glob([
        "data/ebe/*.ebe"
    ]),
)

py_binary(
    name = "gen_xbe",
    srcs = ["gen_xbe.py"],
    deps = [":gseim"],
    data = [
        "//gseim_cpp_lib:headers",
        requirement("importlib-resources"),
    ] + glob([
        "data/xbe/*.xbe"
    ]),
)

genrule(
    name = "subebe_cpp",
    srcs = glob(["data/ebe/*.ebe"]),
    outs = ["subebe.cpp"],
    cmd = "$(location :gen_ebe) $@ $(SRCS)",
    tools = [":gen_ebe"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "subxbe_cpp",
    srcs = glob(["data/xbe/*.xbe"]),
    outs = ["subxbe.cpp"],
    cmd = "$(location :gen_xbe) $@ $(SRCS)",
    tools = [":gen_xbe"],
    visibility = ["//visibility:public"],
)
