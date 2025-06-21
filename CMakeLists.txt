cmake_minimum_required(VERSION 3.10)

# Project name
project(CoreUtils LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define source and header file conventions
file(GLOB_RECURSE COREUTILS_SOURCE src/*.cxx)
file(GLOB_RECURSE COREUTILS_HEADERS src/*.hxx)

# Add the library target
add_library(CoreUtils SHARED ${COREUTILS_SOURCE} ${COREUTILS_HEADERS})
add_library(CoreUtilsStatic STATIC ${COREUTILS_SOURCE} ${COREUTILS_HEADERS})

# Set the output directories for libraries
set_target_properties(CoreUtils PROPERTIES OUTPUT_NAME "CoreUtils")
set_target_properties(CoreUtilsStatic PROPERTIES OUTPUT_NAME "CoreUtils")

# Include directories
include_directories(src)

# Example setup
add_executable(CoreUtilsExample example/example.cxx)
target_link_libraries(CoreUtilsExample CoreUtilsStatic)

# Copy compile_commands.json to project root after generation
add_custom_command(TARGET CoreUtils POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Copying compile_commands.json to project root"
)

# Add install directives for libraries (optional)
install(TARGETS CoreUtils CoreUtilsStatic
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.hxx")
