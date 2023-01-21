#pragma once

#include "app.hpp"

#define BACKGROUND_COLOUR 0x252525
#define ARENA_COLOUR 0x1e1e1e
#define B_COLOUR 0xffffff
#define P_COLOUR 0xf80505

namespace WinGameAlpha {

// Default textures

// notice that player collider height will need to be adjusted to this
uint32_t dplayer_matrix[] {
    0xff0000,
    0xff0000,
    0xff0000,
    0xff0000,
    0xff0000
};
// single square matrices
uint32_t dball_matrix[] {
    B_COLOUR
};
uint32_t darena_matrix[] {
    ARENA_COLOUR
};

}