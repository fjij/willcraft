#ifndef GFX_TEXTURE_H
#define GFX_TEXTURE_H

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>

#include <iostream>
#include <vector>
#include <string>

#include <gfx/bindable.hpp>

#ifndef STB_IMAGE_H
#include <stb_image/stb_image.h>
#endif
#define STB_IMAGE_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Declarations                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace gfx {
    class Texture : public gfx::Bindable {
    public:
        GLuint ID;

        Texture(std::string filename);

        void apply_parameter(GLenum pname, GLint param);

        void bind_to_texture_unit(GLuint texture_unit_number);

    protected:
        int width, height;
        // interface
        static bool IS_BOUND;
        static GLuint BOUND_ID;

        void bind_function() const;

        GLuint get_ID() const;

        bool &get_IS_BOUND() const;

        GLuint &get_BOUND_ID() const;
    };
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                             Static Variables                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace gfx {
    bool Texture::IS_BOUND{false};
    GLuint Texture::BOUND_ID{0};
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Definitions                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace gfx {
    Texture::Texture(std::string filename) {
        glGenTextures(1, &(this->ID));

        // Load image
        int nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(filename.c_str(), &(this->width), &(this->height), &nrChannels, 0);
        if (!data) {
            std::cerr << "ERROR: could not load texture: " + filename << std::endl;
            return;
        }

        // Dump image into texture
        this->bind();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     width, height, 0, (nrChannels == 4) ? (GL_RGBA) : (GL_RGB),
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        this->unbind();

        // free image
        stbi_image_free(data);
    }

    void Texture::apply_parameter(GLenum pname, GLint param) {
        this->bind();
        glTexParameteri(GL_TEXTURE_2D, pname, param);
        this->unbind();
    }

    void Texture::bind_to_texture_unit(GLuint texture_unit_number) {
        glActiveTexture(GL_TEXTURE0 + texture_unit_number);
        glBindTexture(GL_TEXTURE_2D, this->ID);
    }

    // Interface
    void Texture::bind_function() const { glBindTexture(GL_TEXTURE_2D, this->ID); }

    GLuint Texture::get_ID() const { return this->ID; }

    bool &Texture::get_IS_BOUND() const { return this->IS_BOUND; }

    GLuint &Texture::get_BOUND_ID() const { return this->BOUND_ID; }
}
#endif
