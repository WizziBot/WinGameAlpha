#pragma once

#include "app.hpp"

#define BACKGROUND_COLOUR 0x2A2A2A
#define ARENA_COLOUR 0x222222
#define B_COLOUR 0xffffff
#define P_COLOUR 0xf80505

// struct render_rect_properties{
//     float x_offset=0;
//     float y_offset=0;
//     float width;
//     float height;
//     uint32_t colour;
// };

namespace WinGameAlpha {

// notice that player collider height will need to be adjusted to this
uint32_t player_matrix[] {
    0xff0000,
    0xff0000,
    0xff0000,
    0xff0000,
    0xff0000
};
// single square matrices
uint32_t ball_matrix[] {
    B_COLOUR
};
uint32_t arena_matrix[] {
    ARENA_COLOUR
};

Render_Matrix player_render_matrix(0,0,1,5,player_matrix,4,4);
Render_Matrix ball_render_matrix(0,0,1,1,ball_matrix,B_DIAMETER,B_DIAMETER);
Render_Matrix arena_render_matrix(0,0,1,1,arena_matrix,ARENA_R*2,ARENA_U*2);

}