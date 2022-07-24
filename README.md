
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

## Installation instructions (for Linux only)

- Make sure you have ```python3```, ```numpy```, ```psutil```,
  ```matplotlib```, ```pyyaml```, ```Gtk3```,
  ```PyQt6```, and ```g++``` installed.

- Download the ```gseim_grc``` folder into any directory.
- ```cd gseim_grc/gseim/cppsrc```
- ```make```
- ```make install_libfilter```
- ```cd ../../gseim```
- ```mkdir output```
- ```cd ../..```
- ```python3 ./gseim_grc/grc/scripts/run_gseim```
- The GSEIM GUI should show up. Change the window to full size;
  quit the GUI and run ```run_gseim``` again.
- Follow instructions in the **Getting started** page of the
  [GSEIM manual](https://gseim.github.io).

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
