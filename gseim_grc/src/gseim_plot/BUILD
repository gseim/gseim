load("@pip_deps//:requirements.bzl", "requirement")

py_library(
    name = "gseim_plot_lib",
    srcs = ["main.py"],
    deps = [
        requirement("matplotlib"),
        requirement("numpy"),
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "gseim_plot",
    srcs = ["main.py"],
    main = "main.py",
    deps = [
        ":gseim_plot_lib",
    ],
)

py_test(
    name = "plot_test",
    srcs = ["plot_test.py"],
    deps = [
        ":gseim_plot_lib",
        requirement("pytest"),
        requirement("pytest-qt"),
        requirement("pytest-xvfb"),
    ],
)
