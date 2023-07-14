#pragma once

#include "SDL.h"
#include <map>
#include "input/input.h"
#include "imgui.h"
#include "renderer/camera.h"
#include "renderer/opengl/object_data.h"

struct application_t {
	bool running = true;
	SDL_Window* window = NULL;
	ImGuiIO* io;
	framebuffer_t world_grid_fbo;
	opengl_object_data fbo_draw_data;
	int debug_rec_handle = -1;
};

void init(application_t& app);
void update(camera_t& camera, key_state_t& key_state, float& x_offset);