#pragma once

#include "transform/transform.h"
#include "glm/glm.hpp"
#include "renderer/graphics/mesh.h"
#include "renderer/graphics/render_resources.h"
#include "renderer/camera.h"
#include "app.h"

struct quad_t {
	glm::mat4 model_matrix = glm::mat4(1.0f);
	transform_t last_relevant_transform{};

    // internal_transform responsible for scaling based on actual width/height of the rectangle
	transform_t internal_transform;
	glm::vec3 color = glm::vec3(0, 0, 0);
	float width = -1.f, height = -1.f;
	bool wireframe_mode = false;
    float tex_influence = 0.f;
    int texture_handle = -1;
    int handle = -1;
	int transform_handle = -1;

	static mesh_t quad_mesh;
	static shader_t quad_shader;
	static shader_uniform_t quad_color_uniform;
	static shader_uniform_t quad_model_uniform;
};

void init_quad_data();
int create_quad(int transform_handle, glm::vec3& color, int tex_handle, float width, float height, bool wireframe, float tex_influence);
quad_t* get_quad(int quad_handle);
void remove_quad(int quad_handle);
void set_quad_in_shader(quad_t& quad);
void render_quad(quad_t& quad);
void render_quad(int quad_handle);
void render_quads(camera_t& camera, application_t& app);
