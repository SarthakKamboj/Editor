#pragma once

#include "glm/glm.hpp"
#include <string>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 700
#define RESOURCES_PATH std::string("C:\\Sarthak\\projects\\editor\\resources")
#define SHADERS_PATH (RESOURCES_PATH + std::string("\\shaders"))
#define GRID_SQUARE_WIDTH 40.f
#define GRIDLINE_COLOR glm::vec3(1.f)
#define CAMERA_SCROLL_SPEED 500.f
#define WORLD_ITEM_TEXT_FILE_DELIM " , "
#define WINDOW_TITLE_BAR_HEIGHT 20
#define PI 3.141592653589793f
#define EDITOR_MODE 1