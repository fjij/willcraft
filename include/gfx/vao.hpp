#ifndef GFX_VBO_H
#define GFX_VBO_H

#include <glad/glad.h>

#include <iostream>
#include <vector>

#include <gfx/bindable.hpp>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Declarations                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



namespace gfx {

    class VBO : public Bindable {
    public:
        GLuint ID;

        VBO(std::vector<float> *data, std::vector<std::size_t> attr_sizes, size_t preallocate = 0);

        std::vector<std::size_t> get_attr_sizes() const;

        std::size_t get_data_length() const;

        void *map_buffer();

        bool unmap();

        void set_data_length(std::size_t data_length);

        void sub_data(size_t offset, size_t size, std::vector<float> *data);

    private:
        std::vector<std::size_t> attr_sizes;
        std::size_t data_length;
        std::size_t total_attr_size;
    public:
        size_t getTotalAttrSize() const;

    private:

        // Interface
        static bool IS_BOUND;
        static GLuint BOUND_ID;

        void bind_function() const;

        GLuint get_ID() const;

        bool &get_IS_BOUND() const;

        GLuint &get_BOUND_ID() const;
    };

    class SimpleTexturedVBO : public VBO {
    public:
        SimpleTexturedVBO(std::vector<float> *data);

    private:
        static const std::vector<std::size_t> DEFAULT_ATTR_SIZES;
    };

    class VAO : public Bindable {
        // NOTE: this class only supports floats rn
    public:
        GLuint ID;

        VAO();

        void use_vbo(VBO *vbo);

        void draw();

        VBO *get_vbo() const;

        void set_vbo(VBO *vbo);

    protected:
        VBO *vbo;

        bool has_vbo;

        static bool IS_BOUND;
        static GLuint BOUND_ID;

        void bind_function() const;

        GLuint get_ID() const;

        bool &get_IS_BOUND() const;

        GLuint &get_BOUND_ID() const;
    };

    class IndexedVAO : public VAO {

    };

    class EBO {
    public:
        GLuint ID;

        EBO(std::size_t indices_size, unsigned int *indices);

        void write_to_vao();

    private:
        std::size_t indices_size;
        unsigned int *indices;
    };


    /* EBO STUFF (not in use rn)

    gfx::EBO eb = gfx::EBO(sizeof(indices), indices);
    va.bind();
    eb.write_to_vao();
    va.unbind();

    */
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                             Static Variables                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


namespace gfx {
    bool VBO::IS_BOUND{false};
    GLuint VBO::BOUND_ID{0};

    const std::vector<std::size_t> SimpleTexturedVBO::DEFAULT_ATTR_SIZES{3, 2};

    bool VAO::IS_BOUND{false};
    GLuint VAO::BOUND_ID{0};
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Definitions                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


// VBO

namespace gfx {

    VBO::VBO(std::vector<float> *data, std::vector<std::size_t> attr_sizes, size_t preallocate) :
            attr_sizes{attr_sizes}, data_length{data->size()} {
        glGenBuffers(1, &(this->ID));

        total_attr_size = 0;
        for (size_t i = 0; i < attr_sizes.size(); i ++) {
            total_attr_size += attr_sizes[i];
        }

        this->bind();
        if (preallocate == 0) {
            glBufferData(GL_ARRAY_BUFFER, data_length * sizeof(float), &(data->front()), GL_STATIC_DRAW);
        } else {
            glBufferData(GL_ARRAY_BUFFER, preallocate * sizeof(float), NULL, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, data_length * sizeof(float), &(data->front()));
        }
        this->unbind();
    }

    std::vector<std::size_t> VBO::get_attr_sizes() const {
        return this->attr_sizes;
    }

    std::size_t VBO::get_data_length() const {
        return this->data_length;
    }

    void VBO::bind_function() const { glBindBuffer(GL_ARRAY_BUFFER, this->ID); }

    GLuint VBO::get_ID() const { return this->ID; }

    bool &VBO::get_IS_BOUND() const { return this->IS_BOUND; }

    GLuint &VBO::get_BOUND_ID() const { return this->BOUND_ID; }

    void *VBO::map_buffer() {
        this->bind();
        void *buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        this->unbind();
        return buffer;
    }

    bool VBO::unmap() {
        this->bind();
        bool success = glUnmapBuffer(GL_ARRAY_BUFFER) == GL_TRUE;
        this->unbind();
        return success;
    }

    void VBO::set_data_length(size_t data_length) {
        this->data_length = data_length;
    }

    size_t VBO::getTotalAttrSize() const {
        return total_attr_size;
    }

    void VBO::sub_data(size_t offset, size_t size, std::vector<float> *data) {
        this -> bind();
        glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(float), size*sizeof(float), &(data->front()));
        this -> unbind();
    }

}

// SimpleTexturedVBO

namespace gfx {
    SimpleTexturedVBO::SimpleTexturedVBO(std::vector<float> *data) :
            VBO(data, this->DEFAULT_ATTR_SIZES) {}
}

// EBO

namespace gfx {

    EBO::EBO(std::size_t indices_size, unsigned int *indices) {
        this->indices_size = indices_size;
        this->indices = indices;

        glGenBuffers(1, &(this->ID));
    }

    void EBO::write_to_vao() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices_size, indices, GL_STATIC_DRAW);
    }

}

// VAO

namespace gfx {

    VAO::VAO() :
            has_vbo{false}, vbo{nullptr} {
        // Generate Vertex Array
        glGenVertexArrays(1, &(this->ID));
    }

    void VAO::use_vbo(VBO *vbo) {
        if (this->has_vbo) {
            std::cerr << "This VAO already has a vbo!" << std::endl;
            return;
        }
        this->vbo = vbo;

        this->has_vbo = true;

        this->bind();
        vbo->bind();

        const std::vector<std::size_t> attr_sizes = vbo->get_attr_sizes();
        const std::size_t count = attr_sizes.size();

        // Calculate Stride
        GLuint stride = 0;
        for (std::size_t i = 0; i < count; i++) {
            stride += attr_sizes[i];
        }

        // For each attribute ...
        GLuint offset = 0;
        for (std::size_t layout = 0; layout < count; layout++) {
            glVertexAttribPointer(
                    layout,
                    attr_sizes[layout], GL_FLOAT, GL_FALSE,
                    sizeof(float) * stride,
                    (GLvoid *) (sizeof(float) * offset)
            );
            // Enable the attribute
            glEnableVertexAttribArray(layout);
            offset += attr_sizes[layout];
        }

        vbo->unbind();
        this->unbind();
    }

    void VAO::draw() {
        if (this->has_vbo) {
            this->bind();
            glDrawArrays(GL_TRIANGLES, 0, this->vbo->get_data_length()/(this->vbo->getTotalAttrSize()));
            this->unbind();
        } else {
            std::cerr << "no vbo to draw :(" << std::endl;
        }
    }
    //glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

    void VAO::bind_function() const { glBindVertexArray(this->ID); }

    GLuint VAO::get_ID() const { return this->ID; }

    bool &VAO::get_IS_BOUND() const { return this->IS_BOUND; }

    GLuint &VAO::get_BOUND_ID() const { return this->BOUND_ID; }

    VBO *VAO::get_vbo() const {
        return vbo;
    }

    void VAO::set_vbo(VBO *vbo) {
        VAO::vbo = vbo;
    }

}

#endif
