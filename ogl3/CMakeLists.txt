project(ogl3)

if (EMSCRIPTEN)
    message("Building for emscripten")

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FULL_ES2=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s FULL_ES2=1")
else(EMSCRIPTEN)
    find_package(OpenGLES2 REQUIRED)
    include_directories(${OPENGLES2_INCLUDE_DIR})
endif(EMSCRIPTEN)

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
    target_link_libraries(ogl3 ${SDL2_LIBRARY} ${OPENGLES2_LIBRARY})
endif(NOT EMSCRIPTEN)
install(TARGETS ogl3 RUNTIME DESTINATION ${BIN_DIR})

