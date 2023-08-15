#pragma once

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include <string>

struct shader_t {
	GLuint id = 0;
};

shader_t create_shader(const char* vert_path, const char* frag_path);
void bind_shader(const shader_t& shader);
void unbind_shader();
void shader_set_mat4(shader_t& shader, const char* var_name, const glm::mat4& mat);
void shader_set_int(shader_t& shader, const char* var_name, const int val);
void shader_set_float(shader_t& shader, const char* var_name, const float val);
void shader_set_vec3(shader_t& shader, const char* var_name, const glm::vec3& v);
glm::vec3 shader_get_vec3(const shader_t& shader, const char* var_name);


struct texture_t {
    int handle = -1;
	GLuint id;
	int width = -1, height = -1;
	int num_channels = -1;
    std::string path;
};

int create_texture(const char* path);
void bind_texture(const texture_t& texture);
void bind_texture_by_handle(int texture_handle);
void bind_texture_by_id(int texture_id);
void unbind_texture();
texture_t* get_texture(int texture_handle);