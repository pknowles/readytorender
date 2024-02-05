
set(GIT_HASH "unknown")

find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY "${SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  message(STATUS "Could not find GIT to query current commit hash")
endif()

configure_file("${CMAKE_CURRENT_LIST_DIR}/gitversion.h.in" "${BINARY_DIR}/${PREFIX}_gitversion.h" @ONLY)
