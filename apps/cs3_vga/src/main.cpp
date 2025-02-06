#include "gvga.h"
#include "gfx.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "ls7447.h"
#include "data.h"

/**
 * simple hello world program demonstrating the GVga library
 */

const uint BOARD_LED_PIN = 25; // Example: GPIO 25, which is connected to the onboard LED
int _state = 1;
uint32_t _msLast;

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
	// bool sync = false;

	for (int i = 15; i; --i)
	{
		sleep_ms(250);
		printf("loop %d\n", i);
		sleep_ms(250);
	}

	_init_led();

	GVga *gvga = gvga_init(width, height, bits, doubleBuffer, interlaced, NULL); // note: double-buffering enabled
	gvga_start(gvga);
	auto colorCount = setPalette(gvga);

	size_t i = 0;
	while (true)
	{
		_blink_led(100);
		gfx_clear(gvga, i);
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