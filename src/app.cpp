#include "app.h" 
#include "utils/time.h"
#include "constants.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <editor/add_world_item_modal.h>
#include <editor/world_item.h>
#include <cstdio>

void update_app(camera_t& camera, key_state_t& key_state, float& x_offset) {
	update_camera(camera, key_state);
	if (key_state.key_being_pressed['d']) {
		x_offset += CAMERA_SCROLL_SPEED * engine_time_t::delta_time;
	} else if (key_state.key_being_pressed['a']) {
		x_offset -= CAMERA_SCROLL_SPEED * engine_time_t::delta_time;
	} 
}


