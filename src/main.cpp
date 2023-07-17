#if 1

#include "input/input.h"
#include "SDL.h"
#include "glad/glad.h"
#include "app.h"
#include <iostream>
#include "constants.h"
#include "renderer/opengl/object_data.h"
#include "renderer/renderer.h"
#include "utils/time.h"
#include "editorItems/gridline.h"
#include <cmath>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "renderer/camera.h"
#include "editorItems/worldItem.h"
// #include "imgui-filebrowser/imfilebrowser.h"
#include "editorItems/addWorldItemModal.h"
#include "utils/conversion.h"

#define WINDOW_TITLE_BAR_HEIGHT 20

/*
Screen coordinates will always being (0,0) in the bottom left and (SCREEN_WIDTH, SCREEN_HEIGHT) in top right
*/

// TODO: seems like things stop rendering after 14 world items get placed

int debug_bottom_left_world_grid_tex = -1;

mouse_state_t mouse_state;
key_state_t key_state;
camera_t camera;

std::ostream& operator<< (std::ostream& stream, const ImVec2& vec) {
	stream << "x: " << vec.x << " y: " << vec.y;
	return stream;
}

int main(int argc, char** argv) {
	application_t app;

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

	glm::vec3 hover_color(0, 1, 1);
	int transform_handle = create_transform(glm::vec3(0.f), glm::vec3(1.f), 0.f);
	create_rectangle_render(transform_handle, hover_color, -1, GRID_SQUARE_WIDTH, GRID_SQUARE_WIDTH, false, 0);

	camera = create_camera();

    // scroll offset for the world grid editor 
	float x_offset = 0.f;

	glm::vec3 selected_color(1,0,0);
	int debug_transform_handle = create_transform(glm::vec3(0.f), glm::vec3(1.f), 0.f);
	debug_bottom_left_world_grid_tex = create_rectangle_render(debug_transform_handle, selected_color, -1, 10, 10, false, 0);

	while (app.running) {

        transform_t& transform = *get_transform(transform_handle);
	    transform_t& debug_transform = *get_transform(debug_transform_handle);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		float start = platformer::get_time_since_start_in_sec();
		process_input(mouse_state, key_state, app.window);	
		if (key_state.close_event_pressed) {
			app.running = false;
		}

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PopStyleVar(2);
		ImGuiIO& io = ImGui::GetIO();

        // for making the entire editor window dockable
		bool open = true;
		ImGui::Begin("DockSpace Demo", &open, window_flags);
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		else
		{
			std::cout << "imgui docking not enabled" << std::endl;
		}
		ImGui::End();

		ImGui::ShowDemoWindow();

		if (ImGui::BeginMainMenuBar()) {
			conversion::set_window_top_left_screen_coord();

			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Add World Item")) {
			        open_add_world_modal();
				}
                if (ImGui::MenuItem("Save World Items")) {
                    write_world_items_to_file();
                }
                if (ImGui::MenuItem("Save Level")) {
                    write_world_map_to_file();
                }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		update(camera, key_state, x_offset);

        // render the framebuffer texture from the render pass used to display the actual world grid
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGuiWindowFlags world_win_flags = ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin("World", nullptr, world_win_flags)) {
			ImVec2 size = ImGui::GetWindowSize();
			float x_ratio = size.x / WINDOW_WIDTH;
			float y_ratio = size.y / WINDOW_HEIGHT;
			float ratio = fmin(x_ratio, y_ratio);

			float tex_width = WINDOW_WIDTH * ratio;
			float tex_height = WINDOW_HEIGHT * ratio;

			ImGui::Image((void*)app.world_grid_fbo.framebuffer_texture, ImVec2(tex_width, tex_height), ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::IsWindowFocused()) {
                // grid hover logic
				glm::vec2 hovered_grid_square;

				glm::vec2 win_rel_mouse(0,0);

				ImVec2 region_min = ImGui::GetWindowContentRegionMin();
				ImVec2 region_max = ImGui::GetWindowContentRegionMax();

				ImVec2 top_left_window = conversion::get_window_pos();

			    // position of the actual world grid fbo texture bottom right
				ImVec2 bottom_left_world_grid_tex_pos;
				bottom_left_world_grid_tex_pos.x = top_left_window.x;
				bottom_left_world_grid_tex_pos.y = top_left_window.y - tex_height - WINDOW_TITLE_BAR_HEIGHT;

				debug_transform.position.x = bottom_left_world_grid_tex_pos.x;
				debug_transform.position.y = bottom_left_world_grid_tex_pos.y;
				debug_transform.position.z = 0;

				ImVec2 mouse_pos_rel_tex; 
				mouse_pos_rel_tex.x = mouse_state.x - bottom_left_world_grid_tex_pos.x;
				mouse_pos_rel_tex.y = mouse_state.y - bottom_left_world_grid_tex_pos.y;

				glm::vec2 expanded_pos(mouse_pos_rel_tex.x / ratio, mouse_pos_rel_tex.y / ratio);

				hovered_grid_square.x = floor((expanded_pos.x + x_offset) / GRID_SQUARE_WIDTH);
				hovered_grid_square.y = floor(expanded_pos.y / GRID_SQUARE_WIDTH);	

                // center pos of the hovered grid square
				transform.position.x = hovered_grid_square.x * GRID_SQUARE_WIDTH + GRID_SQUARE_WIDTH / 2;
				transform.position.y = hovered_grid_square.y * GRID_SQUARE_WIDTH + GRID_SQUARE_WIDTH / 2;

                // if grid square gets selected
				if (mouse_state.left_mouse_being_pressed) {	
                    place_world_item(world_item_t::selected_world_item_handle, hovered_grid_square);
				}

				// remove on right mouse press
                if (mouse_state.right_mouse_being_pressed) {
                    remove_placed_world_item(hovered_grid_square);
                }
			}
			ImGui::End();
		}
		ImGui::PopStyleVar();

		render(app, camera);
		float end = platformer::get_time_since_start_in_sec();
		platformer::time_t::delta_time = end - start;
	}
	return -1;
}
#else

#include <iostream>
#include "SDL.h"
#include "glad/glad.h"
#include "imgui.h"

int main(int argc, char* argv[]) {
    std::cout << argv[0] << std::endl;
}

#endif