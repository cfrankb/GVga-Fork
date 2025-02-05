#include "gvga.h"
#include "gfx.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "ls7447.h"

/**
 * simple hello world program demonstrating the GVga library
 */

struct hello_world_state
{
	int width;
	int height;
	int x;
	int y;
	int dx;
	int dy;
	int color1;
	int color2;
} _hello_world;

const uint BOARD_LED_PIN = 25; // Example: GPIO 25, which is connected to the onboard LED
int _state = 1;
uint32_t _msLast;
static GVgaColor _palette[] = {
	GVGA_WHITE, GVGA_RED, GVGA_GREEN, GVGA_BLUE, GVGA_YELLOW, GVGA_CYAN, GVGA_VIOLET, GVGA_BLACK,
	GVGA_COLOR(15, 15, 15), GVGA_COLOR(0, 15, 0), GVGA_COLOR(0, 15, 0), GVGA_COLOR(0, 0, 15),
	GVGA_COLOR(15, 15, 0), GVGA_COLOR(0, 15, 15), GVGA_COLOR(15, 15, 0), GVGA_COLOR(7, 7, 7)};

static void _draw_hello_world(GVga *gvga, struct hello_world_state *state)
{
	int width = state->width;
	int height = state->height;
	int X = state->x;
	int Y = state->y;
	int color1 = state->color1;
	int color2 = state->color2;

	// draw_some_boxes(gvga);
	for (int i = 1; i < 16; i++)
	{
		gfx_box(gvga, i - 1, i - 1, width - i, height - i, i % gvga->colors);
	}
	int x = X;
	int y = Y;
	int h = 20;
	int w = 10;
	int H = h / 2;
	int W = w / 2;
	int dx = w + 10;
	int dy = h + 10;

	// H
	gfx_line(gvga, x + 0, y + 0, x + 0, y + h, color1);
	gfx_line(gvga, x + w, y + 0, x + w, y + h, color1);
	gfx_line(gvga, x + 0, y + H, x + w, y + H, color1);
	x += dx;
	// E
	gfx_line(gvga, x + 0, y + 0, x + 0, y + h, color1);
	gfx_line(gvga, x + 0, y + 0, x + w, y + 0, color1);
	gfx_line(gvga, x + 0, y + H, x + w, y + H, color1);
	gfx_line(gvga, x + 0, y + h, x + w, y + h, color1);
	x += dx;
	// L
	gfx_line(gvga, x + 0, y + 0, x + 0, y + h, color1);
	gfx_line(gvga, x + 0, y + h, x + w, y + h, color1);
	x += dx;
	// L
	gfx_line(gvga, x + 0, y + 0, x + 0, y + h, color1);
	gfx_line(gvga, x + 0, y + h, x + w, y + h, color1);
	x += dx;
	// O
	gfx_line(gvga, x + 0, y + 0, x + 0, y + h, color1);
	gfx_line(gvga, x + w, y + 0, x + w, y + h, color1);
	gfx_line(gvga, x + 0, y + 0, x + w, y + 0, color1);
	gfx_line(gvga, x + 0, y + h, x + w, y + h, color1);
	x = X;
	y += dy;
	// W
	gfx_line(gvga, x + 0, y + 0, x + 0, y + h, color2);
	gfx_line(gvga, x + w, y + 0, x + w, y + h, color2);
	gfx_line(gvga, x + 0, y + h, x + W, y + H, color2);
	gfx_line(gvga, x + w, y + h, x + W, y + H, color2);
	gfx_line(gvga, x + w, y + h, x + w, y + 0, color2);
	x += dx;
	// O
	gfx_line(gvga, x + 0, y + 0, x + 0, y + h, color2);
	gfx_line(gvga, x + w, y + 0, x + w, y + h, color2);
	gfx_line(gvga, x + 0, y + 0, x + w, y + 0, color2);
	gfx_line(gvga, x + 0, y + h, x + w, y + h, color2);
	x += dx;
	// R
	gfx_line(gvga, x + 0, y + 0, x + 0, y + h, color2);
	gfx_line(gvga, x + w, y + 0, x + w, y + H, color2);
	gfx_line(gvga, x + 0, y + 0, x + w, y + 0, color2);
	gfx_line(gvga, x + 0, y + H, x + w, y + H, color2);
	gfx_line(gvga, x + W, y + H, x + w, y + h, color2);
	x += dx;
	// L
	gfx_line(gvga, x + 0, y + 0, x + 0, y + h, color2);
	gfx_line(gvga, x + 0, y + h, x + w, y + h, color2);
	x += dx;
	// D
	gfx_line(gvga, x + W / 2, y + 0, x + W / 2, y + h, color2);
	gfx_line(gvga, x + 0, y + 0, x + w, y + 0, color2);
	gfx_line(gvga, x + w, y + 0, x + w, y + h, color2);
	gfx_line(gvga, x + 0, y + h, x + w, y + h, color2);
	x += dx;
	// !
	gfx_line(gvga, x + 0, y + 0, x + 0, y + H + H / 2, color2);
	gfx_line(gvga, x + 0, y + h - 2, x + 0, y + h, color2);
}

