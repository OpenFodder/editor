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
1. Install deps: `sudo apt-get install cmake ninja-build qt6-base-dev libsdl3-dev libsdl3-mixer-dev`
   (If SDL3 packages are unavailable, build/install SDL3 + SDL3_mixer from source.)
2. Configure: `cmake -S . -B build -G Ninja`
3. Build: `cmake --build build`

Notes:
* Dependencies are fetched with vcpkg via `Project\build_vs.cmd`.
* The build uses Qt6, SDL3, and SDL3_mixer.
* Run folder is the debugger working directory.

#### Requirements

* Installed/Working Open Fodder
* Works with any data supported by Open Fodder
