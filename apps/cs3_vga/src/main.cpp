#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include "hardware/flash.h"
#include "pico/stdlib.h"
#include "gvga.h"
#include "gfx.h"
#include "ls7447.h"
#include "data.h"
#include "tilesdata.h"
#include "game.h"
#include "engine.h"
#include "debug.h"

#define BOARD_LED_PIN 25 // Example: GPIO 25, which is connected to the onboard LED
int _state = 1;
uint32_t _msLast;

const int width = CEngine::CONFIG_WIDTH;   // screen width
const int height = CEngine::CONFIG_HEIGHT; // screen height
const int bits = 8;						   // bits per pixel (256 color palette)
const bool doubleBuffer = false;		   // not enough memory for that
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
	const size_t colorCount = TILES_PAL_SIZE / sizeof(uint32_t);
	uint16_t *palette = new uint16_t[colorCount];
	auto tiles_pal = CEngine::dataPtr(TILES_PAL_OFFSET);
	for (size_t i = 0; i < colorCount; ++i)
	{
		uint8_t rgba[4];
		rgba[0] = tiles_pal[i * sizeof(uint32_t)];
		rgba[1] = tiles_pal[i * sizeof(uint32_t) + 1];
		rgba[2] = tiles_pal[i * sizeof(uint32_t) + 2];
		rgba[3] = tiles_pal[i * sizeof(uint32_t) + 3];
		palette[i] = rgb888torgb555(rgba);
	}
	gvga_setPalette(gvga, reinterpret_cast<GVgaColor *>(palette), 0, colorCount);
	delete[] palette;
	return colorCount;
}

void drawTile(GVga *gvga, int baseX, int baseY, const uint8_t *tile)
{
	auto ptr = gvga->drawFrame + gvga->rowBytes * baseY + baseX;
	for (int y = 0; y < CEngine::TILE_SIZE; ++y)
	{
		for (int x = 0; x < CEngine::TILE_SIZE; ++x)
		{
			ptr[x] = tile[x + y * CEngine::TILE_SIZE];
		}
		ptr += gvga->rowBytes;
	}
}

void fillScreen(GVga *gvga)
{
	const int cols = width / CEngine::TILE_SIZE;
	const int rows = height / CEngine::TILE_SIZE;

	struct tileset_t
	{
		uint32_t offset;
		uint32_t size;
	};

	tileset_t tilesets[] = {
		{ANIMZ_MCZ_OFFSET, ANIMZ_MCZ_SIZE},
		{ANNIE_MCZ_OFFSET, ANNIE_MCZ_SIZE},
		{TILES_MCZ_OFFSET, TILES_MCZ_SIZE},
	};

	const int set = rand() % 3;
	const uint8_t *tiledata = CEngine::dataPtr(tilesets[set].offset);
	const int tileCount = tilesets[set].size / CEngine::TILE_OFFSET;
	for (int y = 0; y < rows; ++y)
	{
		for (int x = 0; x < cols; ++x)
		{
			uint8_t tileID = rand() % tileCount;
			const uint8_t *tile = tiledata + tileID * CEngine::TILE_OFFSET;
			drawTile(gvga, x * CEngine::TILE_SIZE, y * CEngine::TILE_SIZE, tile);
		}
	}
}

uint32_t getTotalHeap(void)
{
	extern char __StackLimit, __bss_end__;

	return &__StackLimit - &__bss_end__;
}

uint32_t getFreeHeap(void)
{
	struct mallinfo m = mallinfo();

	return getTotalHeap() - m.uordblks;
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

	printf("free heap:%ld\n", getFreeHeap());
	GVga *gvga = gvga_init(width, height, bits, doubleBuffer, interlaced, NULL);
	gvga_start(gvga);
	setPalette(gvga);

	printf("free heap:%ld\n", getFreeHeap());

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
		sleep_ms(40);
		engine.mainLoop(ticks);
	}
}