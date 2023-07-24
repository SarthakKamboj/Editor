#include "app.h" 
#include "utils/time.h"
#include "constants.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <editor_items/add_world_item_modal.h>
#include <editor_items/world_item.h>
#include <cstdio>

void update(camera_t& camera, key_state_t& key_state, float& x_offset) {
	update_camera(camera, key_state);
	if (key_state.key_being_pressed['d']) {
		x_offset += CAMERA_SCROLL_SPEED * platformer::time_t::delta_time;
	} else if (key_state.key_being_pressed['a']) {
		x_offset -= CAMERA_SCROLL_SPEED * platformer::time_t::delta_time;
	} 

}

void set_level_in_app(application_t& app, level_info_t& level_info) {
    memcpy(app.cur_level.file_name, level_info.file_name, strlen(level_info.file_name));
    memcpy(app.cur_level.full_path, level_info.full_path, strlen(level_info.full_path));
    memcpy(app.cur_level.output_folder, level_info.output_folder, strlen(level_info.output_folder));
    init_placed_world_items(level_info.full_path);
    app.state = application_state::EDITOR;
}

void create_level_info(level_info_t& level_info) {
    char* last_slash = strrchr(level_info.full_path, '\\');
    memcpy(level_info.file_name, last_slash+1, strlen(last_slash+1));
    int num_chars_of_folder = last_slash - level_info.full_path;
    memcpy(level_info.output_folder, level_info.full_path, num_chars_of_folder);
}

void create_level_info(level_info_t& level_info, char* full_path) {
    char* last_slash = strrchr(full_path, '\\');
    memcpy(level_info.file_name, last_slash+1, strlen(last_slash+1));
    int num_chars_of_folder = last_slash - level_info.full_path;
    memcpy(level_info.output_folder, level_info.full_path, num_chars_of_folder);
    memcpy(level_info.full_path, full_path, strlen(full_path));
}