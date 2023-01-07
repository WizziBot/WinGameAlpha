#include "core.hpp"

namespace WinGameAlpha{
    
class Player {
public:

Player();
~Player();

/* Kinematics tick */
void apply_physics(float dt);

private:
    float m_posY = 0;
    float m_dy = 0;
    float m_ddy = 0;

    void process_collisions();
};

}