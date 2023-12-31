#include "world_item.h"
#include <iostream>
#include <vector>
#include "imgui.h"
#include <string>
#include <fstream>
#include "transform/transform.h"
#include "renderer/graphics/render_resources.h"
#include "renderer/basic/quad.h"
#include "constants.h"
#include "input/input.h"
#include <stdexcept>
#include <map>

// TODO: search for memory leaks

// TODO: need separate scrolling logic and logic for world map render pass so that the world map render
// get info like it takes the whole screen

std::vector<world_item_t> world_items;
std::vector<placed_world_item_t> placed_items;

int world_item_t::selected_world_item_handle = world_item_t::NONE_SELECTED;

// void init_world_items() {
//     const char* file_path = "C:\\Sarthak\\projects\\editor\\build\\level10.gme";
//     FILE* file;
//     file = fopen(file_path, "r");
//     std::string delim(WORLD_ITEM_TEXT_FILE_DELIM);
// 	size_t delim_len = delim.size();
//     static std::string world_item_format = "%1023s" + delim + "%1023s" + delim + "%i" + delim + "%i\n";
//     static const char* world_item_format_char = world_item_format.c_str();
//     if (file) {
//         while (!feof(file)) {
//             char name[1024]{};
//             char path[1024]{};
//             int width = 0;
//             int height = 0;
//             fscanf(file, world_item_format_char, name, path, &width, &height);
//             std::string name_str(name);
//             create_world_item(path, width, height, name_str);
//         }
//         // file.close();
//         fclose(file);
//     } else {
//         std::cout << "could not open world items file" << std::endl;
//     }
// }

int create_world_item(const char *path, int squares_width, int squares_height, std::string &name)
{
    int existing_handle = get_world_item_handle(path, squares_width, squares_height);
    if (existing_handle != -1)
    {
        return existing_handle;
    }

    static int running_count = 0;
    world_item_t world_item;
    world_item.texture_handle = create_texture(path);
    world_item.world_item_name = name;
    world_item.grid_squares_width = squares_width;
    world_item.grid_squares_height = squares_height;
    world_item.handle = running_count;
    world_items.push_back(world_item);
    running_count++;
    world_item_t::selected_world_item_handle = world_item.handle;
    return world_item.handle;
}

int get_world_item_handle(const char *path, int squares_width, int squares_height)
{
    for (world_item_t &world_item : world_items)
    {
        texture_t *ptr = get_texture(world_item.texture_handle);
        assert(ptr != NULL);
        texture_t &texture = *ptr;
        if (texture.path == std::string(path) && squares_width == world_item.grid_squares_width && squares_height == world_item.grid_squares_height)
        {
            return world_item.handle;
        }
    }
    return -1;
}

void clear_world_items() {
    world_item_t::selected_world_item_handle = world_item_t::NONE_SELECTED;
    for (int i = 0; i < world_items.size(); i++) {
        remove_world_item(world_items[i].handle);
    }
}

world_item_t *get_world_item(int world_handle)
{
    for (world_item_t &world_item : world_items)
    {
        if (world_item.handle == world_handle)
        {
            return &world_item;
        }
    }
    return NULL;
}

void remove_world_item(int world_handle)
{
    int idx_to_remove = -1;
    for (int i = 0; i < world_items.size(); i++)
    {
        if (world_items[i].handle == world_handle)
        {
            idx_to_remove = i;
            break;
        }
    }

    assert(idx_to_remove != -1);

    for (int i = 0; i < placed_items.size(); i++)
    {
        if (placed_items[i].world_item_handle == world_handle)
        {
            remove_placed_world_item(placed_items[i].handle);
            i--;
        }
    }

    world_items.erase(world_items.begin() + idx_to_remove, world_items.begin() + idx_to_remove + 1);
    if (world_item_t::selected_world_item_handle == world_handle)
    {
        world_item_t::selected_world_item_handle = world_item_t::NONE_SELECTED;
    }
}

int place_world_item(int world_item_handle, const glm::vec2 &bottom_left_grid_square_pos)
{
    if (world_item_handle == world_item_t::NONE_SELECTED)
    {
        return -1;
    }

    for (const placed_world_item_t &placed_item : placed_items)
    {
        if (placed_item.world_item_handle == world_item_handle &&
            placed_item.bottom_left_grid_square_pos == bottom_left_grid_square_pos)
        {
            return -1;
        }
    }

    static int running_count = 0;

    placed_world_item_t placed_world_item;
    placed_world_item.world_item_handle = world_item_handle;
    placed_world_item.bottom_left_grid_square_pos = bottom_left_grid_square_pos;
    placed_world_item.handle = running_count;
    running_count++;

    world_item_t *world_item_ptr = get_world_item(world_item_handle);
    assert(world_item_ptr != NULL);
    world_item_t &world_item = *world_item_ptr;
    glm::vec2 center_grid_square;
    center_grid_square.x = bottom_left_grid_square_pos.x + (world_item.grid_squares_width / 2.f);
    center_grid_square.y = bottom_left_grid_square_pos.y + (world_item.grid_squares_height / 2.f);

    glm::vec3 position(1.f);
    position.x = center_grid_square.x * GRID_SQUARE_WIDTH;
    position.y = center_grid_square.y * GRID_SQUARE_WIDTH;

    int transform_handle = create_transform(position, glm::vec3(1), 0);
    placed_world_item.transform_handle = transform_handle;
    glm::vec3 color(1);
    placed_world_item.rec_render_handle = create_quad(transform_handle, color,
                                                        world_item.texture_handle,
                                                        world_item.grid_squares_width * GRID_SQUARE_WIDTH,
                                                        world_item.grid_squares_height * GRID_SQUARE_WIDTH, false, 1.0f);
    placed_items.push_back(placed_world_item);
    return placed_world_item.handle;
}

