
#include "textures.hpp"
#include "app.hpp"

namespace WinGameAlpha {

// Globals
extern bool running;

shared_ptr<Drawer> drawer;
shared_ptr<Texture_Manager> texture_manager;
shared_ptr<Entity_Physics> physics;
shared_ptr<Player> player1;
shared_ptr<Player> player2;
shared_ptr<Ball> ball;
vector<Collider_Boundary> bounds;

#ifdef DEBUG_INFO
LARGE_INTEGER time1, time2, end_time;
float time_diff_other = 0.f;
float time_diff_render = 0.f;
float performance_frequency;
#endif

// Functions

void render_init(){
    wga_err err;

    // Init drawer
    drawer = make_shared<Drawer>(&err);
    if (err != WGA_SUCCESS){
        WGACHECKERRNO("Failed to instantiate drawer.",err);
        return;
    }
    texture_manager = make_shared<Texture_Manager>(drawer);
    if (err != WGA_SUCCESS){
        WGACHECKERRNO("Failed to instantiate texture_manager.",err);
        return;
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

    Render_Matrix* player_render_matrix = texture_manager->create_render_matrix(0,0,1,5,player_matrix,P_HEIGHT/5,P_HEIGHT/5);
    Render_Matrix* ball_render_matrix = texture_manager->create_render_matrix(0,0,1,1,ball_matrix,B_DIAMETER,B_DIAMETER);
    Render_Matrix* arena_render_matrix = texture_manager->create_render_matrix(0,0,1,1,arena_matrix,ARENA_R*2,ARENA_U*2);
    WGAERRCHECK(texture_manager->create_render_object(arena_render_matrix,ARENA_RENDER_LAYER));
    
    Collider_Boundary arena_bound(0, 0, arena_aabb, BOUND_TOP | BOUND_BOTTOM);
    bounds.push_back(arena_bound);
    // register the arena bounds
    vector<Collider_Boundary>::iterator bound;
    for (bound = bounds.begin(); bound != bounds.end(); bound++){
        physics->register_collider_boundary(ARENA_COLLISION_GROUP,bound.base());
    }
    vector<int> player_targets(1,ARENA_COLLISION_GROUP);
    vector<int> ball_targets; ball_targets.push_back(ARENA_COLLISION_GROUP); ball_targets.push_back(P_COLLISION_GROUP);
    player1 = make_shared<Player>(physics, drawer, &player1_init, P_COLLISION_GROUP, player_targets, &player_aabb, player_render_matrix, P_B_RENDER_LAYER);
    player2 = make_shared<Player>(physics, drawer, &player2_init, P_COLLISION_GROUP, player_targets, &player_aabb, player_render_matrix, P_B_RENDER_LAYER);
    ball = make_shared<Ball>(physics, drawer, &ball_init, B_COLLISION_GROUP, ball_targets, &ball_aabb, ball_render_matrix, P_B_RENDER_LAYER);
    
    drawer->set_background_colour(BACKGROUND_COLOUR);
    drawer->draw_objects();

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