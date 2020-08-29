//
// Created by Will on 2020-01-26.
//

#ifndef TEST_PROJECT_PLAYER_H
#define TEST_PROJECT_PLAYER_H

#include "craft.h"

namespace craft {
    class Player : public gfx::GameObject {
    public:
        Player();

        ~Player();

        void handleInput(GLFWwindow *window);

        void mouseCallback(GLFWwindow *window, double xpos, double ypos);

        gfx::Camera *getCamera();

    private:
        gfx::Camera *camera;
        float last_frame;

        float lastX;
        float lastY;
        bool firstMouse;
        float yaw;
        float pitch;
    };
}

namespace craft {
    Player::Player() {
        this->camera = new gfx::Camera(120, 16.0 / 9.0);
        this->firstMouse = true;
        this->yaw = 180;
        this->pitch = 0;
    }

    Player::~Player() {
        delete camera;
    }

    void Player::handleInput(GLFWwindow *window) {
        float delta_time = glfwGetTime() - last_frame;
        last_frame = glfwGetTime();


        // Translating player
        float spd = 10.0f * delta_time;
        glm::vec3 cpos = camera->get_transform()->get_position();
        glm::vec3 cfwd = camera->get_direction();
        glm::vec3 cup = camera->get_up();
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cpos += cfwd * spd;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cpos -= cfwd * spd;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cpos -= glm::normalize(glm::cross(cfwd, cup)) * spd;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cpos += glm::normalize(glm::cross(cfwd, cup)) * spd;
        camera->get_transform()->set_position(cpos);

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camera->set_direction(glm::normalize(front));
    }

    void Player::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    gfx::Camera *Player::getCamera() {
        return camera;
    }
}

#endif //TEST_PROJECT_PLAYER_H
