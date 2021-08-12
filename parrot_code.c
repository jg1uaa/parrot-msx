// CC-BY-SA
// original code: SASANO Takayoshi

#include <msx/gfx.h>

extern const unsigned char *parrot_data[];
extern const unsigned char parrot_palette[];

#define DATA_SIZE	768
#define PLANE		10

#define DELAY		500

#define PATTERN_GEN	0x0000	// pattern generator table
#define PATTERN_NAME	0x1800	// pattern name table
#define COLOR_TABLE	0x2000	// color table

#define PALETTE_MAX	16

#define VDP_PORT_R	0x0006	// VDP I/O port (read) stored address
#define VDP_PORT_W	0x0007	// VDP I/O port (write) stored address
#define MSX_VERSION	0x002d	// MSX-BASIC version stored address
#define NEWKEY		0xfbe5	// keyboard matrix status

static void init_screen(void)
{
	int i;

	set_mode(mode_1);
	set_color(15, 0, 0);

	for (i = 0; i < 16; i++) {
		fill(PATTERN_GEN + i * 128, 0, 8);	// blank pattern
		vpoke(COLOR_TABLE + i * 2, i);	// color (as background)
	}
}

static void dummy_loop(int ticks)
{
	int i;

	for (i = 0; i < ticks; i++) {
#asm
		nop
#endasm
	}
}

static void set_palette(const unsigned char *palette)
{
	int i;
	unsigned int port = *(unsigned int *)VDP_PORT_W;

#asm
	di
#endasm

	/* set palette index to 0 */
	outp(port + 1, 0x00);
	outp(port + 1, 0x90);

	/* send palette data */
	for (i = 0; i < PALETTE_MAX; i++) {
		outp(port + 2, *palette++);
		outp(port + 2, *palette++);
	}
#asm
	ei
#endasm
}

static void do_disp(void)
{
	int i;

	for (i = 0; i < PLANE; i++) {
		vwrite(parrot_data[i], PATTERN_NAME, DATA_SIZE);
		dummy_loop(DELAY);
	}
}

int main(int argc, char *argv[])
{
	init_screen();

	/* MSX1 or press "1"; no palette setting */
	if ((*((unsigned char *)NEWKEY) & 2) && *((unsigned char *)MSX_VERSION))
		set_palette(parrot_palette);

	while (1)
		do_disp();

	/* NOTREACHED */
	return 0;
}
