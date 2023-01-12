#pragma once

#include "app.hpp"

#define BACKGROUND_COLOUR 0x252525
#define ARENA_COLOUR 0x1e1e1e
#define B_COLOUR 0xffffff
#define P_COLOUR 0xf80505

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

}