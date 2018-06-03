project(ogl2)

if (NOT EMSCRIPTEN)
    find_package(OpenGLES3 REQUIRED)
    include_directories(${OPENGLES3_INCLUDE_DIR})
endif(EMSCRIPTEN)

file(GLOB nanovg_SRC
    "src/*.h"
    "src/*.cpp"
    "src/render/*.h"
    "src/render/*.cpp"
)

file(GLOB shaders_SRC
    "../res/shaders/tex.vert"
    "../res/shaders/tex.frag"
)

add_executable(ogl2 ${nanovg_SRC} ${shaders_SRC})
if (NOT EMSCRIPTEN)
    target_link_libraries(ogl2 ${SDL2_LIBRARY} ${OPENGLES3_LIBRARY})
endif(NOT EMSCRIPTEN)
install(TARGETS ogl2 RUNTIME DESTINATION ${BIN_DIR})

