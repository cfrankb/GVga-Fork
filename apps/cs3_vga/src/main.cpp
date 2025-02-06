#include <stdio.h>
#include <time.h>
#include "gvga.h"
#include "gfx.h"
#include "pico/stdlib.h"
#include "ls7447.h"
#include "data.h"
#include "tilesdata.h"

/**
 * simple hello world program demonstrating the GVga library
 */

#define TILE_SIZE 16
#define TILES_MAX TILES_LEFTBIRD

const uint BOARD_LED_PIN = 25; // Example: GPIO 25, which is connected to the onboard LED
int _state = 1;
uint32_t _msLast;

const int width = 320;			 // screen width
const int height = 240;			 // screen height
const int bits = 8;				 // bits per pixel (256 color palette)
const bool doubleBuffer = false; // not enough memory for that
const bool interlaced = false;

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

size_t setPalette(GVga *gvga)
{
	const size_t colorCount = sizeof(tiles_pal) / sizeof(uint32_t);
	printf("tiles.pal: %u colors", colorCount);
	uint16_t palette[colorCount];
	for (size_t i = 0; i < colorCount; ++i)
	{
		auto rgba = reinterpret_cast<uint32_t *>(tiles_pal)[i];
		palette[i] = rgb888torgb555(reinterpret_cast<uint8_t *>(&rgba));
	}
	gvga_setPalette(gvga, reinterpret_cast<GVgaColor *>(palette), 0, colorCount);
	return colorCount;
}

void drawTile(GVga *gvga, int baseX, int baseY, uint16_t tileId)
{
	uint8_t *tile = tiles_mcz + tileId * 256;
	auto ptr = gvga->drawFrame + gvga->rowBytes * baseY + baseX;
	for (int y = 0; y < 16; ++y)
	{
		for (int x = 0; x < 16; ++x)
		{
			ptr[x] = tile[x];
		}
		tile += 16;
		ptr += gvga->rowBytes;
	}
}

void fillScreen(GVga *gvga)
{
	const int cols = width / TILE_SIZE;
	const int rows = height / TILE_SIZE;
	for (int y = 0; y < rows; ++y)
	{
		for (int x = 0; x < cols; ++x)
		{
			const int tileID = rand() % TILES_MAX;
			drawTile(gvga, x * TILE_SIZE, y * TILE_SIZE, tileID);
		}
	}
}

int main()
{
	stdio_init_all();
	srand(time(NULL));
	sleep_ms(1000);
	printf("\nGVga test\n");

	for (int i = 15; i; --i)
	{
		sleep_ms(250);
		printf("loop %d\n", i);
		sleep_ms(250);
	}
	_init_led();

	GVga *gvga = gvga_init(width, height, bits, doubleBuffer, interlaced, NULL);
	gvga_start(gvga);
	auto colorCount = setPalette(gvga);

	size_t i = 0;
	while (true)
	{
		_blink_led(100);
		// gfx_clear(gvga, i);
		fillScreen(gvga);
		sleep_ms(1000);
		i++;
		if (i >= colorCount)
		{
			i = 0;
		}
	}
}