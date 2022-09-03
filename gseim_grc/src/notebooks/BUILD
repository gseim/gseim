load("@rules_python//python:defs.bzl", "py_binary")
load("@pip_deps//:requirements.bzl", "requirement")

notebooks = glob(["data/*.ipynb"])

py_binary(
    name = "launcher",
    srcs = ["launcher.py"],
    deps = [
        "//gseim:gseim",
        requirement("notebook"),
    ],
    data = notebooks,
)
