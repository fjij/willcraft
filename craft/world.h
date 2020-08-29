//
// Created by Will on 2020-01-21.
//


#ifndef TEST_PROJECT_WORLD_H
#define TEST_PROJECT_WORLD_H

#include "chunk.h"
//#include <boost/thread/thread.hpp>

namespace craft {
    struct ChunkPos {
        int x;
        int z;
        bool operator==(const ChunkPos &other) const
        {
            return x == other.x && z == other.z;
        }
    };
}

namespace std {
    template <>
    struct hash<craft::ChunkPos>
    {
        std::size_t operator()(const craft::ChunkPos &k) const
        {
            return (k.x) ^ (k.z);
        }
    };
}

namespace craft {

    class World {
    public:
        World(uint chunk_size, uint chunk_height, int seed = -1);

        ~World();

        void load_at(int x, int z, int radius);

        void draw(gfx::Shader *p_shader = nullptr);

    private:
        void updateChunks(std::vector<Chunk *> new_chunks);
        Chunk *find_chunk(int x, int z);
        //std::vector<Chunk *> chunks;
        std::unordered_map<ChunkPos, Chunk *> chunks;
        int seed;
        uint chunk_size;
        uint chunk_height;
    };
}



namespace craft {
    World::World(uint chunk_size, uint chunk_height, int seed) :
            seed{seed}, chunk_size{chunk_size}, chunk_height{chunk_height} {
        //this->chunks = std::vector<Chunk *>();
        this->chunks = std::unordered_map<ChunkPos, Chunk *>();
        this->load_at(0, 0, 3);
    }

    World::~World() {
        for (auto c: chunks) {
            delete c.second;
        }
    }

    void World::updateChunks(std::vector<Chunk *> new_chunks) {
        //boost::thread t([](){

        //});
        for (auto c: new_chunks) {
            int X = c -> get_x();
            int Z = c -> get_z();
            c -> set_neighbors(
                find_chunk(X-1, Z),
                find_chunk(X, Z-1),
                find_chunk(X+1, Z),
                find_chunk(X, Z+1)
            );
        }
        for (auto c: new_chunks) {
            c->load_blocks();
        }
        for (auto c: new_chunks) {
            c->load_lighting();
        }
        for (auto c: new_chunks) {
            c->generate_vbo();
        }
    }

    void World::load_at(int x, int z, int radius) {
        std::vector<Chunk *> new_chunks = std::vector<Chunk *>();
        for (int X = x - radius; X <= x + radius; X++) {
            for (int Z = z - radius; Z <= z + radius; Z++) {
                ChunkPos pos = {X, Z};
                if (find_chunk(X, Z) == nullptr) {
                    Chunk *c = new Chunk(this->chunk_size, this->chunk_height, X, Z);
                    chunks[pos] = c;
                    new_chunks.push_back(c);
                }
            }
        }
        this->updateChunks(new_chunks);
    }

    Chunk *World::find_chunk(int x, int z) {
        ChunkPos pos = {x, z};
        auto it = chunks.find(pos);
        if (it == chunks.end()) {
            return nullptr;
        } else {
            return it->second;
        }
    }

    void World::draw(gfx::Shader *p_shader) {
        for (auto it: chunks) {
            auto c = it.second;
            c->draw(p_shader);
        }
    }
}

#endif //TEST_PROJECT_WORLD_H
