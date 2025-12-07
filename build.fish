#!/usr/bin/env fish

if not test -d build
    emcmake cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    and bear -- cmake --build build
else
    cmake --build build
end
and begin
    cp build/heart.out.js src/web/js/emscripten-output
    cp build/heart.out.wasm src/web/js/emscripten-output
end