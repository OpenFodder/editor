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
1. Install deps: `sudo apt-get install cmake ninja-build qt6-base-dev build-essential git pkg-config libcurl4-openssl-dev`
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
* The build uses C++17, Qt6 Widgets, SDL3, SDL3_mixer, and libcurl.
* Initialize engine and runtime dependencies with `git submodule update --init --recursive`.
* Run folder is the debugger working directory; the build deploys matching scripts, campaigns, and UI artwork there.

#### Requirements

* Installed/Working Open Fodder
* Works with any data supported by Open Fodder

#### Map navigation

* Scroll the mouse wheel to zoom in or out around the pointer.
* Hold the right or middle mouse button and drag to pan a zoomed-in map.
* Use the controls above the map to zoom, fit the map (Ctrl+0), or return to **100%**.
  The editor starts in **Fit map** mode and stays there when the map pane resizes,
  until you zoom manually.
* **Map Overview** sits above **Campaign** on the left. Click or drag the
  overview to navigate. Use the **View** menu to reopen either panel.
* Drag the divider beside the asset tabs to resize the palette.
  Scroll the mouse wheel over the tiles to zoom around the pointer. Zooming out
  all the way fits the tiles back into the pane.
* Scroll through the sprite categories and click a category header's arrow to
  collapse it. Search by sprite or category name, and press **Enter** to select
  the first match. Hold **Ctrl** while scrolling to resize sprite previews.
  Cannon Fodder 2 sprites are available when editing with Cannon Fodder 2 data.
* Left click/drag paints selected tiles; left click places a selected sprite.
  Placement previews follow the pointer at every zoom level.
* Click the selected tile again, use **Clear selection**, or press **Esc** to
  clear the current brush.
* In **Structures**, select a preset and click the map to place it once.
  Available presets and their previews follow the current terrain.
* In **Campaign**, select a mission to browse its phases, then double-click a
  phase or use **Open selected phase** to load its map. Press F2 to rename
  the selected mission or phase.
