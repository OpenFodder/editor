# Open Fodder Editor
[![build](https://github.com/OpenFodder/editor/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/OpenFodder/editor/actions/workflows/build.yml)

![ofed](https://user-images.githubusercontent.com/1327406/47386538-9dd6e400-d758-11e8-90ab-127d81909407.png)

#### About

Open Fodder Editor is a campaign and map editor for [Open Fodder](https://github.com/OpenFodder/openfodder) and Cannon Fodder

#### To Compile

Windows (Visual Studio + vcpkg):
1. `Project\build_vs.cmd` (defaults to x64 Release)
2. The output is placed in `Run\` (exe + required DLLs/plugins).

Linux (Ubuntu/Debian):
1. Install deps: `sudo apt-get install cmake ninja-build qt6-base-dev build-essential git pkg-config`
2. If SDL3 packages are available: `sudo apt-get install libsdl3-dev libsdl3-mixer-dev`
3. If SDL3 packages are not available, build from source:
   - `git clone https://github.com/libsdl-org/SDL.git`
   - `cmake -S SDL -B SDL/build -G Ninja -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TESTS=OFF`
   - `sudo cmake --install SDL/build`
   - `git clone https://github.com/libsdl-org/SDL_mixer.git`
   - `cmake -S SDL_mixer -B SDL_mixer/build -G Ninja -DSDLMIXER_VENDORED=ON -DSDLMIXER_SHARED=ON -DSDLMIXER_STATIC=OFF -DSDLMIXER_TESTS=OFF`
   - `sudo cmake --install SDL_mixer/build`
4. Configure: `cmake -S . -B build -G Ninja`
5. Build: `cmake --build build`

Notes:
* Dependencies are fetched with vcpkg via `Project\build_vs.cmd`.
* The build uses Qt6, SDL3, and SDL3_mixer.
* Run folder is the debugger working directory.

#### Requirements

* Installed/Working Open Fodder
* Works with any data supported by Open Fodder
