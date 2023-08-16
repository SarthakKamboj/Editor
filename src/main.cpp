#include "SDL.h"
#include "input/input.h"
#include "glad/glad.h"
#include "app.h"
#include <iostream>
#include "constants.h"
#include "renderer/graphics/mesh.h"
#include "renderer/renderer.h"
#include "utils/time.h"
#include <cmath>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "renderer/camera.h"
#include "utils/conversion.h"
#include <editor/add_world_item_modal.h>
#include <editor/world_item.h>
#include "editor/gridline.h"
#include "editor/editor_windows.h"
#include "editor/loader.h"
#include "editor/editor.h"
#include "renderer/basic/light.h"

/*
Screen coordinates will always being (0,0) in the bottom left and (SCREEN_WIDTH, SCREEN_HEIGHT) in top right
*/

application_t application;
mouse_state_t* mouse_state_ptr;
editor_t* editor_ptr;

// std::ostream& operator<< (std::ostream& stream, const ImVec2& vec) {
// 	stream << "x: " << vec.x << " y: " << vec.y;
// 	return stream;
// }

int main(int argc, char** argv) {

	application = init_app(WINDOW_WIDTH, WINDOW_HEIGHT);
	mouse_state_ptr = &application.mouse_state;

	editor_t editor = init_editor(application);	
	editor_ptr = &editor;

    // information for the world grid
	const int NUM_COLS = ceil(2*WINDOW_WIDTH / GRID_SQUARE_WIDTH);
	for (int col = 0; col < NUM_COLS; col++) {
		create_gridline(col * GRID_SQUARE_WIDTH, WINDOW_HEIGHT / 2.f, dir_t::COL);
	}

	const int NUM_ROWS = ceil(2*WINDOW_HEIGHT / GRID_SQUARE_WIDTH);
	for (int row = 0; row < NUM_ROWS; row++) {
		create_gridline(WINDOW_WIDTH, row * GRID_SQUARE_WIDTH, dir_t::ROW);
	}
	
	camera_t camera = create_camera();

    // scroll offset for the world grid editor 
	float x_offset = 0.f;

	// create_light(glm::vec3(100 - (WINDOW_WIDTH/2), WINDOW_HEIGHT/2, 0.25f), glm::vec3(1,0,0), 200.f);
	// create_light(glm::vec3((WINDOW_WIDTH/2) - 250, WINDOW_HEIGHT/2, 0.5f), glm::vec3(1,0,0), 300.f);
	create_light(glm::vec3((WINDOW_WIDTH/2), WINDOW_HEIGHT/2, 0.5f), glm::vec3(0.5f), 1700.f);
	// create_light(glm::vec3(0.f, WINDOW_HEIGHT/2, 0), glm::vec3(0,1,0), 1000.f);
	create_light(glm::vec3(100 + (WINDOW_WIDTH/2), WINDOW_HEIGHT/2, 0), glm::vec3(1,1,0), 500.f);

	while (application.running) {
        
		start_of_frame(application);
		editor_new_frame();

        // for making the entire editor window dockable
    	create_dockspace(editor);
        if (editor.editor_settings.state == editor_state::EDITOR_SCREEN) {
            create_editor_windows(editor, application, x_offset) ;
        } else {
            create_load_screen_window(editor);
        }

		update_app(camera, application.key_state, x_offset);
		render(application, camera, editor);

        editor_end_of_frame(editor);
		end_of_frame(application);

		std::cout << "fps: " << (1/engine_time_t::delta_time) << std::endl;
	}

	return -1;
}
