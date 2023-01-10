#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <memory>

using std::shared_ptr;
using std::make_shared;

// Enable OpenCL
// #define USING_OPENCL

// Enable debug information
// #define DEBUG_INFO

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

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