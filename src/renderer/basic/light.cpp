#include "light.h"
#include <vector>
#include "transform/transform.h"
#include "glm/gtx/string_cast.hpp"
#include <iostream>
#include "renderer/graphics/vertex.h"

mesh_t light_t::light_mesh{};
shader_t light_t::light_shader{};

static std::vector<light_t> lights;

void init_light_data(application_t& app) {
	mesh_t& mesh = light_t::light_mesh;
	const int NUM_SIDES = 64;

	vertex_t vertices[NUM_SIDES + 1]{};
	float val = 0.5f;
	vertices[0] = create_vertex(glm::vec3(0), glm::vec3(1), glm::vec2(0,0)); // center
	float angle = 0.f;
	for (int i = 0; i < NUM_SIDES; i++) {
		vertices[i+1] = create_vertex(glm::vec3(val * cosf(angle), val * sinf(angle), 0.f), glm::vec3(0), glm::vec2(1));
		angle += (2 * PI) / NUM_SIDES;
	}

	mesh.vbo = create_vbo((float*)vertices, sizeof(vertices));

	unsigned int indicies[NUM_SIDES * 3]{};
	int running_i = 0;
	for (int i = 0; i < NUM_SIDES - 1; i++) {
		indicies[running_i++] = 0;
		indicies[running_i++] = i + 2;
		indicies[running_i++] = i + 1;
	}

	indicies[running_i++] = 0;
	indicies[running_i++] = 1;
	indicies[running_i] = NUM_SIDES;

	mesh.ebo = create_ebo(indicies, sizeof(indicies));
	mesh.vao = create_vao();

	bind_vao(mesh.vao);
	vao_enable_attribute(mesh.vao, mesh.vbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, position));
	vao_enable_attribute(mesh.vao, mesh.vbo, 1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, color));
	vao_enable_attribute(mesh.vao, mesh.vbo, 2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, tex_coord));
	bind_ebo(mesh.ebo);
	unbind_vao();
	unbind_ebo();

	shader_t& light_shader = light_t::light_shader;
	light_shader = create_shader((SHADERS_PATH + "\\light.vert").c_str(), (SHADERS_PATH + "\\light.frag").c_str());
	shader_set_mat4(light_shader, "model", glm::mat4(1));
	glm::mat4 projection = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);
	shader_set_mat4(light_shader, "projection", projection);	
}

int create_light(glm::vec3 pos, glm::vec3 color, float radius) {
    static int running_cnt = 0;
    light_t light;
    light.id = running_cnt;
    light.color = color;
    light.world_pos = pos;
    light.radius = radius;
    light.transform_handle = create_transform(glm::vec3(light.world_pos.x, light.world_pos.y, 0.f), glm::vec3(light.radius), 0.f);
    lights.push_back(light);
    running_cnt++;
}

void set_light_in_shader(const light_t& light) {
    transform_t* transform_ptr = get_transform(light.transform_handle);
    assert(transform_ptr != NULL);
    transform_t& transform = *transform_ptr;
    glm::mat4 model_matrix = get_model_matrix(transform);

	shader_t& light_shader = light_t::light_shader;
	shader_set_mat4(light_shader, "model", model_matrix);
	shader_set_vec3(light_shader, "color", light.color);
}

void render_light(const light_t& light) {
    set_light_in_shader(light);
    bind_shader(light_t::light_shader);
	render_mesh(light_t::light_mesh);
}

void render_lights(camera_t& camera) {
    glm::mat4 view_matrix = get_view_matrix(camera);
    std::cout << "light view matrix: " << glm::to_string(view_matrix) << std::endl;
	shader_set_mat4(light_t::light_shader, "view", view_matrix);
	for (const light_t& light : lights) {
		render_light(light);
	}
}