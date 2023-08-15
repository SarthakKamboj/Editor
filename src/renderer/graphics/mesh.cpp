#include "mesh.h"
#include "buffers.h"
#include "glad/glad.h"

void render_mesh(const mesh_t& mesh) {
	bind_vao(mesh.vao);
	draw_ebo(mesh.ebo);
}
