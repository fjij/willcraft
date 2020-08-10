#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <fstream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Declarations                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace gfx {
    class Shader {
    public:
        GLuint ID;

        Shader(std::string vertex_shader_path, std::string fragment_shader_path);

        void use();

        void set_bool(const std::string &name, bool value) const;

        void set_int(const std::string &name, int value) const;

        void set_float(const std::string &name, float value) const;

        void set_mat4fv(const std::string &name, glm::mat4 matrix) const;
    };
}

namespace {
    GLuint load_shader(std::string shader_filename, GLenum shader_type);

    GLuint make_shader_program(GLuint vertex_shader, GLuint fragment_shader);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Definitions                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace gfx {
    Shader::Shader(std::string vertex_shader_path, std::string fragment_shader_path) {
        GLuint vertex_shader = load_shader(vertex_shader_path, GL_VERTEX_SHADER);
        GLuint fragment_shader = load_shader(fragment_shader_path, GL_FRAGMENT_SHADER);

        ID = make_shader_program(vertex_shader, fragment_shader);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    };

    void Shader::use() {
        glUseProgram(ID);
    }

    void Shader::set_bool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
    }

    void Shader::set_int(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::set_float(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::set_mat4fv(const std::string &name, glm::mat4 matrix) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }
}

namespace {
    GLuint load_shader(std::string shader_filename, GLenum shader_type) {
        std::ifstream in(shader_filename);
        std::string shader_text(
                (std::istreambuf_iterator<char>(in)),
                std::istreambuf_iterator<char>()
        );
        const char *shader_chars = shader_text.c_str();
        //std::cout << "Shader File \"" + shader_filename + "\" Loaded. \n Contents: \n" + shader_chars << std::endl;

        GLuint shader = glCreateShader(shader_type);
        glShaderSource(shader, 1, &shader_chars, NULL);
        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if (status != GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(shader, 512, NULL, buffer);
            std::cerr << "Shader did not compile. Error log:" << std::endl;
            std::cerr << buffer << std::endl;
        }

        return shader;
    }

    GLuint make_shader_program(GLuint vertex_shader, GLuint fragment_shader) {
        GLuint shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);

        GLint status;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &status);

        if (status != GL_TRUE) {
            char buffer[512];
            glGetProgramInfoLog(shader_program, 512, NULL, buffer);
            std::cerr << "Shaders could not link. Error log:" << std::endl;
            std::cerr << buffer << std::endl;
        }

        return shader_program;
    }
}

#endif