placed_world_item_t *get_placed_world_item(int placed_handle)
{
    for (placed_world_item_t &placed_item : placed_items)
    {
        if (placed_handle == placed_item.handle)
        {
            return &placed_item;
        }
    }
    return NULL;
}

void remove_placed_world_item(glm::vec2 grid_square_pos)
{
    for (int i = 0; i < placed_items.size(); i++)
    {
        const placed_world_item_t &placed_item = placed_items[i];
        if (placed_item.bottom_left_grid_square_pos.x == grid_square_pos.x &&
            placed_item.bottom_left_grid_square_pos.y == grid_square_pos.y)
        {
            remove_quad(placed_item.rec_render_handle);
            placed_items.erase(placed_items.begin() + i, placed_items.begin() + i + 1);
            i--;
        }
    }
}

void remove_placed_world_item(int placed_handle)
{
    int idx_to_remove = 0;
    for (int i = 0; i < placed_items.size(); i++)
    {
        const placed_world_item_t &placed_item = placed_items[i];
        if (placed_item.handle == placed_handle)
        {
            remove_quad(placed_item.rec_render_handle);
            idx_to_remove = i;
        }
    }
    placed_items.erase(placed_items.begin() + idx_to_remove, placed_items.begin() + idx_to_remove + 1);
}

void clear_placed_items() {
    for (int i = 0; i < placed_items.size(); i++) {
        remove_placed_world_item(placed_items[i].handle);
    }
}

// EDITOR FUNCTIONS

void create_world_item_catalog()
{
    const float image_height = 50.f;
    ImGui::Begin("World Items");
    std::string select_handle_text = "Selected Handle: " + std::to_string(world_item_t::selected_world_item_handle);
    ImGui::Text(select_handle_text.c_str());
    for (int i = 0; i < world_items.size(); i++)
    {
        // push id because the titles are used as IDs and if within the same window, multiple items have the same id, it can cause issues
        // push id helps solve that by ensuring unique ids
        // https://github.com/ocornut/imgui/issues/74
        ImGui::PushID(i);
        world_item_t &item = world_items[i];
        texture_t *tex_ptr = get_texture(item.texture_handle);
        assert(tex_ptr != NULL);
        texture_t &texture = *tex_ptr;
        float ratio = 50.f / texture.height;
        // ? uv0 = top left uv coordinate and uv1 = bottom right uv coordinate
        ImGui::Image((void *)texture.id, ImVec2(texture.width * ratio, texture.height * ratio), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Text(item.world_item_name.c_str());
        std::string width_text = "Width: " + std::to_string(item.grid_squares_width);
        ImGui::Text(width_text.c_str());
        std::string height_text = "Height: " + std::to_string(item.grid_squares_height);
        ImGui::Text(height_text.c_str());
        std::string handle_text = "Handle: " + std::to_string(item.handle);
        ImGui::Text(handle_text.c_str());
        if (world_item_t::selected_world_item_handle != item.handle)
        {
            bool clicked_on_select = ImGui::Button("Select Item");
            if (clicked_on_select)
            {
                world_item_t::selected_world_item_handle = item.handle;
            }
        }
        else
        {
            ImGui::Button("Currently Selected");
        }
        if (ImGui::Button("Remove World Item"))
        {
            remove_world_item(item.handle);
        }
        ImGui::PopID();
    }
    ImGui::End();
}

void create_placed_world_items_editor()
{
    ImGui::Begin("Placed World Items");
    for (int i = 0; i < placed_items.size(); i++)
    {
        // push id because the titles are used as IDs and if within the same window, multiple items have the same id, it can cause issues
        // push id helps solve that by ensuring unique ids
        // https://github.com/ocornut/imgui/issues/74
        ImGui::PushID(i);
        placed_world_item_t &placed_item = placed_items[i];
        world_item_t *world_item_ptr = get_world_item(placed_item.world_item_handle);
        assert(world_item_ptr);
        world_item_t &world_item = *world_item_ptr;
        if (ImGui::CollapsingHeader(world_item.world_item_name.c_str())) {
            std::string x_text = "X: " + std::to_string(placed_item.bottom_left_grid_square_pos.x);
            ImGui::Text(x_text.c_str());
            std::string y_text = "Y: " + std::to_string(placed_item.bottom_left_grid_square_pos.y);
            ImGui::Text(y_text.c_str());
            if (ImGui::Button("Delete")) {
                remove_placed_world_item(placed_item.handle);
            }
        }
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::End();
}