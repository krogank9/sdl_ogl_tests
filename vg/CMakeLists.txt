project(vg)

if (NOT EMSCRIPTEN)
    find_package(OpenGLES3 REQUIRED)
    include_directories(${OPENGLES3_INCLUDE_DIR})
endif(NOT EMSCRIPTEN)

file(GLOB nanovg_SRC
    "src/*.h"
    "src/*.cpp"
    "src/render/*.h"
    "src/render/*.cpp"
)

file(GLOB nanovg_tests_SRC
    "src/*.h"
    "src/*.cpp"
    "src/render/*.h"
    "src/render/*.cpp"
    )

list(FILTER nanovg_SRC EXCLUDE REGEX "src/main.*cpp")
list(FILTER nanovg_tests_SRC EXCLUDE REGEX "src/main.*cpp")

file(GLOB shaders_SRC
    "../res/shaders/vg_test.vert"
    "../res/shaders/vg_test.frag"
)

add_executable(vg "src/main.cpp" ${nanovg_SRC} ${shaders_SRC})
add_executable(vg_tests "src/main_tests.cpp" ${shaders_SRC})
if (NOT EMSCRIPTEN)
    target_link_libraries(vg ${SDL2_LIBRARY} ${OPENGLES3_LIBRARY})
    target_link_libraries(vg_tests ${SDL2_LIBRARY} ${OPENGLES3_LIBRARY})
endif(NOT EMSCRIPTEN)
install(TARGETS vg RUNTIME DESTINATION ${BIN_DIR})

