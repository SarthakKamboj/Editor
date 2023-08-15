#include <string>
#include <stdexcept>
#include <iostream>
#include "glad/glad.h"
#include "constants.h"
#include "app.h"
#include "SDL.h"
#include "renderer/graphics/buffers.h"
#include "renderer/graphics/render_resources.h"
#include "renderer/graphics/vertex.h"
#include "renderer/basic/quad.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "utils/time.h"
#include <fstream>
#include <string>
#include "editor/world_item.h"
#include <map>
#include <vector>
#include "renderer/basic/light.h"

// TODO: add a screen listing all the possible level data files that exist

void init_sdl(application_t& app, float window_width, float window_height) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		const char* sdl_error = SDL_GetError();
		std::string error_msg = "SDL could not be initialized: " + std::string(sdl_error);
		throw std::runtime_error(error_msg);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_Window* window = SDL_CreateWindow("window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	if (window == NULL) {
		const char* sdl_error = SDL_GetError();
		std::string error_msg = "SDL could not be initialized: " + std::string(sdl_error);
		throw std::runtime_error(error_msg);
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	SDL_GL_MakeCurrent(window, context);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
 
	app.window.sdl_window = window;
    app.window.width = window_width;
    app.window.height = window_height;
    app.window.context = context;

    // init_editor(app);
}


application_t init_app(int window_width, int window_height) {
	application_t app{};
	app.running = true;
	init_sdl(app, window_width, window_height);
    // load_level_files();
	init_quad_data();
	init_light_data(app);
	app.light_map_fbo = create_framebuffer();
	app.main_fbo = create_framebuffer();
	return app;
}

static float frame_start_time = 0.f;
void start_of_frame(application_t& app) {
	frame_start_time = get_time_since_start_in_sec();
	process_input(app.mouse_state, app.key_state, app.window);	
	if (app.key_state.close_event_pressed) {
		app.running = false;
	}
}

void end_of_frame(application_t& app) {
    SDL_GL_SwapWindow(app.window.sdl_window);
	float frame_end_time = get_time_since_start_in_sec();
	engine_time_t::delta_time = frame_end_time - frame_start_time;
}