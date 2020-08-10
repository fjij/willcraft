//
// Created by Will on 2020-01-21.
//


#ifndef TEST_PROJECT_WORLD_H
#define TEST_PROJECT_WORLD_H

#include "chunk.h"


namespace craft {
    class World {
    public:
        World(uint chunk_size, uint chunk_height, int seed = -1);

        ~World();

        void load_at(int x, int z, int radius);

        void draw(gfx::Shader *p_shader = nullptr);

    private:
        std::vector<Chunk *> chunks;
        int seed;
        uint chunk_size;
        uint chunk_height;
    };
}



namespace craft {
    World::World(uint chunk_size, uint chunk_height, int seed) :
            seed{seed}, chunk_size{chunk_size}, chunk_height{chunk_height} {
        this->chunks = std::vector<Chunk *>();
        this->load_at(0, 0, 3);
    }

    World::~World() {
        for (std::size_t i; i < chunks.size(); i++) {
            delete chunks[i];
        }
    }

    void World::load_at(int x, int z, int radius) {
        for (int X = x - radius; X <= x + radius; X++) {
            for (int Z = z - radius; Z <= z + radius; Z++) {
                Chunk *c = new Chunk(this->chunk_size, this->chunk_height, X, Z);
                c->load();
                c->generate_vbo();
                this->chunks.push_back(c);
                std::cout << X << ", " << Z << std::endl;
            }
        }
    }

    void World::draw(gfx::Shader *p_shader) {
        for (int i = 0; i < chunks.size(); i++) {
            this->chunks[i]->draw(p_shader);
        }
    }
}

#endif //TEST_PROJECT_WORLD_H
