#include "renderer.hpp"
#include "app.hpp"
#include "utils.hpp"

#define P_SPEED 0.7f

namespace WinGameAlpha {

bool changes = false;

static int player_posX = 0;
static int player_posY = 0;

void app_main(){

}

void render_init(){
    clear_screen(0x111111);
    draw_rect(10,10,50,10,0xffffff);
    draw_rect(10,30,50,10,0xffffff);
    draw_crect(-10,-10,15,15,0x550055);
}

void render_update(){
    clear_screen(0x111111);
    draw_rect(10,10,50,10,0xffffff);
    draw_rect(10,30,50,10,0xffffff);
    draw_crect(-10,-10,15,15,0x550055);
}

void render_tick(Input& input){
    if (!btn_down(BUTTON_UP) && !btn_down(BUTTON_DOWN) && !btn_down(BUTTON_LEFT) && !btn_down(BUTTON_RIGHT)){
        draw_rect(10,10,50,10,0xffffff);
        draw_rect(10,30,50,10,0xffffff);
        draw_crect(-10 + player_posX*P_SPEED,-10 + player_posY*P_SPEED,15,15,0x550055);
        return;
    }
    if (btn_down(BUTTON_UP)){
        player_posY++;
    } 
    if (btn_down(BUTTON_DOWN)){
        player_posY--;
    }
    if (btn_down(BUTTON_RIGHT)){
        player_posX++;
    }
    if (btn_down(BUTTON_LEFT)){
        player_posX--;
    }
    clear_screen(0x111111);
    draw_rect(10,10,50,10,0xff0000);
    draw_rect(10,30,50,10,0xff0000);
    draw_crect(-10 + player_posX*P_SPEED,-10 + player_posY*P_SPEED,15,15,0x0000ff);
    
}

}