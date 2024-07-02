# Add source to this project's executable.
add_executable (Counter "CounterProblem.cpp")

# Specify installation directories
install(TARGETS Counter
    RUNTIME DESTINATION bin
)