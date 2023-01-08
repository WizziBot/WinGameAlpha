#pragma once

#include "ball.hpp"

namespace WinGameAlpha {

void Ball::tick(const float dt){
    m_dy = m_dy + m_ddy * dt;
    m_dx = m_dx + m_ddx * dt;
}

void Ball::onCollision(collider_type other_type, aabb_bounds other_bounds, bound_flags flags){
    if (other_type == COLLIDER_BOUNDARY){
        m_dy = 0;
    }
}

void Ball::draw(){
    vector<render_rect_properties>::iterator rect;
    for (rect = m_rect_props.begin(); rect != m_rect_props.end(); rect++){
        m_drawer->draw_rect(m_posX+rect->x_offset,m_posY+rect->y_offset,rect->width,rect->height,rect->colour);
    }
}

}