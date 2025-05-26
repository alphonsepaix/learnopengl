add_library(glad STATIC glad/src/glad.c)

target_include_directories(glad PUBLIC glad/include)

add_library(stb INTERFACE)

target_include_directories(stb INTERFACE stb)