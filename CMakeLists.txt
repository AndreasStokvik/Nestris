cmake_minimum_required(VERSION 3.20)
project(Nestris)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)

set(SDL3_DIR "C:/libs/SDL3")

include_directories(${SDL3_DIR}/include)

#add_executable(Nestris src/main.cpp)
add_executable(Nestris WIN32 src/main.cpp)

target_link_libraries(Nestris
    ${SDL3_DIR}/lib/x64/SDL3.lib
)

add_custom_command(TARGET Nestris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/libs/SDL3/lib/x64/SDL3.dll"
    $<TARGET_FILE_DIR:Nestris>
)

add_custom_command(TARGET Nestris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/assets
    $<TARGET_FILE_DIR:Nestris>/assets
)