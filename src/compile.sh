#!/bin/bash

cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ .. || exit 1
make -j$(nproc) || exit 1
exit 0
