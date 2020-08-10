#ifndef GFX_BINDABLE_H
#define GFX_BINDABLE_H

#include <glad/glad.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Declarations                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace gfx {
    class Bindable {
    public:
        void bind();

        void unbind();

        virtual GLuint get_ID() const = 0;

    protected:
        virtual void bind_function() const = 0;

        virtual bool &get_IS_BOUND() const = 0;

        virtual GLuint &get_BOUND_ID() const = 0;
    };
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          Function Definitions                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


namespace gfx {
    void Bindable::bind() {
        if (!this->get_IS_BOUND()) {
            // Check if this is already bound
            if (this->get_BOUND_ID() != this->get_ID()) {
                this->bind_function();
                this->get_BOUND_ID() = this->get_ID();
            }
            this->get_IS_BOUND() = true;
        } else {
            std::cerr << "ERROR: Binding failed. Something is already bound." << std::endl;
        }
    }

    void Bindable::unbind() {
        if (this->get_IS_BOUND() && this->get_BOUND_ID() == this->get_ID()) {
            this->get_IS_BOUND() = false;
        } else {
            std::cerr << "ERROR: Unbinding failed. This object is not bound!" << std::endl;
        }
    }
}

#endif
