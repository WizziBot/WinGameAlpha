#include "player.hpp"

namespace WinGameAlpha {

void Player::tick(float dt){
    if (cooldown_timer > 0) cooldown_timer -= dt;
    m_posY = m_posY + m_dy * dt + m_ddy*dt*dt*.5f;
    m_dy = m_dy + m_ddy * dt - P_DRAG*m_dy;
    m_ddy = 0;
}

void Player::accelerate(acceleration_dir dir_flags){
    if (cooldown_timer <= 0) restricted_dir = 0;
    if (dir_flags & ACC_UP && !(restricted_dir & ACC_UP)){
        m_ddy = P_ACCELERATION;
        restricted_dir = 0;
    }
    if (dir_flags & ACC_DOWN && !(restricted_dir & ACC_DOWN)) {
        m_ddy = -P_ACCELERATION;
        restricted_dir = 0;
    }
}

void Player::onCollision(const collider_type other_type, void* other_collider_ptr, bound_flags active_flags){
    if (other_type == COLLIDER_BOUNDARY){
        if (active_flags & BOUND_TOP) {
            m_dy = 0;
            m_ddy = -P_ACCELERATION*2;
            restricted_dir = ACC_UP;
        } else if (active_flags & BOUND_BOTTOM) {
            m_dy = 0;
            m_ddy = P_ACCELERATION*2;
            restricted_dir = ACC_DOWN;
        }
        cooldown_timer = 0.1;
    }
}

}