project ("Parser")

file(GLOB_RECURSE SOURCES "src/*.cpp" "include/*.h")

add_library (TelemetryParser ${SOURCES})

# Specify include directories for the target
target_include_directories(TelemetryParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)