cmake_minimum_required(VERSION 3.16)

project(openfodder_editor LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
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
    set(SDL3_MIXER_GIT_TAG "release-3.2.0")

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


else()
    find_package(SDL3 3.4 CONFIG QUIET)
    find_package(SDL3_mixer 3.2 CONFIG QUIET)

    if(NOT SDL3_FOUND OR NOT SDL3_mixer_FOUND)
        find_package(PkgConfig REQUIRED)
    endif()

    if(NOT SDL3_FOUND)
        pkg_check_modules(SDL3 REQUIRED sdl3>=3.4)
        add_library(SDL3::SDL3 INTERFACE IMPORTED)
        target_include_directories(SDL3::SDL3 INTERFACE ${SDL3_INCLUDE_DIRS})
        target_link_libraries(SDL3::SDL3 INTERFACE ${SDL3_LINK_LIBRARIES})
        target_compile_options(SDL3::SDL3 INTERFACE ${SDL3_CFLAGS_OTHER})
    endif()

    if(NOT SDL3_mixer_FOUND)
        pkg_check_modules(SDL3_MIXER REQUIRED sdl3_mixer>=3.2)
        add_library(SDL3_mixer::SDL3_mixer INTERFACE IMPORTED)
        target_include_directories(SDL3_mixer::SDL3_mixer INTERFACE ${SDL3_MIXER_INCLUDE_DIRS})
        target_link_libraries(SDL3_mixer::SDL3_mixer INTERFACE ${SDL3_MIXER_LINK_LIBRARIES})
        target_compile_options(SDL3_mixer::SDL3_mixer INTERFACE ${SDL3_MIXER_CFLAGS_OTHER})
    endif()
endif()

# Keep the editor in step with upstream engine source splits and renames.
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "Dependencies/OpenFodder/Source/*.hpp" "Dependencies/OpenFodder/Source/*.h")
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "Dependencies/OpenFodder/Source/*.cpp")
list(APPEND ENGINE_SOURCES Dependencies/OpenFodder/Source/Utils/miniz.c)
# The editor supplies start(); browser entry points are not part of this build.
list(FILTER ENGINE_SOURCES EXCLUDE REGEX "Start_Emscripten.cpp$")
file(GLOB_RECURSE EDITOR_HEADERS CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_LIST_DIR}" "Source/*.hpp")
list(APPEND EDITOR_HEADERS ${ENGINE_HEADERS})

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
    ${ENGINE_SOURCES}
)

if(WIN32)
    # libcurl — used by the setup wizard / updater for HTTPS downloads of
    # retail-data manifests, scripts, and engine update archives. On Windows
    # we vendor it via FetchContent and pin TLS to Schannel so we don't
    # have to ship an OpenSSL CA bundle. HTTP_ONLY plus the per-protocol
    # disables strip out everything we don't need (LDAP, SMTP, etc.).
    set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
    set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
    set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
    set(CURL_DISABLE_LDAP ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_LDAPS ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_TELNET ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_DICT ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_FILE ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_TFTP ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_RTSP ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_POP3 ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_IMAP ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_SMTP ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_GOPHER ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_MQTT ON CACHE BOOL "" FORCE)
    set(HTTP_ONLY ON CACHE BOOL "" FORCE)

    set(LIBCURL_GIT_TAG "curl-8_5_0")
    FetchContent_Declare(libcurl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG ${LIBCURL_GIT_TAG}
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(libcurl)
else()
    find_package(CURL REQUIRED)
endif()
find_package(Threads REQUIRED)

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

if(MSVC)
    target_compile_options(editor PRIVATE /MP)
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
    Threads::Threads
)
if(WIN32)
    target_link_libraries(editor PRIVATE libcurl)
    set_property(TARGET editor PROPERTY VS_GLOBAL_VcpkgAutoLink false)
else()
    target_link_libraries(editor PRIVATE CURL::libcurl)
endif()

add_custom_command(TARGET editor POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        "-DENGINE_RUN=${CMAKE_CURRENT_LIST_DIR}/Dependencies/OpenFodder/Run"
        "-DRUN_DIR=${CMAKE_CURRENT_LIST_DIR}/Run"
        "-DDEPENDENCY_BIN=${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/$<$<CONFIG:Debug>:debug/>bin"
        -P "${CMAKE_CURRENT_LIST_DIR}/cmake/deploy_runtime.cmake"
    COMMENT "Deploy matching engine scripts, campaigns and overview artwork")

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

    # Keep installed data paths and other user settings across rebuilds.
    if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/Run/openfodder.ini")
        configure_file(
            "${CMAKE_CURRENT_LIST_DIR}/Dependencies/OpenFodder/openfodder.ini.example"
            "${CMAKE_CURRENT_LIST_DIR}/Run/openfodder.ini" COPYONLY)
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
