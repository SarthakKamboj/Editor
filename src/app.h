#pragma once

#include "window.h"
#include "input/input.h"
#include "renderer/camera.h"
#include "constants.h"
#include "renderer/graphics/buffers.h"

struct application_t {
	bool running = true;
    window_t window;
	framebuffer_t light_map_fbo;
	framebuffer_t main_fbo;
    mouse_state_t mouse_state;
    key_state_t key_state;
};

application_t init_app(int window_width, int window_height);
void start_of_frame(application_t& app);
void end_of_frame(application_t& app);

void update_app(camera_t& camera, key_state_t& key_state, float& x_offset);