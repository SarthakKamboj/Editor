#include "input/input.h"
#include "SDL.h"
#include "glad/glad.h"
#include "app.h"
#include <iostream>
#include "constants.h"
#include "renderer/opengl/object_data.h"
#include "renderer/renderer.h"
#include "utils/time.h"
#include <cmath>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "renderer/camera.h"
#include "utils/conversion.h"
#include <editor_items/add_world_item_modal.h>
#include <editor_items/world_item.h>
#include "editor_items/gridline.h"
#include "editor_items/editor_items.h"
#include "editor_items/loader.h"


/*
Screen coordinates will always being (0,0) in the bottom left and (SCREEN_WIDTH, SCREEN_HEIGHT) in top right
*/

// TODO: seems like things stop rendering after 14 world items get placed

int debug_bottom_left_world_grid_tex = -1;

mouse_state_t mouse_state;
key_state_t key_state;
camera_t camera;
application_t app;

std::ostream& operator<< (std::ostream& stream, const ImVec2& vec) {
	stream << "x: " << vec.x << " y: " << vec.y;
	return stream;
}

int main(int argc, char** argv) {

	init(app);

    // information for the world grid
	const int NUM_COLS = ceil(2*WINDOW_WIDTH / GRID_SQUARE_WIDTH);
	for (int col = 0; col < NUM_COLS; col++) {
		create_gridline(col * GRID_SQUARE_WIDTH, WINDOW_HEIGHT / 2.f, dir_t::COL);
	}

	const int NUM_ROWS = ceil(2*WINDOW_HEIGHT / GRID_SQUARE_WIDTH);
	for (int row = 0; row < NUM_ROWS; row++) {
		create_gridline(WINDOW_WIDTH, row * GRID_SQUARE_WIDTH, dir_t::ROW);
	}
	
	camera = create_camera();

    // scroll offset for the world grid editor 
	float x_offset = 0.f;

	while (app.running) {

        imgui_new_frame();
		ImGuiIO& io = ImGui::GetIO();

		float start = platformer::get_time_since_start_in_sec();
		process_input(mouse_state, key_state, app.window);	
		if (key_state.close_event_pressed) {
			app.running = false;
		}

        // for making the entire editor window dockable
        create_dockspace(io);
        if (app.state == application_state::EDITOR) {
            create_editor_windows(io, app, x_offset) ;
        } else {
            loader(app);
        }

		update(camera, key_state, x_offset);
		render(app, camera);

        imgui_end_of_frame(io);

        SDL_GL_SwapWindow(app.window);

		float end = platformer::get_time_since_start_in_sec();
		platformer::time_t::delta_time = end - start;
	}

	return -1;
}
