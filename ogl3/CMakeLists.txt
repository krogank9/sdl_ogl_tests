project(ogl3)

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

file(GLOB shaders_SRC
    "../res/shaders/transform.vert"
    "../res/shaders/transform.frag"
)

add_executable(ogl3 ${nanovg_SRC} ${shaders_SRC})
if (NOT EMSCRIPTEN)
    target_link_libraries(ogl3 ${SDL2_LIBRARY} ${OPENGLES3_LIBRARY})
endif(NOT EMSCRIPTEN)
install(TARGETS ogl3 RUNTIME DESTINATION ${BIN_DIR})

