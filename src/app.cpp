#include "app.hpp"
#include <iostream>

#define BLOCK_HEIGHT 24

#define P_SPEED 50.f
#define B_INIT_SPEED 50.f
#define FRICTION .05f

#define ARENA_L -85.f
#define ARENA_R 85.f
#define ARENA_U 45.f
#define ARENA_D -45.f

#define P_WIDTH 5
#define P_X_DISPLACEMENT 80

namespace WinGameAlpha {

struct Player_Data{
    float m_posY = 0;
    float m_dy = 0;
    float m_ddy = 0;
};
struct Ball_Data{
    float m_posY = 0;
    float m_posX = 0;
    float m_dy = 0;
    float m_ddy = 0;
    float m_dx = B_INIT_SPEED;
    float m_ddx = 0;
};

bool changes = false;

static Player_Data player1;
static Player_Data player2;
static Ball_Data ball;

void app_main(){

}

inline static void render_background(){
    draw_crect(0,0,ARENA_R*2,ARENA_U*2,0x222222);

    draw_crect(ball.m_posX,ball.m_posY,2,2,0xffffff);
    draw_crect(P_X_DISPLACEMENT,player2.m_posY,P_WIDTH,BLOCK_HEIGHT,0xff0000);
    draw_crect(-P_X_DISPLACEMENT,player1.m_posY,P_WIDTH,BLOCK_HEIGHT,0xff0000);

}

void render_init(){
    clear_screen(0x333333);
    render_background();
}

void render_update(){
    clear_screen(0x333333);
    render_background();
}

void render_tick(Input& input, float dt){

    // Set acceleration
    player1.m_ddy = 0;
    player2.m_ddy = 0;
    if (btn_down(BUTTON_UP)) player1.m_ddy = 500;
    if (btn_down(BUTTON_DOWN)) player1.m_ddy = -500;
    if (btn_down(BUTTON_KUP)) player2.m_ddy = 500;
    if (btn_down(BUTTON_KDOWN)) player2.m_ddy = -500;

    // Equations of motion
    player1.m_posY = player1.m_posY + player1.m_dy * dt + player1.m_ddy*dt*dt*.5f;
    player1.m_dy = player1.m_dy + player1.m_ddy * dt - FRICTION*player1.m_dy;
    player2.m_posY = player2.m_posY + player2.m_dy * dt + player2.m_ddy*dt*dt*.5f;
    player2.m_dy = player2.m_dy + player2.m_ddy * dt - FRICTION*player2.m_dy;

    ball.m_posX += ball.m_dx * dt;
    ball.m_posY += ball.m_dy * dt;

    // Ball collision

    if (ball.m_posX > P_X_DISPLACEMENT - P_WIDTH/2){
        ball.m_posX = P_X_DISPLACEMENT - P_WIDTH/2;
        ball.m_dx *= -1;
    } else if (ball.m_posX < P_WIDTH/2 - P_X_DISPLACEMENT) {
        ball.m_posX = P_WIDTH/2 - P_X_DISPLACEMENT;
        ball.m_dx *= -1;
    }

    // Bounds of arena
    if (player1.m_posY > ARENA_U-BLOCK_HEIGHT/2){
        player1.m_posY = ARENA_U-BLOCK_HEIGHT/2;
        player1.m_dy = 0;
    } else if (player1.m_posY < ARENA_D+BLOCK_HEIGHT/2){
        player1.m_posY = ARENA_D+BLOCK_HEIGHT/2;
        player1.m_dy = 0;
    }
    if (player2.m_posY > ARENA_U-BLOCK_HEIGHT/2){
        player2.m_posY = ARENA_U-BLOCK_HEIGHT/2;
        player2.m_dy = 0;
    } else if (player2.m_posY < ARENA_D+BLOCK_HEIGHT/2){
        player2.m_posY = ARENA_D+BLOCK_HEIGHT/2;
        player2.m_dy = 0;
    }

    clear_screen(0x333333);
    render_background();

}

}