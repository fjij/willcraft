//
// Created by Will on 2020-01-19.
//

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef __APPLE__
#	define __gl_h_
#	define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gfx/shader.hpp>
#include <gfx/vao.hpp>
#include <gfx/texture.hpp>
#include <gfx/gfx.hpp>

#include "craft/craft.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Declarations                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void handle_input(GLFWwindow *window, glm::vec3 &cpos, const glm::vec3 cfwd, const glm::vec3 cup);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

int main();

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Definitions                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

craft::Player *player;

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    player->mouseCallback(window, xpos, ypos);
}

int main() {
    std::cout << "start" << std::endl;

    gfx::start(SCREEN_WIDTH, SCREEN_HEIGHT, "willcraft");
    GLFWwindow *window = gfx::get_window();

    // mouse stuff

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Create world --------------

    std::cout << "pre world" << std::endl;

    craft::World world = craft::World(16, 64);

    std::cout << "post world" << std::endl;

    // Load Texture --------------------------

    gfx::Texture texture_sheet = gfx::Texture("assets/blocks.png");
    texture_sheet.apply_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    // for that pixelated minecraft look
    texture_sheet.apply_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Camera --------------------------------
    //gfx::Camera cam = gfx::Camera(100.0f, gfx::get_aspect_ratio(), 0.05f, 150.0f);
    //cam.get_transform()->set_position(glm::vec3(0.0f, 30.0f, 0.0f));
    //cam.set_up(glm::vec3(0.0f, 1.0f, 0.0f));
    //cam.set_direction(glm::vec3(0.0f, 0.0f, -1.0f));

    player = new craft::Player();

    // Prep ------------------------------------------

    // Load shaders
    gfx::Shader shader_brightness = gfx::Shader("assets/shader_brightness.vert", "assets/shader_brightness.frag");

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_CULL_FACE);

    shader_brightness.use();
    shader_brightness.set_int("tex", 0);

    // Main loop -----------------------------------------------------------

    while (!glfwWindowShouldClose(window)) {

        // Input -------------------------------------------------

        player->handleInput(window);

        // Render -----------------------------------------------
        glClearColor(143.0f / 255.0f, 186.0f / 255.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        texture_sheet.bind_to_texture_unit(0);

        shader_brightness.use();
        player->getCamera()->use_shader(&shader_brightness);

        int px = ((int) player->getCamera()->get_transform()->get_position().x/16);
        int pz = ((int) player->getCamera()->get_transform()->get_position().z/16);

        world.draw(&shader_brightness);
        world.load_at(px, pz, 3);

        // rest
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
    }

    glfwTerminate();
    return 0;
}
