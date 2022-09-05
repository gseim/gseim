load("@rules_python//python:defs.bzl", "py_library")
load("@pip_deps//:requirements.bzl", "requirement")

py_library(
    name = "parser",
    srcs = [
        "__init__.py",
        "lexer.py",
        "parser.py",
        "syntax_tree.py",
    ],
    deps = [
        requirement("importlib_resources"),
        requirement("more-itertools"),
        requirement("termcolor"),
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "test_data",
    srcs = glob(["test_data/**"]),
)

py_test(
    name = "parser_test",
    srcs = ["parser_test.py"],
    deps = [
        ":parser",
        "//test_support:lib",
        requirement("pytest"),
    ],
    data = [
        "//gseim:test_data",
        ":test_data",
    ],
)
