#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Gonna think about that one later, useless for now
void swap(uint16_t *x, uint16_t *y) {
	int temp = *x;
	*x = *y;
	*y = temp;
} // Works i suppose

// Read *lenght* bytes to buffer from file starting from offset
uint32_t read(FILE *file, uint8_t *buff, uint32_t lenght, uint32_t offset) {
	fseek(file, offset, 0);
	return fread(buff, 1, lenght, file);
} // Done

// Sprites' pixels are stored in pairs of bytes, each two bytes
// representing 8 grayscale 2-bit pixels.
// First byte represents most significant bits of
// pixels, second - least significant.
// This function joins bytes to get 1 16-bit
// value for the row of 8 pixels
uint16_t join_bytes(uint8_t byte1, uint8_t byte2) {
	uint16_t out = 0;
	for (uint8_t i = 0; i < 8; i++) {
		out += ((((byte1 & (1 << i)) >> (i)) << 1) +
			((byte2 & (1 << i)) >> (i)))
		       << (2 * i);
	}
	return out;
} // Done

void draw_sprite(uint16_t *pixels, uint16_t size, uint8_t w, uint8_t h) {
	// Printing image:
	printf("Image:\n");
	for (uint16_t i = 0; i < size; i++) {
		// Printing single line:
		for (uint8_t j = 0; j < 8; j++) {
			switch ((pixels[i] & (0b11 << (j * 2))) >> (j * 2)) {
			case 0b00:
				printf("  ");
				break;
			case 0b01:
				printf("░░");
				break;
			case 0b10:
				printf("▒▒");
				break;
			case 0b11:
				printf("▓▓");
				break;
			}
		}
		printf("\n");
	}
} // Done

int main(int argc, char *argv[]) {
	// Opening file
	FILE *file = fopen("64k-full-rom.bin", "r");

	// Initializing variables and buffers
	uint32_t lenght = 64;
	uint32_t offset = 0x11D0;

	// Arguments: sprites_to_pixels address length
	// Arguments: sprites_to_pixels start_address end_address anything
	// (needs 3 arguments for second one, i'll change it later)
	if (argc > 1) {
		offset = (int)strtol(argv[1], NULL, 16);
		if (argc == 4) {
			lenght = (int)strtol(argv[2], NULL, 16) - offset + 1;
		} else
			lenght = atoi(argv[2]);
	}

	uint8_t *buff = malloc(sizeof(uint8_t) * lenght);
	uint16_t *buff_image = malloc(sizeof(uint16_t) * lenght);

	// Reading from file
	uint32_t bytes = read(file, buff, lenght, offset);
	if (bytes != 0) {
		printf("Read %d bytes starting 0x%x:\n", bytes, offset);
		for (uint32_t i = 0; i < bytes; i++) {
			printf("0x%x, ", buff[i]);
		}
		printf("\n");
	} else {
		printf("Failure to read file on 0x%x offset\n", offset);
	}

	printf("Joined bytes:\n");
	for (uint32_t i = 0; i < lenght / 2; i++) {
		buff_image[i] = join_bytes(buff[2 * i], buff[2 * i + 1]);
		printf("0x%x, ", buff_image[i]);
	}

	printf("\nColor palette:\n ░▒▓\n");
	draw_sprite(buff_image, lenght / 2, 8, 8);

	free(buff);
	fclose(file);
	return 0;
}
