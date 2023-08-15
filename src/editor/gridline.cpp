#include "gridline.h"
#include <vector>
#include "transform/transform.h"
#include "renderer/basic/quad.h"
#include "constants.h"

std::vector<gridline_t> gridlines;

void create_gridline(float x, float y, dir_t dir) {
	gridline_t gridline;
	glm::vec3 pos(x, y, 0.f);
	gridline.dir = dir;
	glm::vec3 color = GRIDLINE_COLOR;
	if (dir == dir_t::ROW) {
		gridline.transform_handle = create_transform(pos, glm::vec3(1.f), 90.f);
		gridline.quad_handle = create_quad(gridline.transform_handle, color, -1, 1.f, 2 * WINDOW_WIDTH, false, 0);
	}
	else {
		gridline.transform_handle = create_transform(pos, glm::vec3(1.f), 0.f );
		gridline.quad_handle = create_quad(gridline.transform_handle, color, -1, 1.f, WINDOW_HEIGHT, false, 0);
	}
	gridlines.push_back(gridline);
}

