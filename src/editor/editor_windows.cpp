#include "editor_windows.h"
#include <iostream>
#include "world_item.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "utils/conversion.h"
#include "constants.h"
#include "renderer/basic/quad.h"
#include "add_world_item_modal.h"

extern mouse_state_t* mouse_state_ptr;
static bool show_level_info_file = false;

void create_dockspace(editor_t& editor)
{
    ImGuiIO &io = *editor.editor_settings.io;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGui::PopStyleVar(2);
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
}

void create_menu_bar(editor_t& editor)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Add World Item"))
            {
                open_add_world_modal();
            }
            if (ImGui::MenuItem("Save World Items"))
            {
                write_world_items_to_file();
            }
            if (ImGui::MenuItem("Save Level"))
            {
                // write_world_map_to_file(app.editor_settings.cur_level);
                write_world_map_to_file(editor.cur_level);
            }
            if (ImGui::MenuItem("Level file info")) {
                show_level_info_file = true;
            }
            if (ImGui::MenuItem("Load New Level")) {
                unload_level_in_editor(editor);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void create_world_map_window(application_t &app, float x_offset)
{
    glm::vec3 hover_color(0, 1, 1);
    static int hovered_transform_handle = create_transform(glm::vec3(0.f), glm::vec3(1.f), 0.f);
    create_quad(hovered_transform_handle, hover_color, -1, GRID_SQUARE_WIDTH, GRID_SQUARE_WIDTH, false, 0);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGuiWindowFlags world_win_flags = ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("World", nullptr, world_win_flags))
    {
        ImVec2 size = ImGui::GetWindowSize();
        float x_ratio = size.x / WINDOW_WIDTH;
        float y_ratio = size.y / WINDOW_HEIGHT;
        float ratio = fmin(x_ratio, y_ratio);

        float tex_width = WINDOW_WIDTH * ratio;
        float tex_height = WINDOW_HEIGHT * ratio;

        ImGui::Image((void *)app.main_fbo.color_texture, ImVec2(tex_width, tex_height), ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::IsWindowFocused())
        {
            // grid hover logic
            glm::vec2 hovered_grid_square;

            glm::vec2 win_rel_mouse(0, 0);

            ImVec2 region_min = ImGui::GetWindowContentRegionMin();
            ImVec2 region_max = ImGui::GetWindowContentRegionMax();

            ImVec2 top_left_window = conversion::get_window_pos();

            // position of the actual world grid fbo texture bottom right
            ImVec2 bottom_left_world_grid_tex_pos;
            bottom_left_world_grid_tex_pos.x = top_left_window.x;
            bottom_left_world_grid_tex_pos.y = top_left_window.y - tex_height - WINDOW_TITLE_BAR_HEIGHT;

            ImVec2 mouse_pos_rel_tex;
            mouse_pos_rel_tex.x = mouse_state_ptr->x - bottom_left_world_grid_tex_pos.x;
            mouse_pos_rel_tex.y = mouse_state_ptr->y - bottom_left_world_grid_tex_pos.y;

            glm::vec2 expanded_pos(mouse_pos_rel_tex.x / ratio, mouse_pos_rel_tex.y / ratio);

            hovered_grid_square.x = floor((expanded_pos.x + x_offset) / GRID_SQUARE_WIDTH);
            hovered_grid_square.y = floor(expanded_pos.y / GRID_SQUARE_WIDTH);

            // center pos of the hovered grid square
            transform_t &hovered_transform = *get_transform(hovered_transform_handle);
            hovered_transform.position.x = hovered_grid_square.x * GRID_SQUARE_WIDTH + GRID_SQUARE_WIDTH / 2;
            hovered_transform.position.y = hovered_grid_square.y * GRID_SQUARE_WIDTH + GRID_SQUARE_WIDTH / 2;

            // if grid square gets selected
            if (mouse_state_ptr->left_mouse_being_pressed)
            {
                place_world_item(world_item_t::selected_world_item_handle, hovered_grid_square);
            }

            // remove on right mouse press
            if (mouse_state_ptr->right_mouse_being_pressed)
            {
                remove_placed_world_item(hovered_grid_square);
            }
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();
}

// void imgui_end_of_frame(ImGuiIO &io)
// {
//     if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//     {
//         SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
//         SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
//         ImGui::UpdatePlatformWindows();
//         ImGui::RenderPlatformWindowsDefault();
//         SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
//     }
// }

void create_level_info_window(editor_t &editor) {
    ImGui::Begin("Level Info");
    ImGui::Text(editor.cur_level.output_folder);
    ImGui::Text(editor.cur_level.file_name);
    ImGui::End();
}

void create_editor_windows(editor_t& editor, application_t &app, float x_offset)
{
    // ImGui::ShowDemoWindow();
    create_menu_bar(editor);
    // render the framebuffer texture from the render pass used to display the actual world grid
    create_world_map_window(app, x_offset);

    update_add_world_item_modal();
	create_world_item_catalog();
    create_placed_world_items_editor();

    if (show_level_info_file) {
        create_level_info_window(editor);
    }
}

