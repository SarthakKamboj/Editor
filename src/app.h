#pragma once

#include "SDL.h"
#include <map>
#include "input/input.h"
#include "imgui.h"
#include "renderer/camera.h"
#include "renderer/opengl/object_data.h"
#include "constants.h"

struct level_info_t {
    char output_folder[1024]{};
    char file_name[256]{};
    char full_path[1024+256]{};
};

void create_level_info(level_info_t& level_info, char* full_path);
void create_level_info(level_info_t& level_info);

enum application_state {
    LOADER = 0,
    EDITOR
};

struct application_t {
	bool running = true;
	SDL_Window* window = NULL;
	framebuffer_t world_grid_fbo;
	opengl_object_data fbo_draw_data;
	int debug_rec_handle = -1;
	ImGuiIO* io;
    application_state state = application_state::LOADER;
    level_info_t cur_level{};
};

void init(application_t& app);
void init_placed_world_items(const char* path);
void set_level_in_app(application_t& app, level_info_t& level_info);
void update(camera_t& camera, key_state_t& key_state, float& x_offset);