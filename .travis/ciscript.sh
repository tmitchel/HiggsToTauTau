#!/bin/bash
# Wrap configure/build/test steps on Travis-CI into a script

# Set(ings):
# - Echo each command to stdout
# - Stop on first command that fails
set -ex

# Find ourselves
SELFDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECTDIR="$(dirname "${SELFDIR}")"

./build et_analyzer.cc test_et
