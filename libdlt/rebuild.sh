#!/bin/bash
set -e

[ -d build_debug  ] && rm -r build_debug
[ -d build_release ] && rm -r build_release

echo "*** Building Debug"
mkdir build_debug && cd build_debug && cmake -DCMAKE_BUILD_TYPE=Debug .. && make && make test && ctest -T memcheck && make libdlt_coverage && cd ..

echo "*** Building Release"
mkdir build_release && cd build_release && cmake -DCMAKE_BUILD_TYPE=Release .. && make && make test && ctest -T memcheck && make doc && cd ..

echo "*** All done"