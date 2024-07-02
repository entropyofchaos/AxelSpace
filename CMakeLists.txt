# CMakeList.txt : CMake project for AxelSpace, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Set C++20 as the standard for all targets
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project ("AxelSpace")

file(GLOB_RECURSE SOURCES "src/*.cpp" "include/*.h")

add_library (TelemetryParser ${SOURCES})

# Specify include directories for the target
target_include_directories(TelemetryParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Enable testing with CTest
enable_testing()

add_subdirectory(tests)
