// ============================================================
//  Camera —— 包掉飛行相機的狀態與矩陣
//  用法：cam.view() 拿 View 矩陣；cam.keyboard(dir,dt)；cam.mouse(dx,dy)
// ============================================================
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 pos   = glm::vec3(0.0f, 1.5f, 9.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up    = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = -90.0f, pitch = 0.0f;
    float speed = 4.0f, sensitivity = 0.1f;

    glm::mat4 view() const { return glm::lookAt(pos, pos + front, up); }

    // dir: 0=前 1=後 2=左 3=右
    void keyboard(int dir, float dt) {
        float v = speed * dt;
        if (dir == 0) pos += front * v;
        if (dir == 1) pos -= front * v;
        if (dir == 2) pos -= glm::normalize(glm::cross(front, up)) * v;
        if (dir == 3) pos += glm::normalize(glm::cross(front, up)) * v;
    }

    void mouse(float dx, float dy) {
        yaw += dx * sensitivity;
        pitch += dy * sensitivity;
        if (pitch >  89.0f) pitch =  89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        glm::vec3 d;
        d.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        d.y = sin(glm::radians(pitch));
        d.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(d);
    }
};
