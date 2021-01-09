#ifndef OBJECT_H
#define OBJECT_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "model.hpp"
class object_t
{
public:
    model_t *m_model;

private:
    glm::vec3 m_pos;                 // object position (posisi objek)
    glm::f32 m_angle;                // rotation angle (sudut rotasi)
    glm::vec3 m_axis;                // rotation axis (sumbu rotasi)
    glm::quat m_rotation;            // rotation quaternion
    glm::vec3 m_scale;               // object scale (ukuran objek)
    glm::vec3 m_around;              // object view target
    glm::mat4 m_model_mat;           // model matrix
    glm::mat4 m_chase_view_mat;      // vie3 third person
    glm::mat4 m_fp_view_mat;         // view first person
    bool m_changed;                  // flag for specifying when to construct a new model matrix
    std::array<glm::vec3, 8> m_hbox; // hitbox
    double m_x_len;
    double m_y_len;
    double m_z_len;

private:
    void calc_mats()
    {
        if (this->m_changed)
        {
            this->m_model_mat = glm::translate(glm::mat4{1.0}, this->m_pos) * glm::mat4_cast(this->m_rotation) * glm::scale(glm::mat4{1.0}, this->m_scale);
            this->m_chase_view_mat = glm::lookAt(glm::vec3{this->m_model_mat * glm::vec4{0.0, this->m_model->m_max_y + 1.4, -5.0, 1.0}}, glm::vec3{this->m_pos.x + this->m_around.x, this->m_pos.y + this->m_model->m_max_y + 0.05 + this->m_around.y, this->m_pos.z + this->m_around.z}, glm::vec3{0, 1, 0});
            this->m_fp_view_mat = glm::lookAt(glm::vec3{this->m_model_mat * glm::vec4{0.0, this->m_model->m_max_y, -3.0, 1.0}}, glm::vec3{this->m_pos.x + this->m_around.x, this->m_pos.y + this->m_model->m_max_y + this->m_around.y, this->m_pos.z + this->m_around.z}, glm::vec3{0, 1, 0});
            this->m_changed = false;
        }
    }

public:
    object_t(model_t *model, glm::vec3 pos = glm::vec3{0, 0, 0}, glm::f32 angle = 0.0F, glm::vec3 axis = glm::vec3{1, 0, 0}, glm::vec3 scale = glm::vec3{1.0, 1.0, 1.0}, glm::vec3 around = glm::vec3(0.0f)) : m_model{model}, m_pos{pos}, m_angle{angle}, m_scale{scale}, m_around{around}, m_model_mat{1.0}, m_changed{true}, m_hbox{{}}
    {
        this->m_axis = glm::normalize(axis);
        this->m_rotation = glm::rotate(glm::quat{glm::vec3{0.0}}, angle, glm::normalize(axis));
    }

    // copy constructor
    object_t(const object_t &object) : m_model{object.m_model}, m_pos{object.m_pos}, m_angle{object.m_angle}, m_axis{object.m_axis}, m_rotation{object.m_rotation}, m_scale{object.m_scale}, m_model_mat{object.m_model_mat}, m_changed{true}
    {
    }

    // destructor
    ~object_t()
    {
        this->m_model = nullptr;
    }
    // Builds and returns a model matrix
    glm::mat4 get_model_mat()
    {
        this->calc_mats();
        return this->m_model_mat;
    }
    glm::mat4 get_chase_cam_view_mat()
    {
        this->calc_mats();
        return this->m_chase_view_mat;
    }

    glm::mat4 get_fp_cam_view_mat()
    {
        this->calc_mats();
        return this->m_fp_view_mat;
    }

