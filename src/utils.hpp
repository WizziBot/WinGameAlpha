#pragma once

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

/* Returns 0 if within bounds, 1 if below min and 2 if above max */
inline bool within_bounds(float min, float val, float max){
    if (val > max || val < min) return false;
    else return true;
}

}