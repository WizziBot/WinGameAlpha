#pragma once
#include "core.hpp"
#include "common.hpp"

/* DEFINES */
#define APP_WIDTH 800
#define APP_HEIGHT 450
#define TICK_DELAY 10

namespace WinGameAlpha{

void app_main();

void render_init();

void render_tick(Input& input);

void render_update();

}