
# Part 1

The following looks to address the requirements of the Embedded Software Developer Assessment. The following is instructions on how to build the Parser library and how to run a series of unit tests to validate the Parser class.

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
    rm -r out
    ```

# Part 2
  1. What is your favorite IPC mechanism and why?
      - As for interprocess communication, I don't specifically have a favorite. Most of what I have written code that utilizes shared files as well as code that utilized sockets. I think sockets are probably a better for of IPC just because you aren't reliant on a file where you have to wait on the resource to be unlocked to check for a response. Though with sockets you can communicate in both directions simultaneously. With a good library (for C++ at least) they are usually pretty easy to setup as well. Outside of this, I haven't written low level interprocess communication code so I still have a lot more to learn.  
  2. Speed up problem
      - Implementation can be found in the Part2 colder and is compiled as part of the process described for Part 1. The executable can be found in the install directory that is created during building.
      - Given the description, it sounded like you were looking for a solution implemented using threading. This is what I provided. However, it is very likely that only utilizing the regular C++ standards, a multi-threaded solution is actually slower. There are a few reason I can think of. One is because of thread contention on one variable. The only thing going on is addition on that one variable so the compiler can't make good use of downtime between threads. Additionally, every time the variable is updated, the cache lines will probably need to be updated for every core that is holding a copy of that memory causing memory invalidation and more slowdowns. The result was that instead of speeding things up, they are actually slowed down dramatically.
      - On my personal computer, using a single core took around 6 seconds while utilizing all 12 threads took around 16 seconds.
      - Minor adjustments that could have potentially helped speed were using prefix based incrementation and removing the volatile keyword from the counter as this will prevent optimizations by the compiler.
      - Last, if this had been allowable, an alternative solution would be to have each thread hold its own copy of the value to increment and then allow them to do their calculations independently. Then, add all the values together at the end. This way you remove the memory contention. But this really relies on understanding the scope of the problem better. 

# License:
  - This project is licensed under the terms of the [LICENSE](./LICENSE) file.
