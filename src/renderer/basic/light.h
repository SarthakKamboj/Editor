#pragma once

#include "glm/glm.hpp"
#include "renderer/graphics/mesh.h"
#include "renderer/camera.h"
#include "renderer/graphics/render_resources.h"
#include "app.h"

struct light_t {
    glm::vec3 color = glm::vec3(1,1,1);
    glm::vec2 world_pos;
    float radius = 20.f;
    int id = -1;
    int transform_handle = -1;
    float intensity = 1.0f;

    static mesh_t light_mesh;
    static shader_t light_shader;
};

void init_light_data(application_t& app);
int create_light(glm::vec3 pos, glm::vec3 color, float radius, float intensity);
void set_light_in_shader(const light_t& light);
void render_light(const light_t& light);
void render_lights(camera_t& camera, float ambient, float alpha);
void remove_light(light_t& light);