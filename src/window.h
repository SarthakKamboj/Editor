#pragma once

#include "SDL.h"

struct window_t {
    SDL_Window* sdl_window = NULL;
    SDL_GLContext context;
    int width = -1;
    int height = -1;
};