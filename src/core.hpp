#pragma once
#include <windows.h>

namespace WinGameAlpha{

struct Render_State {
    int height, width;
    void* memory;
    BITMAPINFO bitmap_info;
};

enum wga_err {
    WGA_SUCCESS,
    WGA_FAILURE
};

}