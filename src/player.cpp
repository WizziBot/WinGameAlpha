#include "player.hpp"

namespace WinGameAlpha {

void Player::tick(const float dt){
    m_posY = m_posY + m_dy * dt + m_ddy*dt*dt*.5f;
    m_dy = m_dy + m_ddy * dt - P_DRAG*m_dy;
    m_ddy = 0;
}

void Player::accelerate(acceleration_dir dir_flags, float magnitude){
    if (dir_flags & ACC_UP){
        m_ddy += magnitude;
    } else if (dir_flags & ACC_DOWN) {
        m_ddy -= magnitude;
    }
}

void Player::onCollision(const collider_type other_type, void* other_collider_ptr, bound_flags active_flags){
    if (other_type == COLLIDER_BOUNDARY){
        Collider_Boundary* other_collider = (Collider_Boundary*)other_collider_ptr;
        const aabb_bounds bounds = m_collider->getBounds();
        const aabb_bounds other_bounds = other_collider->getBounds();
        const float y_offset = other_collider->getYoffset();
        m_dy = 0;
        if (active_flags & BOUND_TOP) m_posY = y_offset + other_bounds.half_height - bounds.half_height;
        else if (active_flags & BOUND_BOTTOM) m_posY = y_offset - other_bounds.half_height + bounds.half_height;
    }
}

void Player::draw(Drawer* drawer){
    vector<render_rect_properties>::iterator rect;
    for (rect = m_rect_props.begin(); rect != m_rect_props.end(); rect++){
        drawer->draw_rect(m_posX+rect->x_offset,m_posY+rect->y_offset,rect->width,rect->height,rect->colour);
    }
}

}