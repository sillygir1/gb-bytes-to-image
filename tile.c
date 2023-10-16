#include "tile.h"

uint8_t colors[4] = {0x00, 0x67, 0xb6, 0xff};

uint32_t read_bytes(uint8_t *bytes, uint8_t *buff, uint32_t n,
		    uint32_t start_offset) {
	uint32_t read = 0;
	for (uint32_t i = 0; i < n; i++) {
		buff[i] = bytes[start_offset + i];
		read += 1;
	}
	return read;
}

// Tiles' pixels are stored in pairs of bytes, each two bytes
// representing 8 grayscale 2-bit pixels.
// First byte represents least significant bits of
// pixels, second - most significant.
// This function joins bytes to get 1 16-bit
// value for the row of 8 pixels
uint16_t join_bytes(uint8_t byte1, uint8_t byte2) {
	uint16_t out = 0;
	for (uint8_t i = 0; i < 8; i++) {
		out += ((((byte1 & (1 << i)) >> (i)) << 1) +
			((byte2 & (1 << i)) >> (i)))
		       << (14 - 2 * i);
	}
	return out;
}

void join_all(uint8_t *input_buff, uint16_t *output_buff, uint32_t length) {
	for (uint32_t i = 0; i < length; i++) {
		output_buff[i] =
		    join_bytes(input_buff[2 * i], input_buff[2 * i + 1]);
	}
}

void create_texture(uint16_t *input_buff, SDL_Texture *texture) {
	uint8_t *pixels;
	int pitch;

	SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);

	// Turning every very 2-bit pixel into 4 RGBA bytes
	for (uint8_t i = 0; i < 8; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			for (uint8_t k = 0; k < 4; k++) {
				if (k != 0)
					pixels[k + 4 * (8 * i + j)] =
					    colors[(input_buff[i] &
						    (0b11 << (j * 2))) >>
						   (j * 2)];
				else
					pixels[k + 4 * (8 * i + j)] = 0;
			}
		}
	}

	SDL_UnlockTexture(texture);
}

// Get texture from byte array
bool get_tile(uint8_t *input_buff, SDL_Texture *texture,
	      uint32_t start_offset) {
	uint8_t buff[16];
	uint16_t tile_buff[8];
	if (!read_bytes(input_buff, buff, 16, start_offset)) {
		return false;
	}
	join_all(buff, tile_buff, 8);
	create_texture(tile_buff, texture);
	return true;
}
