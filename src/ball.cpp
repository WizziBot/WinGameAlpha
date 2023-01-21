#include "ball.hpp"

namespace WinGameAlpha {

void Ball::tick(float dt){
    if (abs(m_dx) > B_INIT_SPEED){
        boost_timer -= dt;
        m_posX = m_posX + m_dx * dt + m_ddx*dt*dt*.5f;
        m_dx = m_dx + m_ddx * dt - B_DRAG*dt*m_dx;
        if (boost_timer > 0) boost_timer -= dt;
        else m_ddx = 0;
    } else {
        m_posX += m_dx * dt;
    }

    if (abs(m_dy) > B_Y_SPEED){
        boost_timer2 -= dt;
        m_posY = m_posY + m_dy * dt + m_ddy*dt*dt*.5f;
        m_dy = m_dy + m_ddy * dt - B_Y_DRAG*dt*m_dy;
        if (boost_timer2 > 0) boost_timer2 -= dt;
        else m_ddy = 0;
    } else {
        m_posY += m_dy * dt;
    }

}

void Ball::onCollision(const collider_type other_type, void* other_collider_ptr, bound_flags active_flags, int other_collider_group){
    if (other_type == COLLIDER_BOUNDARY){
        if (active_flags & (BOUND_TOP | BOUND_BOTTOM)) m_dy *= -1;
        // if (active_flags & (BOUND_LEFT | BOUND_RIGHT)) m_dx *= -1;
    } else if (other_type == OBJECT_COLLIDER){
        float other_dy = ((Kinematic_Object*)other_collider_ptr)->getDY();
        if (active_flags & (BOUND_TOP | BOUND_BOTTOM)){
            m_dx *= -1;
            if (active_flags & BOUND_BOTTOM) m_dy += other_dy;
            else m_dy += other_dy;
            if (m_posX > 0) m_dx = -B_INIT_SPEED;
            else m_dx = B_INIT_SPEED;
            m_ddy += other_dy;
            boost_timer2 = 0.3f;
        } else if (active_flags & (BOUND_LEFT | BOUND_RIGHT)){
            if (active_flags & BOUND_LEFT) m_ddx = B_ACCELERATION, m_dx = B_INIT_SPEED + 12, m_dy += other_dy;
            else m_ddx = -B_ACCELERATION, m_dx = -B_INIT_SPEED - 12, m_dy += other_dy;
            m_ddy += other_dy/2;
            boost_timer = 0.35f;
            boost_timer2 = 0.2f;
        }
    }
}

}