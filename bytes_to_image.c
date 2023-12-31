#include "bytes_to_image.h"
#include "tile.h"

#define width 8	       // px
#define height 8       // px
#define pixel_scale 40 // ratio of gameboy to screen

uint32_t start = 0;
uint32_t end = 0;
char *filename;
uint8_t *bytes_input;

// Read *end* bytes to buffer from file starting from *start*
uint32_t read_(FILE *file, uint8_t *buff, uint32_t end, uint32_t start) {
	fseek(file, start, 0);
	return fread(buff, 1, end, file);
}

// Print tiles into stdout
void print_tiles(uint16_t *pixels, uint16_t size) {
	// Printing image:
	printf("Image:\n");
	for (uint16_t i = 0; i < size; i++) {
		if (i % 8 == 0 && i != 0)
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
}

// Print help
void help() {
	printf("usage:\nbytes_to_image -b length [bytes...]\n");
	printf("bytes_to_image filename [option] start_address arg2 \n");
	printf("  option:\n");
	printf("    -n\tRead arg2 bytes starting address "
	       "start_address\n");
	printf("    -r\tRead range start_address to arg2\n");
	printf("    -h\tHelp\n");
}

// Parse arguments
uint8_t parse_arguments(int argc, char *argv[]) {
	if (argc > 1) {
		start = (int)strtol(argv[3], NULL, 16);
		if (strcmp(argv[1], "-h") == 0 ||
		    strcmp(argv[2], "-h") == 0) { // Help
			help();
			return false;
		} else {
			strncpy(filename, argv[1], 64);
			// filename = argv[1];
		}
		if (strcmp(argv[2], "-n") == 0) { // Read n bytes
			if (argv[4][0] == '0' && argv[4][1] == 'x')
				end = (int)strtol(argv[4], NULL, 16);
			else
				end = (int)strtol(argv[4], NULL, 10);
		} else if (strcmp(argv[2], "-r") == 0) { // Read range
			end = (int)strtol(argv[4], NULL, 16) - start + 1;
		} else if (strcmp(argv[1], "-b") == 0) { // Read bytes
			if (argv[2][0] == '0' && argv[2][1] == 'x')
				end = (int)strtol(argv[2], NULL, 16);
			else
				end = (int)strtol(argv[2], NULL, 10);
			bytes_input = malloc(sizeof(uint8_t) * end);
			for (int i = 0; i < end; i++) {
				bytes_input[i] = strtol(argv[i + 3], NULL, 16);
			}
		} else { // Print help if no valid key
			help();
			return false;
		}
	} else { // Print help if no arguments
		help();
		return false;
	}
	return true;
}

void show_tile(uint16_t *input_buff) {
	// Init window
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Failed to initialize the SDL2 library\n");
		return;
	}
	// Create window
	SDL_Window *window = SDL_CreateWindow(
	    "tile_display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	    width * pixel_scale, height * pixel_scale,
	    SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY);
	// Create renderer
	SDL_Renderer *renderer =
	    SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Create texture
	SDL_Texture *texture =
	    SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
			      SDL_TEXTUREACCESS_STREAMING, 8, 8);
	create_texture(input_buff, texture, NULL);

	// Initializing framerate-related variables
	unsigned int a = SDL_GetTicks();
	unsigned int b = SDL_GetTicks();
	double delta = 0;

	// Rectangle to apply texture to
	SDL_Rect dst;
	dst.x = 0;
	dst.y = 0;
	dst.w = pixel_scale * 8;
	dst.h = pixel_scale * 8;

	// Window display loop
	bool keep_window_open = true;
	while (keep_window_open) {
		SDL_Event e;
		while (SDL_PollEvent(&e) > 0) {
			switch (e.type) {
			case SDL_QUIT:
				keep_window_open = false;
				break;
			}
		}

		// Draw 60 times per second
		a = SDL_GetTicks();
		delta = a - b;
		if (delta > 1000 / 60.0) {
			// Clear screen
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			// Render copy of texture on rectangle dst
			SDL_RenderCopy(renderer, texture, NULL, &dst);

			// Show the thing
			SDL_RenderPresent(renderer);
		}
	}
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
		bytes = read_bytes(bytes_input, buff, end, 0);
	else
		bytes = read_(file, buff, end, start);

	// Display read bytes
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

	// Display joined values
	for (uint32_t i = 0; i < end / 2; i++) {
		printf("0x%04x, ", buff_image[i]);
	}

	printf("\nColor palette:\n ░▒▓\n");

	// Print tile to stdout
	print_tiles(buff_image, end / 2);

	// Draw if just 8x8 tile is read
	if (end / 2 == 8)
		show_tile(buff_image);

	// Freeing everything
	if (filename)
		free(filename);
	free(bytes_input);
	free(buff);
	if (file)
		fclose(file);
	return 0;
}
