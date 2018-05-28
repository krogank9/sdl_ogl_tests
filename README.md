SDL OGL sandbox
=

sdl cmake build taken from twinklebear sdl lessons. opengl cmake projects in the ogl directories

to compile with emscripten on linux go download & setup emscripten emsdk then from build/emscripten:
`emcmake cmake ../.. -DCMAKE_BUILD_TYPE=Debug && make -j8`
