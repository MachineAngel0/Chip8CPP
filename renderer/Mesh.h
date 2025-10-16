#ifndef MESH_H
#define MESH_H

#include <vector>
#include "glm/glm.hpp"


struct VERTEX_DYNAMIC_INFO;
struct Vertex;



void move_quad(int id, VERTEX_DYNAMIC_INFO& vertex_info, glm::vec2 move_amount);

std::vector<Vertex> create_quad_textured(glm::vec2 pos, float scale);
int add_quad_textured(glm::vec2 pos, float scale, VERTEX_DYNAMIC_INFO& vertex_info);
int add_full_screen_quad_textured(VERTEX_DYNAMIC_INFO& vertex_info);



#endif
