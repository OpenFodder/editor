if(DEFINED DEPLOY_CONFIG AND DEPLOY_CONFIG STREQUAL "Debug")
    message(STATUS "Skipping windeployqt for Debug")
    return()
endif()

if(NOT DEFINED WINDEPLOYQT OR NOT EXISTS "${WINDEPLOYQT}")
    message(FATAL_ERROR "windeployqt not found: ${WINDEPLOYQT}")
endif()

if(NOT DEFINED RUN_DIR OR NOT DEFINED TARGET_EXE)
    message(FATAL_ERROR "RUN_DIR or TARGET_EXE not set for windeployqt")
endif()

set(_path "")
if(DEFINED QT_DEBUG_BIN AND QT_DEBUG_BIN)
    set(_path "${QT_DEBUG_BIN}")
endif()
if(DEFINED QT_RELEASE_BIN AND QT_RELEASE_BIN)
    if(_path STREQUAL "")
        set(_path "${QT_RELEASE_BIN}")
    else()
        set(_path "${_path};${QT_RELEASE_BIN}")
    endif()
endif()
if(DEFINED QT_BIN_DIR AND QT_BIN_DIR)
    if(_path STREQUAL "")
        set(_path "${QT_BIN_DIR}")
    else()
        set(_path "${_path};${QT_BIN_DIR}")
    endif()
endif()

set(ENV{PATH} "${_path};$ENV{PATH}")

execute_process(
    COMMAND "${WINDEPLOYQT}" --release --no-translations --no-opengl-sw --dir "${RUN_DIR}" "${TARGET_EXE}"
    RESULT_VARIABLE _windeployqt_result
)

if(NOT _windeployqt_result EQUAL 0)
    message(FATAL_ERROR "windeployqt failed with code ${_windeployqt_result}")
endif()
