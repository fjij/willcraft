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

    private:
        gfx::Camera *camera;
        float last_frame;
    };
}

namespace craft {
    Player::Player() : camera{new gfx::Camera(120, 16.0 / 9.0)} {}

    Player::~Player() {
        delete camera;
    }

    void Player::handleInput(GLFWwindow *window) {
        float delta_time = glfwGetTime() - last_frame;
        last_frame = glfwGetTime();


        // Translating player
        float spd = 5.0f * delta_time;
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
    }
}

#endif //TEST_PROJECT_PLAYER_H
