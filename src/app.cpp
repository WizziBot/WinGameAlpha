
#include "app.hpp"
#include "render_objects.hpp"
#include "texture_manager.hpp"
#include "renderer.hpp"
#include "physics.hpp"
#include "common.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "ball.hpp"

#define GOLD_COL 0xBC9928

namespace WinGameAlpha {

// Globals
extern bool running;
bool run_game = false;

shared_ptr<Drawer> drawer;
shared_ptr<Texture_Manager> texture_manager;
shared_ptr<Entity_Physics> physics;
shared_ptr<Player> player1;
shared_ptr<Player> player2;
shared_ptr<Ball> ball;
vector<Collider_Boundary> bounds;
shared_ptr<Text_Object> scorep1;
shared_ptr<Text_Object> scorep2;
int score_counterp1 = 0;
int score_counterp2 = 0;

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
    physics = make_shared<Entity_Physics>();

    // Physics properties

    kinematic_dynamic_properties player1_init = {
        .posX = -P_X_DISPLACEMENT
    };
    kinematic_dynamic_properties player2_init = {
        .posX = P_X_DISPLACEMENT
    };
    kinematic_dynamic_properties ball_init = {
        .dy = B_Y_SPEED,
        .dx = B_INIT_SPEED
    };
    aabb_bounds player_aabb = {
        .half_width = (float)P_WIDTH/2.f,
        .half_height = (float)P_HEIGHT/2.f
    };
    aabb_bounds ball_aabb = {
        .half_width = (float)B_DIAMETER/2.f,
        .half_height = (float)B_DIAMETER/2.f
    };
    aabb_bounds arena_aabb = {
        .half_width = (float)ARENA_R,
        .half_height = (float)ARENA_U
    };

    // Load textures

    int width,height;
    float unit_size;
    float unit_size_2;
    uint32_t* temp_m;

#define ld_texture(path) err = texture_manager->load_texture(&temp_m,&width,&height,&unit_size,path);\
                         WGACHECKERRNO("Could not load " << path,err);\
                         if (err == WGA_FAILURE) return;
                           
    // Players
    ld_texture("./textures/player.wgat")
    texture_manager->rotate_matrix(temp_m,&width,&height,1);
    shared_ptr<Render_Matrix> player1_render_matrix = texture_manager->create_render_matrix(0,0,width,height,temp_m,unit_size,unit_size);
    ld_texture("./textures/player_bounce.wgat")
    texture_manager->rotate_matrix(temp_m,&width,&height,1);
    shared_ptr<Render_Matrix> player1_bounce = texture_manager->create_render_matrix(0,0,width,height,temp_m,unit_size,unit_size);
    
    ld_texture("./textures/player.wgat")
    texture_manager->rotate_matrix(temp_m,&width,&height,1);
    texture_manager->flip_matrix(temp_m,width,height);
    shared_ptr<Render_Matrix> player2_render_matrix = texture_manager->create_render_matrix(0,0,width,height,temp_m,unit_size,unit_size);
    ld_texture("./textures/player_bounce.wgat")
    texture_manager->rotate_matrix(temp_m,&width,&height,1);
    texture_manager->flip_matrix(temp_m,width,height);
    shared_ptr<Render_Matrix> player2_bounce = texture_manager->create_render_matrix(0,0,width,height,temp_m,unit_size,unit_size);
    
    // Ball
    ld_texture("./textures/ball.wgat")
    shared_ptr<Render_Matrix> ball_render_matrix = texture_manager->create_render_matrix(0,0,width,height,temp_m,unit_size,unit_size);
    ld_texture("./textures/ball_bounce.wgat")
    shared_ptr<Render_Matrix> ball_bounce = texture_manager->create_render_matrix(0,0,width,height,temp_m,unit_size,unit_size);
    
