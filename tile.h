#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief Read n bytes from input buffer
/// @param bytes Input buffer
/// @param buff Output buffer
/// @param n Number of bytes to read
/// @param start_offset Beginning offset
/// @return Bytes read
uint32_t read_bytes(uint8_t *bytes, uint8_t *buff, uint32_t n,
		    uint32_t start_offset);

/// @brief Join bytes from input buffer into output buffer
/// @param input_buff Input buffer
/// @param output_buff Output buffer
/// @param length Length of output buffer
void join_all(uint8_t *input_buff, uint16_t *output_buff, uint32_t length);

/// @brief Create texture from buffer
/// @param input_buff Input buffer
/// @param texture Texture
void create_texture(uint16_t *input_buff, SDL_Texture *texture,
		    uint8_t color_palette[4][3]);

/// @brief Load tile data into SDL texture
/// @param input_buff Input buffer
/// @param texture Texture
/// @param start_offset Beginning offset
/// @return Is read
bool get_tile(uint8_t *input_buff, SDL_Texture *texture, uint32_t start_offset,
	      uint8_t color_palette[4][3]);
