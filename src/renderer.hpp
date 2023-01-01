#pragma once
#include "core.hpp"
#include <stdint.h>

namespace WinGameAlpha{

extern Render_State render_state;
    
/* Clear screen to one colour */
void clear_screen(uint32_t colour);
/* Draw rectangle absolute coordinates*/
void draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour);
/* Draw rectangle relative coordinates
    All parameters as percentage of screen height
*/
void draw_rect(float x, float y, float width, float height, uint32_t colour);
/* Centered version of draw_rect
*/
void draw_crect(float x, float y, float width, float height, uint32_t colour);

}