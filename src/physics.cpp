#include "core.hpp"
#include "physics.hpp"

namespace WinGameAlpha{

Entity_Physics::~Entity_Physics(){

}

void Entity_Physics::physics_tick(const float dt){
    process_kinematics(dt);
    process_collisions();
}

void Entity_Physics::register_collider_object(int collision_group, Kinematic_Object* kobj){
    if (collision_group == collision_groups.size()){
        Collider_Group new_group;
        vector<Kinematic_Object*> k_objects;
        vector<Collider_Boundary*> bounds;
        k_objects.push_back(kobj);
        new_group.k_objects = k_objects;
        new_group.bounds = bounds;
        collision_groups.push_back(new_group);
    } else{
        collision_groups.at(collision_group).k_objects.push_back(kobj);
    }
}

void Entity_Physics::register_collider_boundary(int collision_group, Collider_Boundary* bound){
    if (collision_group == collision_groups.size()){
        Collider_Group new_group;
        vector<Kinematic_Object*> k_objects;
        vector<Collider_Boundary*> bounds;
        new_group.k_objects = k_objects;
        bounds.push_back(bound);
        new_group.bounds = bounds;
        collision_groups.push_back(new_group);
    } else{
        collision_groups.at(collision_group).bounds.push_back(bound);
    }
}

void Entity_Physics::register_kinematic_object(Kinematic_Object* kobj){
    kinematic_objects.push_back(kobj);
}

void Entity_Physics::process_kinematics(const float dt){
    vector<Kinematic_Object*>::iterator kobj;
    for (kobj=kinematic_objects.begin(); kobj != kinematic_objects.end(); kobj++){
        (**kobj).tick(dt);
    }
}

void Entity_Physics::process_collisions(){
    vector<Collider_Group>::iterator group;
    vector<Kinematic_Object*>::iterator kobj;
    vector<Kinematic_Object*>::iterator kobj_other;
    vector<Collider_Boundary*>::iterator bound;
    aabb_bounds collider1;
    aabb_bounds collider2;
    bound_flags flags;
    bound_flags out_flag;
    float kobjX;
    float kobjY;
    float kobj_otherX;
    float kobj_otherY;
    float boundX;
    float boundY;
    for (group = collision_groups.begin(); group != collision_groups.end(); group++){
        for(kobj = (*group).k_objects.begin(); kobj != (*group).k_objects.end(); kobj++){
            kobjX = (*kobj)->m_posX;
            kobjY = (*kobj)->m_posY;
            collider1 = (*kobj)->getCollider().get()->m_bounds;
            for (kobj_other = (*group).k_objects.begin(); kobj_other != (*group).k_objects.end(); kobj_other++){
                if (kobj == kobj_other) continue;
                kobj_otherX = (*kobj_other)->m_posX;
                kobj_otherY = (*kobj_other)->m_posY;
                collider2 = (*(*kobj_other)->getCollider()).m_bounds;
                // check the absolute differences in x and y coords compared the the sum of the distances to the edge from the centre
                if (abs(kobjX-kobj_otherX) < collider2.half_width+collider1.half_width && kobjY+collider1.half_height > kobj_otherY-collider2.half_height) out_flag = BOUND_TOP;
                if (abs(kobjX-kobj_otherX) < collider2.half_width+collider1.half_width && kobjY-collider1.half_height < kobj_otherY+collider2.half_height) out_flag = BOUND_BOTTOM;
                if (abs(kobjY-kobj_otherY) < collider2.half_height+collider1.half_height && kobjX+collider1.half_width > kobj_otherY-collider2.half_width) out_flag = BOUND_LEFT;
                if (abs(kobjY-kobj_otherY) < collider2.half_height+collider1.half_height && kobjX-collider1.half_width < kobj_otherY+collider2.half_width) out_flag = BOUND_TOP;
                (*kobj)->onCollision(OBJECT_COLLIDER,(void*)kobj_other.base(),0);
            }
            for (bound = (*group).bounds.begin(); bound != (*group).bounds.end(); bound++){
                boundX = (*bound)->m_x_offset;
                boundY = (*bound)->m_y_offset;
                collider2 = (*bound)->m_bounds;
                flags = (*bound)->m_flags;
                // check if bounding collider has the directional flag and then check whether collision
                if (flags & BOUND_TOP && kobjY+collider1.half_height > boundY+collider2.half_height) out_flag = BOUND_TOP;
                else if (flags & BOUND_BOTTOM && kobjY-collider1.half_height < boundY-collider2.half_height) out_flag = BOUND_BOTTOM;
                else if (flags & BOUND_LEFT && kobjX-collider1.half_width < boundX-collider2.half_width) out_flag = BOUND_LEFT;
                else if (flags & BOUND_RIGHT && kobjX+collider1.half_width > boundX+collider2.half_width) out_flag = BOUND_RIGHT;
                else continue; // do not to call oncollsion if none of the conditions met
                (*kobj)->onCollision(COLLIDER_BOUNDARY,(void*)bound.base(),out_flag);
            }
        }
    }
}

Kinematic_Object::Kinematic_Object(shared_ptr<Entity_Physics> physics, kinematic_initial_properties* init_prop, int collision_group, aabb_bounds* bound_data){
    if (collision_group > physics->getCollisionGoupsSize()) throw std::invalid_argument("Invalid index for collision_group: Collision groups must be contiguous.");
    if (physics == NULL || init_prop == NULL || bound_data == NULL)
    if (init_prop != NULL){
        m_posY = init_prop->posY;
        m_posX = init_prop->posX;
        m_dy = init_prop->dy;
        m_dx = init_prop->dx;
        m_ddy = init_prop->ddy;
        m_ddx = init_prop->ddx;
    }
    m_collider = make_shared<Collider>(bound_data);

    physics->register_kinematic_object(this);
    physics->register_collider_object(collision_group,this);
}

Kinematic_Object::Kinematic_Object(shared_ptr<Entity_Physics> physics, kinematic_initial_properties* init_prop)
: m_posY(init_prop->posY), m_posX(init_prop->posX), m_dy(init_prop->dy), m_dx(init_prop->dx), m_ddy(init_prop->ddy), m_ddx(init_prop->ddx){

}

}