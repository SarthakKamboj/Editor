#include "conversion.h"
#include "constants.h"
#include <iostream>
#include "SDL.h"
#include "app.h"

extern application_t application;

namespace conversion {
	glm::vec2 ndc_coord_to_window_coord(const glm::vec2& ndc_coord) {
        assert(ndc_coord.x >= -1.f && ndc_coord.x <= 1.f);
        assert(ndc_coord.y >= -1.f && ndc_coord.y <= 1.f);
		glm::vec2 ndc_0_to_1 = (ndc_coord + 1.0f) * 0.5f;
		return ndc_0_to_1 * glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT);
	}

	glm::vec2 ndc_size_to_window_size(const glm::vec2& ndc_size) {
        assert(ndc_size.x >= 0 && ndc_size.x <= 2.f);
        assert(ndc_size.y >= 0 && ndc_size.y <= 2.f);
		return ndc_size * (glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT) / 2.f);
	}

	glm::vec2 window_coord_to_ndc_coord(const glm::vec2& screen_coord) {
        assert(screen_coord.x >= 0 && screen_coord.x <= WINDOW_WIDTH);
        assert(screen_coord.y >= 0 && screen_coord.y <= WINDOW_HEIGHT);
		glm::vec2 normalized_screen = screen_coord / glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT);
		glm::vec2 ndc = (normalized_screen * 2.0f) - 1.0f;
		return glm::vec2(ndc.x, ndc.y);
	}

	glm::vec2 convert_top_left_origin_to_bottom_left_origin(glm::vec2 top_left_origin_coords) {
		return glm::vec2(top_left_origin_coords.x, -top_left_origin_coords.y + WINDOW_HEIGHT);
	}

	static ImVec2 entire_window_top_left_screen_pos;
	ImVec2 get_window_pos() {
        // relative to screen
		ImVec2 top_left_screen = ImGui::GetWindowPos();
		ImVec2 top_left_win; 
		top_left_win.x = top_left_screen.x - entire_window_top_left_screen_pos.x;
		top_left_win.y = top_left_screen.y - entire_window_top_left_screen_pos.y;
		glm::vec2 top_left = glm::vec2(top_left_win.x, top_left_win.y);
		glm::vec2 converted = convert_top_left_origin_to_bottom_left_origin(top_left);
		return ImVec2(converted.x, converted.y);
	}

	void set_window_top_left_screen_coord() {
		// entire_window_top_left_screen_pos = ImGui::GetWindowPos();

        int x, y;
        SDL_GetWindowPosition(application.window.sdl_window, &x, &y);
        entire_window_top_left_screen_pos.x = x;
        entire_window_top_left_screen_pos.y = y;
	}
}