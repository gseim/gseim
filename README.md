
[GSEIM](https://arxiv.org/abs/2204.12924)
(General-purpose Simulator with Explicit and Implicit
Methods) is meant for simulation of electrical circuits,
especially power electronic circuits, and also for numerical
solution of ordinary differential equations (ODEs). In terms of
basic functionality, GSEIM is similar to commercial packages such as
[Simulink](https://in.mathworks.com/products/simulink.html),
[Simscape](https://in.mathworks.com/products/simscape.html),
[PSIM](https://powersimtech.com/products/psim/capabilities-applications/),
and
[PLECS](https://www.plexim.com/).
However, at this stage, GSEIM does not handle real-time simulation.

GSEIM includes a schematic entry GUI (python) adapted from the
[GNU Radio](https://www.gnuradio.org//) package, a C++ solver,
a Qt-python plotting GUI, and a few additional python programs.

## Installation instructions

### Installing pre-built image (Linux only)

Pre-compiled images are available on GitHub. To install the latest image,
download the Python wheel image from the latest GitHub CI run.

1. [GitHub Actions for GSEIM project](https://github.com/gseim/gseim/actions?query=branch%3Amain)
2. Click on latest passing (green) CI run
3. Download `gseim-wheel` package at the bottom of the page, which will expand
   to a `.whl` file.
4. Run `pip3 install ~/Downloads/gseim*.whl`. (You may want to do this in
   a virtual environment.)
5. To run the GUI, it is also necessary to install PyQt5 or PyQt6
   (`pip3 install pyqt5` or `pip3 install pyqt6`, depending on your system).

Once installed, the GUI can be launched with `gseim-gui`.

### Building from source (Linux or Mac)

GSEIM is built using [Bazel](http://bazel.build), so it must be installed
prior to attempting to build the source. Gtk and Qt are also both required
for the GUI.

1. If you do not already have Bazel installed, install it.
   - On Linux, you can follow [the instructions provided by Bazel](https://bazel.build/install/ubuntu).
     The simplest method might be installing Bazelisk through `npm`:
     ```
     sudo apt install npm
     npm i @bazel/bazelisk
     ```
   - On Mac, the easiest way is [Homebrew](https://brew.sh):
     ```
     brew install bazelisk
     ```
2. Navigate to the source directory
   ```
   cd gseim_grc/src/
   ```
3. Install PyQT5 or PyQT6. This must be done manually (the package does not
   directly depend on these) because the required package varies by OS.
   - For Ubuntu prior to 22.04:
     ```
     pip3 install pyqt5
     ```
   - For Mac or Ubuntu 22.04 and newer:
     ```
     pip3 install pyqt6
     ```

You're now ready to run GSEIM directly, or build a distributable wheel package.

#### Running the GSEIM GUI

- On Linux:
  ```
  bazel run //grc:gseim_gui --cxxopt=-std=c++17 --linkopt=-lstdc++fs
  ```
- On Mac:
  ```
  bazel run //grc:gseim_gui --cxxopt=-std=c++17
  ```
The GSEIM GUI should show up. Follow instructions in the **Getting started**
page of the [GSEIM manual](https://gseim.github.io).

#### Building distributable Python wheels

- On Linux:
  ```
  bazel build //:gseim_wheel --cxxopt=-std=c++17 --linkopt=-lstdc++fs
  ```
- On Mac:
  ```
  bazel build //:gseim_wheel --cxxopt=-std=c++17
  ```

The wheel will be located in `bazel-bin/gseim-*.whl`.

## Documentation

The GSEIM documentation can be accessed
online [(https://gseim.github.io)](https://gseim.github.io).

## License

The use and redistribution of GSEIM is governed by GPLv3.

## Future Plans

- enhancement of element library
- additional power electronics examples
- development of GSEIM-based educational material for power electronics
  courses
