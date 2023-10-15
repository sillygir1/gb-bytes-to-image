#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Gonna think about that one later, useless for now
void swap(uint16_t *x, uint16_t *y) {
	int temp = *x;
	*x = *y;
	*y = temp;
} // Works i suppose

// Read *end* bytes to buffer from file starting from start
uint32_t read(FILE *file, uint8_t *buff, uint32_t end, uint32_t start) {
	fseek(file, start, 0);
	return fread(buff, 1, end, file);
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

uint32_t start = 0;
uint32_t end = 0;
char *filename;

void help() {
	printf("usage: bytes_to_image filename [option] start_address arg2 \n");
	printf("  option:\n");
	printf("    -n\tRead n (hexadecimal) bytes starting address "
	       "start_address\n");
	printf("    -r\tRead range start_address to arg2\n");
	printf("    -h\tHelp\n");
}

uint8_t parse_arguments(int argc, char *argv[]) {
	if (argc > 1) {
		start = (int)strtol(argv[3], NULL, 16);
		if (strcmp(argv[1], "-h") == 0 ||
		    strcmp(argv[2], "-h") == 0) { // Help
			help();
			return false;
		} else {
			filename = argv[1];
		}
		if (strcmp(argv[2], "-n") == 0) { // N bytes
			end = (int)strtol(argv[4], NULL, 16);
		} else if (strcmp(argv[2], "-r") == 0) { // Range
			end = (int)strtol(argv[4], NULL, 16) - start + 1;
		}
	} else {
		help();
		return false;
	}
	return true;
}

int main(int argc, char *argv[]) {

	filename = malloc(sizeof(char) * 64);

	if (!parse_arguments(argc, argv)) {
		return 1;
	}
	// Opening file
	FILE *file = fopen(filename, "r");

	// Initializing variables and buffers
	uint8_t *buff = malloc(sizeof(uint8_t) * end);
	uint16_t *buff_image = malloc(sizeof(uint16_t) * end);

	// Reading from file
	uint32_t bytes = read(file, buff, end, start);
	if (bytes != 0) {
		printf("Read %d bytes starting 0x%x:\n", bytes, start);
		for (uint32_t i = 0; i < bytes; i++) {
			printf("0x%x, ", buff[i]);
		}
		printf("\n");
	} else {
		printf("Failure to read file on 0x%x offset\n", start);
	}

	printf("Joined bytes:\n");
	for (uint32_t i = 0; i < end / 2; i++) {
		buff_image[i] = join_bytes(buff[2 * i], buff[2 * i + 1]);
		printf("0x%x, ", buff_image[i]);
	}

	printf("\nColor palette:\n ░▒▓\n");
	draw_sprite(buff_image, end / 2, 8, 8);

	free(buff);
	fclose(file);
	return 0;
}
