#include <stdio.h>
#include <time.h>
#include "gvga.h"
#include "gfx.h"
#include "pico/stdlib.h"
#include "ls7447.h"
#include "data.h"
#include "tilesdata.h"
#include "game.h"
#include "engine.h"
#include "decoder.h"

/**
 * simple hello world program demonstrating the GVga library
 */

#define TILE_SIZE 16
#define TILE_BYTES (TILE_SIZE * TILE_SIZE)
#define TILES_MAX TILES_YELKILLER

const uint BOARD_LED_PIN = 25; // Example: GPIO 25, which is connected to the onboard LED
int _state = 1;
uint32_t _msLast;

const int width = 320;			 // screen width
const int height = 200;			 // screen height
const int bits = 8;				 // bits per pixel (256 color palette)
const bool doubleBuffer = false; // not enough memory for that
const bool interlaced = false;

static void _init_led()
{
	// heartbeat led
	gpio_init(BOARD_LED_PIN);
	gpio_set_dir(BOARD_LED_PIN, GPIO_OUT);
}

static bool _blink_led(uint32_t rate)
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
	uint16_t palette[colorCount];
	for (size_t i = 0; i < colorCount; ++i)
	{
		auto rgba = reinterpret_cast<uint32_t *>(tiles_pal)[i];
		palette[i] = rgb888torgb555(reinterpret_cast<uint8_t *>(&rgba));
	}
	gvga_setPalette(gvga, reinterpret_cast<GVgaColor *>(palette), 0, colorCount);
	return colorCount;
}

void drawTile(GVga *gvga, int baseX, int baseY, uint8_t *tile)
{
	auto ptr = gvga->drawFrame + gvga->rowBytes * baseY + baseX;
	Decoder decoder;
	decoder.start(tile);
	for (int y = 0; y < TILE_SIZE; ++y)
	{
		for (int x = 0; x < TILE_SIZE; ++x)
		{
			ptr[x] = decoder.get();
		}
		ptr += gvga->rowBytes;
	}
}

void fillScreen(GVga *gvga)
{
	const int cols = width / TILE_SIZE;
	const int rows = height / TILE_SIZE;

	uint8_t *tilesets[]{
		tiles_mcz,
		annie_mcz,
		animz_mcz,
	};

	const int set = rand() % 3;
	uint8_t *tiledata = tilesets[set];
	const int tileCount = tiledata[0] + (tiledata[1] << 8);
	for (int y = 0; y < rows; ++y)
	{
		for (int x = 0; x < cols; ++x)
		{
			uint8_t tileID = rand() % tileCount;
			auto tile = Decoder::data(tiledata, tileID);
			drawTile(gvga, x * TILE_SIZE, y * TILE_SIZE, tile);
		}
	}
}

int main()
{
	stdio_init_all();
	srand(time(NULL));
	sleep_ms(1000);

	for (int i = 15; i; --i)
	{
		sleep_ms(500);
		printf("%d\n", i);
	}
	_init_led();

	GVga *gvga = gvga_init(width, height, bits, doubleBuffer, interlaced, NULL);
	gvga_start(gvga);
	setPalette(gvga);
	while (false)
	{
		_blink_led(100);
		fillScreen(gvga);
		sleep_ms(1000);
	}

	CGame &game = *CGame::getGame();
	CEngine &engine = *CEngine::getEngine();
	game.loadLevel(false);

	uint32_t ticks = 0;

	while (true)
	{
		_blink_led(100);
		switch (game.mode())
		{
		case CGame::MODE_INTRO:
		case CGame::MODE_RESTART:
		case CGame::MODE_GAMEOVER:
			engine.drawLevelIntro(gvga);
			sleep_ms(2000);
			if (game.mode() == CGame::MODE_GAMEOVER)
			{
				game.restartGame();
			}
			else
			{
				game.setMode(CGame::MODE_LEVEL);
			}
			break;
		case CGame::MODE_LEVEL:
			engine.drawScreen(gvga);
		}

		engine.mainLoop(ticks);
	}
}