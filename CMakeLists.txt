cmake_minimum_required(VERSION 3.21)
project(MallMerge LANGUAGES C)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

# -------------------------
# Raylib Fetching
# -------------------------
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master # Using master ensures the most modern CMake compatibility
)

# Disable building extras we don't need for the merge game
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

# -------------------------
# Executable Setup
# -------------------------
add_executable(MallMerge 
    src/main.c
    src/game_state.c
    src/item.c
    src/items.c
    src/drag.c
    src/generator.c
    src/input.c
    src/render.c
    src/task.c
)

# -------------------------
# Include & Link
# -------------------------
target_include_directories(MallMerge PRIVATE include src)
target_link_libraries(MallMerge PRIVATE raylib)

# Platform specific linking
if(WIN32)
    target_link_libraries(MallMerge PRIVATE winmm gdi32 opengl32)
elseif(APPLE)
    target_link_libraries(MallMerge PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
endif()

# -------------------------
# Asset Syncing (The "Vibe" assets)
# -------------------------
add_custom_command(
    TARGET MallMerge POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:MallMerge>/assets
)