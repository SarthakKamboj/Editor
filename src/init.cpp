#include <string>
#include <stdexcept>
#include <iostream>
#include "glad/glad.h"
#include "constants.h"
#include "app.h"
#include "SDL.h"
#include "renderer/opengl/buffers.h"
#include "renderer/opengl/resources.h"
#include "renderer/opengl/vertex.h"
#include "renderer/basic/shape_renders.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <fstream>
#include <string>
#include <editor_items/world_item.h>
#include <map>

// TODO: add a screen listing all the possible level data files that exist

#define C_FILE_IO 1

void init_sdl(application_t& app) {
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

	SDL_Window* window = SDL_CreateWindow("window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	const char* glsl_version = "#version 410";
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	app.io = &io;

	app.window = window;
}

void init_rectangle_data() {
	opengl_object_data& data = rectangle_render_t::obj_data;

	vertex_t vertices[4];
    vertices[0] = create_vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0,1,1), glm::vec2(1,1)); // top right
	vertices[1] = create_vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0,0,1), glm::vec2(1,0)); // bottom right
	vertices[2] = create_vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0,1,0), glm::vec2(0,0)); // bottom left
	vertices[3] = create_vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1,0,0), glm::vec2(0,1)); // top left

	data.vbo = create_vbo((float*)vertices, sizeof(vertices));

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	data.ebo = create_ebo(indices, sizeof(indices));
	data.vao = create_vao();

	bind_vao(data.vao);
	vao_enable_attribute(data.vao, data.vbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, position));
	vao_enable_attribute(data.vao, data.vbo, 1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, color));
	vao_enable_attribute(data.vao, data.vbo, 2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, tex_coord));
	bind_ebo(data.ebo);
	unbind_vao();
	unbind_ebo();

	data.shader = create_shader((SHADERS_PATH + "\\rectangle.vert").c_str(), (SHADERS_PATH + "\\rectangle.frag").c_str());
	shader_set_mat4(data.shader, "model", glm::mat4(1));
	glm::mat4 projection = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);
	shader_set_mat4(data.shader, "projection", projection);
    shader_set_int(data.shader, "tex", 0);
}

void init_fbo_draw_data(application_t& app) {
	opengl_object_data& data = app.fbo_draw_data;

	vertex_t vertices[4];
	float val = 1.f;
	vertices[0] = create_vertex(glm::vec3(val, val, 0.0f), glm::vec3(0,1,1), glm::vec2(1.0f, 1.f)); // top right
	vertices[1] = create_vertex(glm::vec3(val, -val, 0.0f), glm::vec3(0,0,1), glm::vec2(1.0f, 0.f)); // bottom right
	vertices[2] = create_vertex(glm::vec3(-val, -val, 0.0f), glm::vec3(0,1,0), glm::vec2(0.f, 0.f)); // bottom left
	vertices[3] = create_vertex(glm::vec3(-val, val, 0.0f), glm::vec3(1,0,0), glm::vec2(0.f, 1.0f)); // top left

	data.vbo = create_vbo((float*)vertices, sizeof(vertices));

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	data.ebo = create_ebo(indices, sizeof(indices));
	data.vao = create_vao();

	bind_vao(data.vao);
	vao_enable_attribute(data.vao, data.vbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, position));
	vao_enable_attribute(data.vao, data.vbo, 1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, tex_coord));
	bind_ebo(data.ebo);
	unbind_vao();
	unbind_ebo();

	data.shader = create_shader((SHADERS_PATH + "\\fbo.vert").c_str(), (SHADERS_PATH + "\\fbo.frag").c_str());
	shader_set_mat4(data.shader, "model", glm::mat4(1));
	glm::mat4 projection = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);
	shader_set_mat4(data.shader, "projection", projection);
	transform_t transform;
	transform.position.x = WINDOW_WIDTH / 2;
	transform.position.y = WINDOW_HEIGHT / 2;
	glm::mat4 model_matrix = get_model_matrix(transform);
	shader_set_mat4(data.shader, "model", model_matrix);
	shader_set_int(data.shader, "fbo_texture", 0);
}

