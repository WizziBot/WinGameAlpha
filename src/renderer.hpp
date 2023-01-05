#pragma once
#include "core.hpp"
#include "utils.hpp"
#include <stdint.h>
#include <math.h>
#include <iostream>

namespace WinGameAlpha{

extern Render_State render_state;
    
/* Clear window to one colour */
void clear_screen(uint32_t colour);
/* Draw rectangle absolute (pixel) coordinates*/
void draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour);
/* Draw rectangle with normalised (to 100) coordinates with respect to the window height
*/
void draw_rect(float x, float y, float width, float height, uint32_t colour);
/* Centered version of draw_rect where 0,0 is the middle of the window */
void draw_crect(float x, float y, float width, float height, uint32_t colour);

}