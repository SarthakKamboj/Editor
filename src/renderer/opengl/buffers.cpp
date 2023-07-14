#include "buffers.h"
#include <array>
#include "constants.h"
#include <iostream>

// VBO
vbo_t create_vbo(const float* vertices, const int data_size) {
	vbo_t vbo;
	glGenBuffers(1, &vbo.id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
	glBufferData(GL_ARRAY_BUFFER, data_size, vertices, GL_STATIC_DRAW);
	return vbo;
}

void update_vbo_data(const vbo_t& vbo, const float* vertices, const int data_size) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
	glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, vertices);
}

void bind_vbo(const vbo_t& vbo) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
}

void unbind_vbo() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void delete_vbo(const vbo_t& vbo) {
	glDeleteBuffers(1, &vbo.id);
}

// EBO
ebo_t create_ebo(const unsigned int* indicies, const int size_of_buffer) {
    assert(indicies != NULL);
	ebo_t ebo;
	ebo.num_indicies = size_of_buffer / sizeof(indicies[0]);
	glGenBuffers(1, &ebo.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_of_buffer, indicies, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ebo;
}

ebo_t create_ebo(const unsigned int* indicies, const int size_of_buffer, GLenum usage_pattern) {
    assert(indicies != NULL);
	ebo_t ebo;
	ebo.num_indicies = size_of_buffer / sizeof(indicies[0]);
	glGenBuffers(1, &ebo.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_of_buffer, indicies, usage_pattern);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ebo;
}

void draw_ebo(const ebo_t& ebo) {
	glDrawElements(GL_TRIANGLES, ebo.num_indicies, GL_UNSIGNED_INT, 0);
}

void bind_ebo(const ebo_t& ebo) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.id);
}

void unbind_ebo() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void delete_ebo(const ebo_t& ebo) {
	glDeleteBuffers(1, &ebo.id);
}

// VAO
vao_t create_vao() {
	vao_t vao;
	glGenVertexArrays(1, &vao.id);
	return vao;
}

void bind_vao(const vao_t& vao) {
	glBindVertexArray(vao.id);
}

void unbind_vao() {
	glBindVertexArray(0);
}

void vao_enable_attribute(vao_t& vao, const vbo_t& vbo, const int attrId, const int numValues, const int dType, const int normalized, const int stride, const int offset) {
	bind_vao(vao);
	bind_vbo(vbo);
	glVertexAttribPointer(attrId, numValues, dType, normalized, stride, (void*)offset);
	glEnableVertexAttribArray(attrId);
}

void delete_vao(const vao_t& vao) {
	glDeleteVertexArrays(1, &vao.id);
}

// framebuffers
framebuffer_t create_framebuffer() {
	framebuffer_t framebuffer;
	glGenFramebuffers(1, &framebuffer.id);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);

	glGenTextures(1, &framebuffer.framebuffer_texture);
	glBindTexture(GL_TEXTURE_2D, framebuffer.framebuffer_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	// learn what these texture parameters do since it effects how the colorbuffer looks in the end
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.framebuffer_texture, 0);

	glGenRenderbuffers(1, &framebuffer.renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.renderbuffer);

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "framebuffer error: " << status << std::endl;
		switch (status) {
		case GL_FRAMEBUFFER_UNDEFINED:
			std::cout << "undefined" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			std::cout << "incomplete attachement" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			std::cout << "missing attachement" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl;
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << std::endl;
			break;
		default:
			std::cout << "something else" << std::endl;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return framebuffer;
}

void bind_framebuffer(const framebuffer_t& framebuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
}

void unbind_framebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}