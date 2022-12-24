#include "renderer.hpp"
#include "app.hpp"

namespace WinGameAlpha {

void app_main(){

}

void render_init(){
    clear_screen(0x111111);
    draw_rect(10,10,50,10,0xffffff);
    draw_rect(10,30,50,10,0xffffff);
    draw_crect(-10,-10,20,20,0x550055);
}

void render_update(){
    clear_screen(0x111111);
    draw_rect(10,10,50,10,0xffffff);
    draw_rect(10,30,50,10,0xffffff);
    draw_crect(-10,-10,20,20,0x550055);
}

void render_tick(){

}

}