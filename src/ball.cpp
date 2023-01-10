#include "ball.hpp"

namespace WinGameAlpha {

void Ball::tick(float dt){
    m_posY += m_dy * dt;
    m_posX += m_dx * dt;
}

void Ball::onCollision(const collider_type other_type, void* other_collider_ptr, bound_flags active_flags){
    if (other_type == COLLIDER_BOUNDARY){
        if (active_flags & (BOUND_TOP | BOUND_BOTTOM)) m_dy *= -1;
        // if (active_flags & (BOUND_LEFT | BOUND_RIGHT)) m_dx *= -1;
    } else if (other_type == OBJECT_COLLIDER){
        Collider* other_collider = (Collider*)other_collider_ptr;
        if (active_flags & (BOUND_TOP | BOUND_BOTTOM)) m_dy *= -1;
        if (active_flags & (BOUND_LEFT | BOUND_RIGHT)) m_dx *= -1;
    }
}

}