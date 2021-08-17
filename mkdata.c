// CC-BY-SA
// original code: SASANO Takayoshi

#include <stdio.h>
#include <string.h>
#include <stdint.h>

// size of original picture
#define X_WIDTH	35
#define Y_WIDTH 25

static uint8_t buf[X_WIDTH * Y_WIDTH * 3];

// size of display and position of original picture
#define X_POS	1
#define Y_POS	1
#define X_SIZE	32
#define Y_SIZE	24

// reference: https://paulwratt.github.io/programmers-palettes/HW-MSX/HW-MSX-palettes.html
#define PALETTE_MAX	16
static uint32_t msx_palette[PALETTE_MAX] = {
	0x000000, 0x010101, 0x3eb849, 0x74d07d,
	0x5955e0, 0x8076f1, 0xb95e51, 0x65dbef,
	0xdb6559, 0xff897d, 0xccc35e, 0xded087,
	0x3aa241, 0xb766b5, 0xcccccc, 0xffffff,
};

static uint32_t palette_data[PALETTE_MAX];
static uint32_t palette_dist[PALETTE_MAX];

static void init_palette(void)
{
	memset(palette_data, 0, sizeof(palette_data));
	memset(palette_dist, ~0, sizeof(palette_dist));
}

static int find_near_color(uint32_t col)
{
	int i, ix;
	uint8_t r0, g0, b0, r1, g1, b1;
	uint32_t d, dx;
	int32_t dr, dg, db;

	r0 = (col >> 16) & 0xff;
	g0 = (col >> 8) & 0xff;
	b0 = col & 0xff;

	dx = ~0;
	ix = -1;
	for (i = 0; i < PALETTE_MAX; i++) {
		r1 = (msx_palette[i] >> 16) & 0xff;
		g1 = (msx_palette[i] >> 8) & 0xff;
		b1 = msx_palette[i] & 0xff;

		dr = r1 - r0;
		dg = g1 - g0;
		db = b1 - b0;

		d = (dr * dr) + (dg * dg) + (db * db);
		if (d < dx) {
			dx = d;
			ix = i;
		}
	}

	if (d < palette_dist[ix])
		palette_data[ix] = col;

	return ix;
}

static void dump_palette(void)
{
	int i;
	uint8_t r, g, b;

	for (i = 0; i < PALETTE_MAX; i++) {
		r = (palette_data[i] >> 21) & 7;
		g = (palette_data[i] >> 13) & 7;
		b = (palette_data[i] >> 5) & 7;

		printf("%d,%d, ", (r << 4) | b, g);
	}

	printf("\n");
}

static void dump(char *filename)
{
	FILE *fp;
	int32_t x, y, c;
	uint8_t *p;

	memset(buf, 0, sizeof(buf));
	fp = fopen(filename, "r");
	if (fp != NULL) {
		fread(buf, sizeof(buf), 1, fp);
		fclose(fp);
	}

	for (y = Y_POS; y < Y_POS + Y_SIZE; y++) {
		printf("\t");
		for (x = X_POS; x < X_POS + X_SIZE; x++) {
			p = &buf[(y * X_WIDTH + x) * 3];
			c = (p[0] << 16) | (p[1] << 8) | p[2];

			// special care (only #0 sets background color)
			if (c == 0)
				c = ~0;

			printf("%3d,", find_near_color(c) << 4);

		}
		printf("\n");
	}

	return;
}

int main(int argc, char *argv[])
{
	int i;

	if (argc < 2) {
		printf("usage: %s [fllename ...]\n", argv[0]);
		return 0;
	}

	printf("// this is auto-generated, do not edit!\n");
	printf("\n");

	init_palette();

	for (i = 1; i < argc; i++) {
		printf("/* %s */\n", argv[i]);
		printf("const unsigned char parrot_data%02d[] = {\n", i - 1);
		dump(argv[i]);
		printf("};\n\n");
	}

	printf("const unsigned char *parrot_data[] = {\n");
	for (i = 1; i < argc; i++)
		printf("\tparrot_data%02d,\n", i - 1);
	printf("\t(void *)0,\n");
	printf("};\n\n");

	printf("const unsigned char parrot_palette[] = {\n");
	dump_palette();
	printf("};\n");

	return 0;
}
