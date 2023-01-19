#include "ball.hpp"

namespace WinGameAlpha {

void Ball::tick(float dt){
    if (abs(m_dx) > B_INIT_SPEED){
        boost_timer -= dt;
        m_posX = m_posX + m_dx * dt + m_ddx*dt*dt*.5f;
        m_dx = m_dx + m_ddx * dt - B_DRAG*dt*m_dx;
        if (boost_timer > 0) boost_timer -= dt;
        else {
            m_ddx = 0;
            if (abs(m_dy) < B_Y_SPEED) m_dy = m_dy*2;
        }
    } else {
        m_posX += m_dx * dt;
    }

    m_posY += m_dy * dt;
}

void Ball::onCollision(const collider_type other_type, void* other_collider_ptr, bound_flags active_flags, int other_collider_group){
    if (other_type == COLLIDER_BOUNDARY){
        if (active_flags & (BOUND_TOP | BOUND_BOTTOM)) m_dy *= -1;
        // if (active_flags & (BOUND_LEFT | BOUND_RIGHT)) m_dx *= -1;
    } else if (other_type == OBJECT_COLLIDER){
        m_ddx = 0;
        Collider* other_collider = (Collider*)other_collider_ptr;
        if (active_flags & (BOUND_TOP | BOUND_BOTTOM)) m_dy *= -1;
        if (active_flags & (BOUND_LEFT | BOUND_RIGHT)) {
            m_dx *= -1;
            if (active_flags & BOUND_LEFT) m_ddx = B_ACCELERATION, m_dx = B_INIT_SPEED + 10;
            else m_ddx = -B_ACCELERATION, m_dx = -B_INIT_SPEED - 10;
            m_dy = m_dy/2;
            boost_timer = 0.35f;
        }
    }
}

}