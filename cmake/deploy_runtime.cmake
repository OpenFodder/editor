if(NOT DEFINED ENGINE_RUN OR NOT DEFINED RUN_DIR)
    message(FATAL_ERROR "ENGINE_RUN and RUN_DIR are required")
endif()
# A local Data root lets the engine discover these bundled resources while
# retail game files can still come from the user's configured search paths.
file(MAKE_DIRECTORY "${RUN_DIR}/Data")
file(COPY "${ENGINE_RUN}/Scripts" "${ENGINE_RUN}/Campaigns"
    DESTINATION "${RUN_DIR}" PATTERN ".git" EXCLUDE PATTERN ".github" EXCLUDE)
file(COPY "${ENGINE_RUN}/about.png" DESTINATION "${RUN_DIR}")

# windeployqt handles Qt plugins; include the manifest's supporting DLLs too
# (for example libpng, zlib and double-conversion) for a clean Run directory.
if(WIN32 AND DEFINED DEPENDENCY_BIN AND IS_DIRECTORY "${DEPENDENCY_BIN}")
    file(GLOB dependencies "${DEPENDENCY_BIN}/*.dll")
    if(dependencies)
        file(COPY ${dependencies} DESTINATION "${RUN_DIR}")
    endif()
endif()
