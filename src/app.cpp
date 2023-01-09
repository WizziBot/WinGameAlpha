#include "app.hpp"

// Rendering
#define ARENA_RENDER_LAYER 0
#define P_B_RENDER_LAYER 1

// Physics
#define P_SPEED 50.f
#define P_ACCELERATION 500
#define B_Y_SPEED 25.f
#define B_INIT_SPEED 50.f
#define DRAG .05f
#define GAME_COLLISION_GROUP 0

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

// Globals
extern bool running;

shared_ptr<Drawer> drawer;
shared_ptr<Entity_Physics> physics;
shared_ptr<Player> player1;
shared_ptr<Player> player2;
shared_ptr<Ball> ball;
vector<Collider_Boundary> bounds;
vector<shared_ptr<Render_Object>> render_objects;

// static Player_Data player1;
// static Player_Data player2;
// static Ball_Data ball;

#ifdef DEBUG_INFO
LARGE_INTEGER time1, time2, end_time;
float time_diff_other = 0.f;
float time_diff_render = 0.f;
float performance_frequency;
#endif

// Functions

// inline static void render_background(){
//     drawer->draw_rect(0,0,ARENA_R*2,ARENA_U*2,ARENA_COLOUR);
//     drawer->draw_rect(ball.m_posX,ball.m_posY,B_DIAMETER,B_DIAMETER,B_COLOUR);
//     drawer->draw_rect(-P_X_DISPLACEMENT,player1.m_posY,P_WIDTH,P_HEIGHT,P_COLOUR);
//     drawer->draw_rect(P_X_DISPLACEMENT,player2.m_posY,P_WIDTH,P_HEIGHT,P_COLOUR);
// #ifdef USING_OPENCL
//     drawer->cl_draw_finish();
// #endif
// }

void app_cleanup(){
    drawer.reset();
}

void render_init(){
    wga_err err;

    // Init drawer
    drawer = make_shared<Drawer>(err);
    if (err != WGA_SUCCESS){
        drawer.reset();
#ifdef USING_OPENCL
        WGACHECKERRNO("Failed to instantiate drawer.",err);
#endif
    }

    // Register objects
    physics = make_shared<Entity_Physics>();

    kinematic_initial_properties player1_init = {
        .posX = -P_X_DISPLACEMENT
    };
    kinematic_initial_properties player2_init = {
        .posX = P_X_DISPLACEMENT
    };
    kinematic_initial_properties ball_init = {
        .dy = B_Y_SPEED,
        .dx = B_INIT_SPEED
    };
    aabb_bounds player_aabb = {
        .half_width = P_WIDTH/2,
        .half_height = P_HEIGHT/2
    };
    aabb_bounds ball_aabb = {
        .half_width = B_DIAMETER/2,
        .half_height = B_DIAMETER/2
    };
    aabb_bounds arena_aabb = {
        .half_width = ARENA_R,
        .half_height = ARENA_U
    };
    render_rect_properties arena_rect = {
        .width = ARENA_R*2,
        .height = ARENA_U*2,
        .colour = ARENA_COLOUR
    };
    render_rect_properties player_rect = {
        .width = P_WIDTH,
        .height = P_HEIGHT,
        .colour = P_COLOUR
    };
    render_rect_properties ball_rect = {
        .width = B_DIAMETER,
        .height = B_DIAMETER,
        .colour = B_COLOUR
    };
    shared_ptr<Render_Object> arena_robj = make_shared<Render_Object>(drawer, &arena_rect,1,ARENA_RENDER_LAYER,false);
    render_objects.push_back(arena_robj);
    
    player1 = make_shared<Player>(physics, drawer, &player1_init, GAME_COLLISION_GROUP, &player_aabb, &player_rect, 1, P_B_RENDER_LAYER);
    player2 = make_shared<Player>(physics, drawer, &player2_init, GAME_COLLISION_GROUP, &player_aabb, &player_rect, 1, P_B_RENDER_LAYER);
    ball = make_shared<Ball>(physics, drawer, &ball_init, GAME_COLLISION_GROUP, &ball_aabb, &ball_rect, 1, P_B_RENDER_LAYER);
    Collider_Boundary arena_bound(0, 0, arena_aabb, BOUND_TOP | BOUND_BOTTOM);
    bounds.push_back(arena_bound);
    // register the arena bounds
    vector<Collider_Boundary>::iterator bound;
    for (bound = bounds.begin(); bound != bounds.end(); bound++){
        physics->register_collider_boundary(0,bound.base());
    }

    drawer->clear_screen(BACKGROUND_COLOUR);

#ifdef DEBUG_INFO
    {
        LARGE_INTEGER perf;
        QueryPerformanceFrequency(&perf);
        performance_frequency = (float)perf.QuadPart;
    }
#endif
}

void render_update(){
#ifdef USING_OPENCL
    WGAERRCHECK(drawer->cl_resize());
#endif
    // drawer->clear_screen(BACKGROUND_COLOUR);
    // render_background();
}

void render_tick(Input& input, float dt){

#ifdef DEBUG_INFO
    std::cout << "FPS: " << 1/dt << std::endl;
    std::cout << "TDO: " << time_diff_other << "\n" << "TDR: " << time_diff_render << std::endl;
    QueryPerformanceCounter(&time1);
#endif

    // Set acceleration
    if (btn_down(BUTTON_UP)) player1->accelerate(ACC_UP);
    if (btn_down(BUTTON_DOWN)) player1->accelerate(ACC_DOWN);
    if (btn_down(BUTTON_KUP)) player2->accelerate(ACC_UP);
    if (btn_down(BUTTON_KDOWN)) player2->accelerate(ACC_DOWN);

    physics->physics_tick(dt);
    
    #ifdef DEBUG_INFO
    QueryPerformanceCounter(&time2);
    if (!running) return;
#endif
    drawer->draw_objects();
#ifdef DEBUG_INFO
    QueryPerformanceCounter(&end_time);
    time_diff_other = (float)(time2.QuadPart - time1.QuadPart)/performance_frequency;
    time_diff_render = (float)(end_time.QuadPart - time2.QuadPart)/performance_frequency;
#endif
}

}