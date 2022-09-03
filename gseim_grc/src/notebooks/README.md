Jupyter notebooks allow experimenting with the GSEIM API directly from within
a Python notebook environment.

When the application is installed as a Python package (e.g. with `pip`),
notebooks can directly import packages directly. Run the Jupyter notebook as
you always would:

```
jupyter notebook notebooks/data/
```

To run the notebooks from the source distribution without building/installing
the Python package, the notebook can be launched from Bazel:

```
bazel run //notebooks:launcher -- notebooks/data/
```

(The argument in this command tells the notebook where to look for notebook
files. Without an argument, it presumes the current working directory, which
would be in the isolated Bazel runtime sandbox.)
