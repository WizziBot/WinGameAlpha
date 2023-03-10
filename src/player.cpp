#include "player.hpp"
#include <math.h>

namespace WinGameAlpha {

inline static float shake_animation(float t){
    float val = (2/t)*sinf(100/(t*0.35+1));
    val = clamp(-14,val,14);
    return val*0.5;
}

void Player::tick(float dt){
    if (cooldown_timer > 0) cooldown_timer -= dt;
    if (cooldown_timer2 > 0) {
        cooldown_timer2 -= dt;
        m_ddy = rebound_ddy;
    }
    m_posY = m_posY + m_dy * dt + m_ddy*dt*dt*.5f;
    m_dy = m_dy + m_ddy * dt - P_DRAG*dt*m_dy;
    m_ddy = 0;
    if (shaking){
        if (shake_timer < 5){
            m_render_matrix->edit_matrix_offset(0,shake_animation(shake_timer));
            shake_timer += dt*3;
        } else {
            m_render_matrix->edit_matrix_offset(0,0);
            shaking = false;
        }
    }
    if (shaking2){
        if (shake_timer2 > 0){
            shake_timer2 -= dt;

        } else {
            switch_active_matrix(0);
        }
    }
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

void Player::onCollision(const collider_type other_type, void* other_collider_ptr, bound_flags active_flags, int other_collider_group){
    if (other_type == COLLIDER_BOUNDARY){
        if (active_flags & BOUND_TOP) {
            m_dy = -1;
            rebound_ddy = -C_ACCELERATION;
            restricted_dir = ACC_UP;
        } else if (active_flags & BOUND_BOTTOM) {
            m_dy = 1;
            rebound_ddy = C_ACCELERATION;
            restricted_dir = ACC_DOWN;
        }
        cooldown_timer = .1f;
        cooldown_timer2 = .02f;
    } else if (other_type == OBJECT_COLLIDER){
        shake_timer = 0.f;
        shake_timer2 = 1.f;
        shaking = true;
        shaking2 = true;
        switch_active_matrix(1);
    }
}

}