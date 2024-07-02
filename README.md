
# Project Name

Brief description of the project.

## Prerequisites

- **Windows:**
  - [CMake](https://cmake.org/download/)
  - [Visual Studio](https://visualstudio.microsoft.com/) with a compiler that supports C++20
    - Use version 2019 (16.8 or later) or 2022 to ensure full compatibility with C++20
    - CMake likely is already installed as part of the Visual Studio C++ installation process
  
- **Linux:**
  - `cmake` (can be installed via `apt`, `yum`, or other package managers)
  - `g++` (or another C++ compiler that supports C++20)
    - Use GCC 10 or newer to ensure full compatibility with C++20

## Building the Project

1. **Open a Command Line**
    - **Windows:**
      - Open a `Developer Command Prompt for VS 2019` or `Developer Command Prompt VS 2022` and navigate to the project's root directory. It should be easily searchable Windows Start Menu.
    - **Linux:**
      - Open a Terminal

2. **Generate Build Files**:
    - Navigate to the project's top level directory.
    - Run the following command to generate the build files:
      ```sh
      cmake -S . --preset x64-release
      ```

2. **Build and Install the Project**:
   - Run the following command to build and install the project:
     ```sh
     cmake --build --preset x64-release --target install
     ```

## Running Tests
- Navigate to the project's top level directory.
- Run the following command: 
  - Note: `ctest` is automatically included with CMake
  ```sh
  ctest --preset x64-release-test
  ```
- Output will be displayed showing 162 assertions have been run for two different test cases.

## Additional Information

- **Cleaning the Build**:
  - To clean the build files, run the following command from the project's root directory:
    ```sh
    rm -rf out
    ```

- **License**:
  - This project is licensed under the terms of the [LICENSE](./LICENSE) file.