static void _erase_hello_world(GVga *gvga, struct hello_world_state *state, bool clear)
{
	int color1 = state->color1;
	int color2 = state->color2;
	if (clear)
	{
		gfx_clear(gvga, 0);
	}
	else
	{
		_hello_world.color1 = 0;
		_hello_world.color2 = 0;
		_draw_hello_world(gvga, state);
		state->color1 = color1;
		state->color2 = color2;
	}
}

static void _init_led()
{
	// heartbeat led
	gpio_init(BOARD_LED_PIN);
	gpio_set_dir(BOARD_LED_PIN, GPIO_OUT);
}

static bool _blink_led(int rate)
{
	uint32_t ms_since_boot = to_ms_since_boot(get_absolute_time());
	if (ms_since_boot - _msLast < rate)
		return false;
	_msLast = ms_since_boot;
	gpio_put(BOARD_LED_PIN, _state % 2);
	_state++;
	return true;
}

static void _init_hello_world(struct hello_world_state *state, int width, int height)
{
	state->width = width;
	state->height = height;
	state->x = 20;
	state->y = 20;
	state->dx = 5;
	state->dy = 5;
	state->color1 = 1;
	state->color2 = 2;
}

static void _move_hello_world(struct hello_world_state *state)
{
	state->x += state->dx;
	state->y += state->dy;
	if (state->x > state->width - 120 || state->x < 20)
	{
		state->dx = -state->dx;
	}
	if (state->y > state->height - 70 || state->y < 20)
	{
		state->dy = -state->dy;
	}
}

uint16_t rgb888torgb555(const uint8_t *rgb888Pixel)
{
	const uint8_t &red = rgb888Pixel[0];
	const uint8_t &green = rgb888Pixel[1];
	const uint8_t &blue = rgb888Pixel[2];

	const uint16_t b = ((blue >> 3) & 0x1f) << 11;
	const uint16_t g = ((green >> 3) & 0x1f) << 6;
	const uint16_t r = ((red >> 3) & 0x1f);
	return r | g | b;
}

// RGB 565
uint16_t rgb888torgb565(const uint8_t *rgb888Pixel)
{
	const uint8_t &red = rgb888Pixel[0];
	const uint8_t &green = rgb888Pixel[1];
	const uint8_t &blue = rgb888Pixel[2];

	const uint16_t b = (blue >> 3) & 0x1f;
	const uint16_t g = ((green >> 2) & 0x3f) << 5;
	const uint16_t r = ((red >> 3) & 0x1f) << 11;
	return r | g | b;
}

void fillPalette()
{
}

int main()
{
	stdio_init_all();
	sleep_ms(1000);
	printf("\nGVga test\n");

	int width = 320;  // 640;
	int height = 240; // 480;
	int bits = 8;	  // 1;
	bool doubleBuffer = false;
	bool interlaced = false;
	bool sync = false;

	for (int i = 15; i; --i)
	{
		sleep_ms(250);

		printf("loop %d\n", i);
		sleep_ms(250);
	}

	_init_led();
	_init_hello_world(&_hello_world, width, height);

	GVga *gvga = gvga_init(width, height, bits, doubleBuffer, interlaced, NULL); // note: double-buffering enabled
	if (bits < 8)
		gvga_setPalette(gvga, _palette, 0, gvga->colors);
	else
		gvga_setPalette(gvga, _palette, 0, 16);
	gvga_start(gvga);

	int i = 0;
	//	_draw_hello_world(gvga, &_hello_world);

	const uint32_t colors[] = {
		0x000000, // black
		0x000080,
		0x000040,
		0x000020,
		0x000010,
		0x000008,
		0x0000ff, // red
		0x00FF00, // green
		0xff0000, // blue
		0xff00ff, // purple
		0xffff00, // cyan
		0x00ffff, // yellow
	};

	enum
	{
		BLACK,
		RED,
		GREEN,
		BLUE,
		PURPLE,
		CYAN,
		YELLOW
	};

	const size_t colorCount = sizeof(colors) / sizeof(colors[0]);
	uint16_t palette[colorCount];
	for (size_t i = 0; i < colorCount; ++i)
	{
		auto rgba = colors[i];
		palette[i] = rgb888torgb565(reinterpret_cast<uint8_t *>(&rgba));
	}

	//	gfx_clear(gvga, palette[i]);
	gvga_setPalette(gvga, reinterpret_cast<GVgaColor *>(palette), 0, colorCount);

	while (true)
	{
		//	printf("loop %d\n", i++);
		_blink_led(100);
		//_erase_hello_world(gvga, &_hello_world, true);
		//_move_hello_world(&_hello_world);
		//_draw_hello_world(gvga, &_hello_world);
		printf("color idx: %d [0x%.4x]\n", i, palette[i]);
		gfx_clear(gvga, palette[i]);
		// if (sync)
		//	gvga_sync(gvga);	// wait for the other core to finish displaying the frame buffer
		// gvga_swap(gvga, false); // double-buffering with/without copy
		sleep_ms(1000);
		i++;
		if (i >= colorCount)
		{
			i = 0;
		}
	}
}