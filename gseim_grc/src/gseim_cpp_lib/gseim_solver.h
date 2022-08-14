#ifndef GSEIM_SOLVER_H
#define GSEIM_SOLVER_H

// Put most of `main` into a separate function so it can be tested by a C++
// test suite. Otherwise, two `main` functions exist and cannot be tested.
int solve(int argc, char** argv);

#endif
