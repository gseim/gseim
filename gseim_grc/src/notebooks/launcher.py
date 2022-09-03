# This launcher tool was inspired by the various discussions in [1].
#
# [1] https://github.com/bazelbuild/rules_python/issues/63

import sys

from notebook.notebookapp import main

if __name__ == '__main__':
    sys.exit(main())
