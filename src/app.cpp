#include "renderer.hpp"
#include "app.hpp"
#include "utils.hpp"
#include <iostream>

#define P_SPEED 50.f

namespace WinGameAlpha {

bool changes = false;

static float player_posX = 0;
static float player_posY = 0;

void app_main(){

}

void render_init(){
    clear_screen(0x333333);
    draw_crect(0,0,120,80,0x222222);
}

void render_update(){
    clear_screen(0x333333);
    draw_crect(0,0,120,80,0x222222);
    draw_rect(10,10,50,10,0xffffff);
    draw_rect(10,30,50,10,0xffffff);
    draw_crect(player_posX,player_posY,15,15,0x550055);
}

void render_tick(Input& input, float dt){
    if (!btn_down(BUTTON_UP) && !btn_down(BUTTON_DOWN) && !btn_down(BUTTON_LEFT) && !btn_down(BUTTON_RIGHT)){
        draw_rect(10,10,50,10,0xffffff);
        draw_rect(10,30,50,10,0xffffff);
        draw_crect(player_posX,player_posY,15,15,0x550055);
        return;
    }

    // std::cout << "dt: " << dt << " pY: " << player_posY << " pX:" << player_posX << std::endl;

    if (btn_down(BUTTON_UP)) player_posY += P_SPEED*dt;
    if (btn_down(BUTTON_DOWN)) player_posY -= P_SPEED*dt;
    if (btn_down(BUTTON_RIGHT)) player_posX += P_SPEED*dt;
    if (btn_down(BUTTON_LEFT)) player_posX -= P_SPEED*dt;

    clear_screen(0x333333);
    draw_crect(0,0,120,80,0x222222);
    draw_rect(10,10,50,10,0xff0000);
    draw_rect(10,30,50,10,0xff0000);
    draw_crect(player_posX,player_posY,15,15,0x0000ff);
    
}

}