#!/bin/bash

set -e

cd "$(dirname "$0")"

wget http://vlg.cs.dartmouth.edu/projects/vlg_extractor/data/parameters_1.1.zip

if [ `uname -s` == "Darwin" ]; then
    ditto -x -k parameters_1.1.zip .
else
    unzip parameters_1.1.zip
fi
