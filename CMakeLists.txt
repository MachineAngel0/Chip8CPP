cmake_minimum_required(VERSION 3.31)
project(Chip8CPP)

set(CMAKE_CXX_STANDARD 20)

# TODO: create a unity file later
#set(CMAKE_UNITY_BUILD TRUE) # UNITY BUILD

find_package(Vulkan	REQUIRED)

add_subdirectory(lib/glfw-3.4)
add_subdirectory(lib/glm)

include_directories(${PROJECT_NAME} ./renderer/)
include_directories(${PROJECT_NAME} ./lib/)


add_executable(${PROJECT_NAME}
        main.cpp
        chip8.h
        input.h
        input.cpp


        renderer/Renderer.h
        renderer/Renderer.cpp
        renderer/Mesh.cpp
        renderer/Mesh.h
        renderer/texture.cpp
        renderer/texture.h
        renderer/vk_command_buffer.h
        renderer/vk_command_buffer.cpp
        renderer/vk_command_buffer.h
        renderer/vk_device.cpp
        renderer/vk_device.h
        renderer/vk_buffer.cpp
        renderer/vk_buffer.h
        renderer/vk_vertex.cpp
        renderer/vk_vertex.h
        renderer/vk_descriptor.cpp
        renderer/vk_descriptor.h
        renderer/vk_renderpass.cpp
        renderer/vk_renderpass.h
        renderer/clock.cpp
        renderer/clock.h

        lib/stb_impl.cpp

)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Vulkan::Vulkan
        glfw
        glm
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/Lib/freetype.lib
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:Release>:RELEASE_BUILD>
)

#Here because ill get .dll missing errors
# Force static linking approach for MinGW
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

# Also try to force GLFW to link statically if possible
set(GLFW_USE_STATIC_LIBS ON)

# Add static linking flags
target_link_options(${PROJECT_NAME} PRIVATE
        -static-libgcc
        -static-libstdc++
        -static
)
