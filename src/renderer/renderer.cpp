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

	glGetError();

	// 2d light map pass
	
	// TODO: lights are not blending together
	bind_framebuffer(app.light_map_fbo);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	// glEnable(GL_BLEND);
	// glBlendColor(0,0,0,0.0f);
	// glBlendFunc(GL_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	float ambient = 0.1f;
	glClearColor(ambient, ambient, ambient, 1.f);
	glClearStencil(0);
	glStencilMask(0xff);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);		

	// first stencil fill pass to know how many lights located at a certain pixel
	// glStencilFunc(GL_ALWAYS, 1, 0xff);
	// glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
	// render_light_stencils(camera);	
	// // clear out color buffer again since any rendering done in the first part of the light pass is pointless
	// glClear(GL_COLOR_BUFFER_BIT);	

	// disable stencil writing, but still need to use stencil buffer for reading
	// glStencilMask(0x00);

	// 2nd part of light pass where we render lights/parts of lights that only show up against the background
	
	// glDisable(GL_BLEND);
	// glStencilFunc(GL_EQUAL, 0, 0xff);
	// glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
	// render_lights(camera, ambient, 1.0f);

	// 3rd part of light pass where we render lights/parts of lights that mix with other lights
	// basically there is more than one light at these pixels
	// glStencilFunc(GL_NOTEQUAL, 0, 0xff);
	glEnable(GL_BLEND);
	// glBlendColor(0,0,0,1.0f);
	glBlendColor(1,1,1,1);
	// glBlendFunc(GL_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);
	glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_COLOR);
	render_lights(camera, ambient, 1.0f);

	glDisable(GL_STENCIL_TEST);

    // quads render pass
	glEnable(GL_DEPTH_TEST);
	bind_framebuffer(app.main_fbo);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render_quads(camera, app);

	unbind_framebuffer();

	ImGuiIO& io = *editor.editor_settings.io;
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
}