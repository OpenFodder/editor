cmake_minimum_required(VERSION 3.16)

project(openfodder_editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS
    "${CMAKE_CURRENT_LIST_DIR}/Project/Qt"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Dialogs"
)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

if(WIN32)
    include(FetchContent)

    set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
    set(SDL_SHARED ON CACHE BOOL "" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)

    set(SDLMIXER_VENDORED ON CACHE BOOL "" FORCE)
    set(SDLMIXER_MOD_XMP ON CACHE BOOL "" FORCE)
    set(SDLMIXER_MOD_XMP_LITE ON CACHE BOOL "" FORCE)
    set(SDLMIXER_MOD_XMP_SHARED OFF CACHE BOOL "" FORCE)
    set(SDLMIXER_TESTS OFF CACHE BOOL "" FORCE)
    set(SDLMIXER_INSTALL OFF CACHE BOOL "" FORCE)

    set(SDL3_GIT_TAG "release-3.4.0")
    set(SDL3_MIXER_GIT_TAG "3045b35335113e5e5ba258858cb403d8803bfbeb")

    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG ${SDL3_GIT_TAG}
        GIT_SHALLOW FALSE
    )

    FetchContent_Declare(
        SDL3_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG ${SDL3_MIXER_GIT_TAG}
        GIT_SHALLOW FALSE
    )

    FetchContent_MakeAvailable(SDL3)
    FetchContent_MakeAvailable(SDL3_mixer)

    set(_sdl_mixer_compat_header
        "${CMAKE_CURRENT_LIST_DIR}/Dependencies/OpenFodder/cmake/sdl3_mixer_compat.h")
    foreach(_sdl_mixer_target IN ITEMS SDL3_mixer SDL3_mixer-shared SDL3_mixer-static)
        if(TARGET ${_sdl_mixer_target})
            if(MSVC)
                target_compile_options(${_sdl_mixer_target} PRIVATE "/FI${_sdl_mixer_compat_header}")
            else()
                target_compile_options(${_sdl_mixer_target} PRIVATE "-include" "${_sdl_mixer_compat_header}")
            endif()
        endif()
    endforeach()
else()
    find_package(SDL3 CONFIG QUIET)
    find_package(SDL3_mixer CONFIG QUIET)

    if(NOT SDL3_FOUND OR NOT SDL3_mixer_FOUND)
        find_package(PkgConfig REQUIRED)
    endif()

    if(NOT SDL3_FOUND)
        pkg_check_modules(SDL3 REQUIRED sdl3)
        add_library(SDL3::SDL3 INTERFACE IMPORTED)
        target_include_directories(SDL3::SDL3 INTERFACE ${SDL3_INCLUDE_DIRS})
        target_link_libraries(SDL3::SDL3 INTERFACE ${SDL3_LINK_LIBRARIES})
        target_compile_options(SDL3::SDL3 INTERFACE ${SDL3_CFLAGS_OTHER})
    endif()

    if(NOT SDL3_mixer_FOUND)
        pkg_check_modules(SDL3_MIXER REQUIRED sdl3_mixer)
        add_library(SDL3_mixer::SDL3_mixer INTERFACE IMPORTED)
        target_include_directories(SDL3_mixer::SDL3_mixer INTERFACE ${SDL3_MIXER_INCLUDE_DIRS})
        target_link_libraries(SDL3_mixer::SDL3_mixer INTERFACE ${SDL3_MIXER_LINK_LIBRARIES})
        target_compile_options(SDL3_mixer::SDL3_mixer INTERFACE ${SDL3_MIXER_CFLAGS_OTHER})
    endif()
endif()

