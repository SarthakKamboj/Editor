#include "editor.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "utils/conversion.h"
#include "SDL.h"
#include "constants.h"
#include "world_item.h"

#if EDITOR_MODE
editor_t init_editor(application_t& app) {
    editor_t editor;

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
	ImGui_ImplSDL2_InitForOpenGL(app.window.sdl_window, app.window.context);
	ImGui_ImplOpenGL3_Init(glsl_version);

 	editor.editor_settings.io = &io;
    return editor;
}

void editor_new_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    conversion::set_window_top_left_screen_coord();
}

void editor_end_of_frame(editor_t& editor) {
    ImGuiIO* io_ptr = editor.editor_settings.io;
    assert(io_ptr != NULL);
    ImGuiIO io = *io_ptr;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }
}

void load_level_in_editor(editor_t& editor, level_info_t& level_info) {
    memcpy(editor.cur_level.file_name, level_info.file_name, strlen(level_info.file_name));
    memcpy(editor.cur_level.full_path, level_info.full_path, strlen(level_info.full_path));
    memcpy(editor.cur_level.output_folder, level_info.output_folder, strlen(level_info.output_folder));
    init_placed_world_items(level_info.full_path);
    editor.editor_settings.state = editor_state::EDITOR_SCREEN;
}

void unload_level_in_editor(editor_t& editor) {
    clear_world_items();
    clear_placed_items();
    memset(&editor.cur_level, 0, sizeof(level_info_t));
    editor.editor_settings.state = editor_state::LOAD_SCREEN;
}

#else

editor_t init_editor(application_t& app) {}
void editor_new_frame() {}
void editor_end_of_frame(ImGuiIO &io){}

#endif