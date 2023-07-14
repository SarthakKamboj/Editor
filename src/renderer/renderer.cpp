#include "renderer.h"
#include <vector>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "SDL.h"
#include "editorItems/worldItem.h"
#include "renderer/opengl/buffers.h"
#include "constants.h"
#include <iostream>
#include "editorItems/addWorldItemModal.h"

// will add spritesheet renderers as well in the future
extern int debug_bottom_left_world_grid_tex;

void render(application_t& app, camera_t& camera) {

    // world grid render pass
	bind_framebuffer(app.world_grid_fbo);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_rectangle_renders(camera);

	unbind_framebuffer();

	glClearColor(0.f, 1.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	bind_vao(app.fbo_draw_data.vao);
	bind_shader(app.fbo_draw_data.shader);

	bind_texture_by_id(app.world_grid_fbo.framebuffer_texture);
	draw_obj(app.fbo_draw_data);
	*/

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());	

	draw_rectangle_render(debug_bottom_left_world_grid_tex);

	ImGuiIO& io = *app.io;
	if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	SDL_GL_SwapWindow(app.window);
}