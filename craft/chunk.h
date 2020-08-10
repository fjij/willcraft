//
// Created by Will on 2020-01-21.
//

#ifndef TEST_PROJECT_CHUNK_H
#define TEST_PROJECT_CHUNK_H


#include "craft.h"

namespace craft {
    class Chunk : public gfx::GameObject {
    public:
        Chunk(uint size, uint height, int x, int z);

        ~Chunk();

        static uint gen_func(int x, int y, int z);

        int get_x() const;

        int get_z() const;

        void load();

        void unload();

        void generate_vbo();

        Block *get_block(uint x, uint y, uint z);

        BlockLightingInfo get_block_lighting(uint x, uint y, uint z);

        float light_at_block(int x, int y, int z);

        LightPoint *calculate_lighting_at_point(LocalPos p);

        LightPoint *get_lighting_at_point(uint x, uint y, uint z);

        bool is_air(int x, int y, int z);

    private:
        std::vector<std::vector<std::vector<Block *> *> *> blocks;
        std::vector<std::vector<std::vector<LightPoint *> *> *> lights;
        std::vector<FaceGraphics *> faces;
        uint size;
        uint height;
        int x;
        int z;

        AdjacentBlockInfo calculate_adjacent_block_info(LocalPos p);

        void update_blocks(std::vector<LocalPos> *blocks);

        static void add_point(std::vector<float> *data, glm::vec3 v, glm::vec2 t, float b);

        static void
        add_triangle(std::vector<float> *data, glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec2 a, glm::vec2 b,
                     glm::vec2 c,
                     TriLightingInfo li);

        static FaceGraphics *
        add_quad(std::vector<float> *data, glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, glm::vec2 a, glm::vec2 b,
                 glm::vec2 c, glm::vec2 d, QuadLightingInfo li);

        BlockGraphics *
        add_cube(std::vector<float> *data, glm::vec3 pos, float r, BlockInfo bi, AdjacentBlockInfo ai,
                 BlockLightingInfo li);

        void
        add_blockdata(std::vector<float> *data, Block *block, int x, int y, int z, AdjacentBlockInfo ai,
                      BlockLightingInfo li);

        LightPoint *getLightPoint(LocalPos p);

        void setLightPoint(LocalPos p, LightPoint *lp);

        void recalculateLightingAtPoint(LocalPos p);
    };
}



#endif //TEST_PROJECT_CHUNK_H
