project("TestParser")

# Create the executable for testing
add_executable(TestParser test_parser.cpp)

# Link the TelemetryParser library to the TestParser executable
target_link_libraries(TestParser TelemetryParser)

# Specify include directories for the target
target_include_directories(TelemetryParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Specify installation directories
install(TARGETS TestParser
    RUNTIME DESTINATION bin
)

add_test(NAME TestParser COMMAND TestParser)