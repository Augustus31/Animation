cmake_minimum_required(VERSION 3.16)

project(menv)

# Add source files
set(
    SOURCES
    main.cpp
    src/Camera.cpp
    src/Cube.cpp
    src/Shader.cpp
    src/Tokenizer.cpp
    src/Window.cpp
    src/DOF.cpp
    src/Joint.cpp
    src/Skeleton.cpp
    src/Skin.cpp
    src/Keyframe.cpp
    src/Channel.cpp
    src/AnimClip.cpp
    src/Particle.cpp
    src/Triangle.cpp
    src/SpringDamper.cpp
)

# Add header files
set(
    HEADERS
    include/core.h
    include/Camera.h
    include/Cube.h
    include/Shader.h
    include/Tokenizer.h
    include/Window.h
    include/DOF.h
    include/Joint.h
    include/Skeleton.h
    include/Skin.h
    include/Keyframe.h
    include/Channel.h
    include/AnimClip.h
    include/Particle.h
    include/Triangle.h
    include/SpringDamper.h
)

# Require GL
find_package(OpenGL REQUIRED)

# Add include directories
include_directories(
    include
)

# Add library directories
link_directories(
    lib
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glew32s.lib glfw3)

# Move assets to .exe
add_custom_target(CopyShaders ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/shaders"
	"${CMAKE_BINARY_DIR}/shaders"
)
add_dependencies(menv CopyShaders)