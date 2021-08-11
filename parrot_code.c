// CC-BY-SA
// original code: SASANO Takayoshi

#include <msx/gfx.h>

extern const unsigned char parrot_data[];

#define DATA_SIZE	768
#define PLANE		10

#define	DELAY		500

#define	PATTERN_GEN	0x0000	// pattern generator table
#define	PATTERN_NAME	0x1800	// pattern name table
#define COLOR_TABLE	0x2000	// color table

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


static void do_disp(void)
{
	int i;

	for (i = 0; i < PLANE; i++) {
		vwrite(parrot_data + DATA_SIZE * i, PATTERN_NAME, DATA_SIZE);
		dummy_loop(DELAY);
	}
}

int main(int argc, char *argv[])
{
	init_screen();
	while (1)
		do_disp();

	/* NOTREACHED */
	return 0;
}