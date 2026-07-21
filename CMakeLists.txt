cmake_minimum_required(VERSION 3.20)
project(TetrisTest)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)

set(SDL3_DIR "C:/libs/SDL3")

include_directories(${SDL3_DIR}/include)

#add_executable(TetrisTest src/main.cpp)
add_executable(TetrisTest WIN32 src/main.cpp)

target_link_libraries(TetrisTest
    ${SDL3_DIR}/lib/x64/SDL3.lib
)

add_custom_command(TARGET TetrisTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/libs/SDL3/lib/x64/SDL3.dll"
    $<TARGET_FILE_DIR:TetrisTest>
)

add_custom_command(TARGET TetrisTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/assets
    $<TARGET_FILE_DIR:TetrisTest>/assets
)