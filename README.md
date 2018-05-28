SDL OGL sandbox
=

sdl cmake build taken from twinklebear sdl lessons. opengl cmake projects in the ogl directories

to compile with emscripten on linux go download & setup emscripten emsdk then from build/emscripten:
`emcmake cmake ../.. -DCMAKE_BUILD_TYPE=Debug && make -j8`

to add an opengl project copy an ogl directory then add_subdirectory in the root CMakeLists.txt, and also make sure to modify the CMakeLists.txt in the ogl folder to the new folder name
