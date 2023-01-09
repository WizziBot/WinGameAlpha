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
    cout << "R: " << (int)restricted_dir << endl;
    if (dir_flags & ACC_UP && !(restricted_dir & ACC_UP)){
        m_ddy += P_ACCELERATION;
        cout << "UP"<< endl;
        restricted_dir = 0;
    }
    if (dir_flags & ACC_DOWN && !(restricted_dir & ACC_DOWN)) {
        m_ddy -= P_ACCELERATION;
        cout << "DOWN"<< endl;
        restricted_dir = 0;
    }
}

void Player::onCollision(const collider_type other_type, void* other_collider_ptr, bound_flags active_flags){
    if (other_type == COLLIDER_BOUNDARY){
        if (active_flags & BOUND_TOP) {
            cout << "Top" << endl;
            m_ddy = -P_ACCELERATION*5;
            restricted_dir = ACC_UP;
        } else if (active_flags & BOUND_BOTTOM) {
            cout << "Bottom" << endl;
            m_ddy = P_ACCELERATION*5;
            restricted_dir = ACC_DOWN;
        }
        cooldown_timer = 0.2;
    }
}

void Player::draw(Drawer* drawer){
    vector<render_rect_properties>::iterator rect;
    for (rect = m_rect_props.begin(); rect != m_rect_props.end(); rect++){
        drawer->draw_rect(m_posX,m_posY,rect->width,rect->height,rect->colour);
    }
}

}