#pragma once

#include "glm/glm.hpp"

enum dir_t {
	ROW, COL
};

struct gridline_t {
	glm::vec3 color = glm::vec3(0,0,0);
	int transform_handle = -1;
	int rec_render_handle = -1;
	dir_t dir;
};

void create_gridline(float x, float y, dir_t dir);
