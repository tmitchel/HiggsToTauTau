#!/bin/bash

# Set(ings):
# - Echo each command to stdout
# - Stop on first command that fails
set -ex

# Find ourselves
SELFDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECTDIR="$(dirname "${SELFDIR}")"

sudo chmod 777 $PROJECTDIR
cd $PROJECTDIR

bash build plugins/et_analyzer.cc test_et
bash build plugins/mt_analyzer.cc test_mt
bash build plugins/tt_analyzer.cc test_tt
bash build plugins/finalTemplate.cc test_final
bash build plugins/templateMaker.cc test_temp
