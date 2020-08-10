//
// Created by Will on 2020-01-21.
//

#ifndef TEST_PROJECT_SPRITESHEET_H
#define TEST_PROJECT_SPRITESHEET_H

#include "craft.h"

namespace craft {
    class Spritesheet {
    public:
        Spritesheet(uint rows, uint columns);

        glm::vec2 get_sprite_position(uint row, uint column);

        glm::vec2 get_sprite_size();

    private:
        uint rows;
        uint columns;
    };

    static Spritesheet spritesheet{Spritesheet(4, 4)};
}


namespace craft {
    Spritesheet::Spritesheet(uint rows, uint columns) :
            rows{rows}, columns{columns} {}

    glm::vec2 Spritesheet::get_sprite_position(uint row, uint column) {
        return glm::vec2(((float) column) / ((float) this->columns), ((float) row) / ((float) this->rows));
    }

    glm::vec2 Spritesheet::get_sprite_size() {
        return glm::vec2(1.0f / ((float) this->columns), 1.0f / ((float) this->rows));
    }
}

#endif //TEST_PROJECT_SPRITESHEET_H
