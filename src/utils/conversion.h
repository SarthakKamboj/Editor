#pragma once

#include "glm/glm.hpp"
#include "imgui.h"

namespace conversion {
	glm::vec2 ndc_coord_to_window_coord(const glm::vec2& ndc_coord);
	glm::vec2 ndc_size_to_window_size(const glm::vec2& ndc_size);
	glm::vec2 window_coord_to_ndc_coord(const glm::vec2& screen_coord);
	glm::vec2 convert_top_left_origin_to_bottom_left_origin(glm::vec2 top_left_origin_coords);
	void set_window_top_left_screen_coord();
	ImVec2 get_window_pos();
}
