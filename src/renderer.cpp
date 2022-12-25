#include "renderer.hpp"
#include "utils.hpp"
#include <math.h>
#include <iostream>
namespace WinGameAlpha{

extern Render_State render_state;

void clear_screen(uint32_t colour){
    uint32_t* pixel = (uint32_t*)render_state.memory;
    for (int y = 0; y < render_state.height; y++){
        for (int x = 0; x < render_state.width; x++){
            *pixel++ = colour; 
        }
    }
}

void draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour){
    x0 = clamp(0, x0, render_state.width);
    x1 = clamp(0, x1, render_state.width);
    y0 = clamp(0, y0, render_state.height);
    y1 = clamp(0, y1, render_state.height);

    uint32_t* pixel = (uint32_t*)render_state.memory;
    for (int y = y0; y < y1 ; y++){
        pixel = (uint32_t*)render_state.memory + x0 + y*render_state.width;
        for (int x = x0; x < x1 ; x++){
            *pixel++ = colour; 
        }
    }
}

void draw_rect(float x, float y, float width, float height, uint32_t colour){
    int x0 = floor(render_state.height*(x/100));
    int x1 = floor(render_state.height*(x/100) + render_state.height*(width/100));
    int y0 = floor(render_state.height*(y/100));
    int y1 = floor(render_state.height*(y/100) + render_state.height*(height/100));
    
    draw_rect_px(x0,y0,x1,y1,colour);
}

void draw_crect(float x, float y, float width, float height, uint32_t colour){

    int x0 = floor(render_state.height*(x/100) - render_state.height*(width/200.f) + render_state.width/2.f);
    int x1 = floor(render_state.height*(x/100) + render_state.height*(width/200.f) + render_state.width/2.f);
    int y0 = floor(render_state.height*(y/100) - render_state.height*(height/200.f) + render_state.height/2.f);
    int y1 = floor(render_state.height*(y/100) + render_state.height*(height/200.f) + render_state.height/2.f);

    draw_rect_px(x0,y0,x1,y1,colour);
}

}