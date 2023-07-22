#pragma once

#include "glm/glm.hpp"
#include <string>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define RESOURCES_PATH std::string("C:\\Sarthak\\projects\\Platformer\\Editor\\resources")
#define SHADERS_PATH (RESOURCES_PATH + std::string("\\shaders"))
#define GRID_SQUARE_WIDTH 40.f
#define GRIDLINE_COLOR glm::vec3(1.f)
#define CAMERA_SCROLL_SPEED 500.f
#define WORLD_ITEM_TEXT_FILE_DELIM "GAME_DELIM"
#define ENABLE_IMGUI 1