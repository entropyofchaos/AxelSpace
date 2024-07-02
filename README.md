
# AxelSpace

The following project looks to address the requriements of the Embedded Software Developer Assesment. The following is instructions on how to build the Parser library and how to run a series of unit tests to validate the Parser class.

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

### Windows

1. **Open a Command Line**
    - Open a `Developer PowerShell for VS 2019` or `Developer PowerShell VS 2022` and navigate to the project's root directory. It should be easily searchable Windows Start Menu.

2. **Generate Build Files**:
    - Run the following command to generate the build files:
      ```sh
      cmake -S . --preset windows-x64-release
      ```

3. **Build the Project**:
   - Run the following command to build the project:
     ```sh
     cmake --build --preset windows-x64-release --target install
     ```

### Linux

1. **Open a Command Line**
    - Open a terminal window

2. **Generate Build Files**:
    - Run the following command to generate the build files:
      ```sh
      cmake -S . --preset linux-x64-release
      ```

3. **Build the Project**:
   - Run the following command to build the project:
     ```sh
     cmake --build --preset linux-x64-release --target install
     ```
     - Warning Information:
        1. Depending on the version of GCC used, there may be warnings displayed. One is related to `std::vector::insert()` and is a known bug that is supposed to be fixed in GCC 11.5 (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100366)
        2. Catch2, the test set library also apears to cause a warning to appear in certain versions of GCC. Sinc this is outside the scope of this project, I didn't look to correct it. 

## Running Tests

### Windows

1. **Open a Command Line**
    - Open a `Developer PowerShell for VS 2019` or `Developer PowerShell VS 2022` and navigate to the project's root directory. It should be easily searchable Windows Start Menu.

2. **Generate Build Files**:
    - Run the following command to generate the build files:
      -  Note: `ctest` is automatically included with CMake
      ```sh
      ctest --preset windows-x64-release-test
      ```
    - Output will be displayed showing 162 assertions have been run for two different test cases.

### Linux

1. **Open a Command Line**
    - Open a terminal window

2. **Generate Build Files**:
    - Run the following command to generate the build files:
      -  Note: `ctest` is automatically included with CMake
      ```sh
      ctest --preset linux-x64-release-test
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