set(EDITOR_HEADERS
    Source/ofed.hpp
    Source/stdafx_ofed.hpp
    Source/WindowQT.hpp
    Source/Dialogs/CampaignDialog.hpp
    Source/Dialogs/NewMapDialog.hpp
    Source/Dialogs/ToolboxSprites.hpp
    Source/Dialogs/ToolboxTiles.hpp
    Source/Dialogs/MapView.hpp
    Source/Models/CampaignModel.hpp
    Source/Models/MissionModel.hpp
    Source/Models/SpriteModel.hpp
    Source/gitver.hpp
    Dependencies/OpenFodder/Source/About.hpp
    Dependencies/OpenFodder/Source/Campaign.hpp
    Dependencies/OpenFodder/Source/CopyProtection.hpp
    Dependencies/OpenFodder/Source/Debugger.hpp
    Dependencies/OpenFodder/Source/Dimension.hpp
    Dependencies/OpenFodder/Source/Event.hpp
    Dependencies/OpenFodder/Source/Fodder.hpp
    Dependencies/OpenFodder/Source/FontData.hpp
    Dependencies/OpenFodder/Source/GameData.hpp
    Dependencies/OpenFodder/Source/Graphics.hpp
    Dependencies/OpenFodder/Source/GUI_Element.hpp
    Dependencies/OpenFodder/Source/IntroData.hpp
    Dependencies/OpenFodder/Source/Utils/ini.hpp
    Dependencies/OpenFodder/Source/Utils/md5.hpp
    Dependencies/OpenFodder/Source/Utils/SimplexNoise.hpp
    Dependencies/OpenFodder/Source/Utils/pseudorand.hpp
    Dependencies/OpenFodder/Source/Position.hpp
    Dependencies/OpenFodder/Source/Parameters.hpp
    Dependencies/OpenFodder/Source/Recruits.hpp
    Dependencies/OpenFodder/Source/Resources.hpp
    Dependencies/OpenFodder/Source/ResourceMan.hpp
    Dependencies/OpenFodder/Source/Sound.hpp
    Dependencies/OpenFodder/Source/Sprites.hpp
    Dependencies/OpenFodder/Source/SpriteSheet.hpp
    Dependencies/OpenFodder/Source/stdafx.hpp
    Dependencies/OpenFodder/Source/ScriptingEngine.hpp
    Dependencies/OpenFodder/Source/Surface.hpp
    Dependencies/OpenFodder/Source/Tiles.hpp
    Dependencies/OpenFodder/Source/Types.hpp
    Dependencies/OpenFodder/Source/UnitTesting.hpp
    Dependencies/OpenFodder/Source/Versions.hpp
    Dependencies/OpenFodder/Source/Versions_Files.hpp
    Dependencies/OpenFodder/Source/Window.hpp
    Dependencies/OpenFodder/Source/Map/Map.hpp
    Dependencies/OpenFodder/Source/Map/Original.hpp
    Dependencies/OpenFodder/Source/Map/Random.hpp
    Dependencies/OpenFodder/Projects/resource.h
    Dependencies/OpenFodder/Source/Amiga/audiostream.hpp
    Dependencies/OpenFodder/Source/Amiga/dernc.hpp
    Dependencies/OpenFodder/Source/Amiga/Graphics_Amiga.hpp
    Dependencies/OpenFodder/Source/Amiga/Graphics_Amiga2.hpp
    Dependencies/OpenFodder/Source/Amiga/paula.hpp
    Dependencies/OpenFodder/Source/Amiga/Resource_Amiga_File.hpp
    Dependencies/OpenFodder/Source/Amiga/rjp1.hpp
    Dependencies/OpenFodder/Source/Amiga/Sound_Amiga.hpp
    Dependencies/OpenFodder/Source/Amiga/SpriteData_Amiga.hpp
    Dependencies/OpenFodder/Source/PC/Graphics_PC.hpp
    Dependencies/OpenFodder/Source/PC/Resource_PC_CD.hpp
    Dependencies/OpenFodder/Source/PC/Sound_PC.hpp
    Dependencies/OpenFodder/Source/PC/Sound_PC2.hpp
    Dependencies/OpenFodder/Source/PC/SpriteData_PC.hpp
    Dependencies/OpenFodder/Source/PC/VocTable.hpp
    Dependencies/OpenFodder/Source/Structures/Barracks.hpp
    Dependencies/OpenFodder/Source/Utils/cxxopts.hpp
    Dependencies/OpenFodder/Source/Utils/diamondsquare.hpp
    Dependencies/OpenFodder/Source/Utils/json.hpp
    Dependencies/OpenFodder/Source/Utils/SimplexIslands.hpp
    Dependencies/OpenFodder/Source/Utils/micropather.h
    Dependencies/OpenFodder/Source/Utils/duktape.h
    Dependencies/OpenFodder/Source/Utils/duk_config.h
)

