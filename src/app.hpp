#pragma once
#include "core.hpp"
#include "renderer.hpp"
#include "common.hpp"
#include "utils.hpp"

/* DEFINES */
#define APP_WIDTH 800
#define APP_HEIGHT 450
#define TICK_DELAY 10

namespace WinGameAlpha{

extern Render_State render_state;

void app_main();

/* Render on window start */
void render_init();
/* Render on gameloop cycle */
void render_tick(Input& input, float dt);
/* Render on resize event */
void render_update();

}