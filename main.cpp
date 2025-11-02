#include <cstdio>
#include "chip8.h"
#include "clock.h"
#include "input.h"
#include "Mesh.h"
#include "Renderer.h"
#include "vk_buffer.h"
#include "vk_command_buffer.h"
#include "vk_descriptor.h"
#include "vk_device.h"
#include "vk_vertex.h"


//COMMAND LINE USAGE: ./chip 8 <ROM>

int main(int argc, char** argv)
{

    CHIP8* chip8 = chip8_init();
    if (!chip8_load_rom(chip8, argv[1]))
    {
        std::cout << argv[1] << std::endl;
       throw std::runtime_error("ROM COULD NOT LOAD");
    };

    //TESTING ROMS:
    // if (!chip8_load_rom(chip8, "../games/Tic-Tac-Toe [David Winter].ch8"))
    // if (!chip8_load_rom(chip8, "../games/Tetris [Fran Dachille, 1991].ch8"))
    // if (!chip8_load_rom(chip8, "../games/cave.ch8"))
    // if (!chip8_load_rom(chip8, "../games/test_opcode.ch8"))
    // {
    //    throw std::runtime_error("ROM COULD NOT LOAD");
    // };


    Vulkan_Context vulkan_context{};
    GLFW_Window_Context window_info{};
    Swapchain_Context swapchain_context{};
    Graphics_Context graphics_context{};
    Command_Buffer_Context command_buffer_context{};
    Buffer_Context buffer_context{};
    Semaphore_Fences_Context semaphore_fences_context{};
    Descriptor descriptor_set{};
    Texture texture{};

    //set window size based on scale
    // window_info.WIDTH = VIDEO_SCALE * VIDEO_WIDTH;
    // window_info.HEIGHT = VIDEO_SCALE * VIDEO_WIDTH;

    // reserve space for the vertice data
    VERTEX_DYNAMIC_INFO vertex_info = {}; //TODO: move this into the game state, since a lot of things will want access to this
    vertex_info.dynamic_vertices.reserve(MAX_VERTICES);
    vertex_info.dynamic_vertices.reserve(MAX_INDICES);



    init_vulkan(vulkan_context, window_info, swapchain_context, graphics_context, buffer_context,
                command_buffer_context, semaphore_fences_context, texture, chip8->video, descriptor_set);
    clock_windows_init();

    // add_quad_textured(glm::vec2{0.0f, 0.0f}, 1.0, vertex_info);
    add_full_screen_quad_textured(vertex_info);


    float cycle_time = SECONDS(0.01f);
    float dt = 0.0f; // in ms
    float dt_accumulation = 0.0f; // in ms

    while (!glfwWindowShouldClose(window_info.window))
    {
        glfwPollEvents();

        dt = clock_window_delta_time();
        dt_accumulation += dt;
        // get input
        key_callback(window_info.window, chip8);

        // draw_frame(vulkan_context, window_info, swapchain_context,
                             // graphics_context, command_buffer_context,
                             // buffer_context, vertex_info, semaphore_fences_context, descriptor_set);

        if (dt_accumulation > cycle_time)
        {

            //reset the timer
            dt_accumulation -= cycle_time;

            //process emulator
            chip8_cycle(chip8);



            update_texture_image_pixels(vulkan_context, command_buffer_context, texture, VK_FORMAT_R8_UNORM, chip8->video, VIDEO_WIDTH, VIDEO_HEIGHT);

        }

        //grab the pixel data, send it to a shader basically
        draw_frame(vulkan_context, window_info, swapchain_context,
        graphics_context, command_buffer_context,
        buffer_context, vertex_info, semaphore_fences_context, descriptor_set);
    }


    chip8_free(chip8);
    return 0;
}