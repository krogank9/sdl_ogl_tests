project(alpha_test)

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
    "../res/shaders/alpha_test.vert"
    "../res/shaders/alpha_test.frag"
)

add_executable(alpha_test "src/main.cpp" ${nanovg_SRC} ${shaders_SRC})
if (NOT EMSCRIPTEN)
    target_link_libraries(alpha_test ${SDL2_LIBRARY} ${OPENGLES3_LIBRARY})
endif(NOT EMSCRIPTEN)
install(TARGETS alpha_test RUNTIME DESTINATION ${BIN_DIR})

