#pragma once
#include "core.hpp"

#define WGAERRCHECK(func) if (func != WGA_SUCCESS) {\
                            running = false;\
                            std::cout << "WinGameAlpha Error at\n" \
                            << #func << std::endl;}
#define WGACHECKERRNO(msg,err_no) if (err_no != WGA_SUCCESS) {\
                            running = false;\
                            std::cout << "WinGameAlpha Error:" \
                            << msg << std::endl;}

namespace WinGameAlpha{

inline int clamp(int min, int val, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

inline float fclamp(float min, float val, float max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

/* Returns true if within bounds, false otherwise */
inline bool within_bounds(float min, float val, float max){
    if (val < max && val > min) return true;
    else return false;
}

}