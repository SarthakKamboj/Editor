#pragma once

#include "buffers.h"
#include "render_resources.h"

struct mesh_t {
	vao_t vao;
	ebo_t ebo;
	vbo_t vbo;
};

void render_mesh(const mesh_t& mesh);
