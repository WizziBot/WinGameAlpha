#pragma once

#include "core.hpp"
#include "physics.hpp"
#include "renderer.hpp"

namespace WinGameAlpha{
    
class Player : public Kinematic_Object, public Render_Object {
friend class Drawer;
public:

Player();
~Player();

void tick(float dt) override{
    this->m_posY = this->m_posY + this->m_dy * dt + this->m_ddy*dt*dt*.5f;
    this->m_dy = this->m_dy + this->m_ddy * dt - P_DRAG*this->m_dy;
};

void draw() override{

};

private:
    void apply_physics(float dt);
    void process_collisions();
};

}