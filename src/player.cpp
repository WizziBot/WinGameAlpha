#include "player.hpp"

namespace WinGameAlpha {

void Player::tick(const float dt){
    m_posY = m_posY + m_dy * dt + m_ddy*dt*dt*.5f;
    m_dy = m_dy + m_ddy * dt - P_DRAG*m_dy;
}

void Player::onCollision(collider_type other_type, aabb_bounds other_bounds, bound_flags flags){
    if (other_type == COLLIDER_BOUNDARY){
        m_dy = 0;
        if (flags & BOUND_TOP) m_posY = other_bounds.half_height - m_collider->getBounds().half_height;
        else if (flags & BOUND_BOTTOM) m_posY = other_bounds.half_height - m_collider->getBounds().half_height;
    }
}

void Player::draw(){
    vector<render_rect_properties>::iterator rect;
    for (rect = m_rect_props.begin(); rect != m_rect_props.end(); rect++){
        m_drawer->draw_rect(m_posX+rect->x_offset,m_posY+rect->y_offset,rect->width,rect->height,rect->colour);
    }
}

}