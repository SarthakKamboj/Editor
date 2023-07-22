#pragma once

#include "SDL.h"
#include <map>
#include "input/input.h"
#include "imgui.h"
#include "renderer/camera.h"
#include "renderer/opengl/object_data.h"
#include "constants.h"

struct application_t {
	bool running = true;
	SDL_Window* window = NULL;
	framebuffer_t world_grid_fbo;
	opengl_object_data fbo_draw_data;
	int debug_rec_handle = -1;

#if ENABLE_IMGUI
	ImGuiIO* io;
#endif
};

void init(application_t& app);
void update(camera_t& camera, key_state_t& key_state, float& x_offset);