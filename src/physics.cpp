#include "core.hpp"
#include "physics.hpp"

#define PHYERR(msg) {cout << "Physics Error: " << msg << endl; \
                    return WGA_FAILURE;}

namespace WinGameAlpha{

extern bool running;

Entity_Physics::~Entity_Physics(){

}

void Entity_Physics::physics_tick(const float dt){
    process_kinematics(dt);
    process_collisions();
}

wga_err Entity_Physics::register_collider_object(int collision_group, Kinematic_Object* kobj){
    if (collision_group > collision_groups.size()){
        PHYERR("Invalid index for collision_group: Collision groups must be contiguous.")
    } else if (collision_group == collision_groups.size()){
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
    return WGA_SUCCESS;
}

wga_err Entity_Physics::register_collider_boundary(int collision_group, Collider_Boundary* bound){
    if (collision_group > collision_groups.size()){
        PHYERR("Invalid index for collision_group: Collision groups must be contiguous.")
    } else if (collision_group == collision_groups.size()){
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
    return WGA_SUCCESS;
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
    // sort target groups
    vector<Collider_Group>::iterator group;
    vector<Collider_Group>::iterator target_group;
    vector<int> target_groups;
    vector<int>::iterator target_group_id;
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
    bool hard_collision;
    bool collided = false;
    for (group = collision_groups.begin(); group != collision_groups.end(); group++){
        for(kobj = (*group).k_objects.begin(); kobj != (*group).k_objects.end(); kobj++){
            kobjX = (*kobj)->m_posX;
            kobjY = (*kobj)->m_posY;
            collider1 = (*kobj)->getCollider().get()->m_bounds;
            hard_collision = (*kobj)->getCollider().get()->hard_collider;
            target_groups = (*kobj)->getCollider().get()->m_target_collision_groups;
            target_group_id = target_groups.begin();
            for(target_group = collision_groups.begin()+(*target_group_id); target_group_id != target_groups.end();
            target_group_id++, target_group = collision_groups.begin()+(*target_group_id)){
                for (kobj_other = (*target_group).k_objects.begin(); kobj_other != (*target_group).k_objects.end(); kobj_other++){
                    if (kobj == kobj_other) continue;
                    kobj_otherX = (*kobj_other)->m_posX;
                    kobj_otherY = (*kobj_other)->m_posY;
                    collider2 = (*(*kobj_other)->getCollider()).m_bounds;
                    if (abs(kobjX-kobj_otherX) < collider2.half_width+collider1.half_width &&
                        abs(kobjY-kobj_otherY) < collider1.half_height+collider2.half_height){
                        if (kobjY - collider1.half_height < kobj_otherY - collider2.half_height){
                            out_flag = BOUND_TOP;
                            if (hard_collision) (*kobj)->m_posY = kobj_otherY - collider1.half_height - collider2.half_height;
                        } else if (kobjX + collider1.half_width > kobj_otherX + collider2.half_width){
                            out_flag = BOUND_LEFT;
                            if (hard_collision) (*kobj)->m_posX = kobj_otherX + collider1.half_width + collider2.half_width;
                        }
                        if (kobjY + collider1.half_height > kobj_otherY + collider2.half_height){
                            out_flag = BOUND_BOTTOM;
                            if (hard_collision) (*kobj)->m_posY = kobj_otherY + collider1.half_height + collider2.half_height;
                        }
                        else if (kobjX - collider1.half_width < kobj_otherX - collider2.half_width){
                            out_flag = BOUND_RIGHT;
                            if (hard_collision) (*kobj)->m_posX = kobj_otherX - collider1.half_width - collider2.half_width;
                        }
                        (*kobj)->onCollision(OBJECT_COLLIDER,(void*)kobj_other.base(),out_flag);
                    }
                }
                for (bound = (*target_group).bounds.begin(); bound != (*target_group).bounds.end(); bound++){
                    boundX = (*bound)->m_x_offset;
                    boundY = (*bound)->m_y_offset;
                    collider2 = (*bound)->m_bounds;
                    flags = (*bound)->m_flags;
                    collided = false;
                    if (flags & BOUND_TOP && kobjY+collider1.half_height > boundY+collider2.half_height){
                        out_flag = BOUND_TOP;
                        if (hard_collision) (*kobj)->m_posY = boundY - collider1.half_height + collider2.half_height;
                        collided = true;
                    } else if (flags & BOUND_LEFT && kobjX-collider1.half_width < boundX-collider2.half_width){
                        out_flag = BOUND_LEFT;
                        if (hard_collision) (*kobj)->m_posX = boundX + collider1.half_width - collider2.half_width;
                        collided = true;
                    }
                    if (flags & BOUND_BOTTOM && kobjY-collider1.half_height < boundY-collider2.half_height){
                        out_flag = BOUND_BOTTOM;
                        if (hard_collision) (*kobj)->m_posY = boundY + collider1.half_height - collider2.half_height;
                        collided = true;
                    }
                    else if (flags & BOUND_RIGHT && kobjX+collider1.half_width > boundX+collider2.half_width){
                        out_flag = BOUND_RIGHT;
                        if (hard_collision) (*kobj)->m_posX = boundX - collider1.half_width + collider2.half_width;
                        collided = true;
                    }
                    if (collided){
                        (*kobj)->onCollision(COLLIDER_BOUNDARY,(void*)bound.base(),out_flag);
                    }
                }
            }
        }
    }
}

Kinematic_Object::Kinematic_Object(shared_ptr<Entity_Physics> physics, kinematic_initial_properties* init_prop,
int collision_group, aabb_bounds* bound_data, bool is_hard_collider, vector<int> target_collision_groups){
    if (physics == NULL) throw std::invalid_argument("Physics Error: Physics pointer must not be null in kinematic object");
    if (bound_data == NULL) throw std::invalid_argument("Physics Error: Bound data pointer must not be null in this constructor.");
    if (init_prop != NULL){
        m_posY = init_prop->posY;
        m_posX = init_prop->posX;
        m_dy = init_prop->dy;
        m_dx = init_prop->dx;
        m_ddy = init_prop->ddy;
        m_ddx = init_prop->ddx;
    }
    m_collider = make_shared<Collider>(bound_data,is_hard_collider,target_collision_groups);

    physics->register_kinematic_object(this);
    WGAERRCHECK(physics->register_collider_object(collision_group,this));
}
Kinematic_Object::Kinematic_Object(shared_ptr<Entity_Physics> physics, kinematic_initial_properties* init_prop){
    if (physics == NULL) throw std::invalid_argument("Physics Error: Physics pointer must not be null in kinematic object");
    if (init_prop != NULL){
        m_posY = init_prop->posY;
        m_posX = init_prop->posX;
        m_dy = init_prop->dy;
        m_dx = init_prop->dx;
        m_ddy = init_prop->ddy;
        m_ddx = init_prop->ddx;
    }
}

}