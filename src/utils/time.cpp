#include "time.h"
#include "SDL.h"

float engine_time_t::delta_time = 0.f;

float get_time_since_start_in_sec() {
	Uint32 ms_time = SDL_GetTicks();
	return ms_time / 1000.f;
}
