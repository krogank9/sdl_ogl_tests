project(nanovg)

find_package(OpenGLES2 REQUIRED)
include_directories(${OPENGLES2_INCLUDE_DIR})
include_directories(src/nanovg)

file(GLOB nanovg_SRC
    "src/*.h"
    "src/*.cpp"
    "src/render/*.h"
    "src/render/*.cpp"
)

add_executable(nanovg ${nanovg_SRC} src/nanovg/nanovg.c)
target_link_libraries(nanovg ${SDL2_LIBRARY} ${OPENGLES2_LIBRARY})
install(TARGETS nanovg RUNTIME DESTINATION ${BIN_DIR})