void init_world_items() {
    // const char* file_path = "C:\\Sarthak\\projects\\Platformer\\Editor\\world_items.txt";
    const char* file_path = "C:\\Sarthak\\projects\\editor\\build\\world_items.txt";
    FILE* file;
    file = fopen(file_path, "r");
    std::string delim(WORLD_ITEM_TEXT_FILE_DELIM);
	size_t delim_len = delim.size();
    static std::string world_item_format = "%1023s " + delim + " %1023s " + delim + " %i " + delim + " %i\n";
    static const char* world_item_format_char = world_item_format.c_str();
    if (file) {
        while (!feof(file)) {
            char name[1024]{};
            char path[1024]{};
            int width = 0;
            int height = 0;
            fscanf(file, world_item_format_char, name, path, &width, &height);
            std::string name_str(name);
            create_world_item(path, width, height, name_str);
        }
        // file.close();
        fclose(file);
    } else {
        std::cout << "could not open world items file" << std::endl;
    }
}

void init_placed_world_items() {
    // const char* file_path = "C:\\Sarthak\\projects\\Platformer\\Editor\\level1.txt";
    const char* file_path = "C:\\Sarthak\\projects\\editor\\build\\level10.gme";
    FILE* file;
    file = fopen(file_path, "r");
	size_t delim_len = std::string(WORLD_ITEM_TEXT_FILE_DELIM).size();
    std::map<int, int> idx_to_handle_map;
    int i = 0;
    if (file) {
        bool placed_items_section = false;
        char line[1024];
        while (!feof(file)) {
            memset(line, 0, 1024);
            fgets(line, 1024, file);
            if (strcmp(line, "") == 0) continue;
            if (strcmp(line, "\n") == 0) continue;
            if (strcmp(line, "WORLD_ITEMS\n") == 0) continue;
			// if (!placed_items_section && (strcmp(line, "\n") == 0)) continue;
            if (!placed_items_section && (strcmp(line, "PLACED_ITEMS\n") != 0)) {
                std::string delim(WORLD_ITEM_TEXT_FILE_DELIM);
                size_t delim_len = delim.size();
                static std::string world_item_format = "%1023s " + delim + " %1023s " + delim + " %i " + delim + " %i\n";
                static const char* world_item_format_char = world_item_format.c_str();

                char name[1024]{};
                char path[1024]{};
                int width = 0;
                int height = 0;
                sscanf(line, world_item_format_char, name, path, &width, &height);
                std::string name_str(name);

                int handle = get_world_item_handle(path, width, height);
                if (handle == -1) {
                    std::string title("prev item");
                    idx_to_handle_map[i] = create_world_item(path, width, height, name_str);
                } else {
                    idx_to_handle_map[i] = handle;
                }
				i++;
                continue;
            }
            if (strcmp(line, "PLACED_ITEMS\n") == 0) {
                placed_items_section = true;
                continue;
            }
			if ((strcmp(line, "\n") == 0) && placed_items_section) {
				break;
			}

            std::string delim(WORLD_ITEM_TEXT_FILE_DELIM);
            size_t delim_len = delim.size();
            static std::string placed_item_format = "%i " + delim + " %i " + delim + " %i\n";
            static const char* placed_item_format_char = placed_item_format.c_str();

            int idx = -1;
            int x = -1;
            int y = -1;
            sscanf(line, placed_item_format_char, &idx, &x, &y);

            int handle = idx_to_handle_map[idx];
            
            glm::vec2 grid_pos(x, y);
            place_world_item(handle, grid_pos);
        }
        fclose(file);
    } else {
        std::cout << "could not open world items file" << std::endl;
    }
}

void init(application_t& app) {
	init_sdl(app);
	app.running = true;
    const char* path = "C:\\Sarthak\\projects\\editor\\build";
    memcpy(app.cur_level.output_folder, path, strlen(path));
    app.cur_level.level_num = 10;
	init_rectangle_data();
    // used for separate render pass to render the actual world items in the world map
	init_fbo_draw_data(app);
	app.world_grid_fbo = create_framebuffer();
    init_world_items();
    init_placed_world_items();

	// int transform_handle = create_transform(glm::vec3(400, 400, 0), glm::vec3(0), 0);
	// glm::vec3 color(1, 0, 0);
	// app.debug_rec_handle = create_rectangle_render(transform_handle, color, 10, 10, false);
}