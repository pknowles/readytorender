# Copyright (c) 2024 Pyarelal Knowles, MIT License

cmake_minimum_required(VERSION 3.20)

project(readytorender)

set(CMAKE_CXX_STANDARD 20)

if(BUILD_TESTING)
  option(BUILD_READYTORENDER_TESTING "Enable readytorender testing" ON)
  if(BUILD_READYTORENDER_TESTING)
    enable_testing()
    add_subdirectory(test)
  endif()
endif()

# Add dependencies if not already included by a parent project.
# Add offset_ptr first or header may use fetch_content to get it.
# TODO: if parent projects have these, they probably shouldn't be submodules
if(NOT TARGET decodeless::offset_ptr)
  option(DECODELESS_OFFSET_PTR_SEARCH_PATH "${CMAKE_CURRENT_LIST_DIR}/decodeless_offset_ptr")
  add_subdirectory(decodeless_offset_ptr)
endif()
if(NOT TARGET decodeless::header)
  option(DECODELESS_HEADER_SEARCH_PATH "${CMAKE_CURRENT_LIST_DIR}/decodeless_header")
  add_subdirectory(decodeless_header)
endif()

add_custom_target(readytorender_gitversion
  COMMAND ${CMAKE_COMMAND}
    -DSOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DBINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}/generated"
    -DPREFIX=readytorender
    -P "${CMAKE_CURRENT_SOURCE_DIR}/gitversion.cmake"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/gitversion.cmake"
)

# readytorender uses glm in its API, but this is likely already part of the
# parent project. In the interest of easy setup, download it automatically if it
# does not exist already. Unfotunately this assumes the parent project has
# already added glm if it were ever going to.
option(RTR_AUTOMATIC_DEPENDENCIES "Automatically fetch dependencies such as glm if not already available" ON)
if(NOT TARGET glm)
  find_package(glm CONFIG)
  if(TARGET glm)
  # glm_FOUND is just.. bogus, at least with glmConfig.cmake from version 0.9.9.8
  #elseif(glm_FOUND)
  #  if(glm_SOURCE_DIR)
  #    message(STATUS "Adding glm found at: ${glm_SOURCE_DIR}")
  #    add_subdirectory(${glm_SOURCE_DIR} glm)
  #  elseif(GLM_INCLUDE_DIRS)
  #    message(STATUS "Adding glm found at: ${GLM_INCLUDE_DIRS}")
  #    add_library(glm INTERFACE)
  #    target_include_directories(glm INTERFACE ${GLM_INCLUDE_DIRS})
  #  else()
  #    message(FATAL_ERROR "glm_FOUND but no source or include dirs")
  #  endif()
  elseif(RTR_AUTOMATIC_DEPENDENCIES)
    message(STATUS "Downloading glm with FetchContent (RTR_AUTOMATIC_DEPENDENCIES=${RTR_AUTOMATIC_DEPENDENCIES})")
    include(FetchContent)
    FetchContent_Declare(
      glm
      GIT_REPOSITORY https://github.com/g-truc/glm.git
      GIT_TAG 1.0.1
    )
    FetchContent_MakeAvailable(glm)
  endif()
endif()

add_library(readytorender INTERFACE)
add_dependencies(readytorender readytorender_gitversion)
target_include_directories(readytorender INTERFACE include)
target_include_directories(readytorender INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_link_libraries(readytorender INTERFACE decodeless::header glm)
