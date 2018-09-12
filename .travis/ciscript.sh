#!/bin/bash

# Set(ings):
# - Echo each command to stdout
# - Stop on first command that fails
set -ex

# Find ourselves
SELFDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECTDIR="$(dirname "${SELFDIR}")"

sudo chmod 777 /home/travis/build/tmitchel/HTT2016_analyzer/
cd /home/travis/build/tmitchel/HTT2016_analyzer/

bash build et_analyzer.cc test_et
bash build mt_analyzer.cc test_mt
bash build tt_analyzer.cc test_tt
