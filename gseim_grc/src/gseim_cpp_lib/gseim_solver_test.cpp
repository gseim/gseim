#include <cstdio>
#include <iostream>
#include <filesystem>

#include "gseim_cpp_lib/gseim_solver.h"

using namespace std;

namespace fs = std::filesystem;

int diff(string fname1, string fname2) {
    string diff_cmd = string("diff --unified ") + fname1.c_str() + " " + fname2.c_str();
    FILE *fp = popen(diff_cmd.c_str(), "r");
    if (fp == NULL) {
        cout << "Could not run diff on files" << endl;
    }

    int buf_size = 255;
    char buf[buf_size];
    while (fgets(buf, buf_size, fp) != NULL) {
        cout << buf;
    }

    int retcode = pclose(fp);
    return retcode;
}

void test_solver(string fname) {
    fs::path input_data_path = fs::current_path()/"gseim_cpp_lib"/"test_data"/"input"/fname;

    string input_path_str = input_data_path.string();
    const char* argv[] = {"MISSING", input_path_str.c_str()};

    int retcode = solve(2, (char**)argv);
    if (retcode != 0) {
        cout << "solve for " << fname << " generated return code: " << retcode << endl;
        exit(1);
    }

    fs::path output_path = fs::current_path()/"gseim_cpp_lib"/"test_data"/"input"/fname;
    output_path.replace_extension("dat");

    fs::path expected_output_path = fs::current_path()/"gseim_cpp_lib"/"test_data"/"output"/fname;
    expected_output_path.replace_extension("dat");

    retcode = diff(output_path.string(), expected_output_path.string());
    if (retcode != 0) {
        cout << "diff failed" << endl;
        cout << "diff return code: " << retcode << endl;
        exit(1);
    }
}

int main() {
    test_solver("ac_controller_1.in");
    test_solver("buck.in");
    test_solver("controlled_rectifier_2.in");

    return 0;
}
