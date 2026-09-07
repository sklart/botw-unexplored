#!/usr/bin/env bash
set -euo pipefail
compiler="${CXX:-g++}"
"$compiler" -std=c++14 -Wall -Wextra -pedantic -Isource tests/test_settings.cpp source/Settings.cpp source/Utf8.cpp -o /tmp/test_settings
/tmp/test_settings
"$compiler" -std=c++14 -Wall -Wextra -pedantic -Isource tests/test_manual_progress.cpp source/ManualProgress.cpp -o /tmp/test_manual_progress
/tmp/test_manual_progress
