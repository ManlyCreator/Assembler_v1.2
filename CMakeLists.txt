# Specifies CMake Version
cmake_minimum_required(VERSION 3.10)

# Defines Project Name
project(Assembler)

# Allows compiler to see project libraries as global libraries
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enables debugging
set(CMAKE_BUILD_TYPE Debug)

# Defines Entry Point for the Executable
add_executable(Assembler main.c)

# Adds Subdirectories
add_subdirectory(Opcodes)
add_subdirectory(SymbolTable)

# Links External Libraries
target_link_libraries(Assembler Opcodes SymbolTable)
