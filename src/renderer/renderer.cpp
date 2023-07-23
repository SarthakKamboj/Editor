#include "renderer.h"
#include <vector>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "SDL.h"
#include "renderer/opengl/buffers.h"
#include "constants.h"
#include <iostream>
#include <editor_items/add_world_item_modal.h>
#include <editor_items/world_item.h>

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

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());	

	
}