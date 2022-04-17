cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

project(testApp3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-g)

set(SRC ${PROJECT_SOURCE_DIR}/main.cpp ${PROJECT_SOURCE_DIR}/List.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} ${SRC}) 


option(RUN_CLANG_TIDY_TEST_APP3 "Run clang_tidy to static code checking" ON)

if(${RUN_CLANG_TIDY_TEST_APP3})
  find_program(
    CLANG_TIDY_EXE
    NAMES "clang-tidy"
    DOC "Path to clang-tidy executable"
    )
  if(NOT CLANG_TIDY_EXE)
    message(STATUS "clang-tidy not found.")
  else()
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-config=" "-fix")
  endif()

  if(CLANG_TIDY_EXE)
    set_target_properties(
      ${PROJECT_NAME} PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
    )
  endif()
endif()
