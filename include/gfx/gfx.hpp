#ifndef GFX_GFX_H
#define GFX_GFX_H


#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <exception>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gfx/bindable.hpp>
#include <gfx/shader.hpp>
#include <gfx/vao.hpp>
#include <gfx/texture.hpp>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Declarations                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace gfx {
    static GLFWwindow *GLFW_WINDOW{nullptr};
    static int WINDOW_WIDTH{};
    static int WINDOW_HEIGHT{};
    static int FRAMEBUFER_WIDTH{};
    static int FRAMEBUFFER_HEIGHT{};

    void start(int window_width, int window_height, std::string window_title);

    float get_aspect_ratio();

    GLFWwindow *get_window();

    class Component;

    class Transform;

    class VAORenderer;

    class GameObject;

    class Component {
    public:
        Component(std::string name = "Component", bool enabled = true);

        std::string get_name() const;

        bool get_enabled() const;

        GameObject *get_gameobject() const;

        void set_name(std::string name);

        void set_enabled(bool enabled);

        void set_gameobject(GameObject *gameobject);

    protected:
        std::string name;
        bool enabled;
        GameObject *gameobject;
    };

    class Transform : public Component {
    public:
        Transform(
                glm::vec3 position = glm::vec3(),
                glm::quat rotation = glm::quat(),
                glm::vec3 scale = glm::vec3(1.0f)
        );

        glm::mat4 get_model_matrix() const;

        glm::vec3 get_position() const;

        glm::quat get_rotation() const;

        glm::vec3 get_scale() const;

        void set_position(glm::vec3 position);

        void set_rotation(glm::quat rotation);

        void set_scale(glm::vec3 scale);

    private:
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

    };

    class VAORenderer : public Component {
    public:
        VAORenderer(VAO *p_vao);

        void draw();

    private:
        VAO *p_vao;
    public:
        VAO *get_vao() const;
    };

    class GameObject {
    public:
        GameObject(Transform *transform = new Transform(), std::string name = "GameObject");

        void add_component(Component *component);

        Component *get_component_by_name(std::string name) const;

        Transform *get_transform() const;

        VAORenderer *get_renderer() const;

        void draw(Shader *draw_shader = nullptr);

    protected:
        std::string name;
        std::vector<Component *> components;

    };

    class Camera : public GameObject {
    public:
        Camera(
                float fov,
                float aspect_ratio,
                float near = 0.1f,
                float far = 100.0f,
                glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f),
                Transform *transform = new Transform()
        );

        glm::mat4 get_view_matrix() const;

        glm::mat4 get_projection_matrix() const;

        glm::vec3 get_up() const;

        glm::vec3 get_direction() const;

        void set_up(glm::vec3 up);

        void set_direction(glm::vec3 direction);

        void use_shader(Shader *shader);

    private:
        float fov;
        float aspect_ratio;
        float near;
        float far;
        glm::vec3 up;
        glm::vec3 direction;
    };
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                 Exceptions                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace gfx {
    class ComponentAlreadyBoundException : public std::exception {
        virtual const char *what() const throw() {
            return "This Component is already bound to a GameObject!";
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Definitions                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


namespace gfx {
    void start(int window_width, int window_height, std::string window_title) {
        if (GLFW_WINDOW == nullptr) {

            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

            // Make window
            GLFW_WINDOW = glfwCreateWindow(window_width, window_height, window_title.c_str(), NULL, NULL);
            if (GLFW_WINDOW == nullptr) {
                std::cerr << "Window creation failed." << std::endl;
                glfwTerminate();
                assert(false);
            }

            WINDOW_WIDTH = window_width;
            WINDOW_HEIGHT = window_height;

            glfwMakeContextCurrent(GLFW_WINDOW);

            if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                std::cout << "Failed to initialize GLAD" << std::endl;
                assert(false);
            }

            glfwGetFramebufferSize(GLFW_WINDOW, &FRAMEBUFER_WIDTH, &FRAMEBUFFER_HEIGHT);

            glViewport(0, 0, FRAMEBUFER_WIDTH, FRAMEBUFFER_HEIGHT);
            glEnable(GL_DEPTH_TEST);
        } else {
            std::cerr << "GFX has already been started!" << std::endl;
            assert(false);
        }
    }

    GLFWwindow *get_window() { return GLFW_WINDOW; }

    float get_aspect_ratio() {
        if (GLFW_WINDOW != nullptr) {
            return ((float) WINDOW_WIDTH) / ((float) WINDOW_HEIGHT);
        } else {
            std::cerr << "get_aspect_ratio: GLFW_WINDOW is not defined!" << std::endl;
            return 0.0f;
        }
    }
}

// CLASS: Component -------------------------------------

namespace gfx {

    Component::Component(std::string name, bool enabled) :
            name{name}, enabled{enabled}, gameobject{nullptr} {}

    std::string Component::get_name() const { return this->name; }

    bool Component::get_enabled() const { return this->enabled; }

    GameObject *Component::get_gameobject() const { return this->gameobject; }

    void Component::set_name(std::string name) { this->name = name; }

    void Component::set_enabled(bool enabled) { this->enabled = enabled; }

    void Component::set_gameobject(GameObject *gameobject) {
        if (this->get_gameobject() == nullptr) {
            this->gameobject = gameobject;
        } else {
            throw ComponentAlreadyBoundException();
        }
    }

}

// CLASS: Transform -------------------------------------

namespace gfx {

    Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) :
            Component("transform"), position{position}, rotation{rotation},
            scale{scale} {}

    glm::mat4 Transform::get_model_matrix() const {
        glm::mat4 tra = glm::translate(glm::mat4(1.0f), this->position);
        glm::mat4 rot = glm::mat4_cast(this->rotation);
        glm::mat4 sca = glm::scale(glm::mat4(1.0f), this->scale);
        return tra * rot * sca;
    }

    glm::vec3 Transform::get_position() const { return this->position; }

    glm::quat Transform::get_rotation() const { return this->rotation; }

    glm::vec3 Transform::get_scale() const { return this->scale; }

    void Transform::set_position(glm::vec3 position) { this->position = position; }

    void Transform::set_rotation(glm::quat rotation) { this->rotation = rotation; }

    void Transform::set_scale(glm::vec3 scale) { this->scale = scale; }
}

// CLASS: VAORenderer -------------------------------------

namespace gfx {
    VAORenderer::VAORenderer(VAO *p_vao) :
            Component("vao_renderer"), p_vao{p_vao} {}

    void VAORenderer::draw() {
        if (this->p_vao != nullptr) {
            this->p_vao->draw();
        }
    }

    VAO *VAORenderer::get_vao() const {
        return p_vao;
    }
}

// CLASS: Camera ----------------------------------------

namespace gfx {
    Camera::Camera(
            float fov,
            float aspect_ratio,
            float near,
            float far,
            glm::vec3 up,
            glm::vec3 direction,
            Transform *transform
    ) :
            GameObject(transform, "Camera"),
            fov{fov},
            aspect_ratio{aspect_ratio},
            up{up},
            direction{direction},
            near{near},
            far{far} {}

    glm::mat4 Camera::get_view_matrix() const {
        glm::vec3 position = this->get_transform()->get_position();
        return glm::lookAt(
                position,
                position + this->direction,
                this->up
        );
    }

    glm::mat4 Camera::get_projection_matrix() const {
        return glm::perspective(
                glm::radians(this->fov),
                this->aspect_ratio,
                this->near,
                this->far
        );
    }

    glm::vec3 Camera::get_up() const { return this->up; }

    glm::vec3 Camera::get_direction() const { return this->direction; }

    void Camera::set_up(glm::vec3 up) { this->up = up; }

    void Camera::set_direction(glm::vec3 direction) {
        this->direction = direction;
    }

    void Camera::use_shader(Shader *shader) {
        shader->set_mat4fv("view", this->get_view_matrix());
        shader->set_mat4fv("projection", this->get_projection_matrix());
    }
}

// CLASS: GameObject -------------------------------------

namespace gfx {

    GameObject::GameObject(Transform *transform, std::string name) :
            name{name}, components{std::vector<Component *>()} { this->add_component(transform); }

    void GameObject::add_component(Component *component) {
        component->set_gameobject(this);
        this->components.push_back(component);
    }

    Component *GameObject::get_component_by_name(std::string name) const {
        for (int i = 0; i < this->components.size(); i++) {
            if (components[i]->get_name() == name) {
                return components[i];
            }
        }
        return nullptr;
    }

    Transform *GameObject::get_transform() const {
        return static_cast<Transform *>(this->get_component_by_name("transform"));
    }

    VAORenderer *GameObject::get_renderer() const {
        return static_cast<VAORenderer *>(this->get_component_by_name("vao_renderer"));
    }

    void GameObject::draw(Shader *draw_shader) {
        if (draw_shader != nullptr) {
            Transform *transform = this->get_transform();
            draw_shader->set_mat4fv("model", transform->get_model_matrix());
        }
        VAORenderer *renderer = get_renderer();
        if (renderer != nullptr) {
            renderer->draw();
        }
    }
}


#endif
