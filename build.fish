#!/usr/bin/env fish

if not test -d build
    emcmake cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    and ln -sf build/compile_commands.json .
end

cmake --build build