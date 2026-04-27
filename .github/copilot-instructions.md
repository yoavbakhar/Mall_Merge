<!-- Auto-generated guidance for AI coding agents. Update with repo-specific details only. -->
# Mall_Merge — Copilot Instructions

Purpose
- Help AI agents be productive quickly when editing, building, and extending this C + raylib project.

Big picture
- Single native C executable `MallMerge` (entry: `src/main.c`).
- Uses CMake + FetchContent to pull `raylib` at configure time (see `CMakeLists.txt`).
- Assets live in `/assets` and are copied into the build output by a CMake `POST_BUILD` command.

Build & run (most common)
- Configure & build (generic):

  mkdir build
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build --config Release

- Windows / Visual Studio (recommended when iterating/debugging):

  cmake -S . -B build -G "Visual Studio 17 2022" -A x64
  cmake --build build --config Debug
  # Open MallMerge.sln in Visual Studio to debug.

- Running: execute the produced binary in `build/<config>/` (the `assets/` folder is copied automatically by the post-build step).

Key files & patterns to inspect
- `CMakeLists.txt` — top-level configuration. Add new `.c` sources to `add_executable(MallMerge ...)` and headers to `include/`.
- `src/main.c` — runtime loop, window setup, draw/update placeholders. Use this as the example for raylib patterns (InitWindow, BeginDrawing/EndDrawing).
- `assets/` — place art/audio; these are packaged by the CMake post-build copy command.
- `build/_deps/raylib-src/` — raylib source fetched by CMake; useful for digging into raylib behavior or examples.

Project-specific conventions
- C standard: C11 (see `CMakeLists.txt`). Prefer portable, explicit types and avoid compiler-specific extensions.
- Raylib use: the project keeps the game loop simple in `src/main.c`. New gameplay code should live in separate `.c`/`.h` modules and be added to the executable in `CMakeLists.txt`.
- No unit tests present. Do not assume existing test harnesses — add tests and CMake targets if needed.

Integration points & dependencies
- raylib is pulled with `FetchContent_Declare` in `CMakeLists.txt` (no git submodule required). Changes in `build/_deps` may be overwritten by CMake.
- Platform linking: Windows uses `winmm gdi32 opengl32` (specified in CMake). Be cautious when modifying platform-specific link flags.

Examples (how to add a feature)
- To add `src/board.c` and `include/board.h`:
  - Add the files to `add_executable(MallMerge src/main.c src/board.c)` in `CMakeLists.txt`.
  - Include the header with `#include "board.h"` in files that need it.

What *not* to change without confirmation
- Avoid changing the FetchContent raylib block or build options unless addressing compatibility issues — this impacts CI and local builds.
- Don't move or rename `assets/` without updating the post-build copy command.

If unclear or missing
- Ask for the preferred Visual Studio generator/version, supported target platforms, or where to add tests. When in doubt, open a small PR that updates `CMakeLists.txt` and `src/` with a minimal example.

Feedback
- If any instructions are incomplete or you want CI-specific steps, tell me which OS/IDE/CI to target and I'll iterate.