    glm::vec3 get_pos()
    {
        return this->m_pos;
    }
    glm::vec3 get_scale()
    {
        return this->m_scale;
    }
    glm::f32 get_angle()
    {
        return this->m_angle;
    }
    glm::vec3 get_axis()
    {
        return this->m_axis;
    }
    glm::quat get_rotation_quat()
    {
        return this->m_rotation;
    }
    // move the object by a specified vector
    // @param dx move in the x-axis direction by dx
    // @param dy move in the y-axis direction by dy
    // @param dz move in the z-axis direction by dz
    void move_by(double dx, double dy, double dz)
    {
        this->m_pos += glm::vec3{dx, dy, dz};
        this->m_changed = true;
    }
    // move the object by a specified vector
    // @param dr move in the dr direction by dr
    void move_by(glm::vec3 dr)
    {
        this->m_pos += dr;
        this->m_changed = true;
    }
    // move object to a specified coordinates
    // @param x the x-axis coordinates
    // @param y the y-axis coordinates
    // @param z the z-axis coordinates
    void move_to(double x, double y, double z)
    {
        this->m_pos = glm::vec3{x, y, z};
        this->m_changed = true;
    }
    // move object to a specified coordinates
    // @param coordinates the coordinates
    void move_to(glm::vec3 coordinates)
    {
        this->m_pos = coordinates;
        this->m_changed = true;
    }
    // rotates the object by angle (radians) around axis
    // @param angle angle to rotate object by in radians
    // @param axis axis of rotation
    void rotate_by(glm::f32 angle, glm::vec3 axis)
    {
        glm::quat tmp_quat = glm::rotate(glm::quat{glm::vec3{0.0}}, angle, glm::normalize(axis));
        this->m_rotation = this->m_rotation * tmp_quat;
        this->m_angle = glm::angle(this->m_rotation);
        this->m_axis = glm::axis(this->m_rotation);
        this->m_changed = true;
    }
    // rotates the object by the angle (radians)
    // @param angle_X rotate object by angle_X in radians around the x axis
    // @param angle_Y rotate object by angle_Y in radians around the y axis
    // @param angle_Z rotate object by angle_Z in radians around the z axis
    void rotate_by(glm::f32 angle_X, glm::f32 angle_Y, glm::f32 angle_Z)
    {
        glm::quat tmp_quat = glm::rotate(glm::quat{glm::vec3{0.0}}, angle_X, glm::vec3{1.0, 0.0, 0.0});
        tmp_quat = glm::rotate(tmp_quat, angle_Y, glm::vec3{0.0, 1.0, 0.0});
        tmp_quat = glm::rotate(tmp_quat, angle_Z, glm::vec3{0.0, 0.0, 1.0});
        this->m_rotation = this->m_rotation * tmp_quat;
        this->m_angle = glm::angle(this->m_rotation);
        this->m_axis = glm::axis(this->m_rotation);
        this->m_changed = true;
    }

    void rotate_to(glm::f32 angle, glm::vec3 axis)
    {
        this->m_angle = angle;
        this->m_axis = glm::normalize(axis);
        this->m_rotation = glm::rotate(glm::quat{glm::vec3{0.0}}, this->m_angle, this->m_axis);
        this->m_changed = true;
    }

    void rotate_to(glm::f32 angle_X, glm::f32 angle_Y, glm::f32 angle_Z)
    {
        this->m_rotation = glm::rotate(glm::quat{glm::vec3{0.0}}, angle_X, glm::vec3{1.0, 0.0, 0.0});
        this->m_rotation = glm::rotate(this->m_rotation, angle_Y, glm::vec3{0.0, 1.0, 0.0});
        this->m_rotation = glm::rotate(this->m_rotation, angle_Z, glm::vec3{0.0, 0.0, 1.0});
        this->m_angle = glm::angle(this->m_rotation);
        this->m_axis = glm::axis(this->m_rotation);
        this->m_changed = true;
    }

    void scale_by(glm::f32 dx, glm::f32 dy, glm::f32 dz)
    {
        this->m_scale = this->m_scale * glm::vec3{dx, dy, dz};
        this->m_changed = true;
    }

    void scale_by(glm::vec3 scale)
    {
        this->m_scale = this->m_scale * scale;
        this->m_changed = true;
    }

    void scale_to(glm::f32 dx, glm::f32 dy, glm::f32 dz)
    {
        this->m_scale = glm::vec3{dx, dy, dz};
        this->m_changed = true;
    }

    void scale_to(glm::vec3 scale)
    {
        this->m_scale = scale;
        this->m_changed = true;
    }

    void setAround(glm::vec3 aroundCam){
        this->m_around = aroundCam;
        this->m_changed = true;
    }

    bool operator==(const object_t &rhs)
    {
        return (this == &rhs);
    }

    bool operator!=(const object_t &rhs)
    {
        return !(this->operator==(rhs));
    }


};

#endif
