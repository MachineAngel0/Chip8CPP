//
// Created by Adams Humbert on 9/10/2025.
//

#ifndef VK_DESCRIPTOR_H
#define VK_DESCRIPTOR_H

#include "texture.h"
#include "vk_device.h"
#include "vk_vertex.h"


struct Descriptor {
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorPool descriptor_pool;
    std::vector<VkDescriptorSet> descriptor_sets;
};

void create_descriptor_set_layout(Vulkan_Context& vulkan_context, Descriptor& descriptor);
void create_descriptor_pool(Vulkan_Context& vulkan_context, Descriptor& descriptor);
void create_descriptor_sets(Vulkan_Context& vulkan_context, Texture& texture, Uniform_Buffer_Context uniform_buffer_context, Descriptor& descriptor);


#endif //VK_DESCRIPTOR_H