    // Arena
    ld_texture("./textures/arena.wgat")
    shared_ptr<Render_Matrix> arena_render_matrix = texture_manager->create_render_matrix(0,0,width,height,temp_m,unit_size,unit_size);
    texture_manager->create_render_object(arena_render_matrix,ARENA_RENDER_LAYER);
    
    // Register render objects
    texture_manager->register_all_objects();

    // Register the arena bounds
    Collider_Boundary arena_bound(0, 0, arena_aabb, BOUND_ALL);
    bounds.push_back(arena_bound);
    vector<Collider_Boundary>::iterator bound;
    for (bound = bounds.begin(); bound != bounds.end(); bound++){
        physics->register_collider_boundary(ARENA_COLLISION_GROUP,bound.base());
    }

    // Initialize game objects
    vector<pair<int,bool>> player_targets; player_targets.push_back(make_pair<int,bool>(ARENA_COLLISION_GROUP,true)); player_targets.push_back(make_pair<int,bool>(B_COLLISION_GROUP,false));
    vector<pair<int,bool>> ball_targets; ball_targets.push_back(make_pair(ARENA_COLLISION_GROUP,true)); ball_targets.push_back(make_pair(P_COLLISION_GROUP,true));
    player1 = make_shared<Player>(physics, drawer, &player1_init, P_COLLISION_GROUP, player_targets, &player_aabb, player1_render_matrix, P_B_RENDER_LAYER);
    player1->append_render_matrix(player1_bounce);
    player2 = make_shared<Player>(physics, drawer, &player2_init, P_COLLISION_GROUP, player_targets, &player_aabb, player2_render_matrix, P_B_RENDER_LAYER);
    player2->append_render_matrix(player2_bounce);
    ball = make_shared<Ball>(physics, drawer, &ball_init, B_COLLISION_GROUP, ball_targets, &ball_aabb, ball_render_matrix, P_B_RENDER_LAYER);
    ball->append_render_matrix(ball_bounce);

    // Character textures and score counter
    WGAERRCHECK(texture_manager->load_character_textures())
    scorep2 = make_shared<Text_Object>(drawer,texture_manager,"0",60,35,1,4,2);
    scorep1 = make_shared<Text_Object>(drawer,texture_manager,"0",-60,35,1,4,2);
    scorep2->set_mask(GOLD_COL);
    scorep1->set_mask(GOLD_COL);
    scorep2->change_text("0");
    scorep1->change_text("0");
    
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

void increment_score(int player){
    if (player == 1){
        score_counterp1++;
        string out = std::to_string(score_counterp1);
        scorep1->change_text(out);
    } else {
        score_counterp2++;
        string out = std::to_string(score_counterp2);
        scorep2->change_text(out);
    }
}

void reset_game(){
    ball->reset_position(RIGHT_DIR);
}
void reset_scores(){
    score_counterp1 = 0;
    score_counterp2 = 0;
    string out = std::to_string(score_counterp2);
    scorep1->change_text(out);
    scorep2->change_text(out);
}

void render_tick(Input& input, float dt){

#ifdef DEBUG_INFO
    std::cout << "FPS: " << 1/dt << std::endl;
    std::cout << "TDO: " << time_diff_other << "\n" << "TDR: " << time_diff_render << std::endl;
    QueryPerformanceCounter(&time1);
#endif

    // Barrier for play button
    if (btn_pressed(BUTTON_PAUSE)) run_game = !run_game;
    // Reset game
    if (btn_ctrl_pressed(BUTTON_RESET)) {
        reset_game();
        reset_scores();
    } else if (btn_pressed(BUTTON_RESET)) reset_game();

    if (run_game){
        // Set acceleration
        if (btn_down(BUTTON_UP)) player1->accelerate(ACC_UP);
        if (btn_down(BUTTON_DOWN)) player1->accelerate(ACC_DOWN);
        if (btn_down(BUTTON_KUP)) player2->accelerate(ACC_UP);
        if (btn_down(BUTTON_KDOWN)) player2->accelerate(ACC_DOWN);

        physics->physics_tick(dt);
    }
    
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