set(EDITOR_FORMS
    Project/Qt/ofed.ui
    Source/Dialogs/CampaignDialog.ui
    Source/Dialogs/NewMapDialog.ui
    Source/Dialogs/ToolboxSprites.ui
    Source/Dialogs/ToolboxTiles.ui
    Source/Dialogs/MapView.ui
)

set(EDITOR_SOURCES
    Source/main.cpp
    Source/OFED.cpp
    Source/WindowQT.cpp
    Source/Dialogs/CampaignDialog.cpp
    Source/Dialogs/NewMapDialog.cpp
    Source/Dialogs/ToolboxSprites.cpp
    Source/Dialogs/ToolboxTiles.cpp
    Source/Dialogs/MapView.cpp
    Source/Models/CampaignModel.cpp
    Source/Models/MissionModel.cpp
    Source/Models/SpriteModel.cpp
    Dependencies/OpenFodder/Source/About.cpp
    Dependencies/OpenFodder/Source/Campaign.cpp
    Dependencies/OpenFodder/Source/CopyProtection.cpp
    Dependencies/OpenFodder/Source/Debugger.cpp
    Dependencies/OpenFodder/Source/Event.cpp
    Dependencies/OpenFodder/Source/Fodder.cpp
    Dependencies/OpenFodder/Source/FontData.cpp
    Dependencies/OpenFodder/Source/GameData.cpp
    Dependencies/OpenFodder/Source/Graphics.cpp
    Dependencies/OpenFodder/Source/GUI_Element.cpp
    Dependencies/OpenFodder/Source/Utils/md5.cpp
    Dependencies/OpenFodder/Source/Utils/SimplexNoise.cpp
    Dependencies/OpenFodder/Source/Parameters.cpp
    Dependencies/OpenFodder/Source/Recruits.cpp
    Dependencies/OpenFodder/Source/Resources.cpp
    Dependencies/OpenFodder/Source/ResourceMan.cpp
    Dependencies/OpenFodder/Source/Sound.cpp
    Dependencies/OpenFodder/Source/Sprites.cpp
    Dependencies/OpenFodder/Source/SpriteSheet.cpp
    Dependencies/OpenFodder/Source/ScriptingEngine.cpp
    Dependencies/OpenFodder/Source/stdafx.cpp
    Dependencies/OpenFodder/Source/Surface.cpp
    Dependencies/OpenFodder/Source/Tiles.cpp
    Dependencies/OpenFodder/Source/UnitTesting.cpp
    Dependencies/OpenFodder/Source/Versions.cpp
    Dependencies/OpenFodder/Source/Window.cpp
    Dependencies/OpenFodder/Source/Amiga/dernc.cpp
    Dependencies/OpenFodder/Source/Amiga/Graphics_Amiga.cpp
    Dependencies/OpenFodder/Source/Amiga/Graphics_Amiga2.cpp
    Dependencies/OpenFodder/Source/Amiga/IntroData_Amiga.cpp
    Dependencies/OpenFodder/Source/Amiga/paula.cpp
    Dependencies/OpenFodder/Source/Amiga/Resource_Amiga_File.cpp
    Dependencies/OpenFodder/Source/Amiga/rjp1.cpp
    Dependencies/OpenFodder/Source/Amiga/Sound_Amiga.cpp
    Dependencies/OpenFodder/Source/PC/Graphics_PC.cpp
    Dependencies/OpenFodder/Source/PC/IntroData_PC.cpp
    Dependencies/OpenFodder/Source/MapData.cpp
    Dependencies/OpenFodder/Source/PC/Resource_PC_CD.cpp
    Dependencies/OpenFodder/Source/PC/Sound_PC.cpp
    Dependencies/OpenFodder/Source/PC/Sound_PC2.cpp
    Dependencies/OpenFodder/Source/Structures/Barracks.cpp
    Dependencies/OpenFodder/Source/Map/Map.cpp
    Dependencies/OpenFodder/Source/Map/Original.cpp
    Dependencies/OpenFodder/Source/Map/Random.cpp
    Dependencies/OpenFodder/Source/Utils/duktape.cpp
    Dependencies/OpenFodder/Source/Utils/SimplexIslands.cpp
    Dependencies/OpenFodder/Source/Utils/micropather.cpp
)

