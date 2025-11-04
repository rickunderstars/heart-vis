#!/usr/bin/env fish

if not test -d build
    emcmake cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    and bear -- cmake --build build
else
    cmake --build build
end