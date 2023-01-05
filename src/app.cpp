#include "app.hpp"

// Enable debug information
#define DEBUG_INFO

//Physics
#define P_SPEED 50.f
#define P_ACCELERATION 500
#define B_Y_SPEED 25.f
#define B_INIT_SPEED 50.f
#define FRICTION .05f

// Size defines
#define ARENA_L -85.f
#define ARENA_R 85.f
#define ARENA_U 45.f
#define ARENA_D -45.f
#define P_WIDTH 5
#define P_HEIGHT 24
#define B_DIAMETER 2
#define P_X_DISPLACEMENT 80

// Colours
#define BACKGROUND_COLOUR 0x2A2A2A
#define ARENA_COLOUR 0x222222
#define B_COLOUR 0xffffff
#define P_COLOUR 0xff0000

namespace WinGameAlpha {

// Structs 

struct Player_Data{
    float m_posY = 0;
    float m_dy = 0;
    float m_ddy = 0;
};
struct Ball_Data{
    float m_posY = 0;
    float m_posX = 0;
    float m_dy = 0;
    float m_dx = B_INIT_SPEED;
};

// Globals

Drawer drawer;

static Player_Data player1;
static Player_Data player2;
static Ball_Data ball;

#ifdef DEBUG_INFO
LARGE_INTEGER time1, time2, end_time;
float time_diff_other = 0.f;
float time_diff_render = 0.f;
float performance_frequency;
#endif

// Functions

void app_main(){

}

inline static void render_background(){
    drawer.draw_crect(0,0,ARENA_R*2,ARENA_U*2,ARENA_COLOUR);

    drawer.draw_crect(ball.m_posX,ball.m_posY,B_DIAMETER,B_DIAMETER,B_COLOUR);
    drawer.draw_crect(P_X_DISPLACEMENT,player2.m_posY,P_WIDTH,P_HEIGHT,P_COLOUR);
    drawer.draw_crect(-P_X_DISPLACEMENT,player1.m_posY,P_WIDTH,P_HEIGHT,P_COLOUR);

}

void render_init(){
#ifdef DEBUG_INFO
    {
        LARGE_INTEGER perf;
        QueryPerformanceFrequency(&perf);
        performance_frequency = (float)perf.QuadPart;
    }
#endif
    drawer.clear_screen(BACKGROUND_COLOUR);
    render_background();
}

void render_update(){
    drawer.clear_screen(BACKGROUND_COLOUR);
    render_background();
}

#define APPLY_KINEMATICS_TICK(s,v,a,dt) s = s + v * dt + a*dt*dt*.5f; \
                                        v = v + a * dt - FRICTION*v;

void render_tick(Input& input, float dt){

#ifdef DEBUG_INFO
    std::cout << "FPS: " << 1/dt << std::endl;
    std::cout << "TDO: " << time_diff_other << "\n" << "TDR: " << time_diff_render << std::endl;

    QueryPerformanceCounter(&time1);
#endif

    // Set acceleration
    player1.m_ddy = 0;
    player2.m_ddy = 0;
    if (btn_down(BUTTON_UP)) player1.m_ddy = P_ACCELERATION;
    if (btn_down(BUTTON_DOWN)) player1.m_ddy = -P_ACCELERATION;
    if (btn_down(BUTTON_KUP)) player2.m_ddy = P_ACCELERATION;
    if (btn_down(BUTTON_KDOWN)) player2.m_ddy = -P_ACCELERATION;

    // Apply kinematics
    APPLY_KINEMATICS_TICK(player1.m_posY,player1.m_dy,player1.m_ddy,dt);
    APPLY_KINEMATICS_TICK(player2.m_posY,player2.m_dy,player2.m_ddy,dt);

    ball.m_posX += ball.m_dx * dt;
    ball.m_posY += ball.m_dy * dt;

    // Ball collision players AABB

    if (within_bounds(P_X_DISPLACEMENT - P_WIDTH/2 - B_DIAMETER/2,ball.m_posX,P_X_DISPLACEMENT + P_WIDTH/2 + B_DIAMETER/2) &&
        within_bounds(player2.m_posY-P_HEIGHT/2-B_DIAMETER/2,ball.m_posY,player2.m_posY+P_HEIGHT/2+B_DIAMETER/2)){
        if (ball.m_dx > 0){
            ball.m_dx *= -1;
            if (ball.m_dy == 0) ball.m_dy = (((uint32_t)player2.m_dy & (1 << 31)) ? -B_Y_SPEED : B_Y_SPEED); // conditional on sign of float
        }
    } else if (within_bounds(-P_WIDTH/2 - P_X_DISPLACEMENT - B_DIAMETER/2,ball.m_posX,P_WIDTH/2 - P_X_DISPLACEMENT + B_DIAMETER/2) &&
               within_bounds(player1.m_posY-P_HEIGHT-B_DIAMETER/2,ball.m_posY,player1.m_posY+P_HEIGHT/2+B_DIAMETER/2)) {
        if (ball.m_dx < 0){
            ball.m_dx *= -1;
            if (ball.m_dy == 0) ball.m_dy = ((uint32_t)player1.m_dy & (1 << 31)) ? -B_Y_SPEED : B_Y_SPEED;
        }
    }

    // Bounds of arena collisions
    if (player1.m_posY > ARENA_U-P_HEIGHT/2){
        player1.m_posY = ARENA_U-P_HEIGHT/2;
        player1.m_dy = 0;
    } else if (player1.m_posY < ARENA_D+P_HEIGHT/2){
        player1.m_posY = ARENA_D+P_HEIGHT/2;
        player1.m_dy = 0;
    }
    if (player2.m_posY > ARENA_U-P_HEIGHT/2){
        player2.m_posY = ARENA_U-P_HEIGHT/2;
        player2.m_dy = 0;
    } else if (player2.m_posY < ARENA_D+P_HEIGHT/2){
        player2.m_posY = ARENA_D+P_HEIGHT/2;
        player2.m_dy = 0;
    }
    if (ball.m_posY > ARENA_U-B_DIAMETER/2){
        ball.m_posY = ARENA_U-B_DIAMETER/2;
        ball.m_dy *= -1;
    } else if (ball.m_posY < ARENA_D+B_DIAMETER/2){
        ball.m_posY = ARENA_D+B_DIAMETER/2;
        ball.m_dy *= -1;
    }

    // Render bkg
#ifdef DEBUG_INFO
    QueryPerformanceCounter(&time2);
#endif
    drawer.clear_screen(BACKGROUND_COLOUR);
    render_background();
#ifdef DEBUG_INFO
    QueryPerformanceCounter(&end_time);
    time_diff_other = (float)(time2.QuadPart - time1.QuadPart)/performance_frequency;
    time_diff_render = (float)(end_time.QuadPart - time2.QuadPart)/performance_frequency;
#endif
}

}