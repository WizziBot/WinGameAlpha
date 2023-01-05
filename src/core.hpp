#pragma once
#include <windows.h>

namespace WinGameAlpha{

struct Render_State {
    int height, width;
    void* memory;
    BITMAPINFO bitmap_info;
};

}