set(EDITOR_RESOURCES
    Project/Qt/ofed.qrc
)

set(_editor_files ${EDITOR_HEADERS} ${EDITOR_SOURCES})
set(_openfodder_files ${EDITOR_HEADERS} ${EDITOR_SOURCES})
set(_project_forms ${EDITOR_FORMS})
set(_source_forms ${EDITOR_FORMS})
list(FILTER _editor_files INCLUDE REGEX "^Source/")
list(FILTER _openfodder_files INCLUDE REGEX "^Dependencies/OpenFodder/Source/")
list(FILTER _project_forms INCLUDE REGEX "^Project/")
list(FILTER _source_forms INCLUDE REGEX "^Source/")

source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/Source" PREFIX "Editor" FILES ${_editor_files})
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/Dependencies/OpenFodder/Source" PREFIX "OpenFodder" FILES ${_openfodder_files})
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/Source" PREFIX "Editor" FILES ${_source_forms})
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/Project" PREFIX "Project" FILES ${_project_forms} ${EDITOR_RESOURCES})

add_executable(editor
    ${EDITOR_SOURCES}
    ${EDITOR_HEADERS}
    ${EDITOR_FORMS}
    ${EDITOR_RESOURCES}
)

set_target_properties(editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Run"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/Run"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/Run"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_LIST_DIR}/Run"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_CURRENT_LIST_DIR}/Run"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Run"
)

if(WIN32)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT editor)

    # Rely on windeployqt instead of vcpkg applocal for Qt deployment.
    set(VCPKG_APPLOCAL_DEPS OFF CACHE BOOL "" FORCE)
    set_target_properties(editor PROPERTIES VCPKG_APPLOCAL_DEPS OFF)
    set_property(TARGET editor PROPERTY VS_GLOBAL_VcpkgApplocalDeps "false")
endif()

target_include_directories(editor PRIVATE
    .
    Source
    Dependencies/OpenFodder/Source
)

target_compile_definitions(editor PRIVATE
    _OFED
    QT_DEPRECATED_WARNINGS
    UNICODE
    _UNICODE
    _USE_MATH_DEFINES
)

target_link_libraries(editor PRIVATE
    Qt6::Widgets
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
)

