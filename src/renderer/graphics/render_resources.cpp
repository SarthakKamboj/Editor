#include "render_resources.h"
#include "stb/stb_image.h"
#include <iostream>
#include "utils/io.h"
#include <vector>

// TEXTURE

static std::vector<texture_t> textures;

int create_texture(const char* path) {
    assert(path != NULL);

    for (texture_t& texture : textures) {
        if (texture.path == std::string(path)) {
            return texture.handle;
        }
    }

    static int running_count = 0;
	texture_t texture;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &texture.width, &texture.height, &texture.num_channels, 0);

    assert(data != NULL);

	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (texture.num_channels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

    texture.path = std::string(path);
    texture.handle = running_count;
    textures.push_back(texture);
    running_count++;
	return texture.handle;
}

void bind_texture(const texture_t& texture) {
	glBindTexture(GL_TEXTURE_2D, texture.id);
}

void bind_texture_by_handle(int texture_handle) {
    for (texture_t& texture : textures) {
        if (texture.handle == texture_handle) {
            glBindTexture(GL_TEXTURE_2D, texture.id);
        }
    }
}

void bind_texture_by_id(int texture_id) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void unbind_texture() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

texture_t* get_texture(int texture_handle) {
    for (texture_t& texture : textures) {
        if (texture.handle == texture_handle) {
            return &texture;
        }
    }
    return NULL;
}

// SHADER
shader_t create_shader(const char* vert_path, const char* frag_path) {
	shader_t shader;

	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

	int success;
	char info_log[512];

	std::string vert_code_str = get_file_contents(vert_path);
	const char* vert_shader_source = vert_code_str.c_str();
	glShaderSource(vert_shader, 1, &vert_shader_source, NULL);
	glCompileShader(vert_shader);
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vert_shader, 512, NULL, info_log);
		std::cout << std::string(info_log) << std::endl;
		throw std::runtime_error("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + std::string(info_log));
	}

	std::string frag_code_str = get_file_contents(frag_path);
	const char* frag_shader_source = frag_code_str.c_str();
	glShaderSource(frag_shader, 1, &frag_shader_source, NULL);
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(frag_shader, 512, NULL, info_log);
		std::cout << std::string(info_log) << std::endl;
		throw std::runtime_error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" + std::string(info_log));
	}

	shader.id = glCreateProgram();
	glAttachShader(shader.id, vert_shader);
	glAttachShader(shader.id, frag_shader);
	glLinkProgram(shader.id);
	glGetProgramiv(shader.id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader.id, 512, NULL, info_log);
		throw std::runtime_error("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(info_log));
	}
	return shader;
}

void bind_shader(const shader_t& shader) {
	glUseProgram(shader.id);
}

void unbind_shader() {
	glUseProgram(0);
}

void shader_set_mat4(shader_t& shader, const char* var_name, const glm::mat4& mat) {
	glUseProgram(shader.id);
	GLint loc = glGetUniformLocation(shader.id, var_name);
    if (loc == -1) {
        std::cout << var_name << " does not exist in shader " << shader.id << std::endl;
    }
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void shader_set_int(shader_t& shader, const char* var_name, const int val) {
	glUseProgram(shader.id);
	GLint loc = glGetUniformLocation(shader.id, var_name);
    if (loc == -1) {
        std::cout << var_name << " does not exist in shader " << shader.id << std::endl;
    }
	glUniform1i(loc, val);
}

void shader_set_vec3(shader_t& shader, const char* var_name, const glm::vec3& v) {
	glUseProgram(shader.id);
	GLint loc = glGetUniformLocation(shader.id, var_name);
    if (loc == -1) {
        std::cout << var_name << " does not exist in shader " << shader.id << std::endl;
    }
	glUniform3fv(loc, 1, glm::value_ptr(v));
}

void shader_set_float(shader_t& shader, const char* var_name, const float val) {
	glUseProgram(shader.id);
	GLint loc = glGetUniformLocation(shader.id, var_name);
    if (loc == -1) {
        std::cout << var_name << " does not exist in shader " << shader.id << std::endl;
    }
	glUniform1f(loc, val);
}

glm::vec3 shader_get_vec3(const shader_t& shader, const char* var_name) {
	glm::vec3 v;
	GLint loc = glGetUniformLocation(shader.id, var_name);
    if (loc == -1) {
        std::cout << var_name << " does not exist in shader " << shader.id << std::endl;
    }
	glGetUniformfv(shader.id, loc, &v[0]);
	return v;
}

void shader_set_uniform(shader_t& shader, shader_uniform_t& uniform, const shader_uniform_value_t& value) {
	bind_shader(shader);
	switch (uniform.type) {
		case shader_uniform_type::MAT4: {
			glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(*value.mat4));
			break;
		}
		case shader_uniform_type::VEC3: {
			glUniform3fv(uniform.location, 1, glm::value_ptr(*value.vec3));
			break;
		}
		case shader_uniform_type::INT: {
			glUniform1i(uniform.location, value.int_val);
			break;
		}
		case shader_uniform_type::FLOAT: {
			glUniform1f(uniform.location, value.float_val);
			break;
		}
	}
}

shader_uniform_t create_shader_uniform(shader_t& shader, const char* name, shader_uniform_type type) {
	shader_uniform_t info;
	info.type = type;
	memcpy(info.uniform_name, name, strlen(name));
	bind_shader(shader);
	info.location = glGetUniformLocation(shader.id, info.uniform_name);
	assert(info.location != -1);
	return info;
}