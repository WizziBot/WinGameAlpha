#pragma once
#include <windows.h>
#include <iostream>

// Using OpenCL macro
// #define USING_OPENCL

using std::cout;
using std::cerr;
using std::endl;

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