#include "quad.h"
#include "utils/conversion.h"
#include "transform/transform.h"
#include "renderer/graphics/vertex.h"
#include <vector>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "input/input.h"

mesh_t quad_t::quad_mesh{};
shader_t quad_t::quad_shader{};

std::vector<quad_t> quads;

void init_quad_data() {
	mesh_t& mesh = quad_t::quad_mesh;

	vertex_t vertices[4];
    vertices[0] = create_vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0,1,1), glm::vec2(1,1)); // top right
	vertices[1] = create_vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0,0,1), glm::vec2(1,0)); // bottom right
	vertices[2] = create_vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0,1,0), glm::vec2(0,0)); // bottom left
	vertices[3] = create_vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1,0,0), glm::vec2(0,1)); // top left

	mesh.vbo = create_vbo((float*)vertices, sizeof(vertices));

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	mesh.ebo = create_ebo(indices, sizeof(indices));
	mesh.vao = create_vao();

	bind_vao(mesh.vao);
	vao_enable_attribute(mesh.vao, mesh.vbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, position));
	vao_enable_attribute(mesh.vao, mesh.vbo, 1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, color));
	vao_enable_attribute(mesh.vao, mesh.vbo, 2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, tex_coord));
	bind_ebo(mesh.ebo);
	unbind_vao();
	unbind_ebo();

    shader_t& quad_shader = quad_t::quad_shader;
	quad_shader = create_shader((SHADERS_PATH + "\\quad.vert").c_str(), (SHADERS_PATH + "\\quad.frag").c_str());
	shader_set_mat4(quad_shader, "model", glm::mat4(1));
	glm::mat4 projection = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);
	shader_set_mat4(quad_shader, "projection", projection);
    shader_set_int(quad_shader, "tex", 0);
    shader_set_int(quad_t::quad_shader, "light_map", 1);
}

int create_quad(int transform_handle, glm::vec3& color, int tex_handle, float width, float height, bool wireframe, float tex_influence) {
    static int running_count = 0;
	quad_t quad;
	quad.transform_handle = transform_handle;
	quad.width = width;
	quad.height = height;
	quad.color = color;
	quad.wireframe_mode = wireframe;
    quad.tex_influence = tex_influence;
    // 2D ortho projection represented in pixels and original rec mesh is 1x1, so scale rectangle mesh by rec width and height
	quad.internal_transform.scale = glm::vec3(width, height, 1.f);
    quad.texture_handle = tex_handle;
    quad.handle = running_count;
	quads.push_back(quad);
    running_count++;
	return quad.handle;
}

quad_t* get_quad(int quad_handle) {
    for (quad_t& quad : quads) {
        if (quad.handle == quad_handle) {
            return &quad;
        }
    }
    return NULL;
}

void remove_quad(int quad_handle) {
    quad_t* quad_ptr = get_quad(quad_handle);
    assert(quad_ptr != NULL);
    quad_t& quad = *quad_ptr;
    remove_transform(quad.transform_handle);
    int idx_to_remove = -1;
    for (int i = 0; i < quads.size(); i++) {
        if (quad_handle == quads[i].handle) {
            idx_to_remove = i;
            break;
        }
    }
    if (idx_to_remove != -1) {
        quads.erase(quads.begin() + idx_to_remove, quads.begin() + idx_to_remove + 1);
    }
}

void set_quad_in_shader(quad_t& quad) {
    transform_t* cur_transform_ptr = get_transform(quad.transform_handle);
    assert(cur_transform_ptr != NULL);
	transform_t cur_transform = *cur_transform_ptr;
    transform_t frame_transform{};
    frame_transform.position = cur_transform.position + quad.internal_transform.position;
    frame_transform.scale = quad.internal_transform.scale * cur_transform.scale, 
    frame_transform.rotation_deg = quad.internal_transform.rotation_deg + cur_transform.rotation_deg;

    if (frame_transform.position != quad.last_relevant_transform.position ||
        frame_transform.scale != quad.last_relevant_transform.scale ||
        frame_transform.rotation_deg != quad.last_relevant_transform.rotation_deg) {
        quad.last_relevant_transform.position = frame_transform.position;
        quad.last_relevant_transform.scale = frame_transform.scale;
        quad.last_relevant_transform.rotation_deg = frame_transform.rotation_deg;
        quad.model_matrix = get_model_matrix(frame_transform);
    }

	shader_set_mat4(quad_t::quad_shader, "model", quad.model_matrix);
	shader_set_vec3(quad_t::quad_shader, "color", quad.color);
    if (quad.texture_handle != -1) {
	    shader_set_float(quad_t::quad_shader, "tex_influence", quad.tex_influence);
        glActiveTexture(GL_TEXTURE0);
        bind_texture_by_handle(quad.texture_handle);
    }
	if (quad.wireframe_mode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void render_quad(quad_t& quad) {
    set_quad_in_shader(quad);
    bind_shader(quad_t::quad_shader);
	render_mesh(quad_t::quad_mesh);
}

void render_quad(int quad_handle) {
    for (quad_t& quad : quads) {
        if (quad_handle == quad.handle) {
	        render_quad(quad);
            break;
        }
    }
}

void render_quads(camera_t& camera, application_t& app) {
    glm::mat4 view_matrix = get_view_matrix(camera);
	shader_set_mat4(quad_t::quad_shader, "view", view_matrix);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, app.light_map_fbo.color_texture);
	for (quad_t& quad : quads) {
		render_quad(quad);
	}
}