if(WIN32)
    set(_runtime_dlls "")
    if(TARGET SDL3::SDL3)
        list(APPEND _runtime_dlls $<TARGET_FILE:SDL3::SDL3>)
    elseif(TARGET SDL3-shared)
        list(APPEND _runtime_dlls $<TARGET_FILE:SDL3-shared>)
    endif()

    if(TARGET SDL3_mixer::SDL3_mixer)
        list(APPEND _runtime_dlls $<TARGET_FILE:SDL3_mixer::SDL3_mixer>)
    elseif(TARGET SDL3_mixer-shared)
        list(APPEND _runtime_dlls $<TARGET_FILE:SDL3_mixer-shared>)
    endif()

    if(_runtime_dlls)
        add_custom_command(TARGET editor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${_runtime_dlls}
                "${CMAKE_CURRENT_LIST_DIR}/Run"
            COMMENT "Copy runtime DLLs to Run")
    endif()

    if(TARGET Qt6::qmake)
        get_target_property(_qt6_qmake Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(_qt6_bin_dir "${_qt6_qmake}" DIRECTORY)
        set(_qt6_windeployqt "${_qt6_bin_dir}/windeployqt.exe")
        if(EXISTS "${_qt6_windeployqt}")
            if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
                set(_qt6_debug_bin "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin")
                set(_qt6_release_bin "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
                set(_qt6_debug_plugins "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/Qt6/plugins")
                set(_qt6_release_plugins "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/Qt6/plugins")
            endif()
            add_custom_command(TARGET editor POST_BUILD
                COMMAND ${CMAKE_COMMAND}
                    -DDEPLOY_CONFIG=$<CONFIG>
                    -DWINDEPLOYQT="${_qt6_windeployqt}"
                    -DRUN_DIR="${CMAKE_CURRENT_LIST_DIR}/Run"
                    -DTARGET_EXE="$<TARGET_FILE:editor>"
                    -DQT_DEBUG_BIN="${_qt6_debug_bin}"
                    -DQT_RELEASE_BIN="${_qt6_release_bin}"
                    -DQT_BIN_DIR="${_qt6_bin_dir}"
                    -P "${CMAKE_CURRENT_LIST_DIR}/cmake/windeployqt.cmake"
                COMMENT "Run windeployqt for editor")
        endif()
    endif()

    if(DEFINED _qt6_debug_bin AND DEFINED _qt6_debug_plugins)
        set(_qt_debug_script "${CMAKE_CURRENT_BINARY_DIR}/copy_qt_debug.cmake")
        file(WRITE "${_qt_debug_script}" [=[
if(DEFINED DEPLOY_CONFIG AND NOT DEPLOY_CONFIG STREQUAL "Debug")
    return()
endif()

if(NOT DEFINED RUN_DIR)
    message(FATAL_ERROR "RUN_DIR not set for Qt debug deployment")
endif()

if(NOT DEFINED QT_DEBUG_BIN OR NOT EXISTS "${QT_DEBUG_BIN}")
    message(FATAL_ERROR "QT_DEBUG_BIN not found: ${QT_DEBUG_BIN}")
endif()

if(NOT DEFINED QT_DEBUG_PLUGINS OR NOT EXISTS "${QT_DEBUG_PLUGINS}")
    message(FATAL_ERROR "QT_DEBUG_PLUGINS not found: ${QT_DEBUG_PLUGINS}")
endif()

file(MAKE_DIRECTORY "${RUN_DIR}/platforms")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${QT_DEBUG_BIN}/Qt6Cored.dll"
        "${QT_DEBUG_BIN}/Qt6Guid.dll"
        "${QT_DEBUG_BIN}/Qt6Widgetsd.dll"
        "${RUN_DIR}"
    RESULT_VARIABLE _copy_core_result
)

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${QT_DEBUG_PLUGINS}/platforms/qwindowsd.dll"
        "${RUN_DIR}/platforms"
    RESULT_VARIABLE _copy_platform_result
)

set(_copy_root_result 0)
if(DEFINED QT_RELEASE_BIN AND EXISTS "${QT_RELEASE_BIN}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${QT_DEBUG_BIN}/Qt6Cored.dll"
            "${QT_DEBUG_BIN}/Qt6Guid.dll"
            "${QT_DEBUG_BIN}/Qt6Widgetsd.dll"
            "${QT_RELEASE_BIN}"
        RESULT_VARIABLE _copy_root_result
    )
endif()

if(NOT _copy_core_result EQUAL 0 OR NOT _copy_platform_result EQUAL 0 OR NOT _copy_root_result EQUAL 0)
    message(FATAL_ERROR "Qt debug deployment failed")
endif()
]=])

        add_custom_command(TARGET editor POST_BUILD
            COMMAND ${CMAKE_COMMAND}
                -DDEPLOY_CONFIG=$<CONFIG>
                -DRUN_DIR="${CMAKE_CURRENT_LIST_DIR}/Run"
                -DQT_DEBUG_BIN="${_qt6_debug_bin}"
                -DQT_DEBUG_PLUGINS="${_qt6_debug_plugins}"
                -DQT_RELEASE_BIN="${_qt6_release_bin}"
                -P "${_qt_debug_script}"
            COMMENT "Copy Qt debug DLLs and platform plugin")
    endif()
endif()
