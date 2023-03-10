#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <utility>

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::pair;
using std::make_pair;

// Enable OpenCL
// #define USING_OPENCL

// Enable debug information
// #define DEBUG_INFO

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::list;

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