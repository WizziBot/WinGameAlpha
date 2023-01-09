#pragma once

#include "core.hpp"
#include "renderer.hpp"
#include "common.hpp"
#include "utils.hpp"
#include "physics.hpp"
#include "player.hpp"
#include "ball.hpp"


namespace WinGameAlpha{

extern Render_State render_state;

/* Render on window start */
void render_init();
/* Render on gameloop cycle */
void render_tick(Input& input, float dt);
/* Render on resize event */
void render_update();
/* Cleanup */
void app_cleanup();

}