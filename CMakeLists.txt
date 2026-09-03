cmake_minimum_required(VERSION 3.20)

project(Nestris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(SDL3_DIR "C:/libs/SDL3/cmake")
endif()

find_package(SDL3 REQUIRED CONFIG)

if(WIN32)
    add_executable(Nestris WIN32
        src/main.cpp
        src/GameState.cpp
        src/Mods.cpp
    )
else()
    add_executable(Nestris
        src/main.cpp
        src/GameState.cpp
        src/Mods.cpp
    )
endif()

target_link_libraries(Nestris PRIVATE SDL3::SDL3)

# Windows runtime dependency
if(WIN32)
    add_custom_command(TARGET Nestris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/libs/SDL3/lib/x64/SDL3.dll"
        $<TARGET_FILE_DIR:Nestris>
    )
endif()

add_custom_command(TARGET Nestris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/assets
    $<TARGET_FILE_DIR:Nestris>/assets
)