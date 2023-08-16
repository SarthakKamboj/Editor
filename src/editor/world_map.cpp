#include "world_map.h"
#include <cstdio>
#include <map>
#include <cassert>
#include "world_item.h"
#include "renderer/graphics/render_resources.h"
#include <stdexcept>
#include "renderer/basic/light.h"
#include <iostream>

// void write_world_items_to_file()
// {
//     FILE *out_file;
//     out_file = fopen("world_items.txt", "w");
//     if (out_file)
//     {
//         for (world_item_t &world_item : world_items)
//         {
//             texture_t &tex = *get_texture(world_item.texture_handle);
//             fprintf(out_file, "%s%s%s%s%i%s%i\n", world_item.world_item_name.c_str(), WORLD_ITEM_TEXT_FILE_DELIM, tex.path.c_str(), WORLD_ITEM_TEXT_FILE_DELIM, world_item.grid_squares_width, WORLD_ITEM_TEXT_FILE_DELIM, world_item.grid_squares_height);
//         }
//         fclose(out_file);
//     }
//     else
//     {
//         throw std::runtime_error("could not open world items file");
//     }
// }

extern std::vector<world_item_t> world_items;
extern std::vector<placed_world_item_t> placed_items;
extern std::vector<light_t> lights;

void write_world_map_to_file(level_info_t& level_info)
{
    if (level_info.output_folder[0] == 0) {
        return;
    }
    if(level_info.file_name[0] == 0) {
        return;
    }

    FILE *out_file;
    out_file = fopen(level_info.full_path, "w");
    if (out_file)
    {
        std::map<int, int> handle_to_idx_map;
        fprintf(out_file, "WORLD_ITEMS\n");
        for (int i = 0; i < world_items.size(); i++)
        {
            world_item_t &world_item = world_items[i];
            texture_t *ptr = get_texture(world_item.texture_handle);
            assert(ptr != NULL);
            texture_t &tex = *ptr;
            fprintf(out_file, "%s%s%s%s%i%s%i\n", world_item.world_item_name.c_str(), WORLD_ITEM_TEXT_FILE_DELIM, tex.path.c_str(), WORLD_ITEM_TEXT_FILE_DELIM, world_item.grid_squares_width, WORLD_ITEM_TEXT_FILE_DELIM, world_item.grid_squares_height);
            handle_to_idx_map[world_item.handle] = i;
        }

        fprintf(out_file, "PLACED_ITEMS\n");
        for (placed_world_item_t &placed_item : placed_items)
        {
            fprintf(out_file, "%i%s%i%s%i\n", handle_to_idx_map[placed_item.world_item_handle], WORLD_ITEM_TEXT_FILE_DELIM, (int)placed_item.bottom_left_grid_square_pos.x, WORLD_ITEM_TEXT_FILE_DELIM, (int)placed_item.bottom_left_grid_square_pos.y);
        }

        fprintf(out_file, "LIGHTS\n");
        for (light_t& light : lights) {
            fprintf(out_file, "%f%s%f%s%f%s%f%s%f%s%f%s%f\n", light.color.r, WORLD_ITEM_TEXT_FILE_DELIM, light.color.g, WORLD_ITEM_TEXT_FILE_DELIM, light.color.b, WORLD_ITEM_TEXT_FILE_DELIM,
                light.world_pos.x, WORLD_ITEM_TEXT_FILE_DELIM, light.world_pos.y, WORLD_ITEM_TEXT_FILE_DELIM,
                light.radius, WORLD_ITEM_TEXT_FILE_DELIM, light.intensity
            );
        }

        fclose(out_file);
    }
    else
    {
        throw std::runtime_error("could not open world items file");
    }
}

void read_world_map_file(const char* file_path) {
    FILE* file;
    file = fopen(file_path, "r");
	size_t delim_len = std::string(WORLD_ITEM_TEXT_FILE_DELIM).size();
    std::map<int, int> idx_to_handle_map;
    if (file) {
        // bool placed_items_section = false;
        char line[1024];
        world_map_section section = world_map_section::WORLD_MAP_NONE;
        int world_item_idx = 0;
        while (!feof(file)) {
            memset(line, 0, 1024);
            fgets(line, 1024, file);
            if (strcmp(line, "") == 0 || strcmp(line, "\n") == 0) {
                continue;
            }
            if (strcmp(line, "WORLD_ITEMS\n") == 0) {
                section = world_map_section::WORLD_ITEMS;
                continue;
            }
            if (strcmp(line, "PLACED_ITEMS\n") == 0) {
                section = world_map_section::PLACED_WORLD_ITEMS;
                continue;
            }
            if (strcmp(line, "LIGHTS\n") == 0) {
                section = world_map_section::LIGHTS;
                continue;
            }

            // if (!placed_items_section && (strcmp(line, "PLACED_ITEMS\n") != 0)) {
            if (section == world_map_section::WORLD_ITEMS) {
                std::string delim(WORLD_ITEM_TEXT_FILE_DELIM);
                size_t delim_len = delim.size();
                static std::string world_item_format = "%1023s" + delim + "%1023s" + delim + "%i" + delim + "%i\n";
                static const char* world_item_format_char = world_item_format.c_str();

                char name[1024]{};
                char path[1024]{};
                int width = 0;
                int height = 0;
                sscanf(line, world_item_format_char, name, path, &width, &height);

                int handle = get_world_item_handle(path, width, height);
                if (handle == -1) {
                    std::string name_str(name);
                    int editor_world_item_idx = create_world_item(path, width, height, name_str);
                    idx_to_handle_map[world_item_idx] = editor_world_item_idx;
                } else {
                    idx_to_handle_map[world_item_idx] = handle;
                }
				world_item_idx++;
                continue;
            }
            // if (strcmp(line, "PLACED_ITEMS\n") == 0) {
            //     placed_items_section = true;
            //     continue;
            // }
			// if ((strcmp(line, "\n") == 0) && placed_items_section) {
			// 	break;
			// }
            if (section == world_map_section::PLACED_WORLD_ITEMS) {
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
                continue;
            } 

            if (section == world_map_section::LIGHTS) {
                std::string delim(WORLD_ITEM_TEXT_FILE_DELIM);
                size_t delim_len = delim.size();
                static std::string lights_format = "%f " + delim + " %f " + delim + " %f " + delim + " %f " + delim + " %f " + delim + " %f " + delim + " %f\n";
                static const char* lights_format_char = lights_format.c_str();
                glm::vec3 color;
                glm::vec2 world_pos;
                float radius;
                float intensity;
                sscanf(line, lights_format_char, &color.r, &color.g, &color.b, &world_pos.x, &world_pos.y, &radius, &intensity);
                create_light(glm::vec3(world_pos, 0), color, radius, intensity);
            }
        }
        fclose(file);
    } else {
        std::cout << "could not open world items file" << std::endl;
    }
}