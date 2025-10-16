#include "Mesh.h"

#include "vk_vertex.h"


std::vector<Vertex> create_quad_textured(glm::vec2 pos, float scale)
{
    return {
        {{pos.x - scale, pos.y - scale}, {1.0f, 0.0f}},
        {{pos.x + scale, pos.y - scale}, {0.0f, 0.0f}},
        {{pos.x + scale, pos.y + scale}, {0.0f, 1.0f}},
        {{pos.x - scale, pos.y + scale}, {1.0f, 1.0f}},
    };
}

int add_quad_textured(glm::vec2 pos, float scale, VERTEX_DYNAMIC_INFO& vertex_info)
{
    std::vector<Vertex> new_quad = create_quad_textured(pos, scale);
    uint16_t base_index = static_cast<uint16_t>(vertex_info.dynamic_vertices.size());

    const std::vector<Vertex> temp_vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f}}
    };

    // Add vertices
    vertex_info.dynamic_vertices.insert(vertex_info.dynamic_vertices.end(), temp_vertices.begin(), temp_vertices.end());

    // Add indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        static_cast<uint16_t>(base_index + 0),
        static_cast<uint16_t>(base_index + 1),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 3),
        static_cast<uint16_t>(base_index + 0)
    };

    vertex_info.dynamic_indices.insert(vertex_info.dynamic_indices.end(), quad_indices.begin(), quad_indices.end());


    vertex_info.vertex_buffer_should_update = true;

    return vertex_info.mesh_id++;

    /*
    printf("Added quad at (%.2f, %.2f) with color (%.2f, %.2f, %.2f)\n",
           pos.x, pos.y, color.r, color.g, color.b);
    printf("Total vertices: %zu, Total indices: %zu\n", dynamic_vertices.size(), dynamic_indices.size());
    */
}

int add_full_screen_quad_textured(VERTEX_DYNAMIC_INFO& vertex_info)
{
    uint16_t base_index = static_cast<uint16_t>(vertex_info.dynamic_vertices.size());

    const std::vector<Vertex> temp_vertices = {
        {{-1.f, -1.0f}, {1.0f, 0.0f}},
        {{1.f, -1.0f}, {0.0f, 0.0f}},
        {{1.f, 1.0f}, {0.0f, 1.0f}},
        {{-1.f, 1.0f}, {1.0f, 1.0f}}
    };

    // Add vertices
    vertex_info.dynamic_vertices.insert(vertex_info.dynamic_vertices.end(), temp_vertices.begin(), temp_vertices.end());

    // Add indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        static_cast<uint16_t>(base_index + 0),
        static_cast<uint16_t>(base_index + 1),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 3),
        static_cast<uint16_t>(base_index + 0)
    };

    vertex_info.dynamic_indices.insert(vertex_info.dynamic_indices.end(), quad_indices.begin(), quad_indices.end());


    vertex_info.vertex_buffer_should_update = true;

    return vertex_info.mesh_id++;

    /*
    printf("Added quad at (%.2f, %.2f) with color (%.2f, %.2f, %.2f)\n",
           pos.x, pos.y, color.r, color.g, color.b);
    printf("Total vertices: %zu, Total indices: %zu\n", dynamic_vertices.size(), dynamic_indices.size());
    */
}


void move_quad(int id, VERTEX_DYNAMIC_INFO& vertex_info, glm::vec2 move_amount)
{
    int index_into_stride = id * 4;
    for (int i = 0; i < 4; i++)
    {
        vertex_info.dynamic_vertices[index_into_stride + i].pos += move_amount;
    }

    vertex_info.vertex_buffer_should_update = true;
}
