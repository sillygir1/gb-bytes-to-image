#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief Load tile data into SDL texture
/// @param input_buff Input buffer
/// @param texture Texture
/// @param start_offset Start offset
/// @return Is read
bool get_tile(uint8_t *input_buff, SDL_Texture *texture, uint32_t start_offset);
