#include "core.hpp"

namespace WinGameAlpha{

class Ball {
public:

Ball();
~Ball();

/* Kinematics tick */
void apply_physics(float dt);

private:
    float m_posY = 0;
    float m_posX = 0;
    float m_dy = 0;
    float m_dx = B_INIT_SPEED;
    
    void process_collisions();
};

}