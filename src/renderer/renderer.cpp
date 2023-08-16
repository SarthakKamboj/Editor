#include "renderer.h"
#include <vector>
#include "SDL.h"
#include "renderer/graphics/buffers.h"
#include "constants.h"
#include <iostream>
#include "renderer/basic/light.h"
#include "renderer/basic/quad.h"
#include "backends/imgui_impl_opengl3.h"

// will add spritesheet renderers as well in the future
void render(application_t& app, camera_t& camera, editor_t& editor) {

	// 2d light map pass
	
	// TODO: lights are not blending together
	bind_framebuffer(app.light_map_fbo);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	float ambient = 0.1f;
	glClearColor(ambient, ambient, ambient, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);		
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	// enable blending where lights are added together
	glEnable(GL_BLEND);
	glBlendColor(1,1,1,1);
	glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_COLOR);

	render_lights(camera, ambient, 1.0f);

    // quads render pass
	bind_framebuffer(app.main_fbo);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_quads(camera, app);

	unbind_framebuffer();

	ImGuiIO& io = *editor.editor_settings.io;
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
}