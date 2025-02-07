# Copyright (c) 2024-2025 Pyarelal Knowles, MIT License

cmake_minimum_required(VERSION 3.20)

find_package(GTest QUIET)
if(NOT GTest_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(googletest)
endif()

option(DECODELESS_FETCH_DEPENDENCIES
       "Enable fetching dependencies with cmake FetchContent" ON)
if(NOT TARGET decodeless::allocator)
  if(DECODELESS_FETCH_DEPENDENCIES)
    include(FetchContent)
    FetchContent_Declare(
      decodeless_allocator
      GIT_REPOSITORY https://github.com/decodeless/allocator.git
      GIT_TAG 49e53f01b2784a23b950eff92d748600338f7e52)
    FetchContent_MakeAvailable(decodeless_allocator)
  else()
    message(
      FATAL_ERROR
        "Dependency decodeless::allocator not found! You can enable searching with DECODELESS_SEARCH_DEPENDENCIES or downloading with DECODELESS_FETCH_DEPENDENCIES"
    )
  endif()
endif()

# Unit tests
add_executable(${PROJECT_NAME}_tests src/test_header.cpp src/test_mesh.cpp)
target_include_directories(${PROJECT_NAME}_tests PRIVATE src)
target_link_libraries(${PROJECT_NAME}_tests readytorender gtest_main
                      decodeless::allocator)

if(MSVC)
  target_compile_options(${PROJECT_NAME}_tests PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME}_tests PRIVATE -Wall -Wextra -Wpedantic
                                                       -Werror)

  # Ugly detection for a working _GLIBCXX_DEBUG_BACKTRACE config, but the
  # feature itself is useful
  include(glibcxx_debug_backtrace.cmake)
  if(GLIBCXX_DEBUG_BACKTRACE_SUPPORTED)
    target_compile_definitions(
      ${PROJECT_NAME}_tests PRIVATE $<$<CONFIG:Debug>:_GLIBCXX_DEBUG_BACKTRACE>)
    target_link_libraries(${PROJECT_NAME}_tests
                          $<$<CONFIG:Debug>:${GLIBCXX_DEBUG_BACKTRACE_LIBRARY}>)
    target_compile_features(${PROJECT_NAME}_tests
                            PRIVATE ${GLIBCXX_DEBUG_BACKTRACE_CXX_FEATURE})
  endif()
endif()

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_tests)
