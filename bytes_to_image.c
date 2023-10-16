#include "bytes_to_image.h"

uint32_t start = 0;
uint32_t end = 0;
char *filename;
uint8_t *bytes_input;

uint8_t colors[4] = {0x00, 0x67, 0xb6, 0xff};

// Gonna think about that one later, useless for now
void swap(uint16_t *x, uint16_t *y) {
	int temp = *x;
	*x = *y;
	*y = temp;
} // Works i suppose

// Read *end* bytes to buffer from file starting from *start*
uint32_t read(FILE *file, uint8_t *buff, uint32_t end, uint32_t start) {
	fseek(file, start, 0);
	return fread(buff, 1, end, file);
} // Done

// A bit useless but works, so whatever
uint32_t read_bytes(uint8_t *bytes, uint8_t *buff, uint8_t w, uint8_t h) {
	uint32_t read = 0;
	for (uint32_t i = 0; i < w * h / 8 * 2; i++) {
		buff[i] = bytes[i];
		read += 1;
	}
	return read;
}

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
		       << (14 - 2 * i);
	}
	return out;
} // Done

void join_all(uint8_t *input_buff, uint16_t *output_buff, uint32_t length) {
	for (uint32_t i = 0; i < length; i++) {
		output_buff[i] =
		    join_bytes(input_buff[2 * i], input_buff[2 * i + 1]);
	}
}

void print_sprite(uint16_t *pixels, uint16_t size) {
	// Printing image:
	printf("Image:\n");
	for (uint16_t i = 0; i < size; i++) {
		if (i % 8 == 0)
			printf("\n");
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

void help() {
	printf("usage:\nbytes_to_image -b length [bytes...]\n");
	printf("bytes_to_image filename [option] start_address arg2 \n");
	printf("  option:\n");
	printf("    -n\tRead arg2 bytes starting address "
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
		if (strcmp(argv[2], "-n") == 0) { // Read n bytes
			if (argv[4][0] == '0' && argv[4][1] == 'x')
				end = (int)strtol(argv[4], NULL, 16);
			else
				end = (int)strtol(argv[4], NULL, 10);
		} else if (strcmp(argv[2], "-r") == 0) { // Read range
			end = (int)strtol(argv[4], NULL, 16) - start + 1;
		} else if (strcmp(argv[1], "-b") == 0) { // Read bytes
			if (argv[4][0] == '0' && argv[4][1] == 'x')
				end = (int)strtol(argv[4], NULL, 16);
			else
				end = (int)strtol(argv[4], NULL, 10);
			bytes_input = malloc(sizeof(uint8_t) * end);
			for (int i = 0; i < end; i++) {
				bytes_input[i] = strtol(argv[i + 3], NULL, 16);
			}
		} else {
			help();
			return false;
		}
	} else {
		help();
		return false;
	}
	return true;
}

void show_sprite() {
	// Init window
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		// throw some error idk
	}
	SDL_Window *window = SDL_CreateWindow(
	    "gb_emu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640,
	    480, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY);
	SDL_Renderer *renderer =
	    SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Rect r;
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
	uint32_t bytes = 0;
	if (strcmp(argv[1], "-b") == 0)
		bytes = read_bytes(bytes_input, buff, 8, 8);
	else
		bytes = read(file, buff, end, start);

	if (bytes != 0) {
		if (strcmp(argv[1], "-b") == 0)
			printf("Read %d bytes:\n", bytes);
		else
			printf("Read %d bytes:\n", bytes, start);
		for (uint32_t i = 0; i < bytes; i++) {
			printf("0x%x, ", buff[i]);
		}
		printf("\n");
	} else {
		printf("Failure to read file on 0x%x offset\n", start);
	}

	printf("Joined bytes:\n");

	join_all(buff, buff_image, end / 2);

	for (uint32_t i = 0; i < end / 2; i++) {
		printf("0x%04x, ", buff_image[i]);
	}

	printf("\nColor palette:\n ░▒▓\n");
	if (strcmp(argv[1], "-b") == 0)
		print_sprite(buff_image, end / 2);
	else
		print_sprite(buff_image, end / 2);

	// free(filename);
	free(bytes_input);
	free(buff);
	if (file)
		fclose(file);
	return 0;
}
