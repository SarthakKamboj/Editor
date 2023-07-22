#include "app.h" 
#include "utils/time.h"
#include "constants.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <editorItems/addWorldItemModal.h>
#include <editorItems/worldItem.h>

void update(camera_t& camera, key_state_t& key_state, float& x_offset) {
	update_camera(camera, key_state);
	if (key_state.key_being_pressed['d']) {
		x_offset += CAMERA_SCROLL_SPEED * platformer::time_t::delta_time;
	} else if (key_state.key_being_pressed['a']) {
		x_offset -= CAMERA_SCROLL_SPEED * platformer::time_t::delta_time;
	} 

#if ENABLE_IMGUI
	update_add_world_item_modal();
	update_world_item_catalog();
#endif
}
