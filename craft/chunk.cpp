//
// Created by Will on 2020-01-26.
//


#pragma once
#include "craft.h"

namespace craft {
    static PerlinNoise pn = PerlinNoise(12138);

    Chunk::Chunk(uint size, uint height, int x, int z) :
            size{size}, height{height}, x{x}, z{z} {
        this->get_transform()->set_position(glm::vec3((float) (x * (int) size), 0.0f, (float) (z * (int) size)));
    }

    Chunk::~Chunk() {
        this->unload();
    }

    int Chunk::get_x() const { return this->x; }

    int Chunk::get_z() const { return this->z; }

    void Chunk::set_neighbors(Chunk *x, Chunk *z, Chunk *X, Chunk *Z) {
        this->neighbor_x = x;
        this->neighbor_z = z;
        this->neighbor_X = X;
        this->neighbor_Z = Z;
    }

    float Chunk::light_at_block(int x, int y, int z) {
        if (x >= this->size || z >= this->size || y >= this->height || x < 0 || y < 0 || z < 0)
            return (this->is_air(x, y, z)) ? 1.0f : 0.0f;
        return (float) (this->get_block(x, y, z)->light_level) / 16.0f;
    }

    LightPoint *Chunk::calculate_lighting_at_point(LocalPos p) {
        return new LightPoint{
                // +x
                (light_at_block(p.x, p.y - 1, p.z - 1)
                 + light_at_block(p.x, p.y, p.z - 1)
                 + light_at_block(p.x, p.y - 1, p.z)
                 + light_at_block(p.x, p.y, p.z)) / 4.0f,
                // -x
                (light_at_block(p.x - 1, p.y - 1, p.z - 1)
                 + light_at_block(p.x - 1, p.y, p.z - 1)
                 + light_at_block(p.x - 1, p.y - 1, p.z)
                 + light_at_block(p.x - 1, p.y, p.z)) / 4.0f,
                // +y
                (light_at_block(p.x - 1, p.y, p.z - 1)
                 + light_at_block(p.x, p.y, p.z - 1)
                 + light_at_block(p.x - 1, p.y, p.z)
                 + light_at_block(p.x, p.y, p.z)) / 4.0f,
                // -y
                (light_at_block(p.x - 1, p.y - 1, p.z - 1)
                 + light_at_block(p.x, p.y - 1, p.z - 1)
                 + light_at_block(p.x - 1, p.y - 1, p.z)
                 + light_at_block(p.x, p.y - 1, p.z)) / 4.0f,
                // +z
                (light_at_block(p.x - 1, p.y - 1, p.z)
                 + light_at_block(p.x, p.y - 1, p.z)
                 + light_at_block(p.x - 1, p.y, p.z)
                 + light_at_block(p.x, p.y, p.z)) / 4.0f,
                // -z
                (light_at_block(p.x - 1, p.y - 1, p.z - 1)
                 + light_at_block(p.x, p.y - 1, p.z - 1)
                 + light_at_block(p.x - 1, p.y, p.z - 1)
                 + light_at_block(p.x, p.y, p.z - 1)) / 4.0f
        };
    }

    LightPoint *Chunk::get_lighting_at_point(uint x, uint y, uint z) {
        if (x > this->size || y > this->height || z > this->size)
            return nullptr;
        return (*(*(this->lights)[y])[x])[z];
    }

    BlockLightingInfo Chunk::get_block_lighting(uint x, uint y, uint z) {
        return {
                get_lighting_at_point(x, y, z),
                get_lighting_at_point(x + 1, y, z),
                get_lighting_at_point(x, y + 1, z),
                get_lighting_at_point(x + 1, y + 1, z),
                get_lighting_at_point(x, y, z + 1),
                get_lighting_at_point(x + 1, y, z + 1),
                get_lighting_at_point(x, y + 1, z + 1),
                get_lighting_at_point(x + 1, y + 1, z + 1)
        };
    }

    void Chunk::load_blocks() {
        // Load blocks
        blocks = std::vector<std::vector<std::vector<Block *> *> *>(height);
        for (int y = 0; y < this->height; y++) {
            std::vector<std::vector<Block *> *> *layer = new std::vector<std::vector<Block *> *>(this->size);
            for (int x = 0; x < this->size; x++) {
                std::vector<Block *> *row = new std::vector<Block *>(this->size);
                for (int z = 0; z < this->size; z++) {
                    int world_x{(int) (x + (this->x) * ((int) (this->size)))},
                            world_y{y},
                            world_z{(int) (z + (this->z) * ((int) (this->size)))};
                    uint block_id = this->gen_func(world_x, world_y, world_z);
                    uint light_level = block_id == 0 ? 16 : 0;
                    (*row)[z] = new Block{block_id, light_level, nullptr};
                }
                (*layer)[x] = row;
            }
            blocks[y] = layer;
        }
    }

    void Chunk::load_lighting() {
        // Load lighting
        lights = std::vector<std::vector<std::vector<LightPoint *> *> *>(height + 1);
        for (int y = 0; y <= this->height; y++) {
            std::vector<std::vector<LightPoint *> *> *layer
                    = new std::vector<std::vector<LightPoint *> *>(this->size + 1);
            for (int x = 0; x <= this->size; x++) {
                std::vector<LightPoint *> *row = new std::vector<LightPoint *>(this->size + 1);
                for (int z = 0; z <= this->size; z++) {
                    (*row)[z] = calculate_lighting_at_point({(uint)x, (uint)y, (uint)z});
                }
                (*layer)[x] = row;
            }
            lights[y] = layer;
        }
    }

    void Chunk::unload() {
        // Unload blocks
        for (int y = 0; y < this->height; y++) {
            for (int x = 0; x < this->size; x++) {
                for (int z = 0; z < this->size; z++) {
                    delete (*(*(this->blocks)[y])[x])[z]->graphics;
                    delete (*(*(this->blocks)[y])[x])[z];
                }
                delete (*(this->blocks)[y])[x];
            }
            delete (this->blocks)[y];
        }

        // Unload lights
        for (int y = 0; y <= this->height; y++) {
            for (int x = 0; x <= this->size; x++) {
                for (int z = 0; z <= this->size; z++) {
                    delete (*(*(this->lights)[y])[x])[z];
                }
                delete (*(this->lights)[y])[x];
            }
            delete (this->lights)[y];
        }

        // Unload faces
        for (auto f: faces) {
            delete f;
        }
    }

    // Generates the VBO for a Chunk
    void Chunk::generate_vbo() {

        std::vector<float> data = std::vector<float>();
        this->faces = std::vector<FaceGraphics *>();

        for (int y = 0; y < this->height; y++) {
            for (int x = 0; x < this->size; x++) {
                for (int z = 0; z < this->size; z++) {
                    if (!this->is_air(x, y, z)) {
                        bool touching_air{
                                this->is_air(x + 1, y, z) ||
                                this->is_air(x - 1, y, z) ||
                                this->is_air(x, y + 1, z) ||
                                this->is_air(x, y - 1, z) ||
                                this->is_air(x, y, z + 1) ||
                                this->is_air(x, y, z - 1)
                        };
                        if (touching_air) {

                            BlockLightingInfo li = get_block_lighting(x, y, z);
                            add_blockdata(&data,
                               this->get_block(x, y, z), x, y, z,
                               calculate_adjacent_block_info({(uint) x, (uint) y, (uint) z}), li
                            );
                        }
                    }
                }
            }
        }
        std::vector<std::size_t> sizes{3, 2, 1};
        gfx::VBO *vbo = new gfx::VBO(&data, sizes, this->size * this->size * this->height * 6 * 2 * 3 * 6);
        gfx::VAO *vao = new gfx::VAO();
        vao->use_vbo(vbo);

        gfx::VAORenderer *renderer = new gfx::VAORenderer(vao);
        this->add_component(renderer);

        // Queue deleted blocks for recalculation(testing)
        /*
        std::vector<LocalPos> edit_blocks = std::vector<LocalPos>();
        for (uint x = 0; x < this->size; x++) {
            for (uint z = 0; z < this->size; z++) {
                edit_blocks.push_back({x, 41, z});
                edit_blocks.push_back({x, 40, z});
                edit_blocks.push_back({x, 39, z});

                // Set blocks to air
                get_block(x, 40, z)->id = 0;
                get_block(x, 40, z)->light_level = 16;
            }
        }
        update_blocks(&edit_blocks);*/
    }

    // Returns whether the block at a given relative coordinate is air
    bool Chunk::is_air(int x, int y, int z) {

        // Outside of height range?
        if (y < 0 || y >= this->height) return true;

        Chunk *target = this;
        // Check if outside current chunk
        if (x < 0) {
            target = neighbor_x;
        } else if (x >= this->size) {
            target = neighbor_X;
        }
        if (z < 0) {
            target = neighbor_z;
        } else if (z >= this->size) {
            target = neighbor_Z;
        }
        // Chunk doesn't exist, return air
        if (target == nullptr) {
            return true;
        }

        // Get the block at whatever position.
        Block *b;
        if (target == this) {
            b = target->get_block(x, y, z);
            return (b == nullptr || b->id == 0);
        } else {
            int dx = target->get_x() - this->get_x();
            int dz = target->get_z() - this->get_z();
            int new_x = x - ((int)this->size)*dx;
            int new_z = z - ((int)this->size)*dz;
            return target->is_air(new_x, y, new_z);
        }
    }

    // Returns a pointer to the block at a given relative position
    Block *Chunk::get_block(uint x, uint y, uint z) {
        if (x >= this->size || y >= this->height || z >= this->size)
            return nullptr;
        return (*(*(this->blocks)[y])[x])[z];
    }

    LightPoint *Chunk::getLightPoint(LocalPos p) {
        if (p.x >= this->size || p.y >= this->height || p.z >= this->size)
            return nullptr;
        return (*(*(this->lights)[p.y])[p.x])[p.z];
    }

    void Chunk::recalculateLightingAtPoint(LocalPos p) {
        setLightPoint(p, calculate_lighting_at_point(p));
    }

    void Chunk::setLightPoint(LocalPos p, LightPoint *lp) {
        if (p.x >= this->size || p.y >= this->height || p.z >= this->size)
            return;
        delete (*(*(this->lights)[p.y])[p.x])[p.z];
        (*(*(this->lights)[p.y])[p.x])[p.z] = lp;
    }

    uint Chunk::gen_func(int x, int y, int z) {
        double a{pn.noise(x * 0.05f, z * 0.05f, 0.0f) * 10.0f};
        double b{pn.noise(x * 0.003f, z * 0.003f, 0.0f) * 5.0f};
        double c{pn.noise(x * 0.0004f, z * 0.0004f, 0.0f) * 3.0f};
        double h{a * b * c * c};
        if (y > (int) h)
            return 0;
        if (y == (int) h)
            return 2;
        if (y > (int) h - 3)
            return 1;
        return 3;
    }

    void Chunk::update_blocks(std::vector<LocalPos> *blocks) {
        gfx::VBO *vbo = this->get_renderer()->get_vao()->get_vbo();
        auto *map = (float *) vbo->map_buffer();

        // recalculate lighting (note: not most efficient, repeat calculations: perhaps using a hash table might help?)
        for (auto p : *blocks) {
            recalculateLightingAtPoint(p + LocalPos{0, 0, 0});
            recalculateLightingAtPoint(p + LocalPos{0, 0, 1});
            recalculateLightingAtPoint(p + LocalPos{0, 1, 0});
            recalculateLightingAtPoint(p + LocalPos{0, 1, 1});
            recalculateLightingAtPoint(p + LocalPos{1, 0, 0});
            recalculateLightingAtPoint(p + LocalPos{1, 0, 1});
            recalculateLightingAtPoint(p + LocalPos{1, 1, 0});
            recalculateLightingAtPoint(p + LocalPos{1, 1, 1});
        }

        // delete all block faces
        for (auto p : *blocks) {
            Block *block = get_block(p.x, p.y, p.z);
            BlockGraphics *bg = block->graphics;
            if (bg != nullptr) {
                for (size_t j = 0; j < bg->count; j++) {
                    FaceGraphics *fg = bg->fgs[j];
                    FaceGraphics *endfg = faces[faces.size() - 1];
                    std::swap(faces[fg->index], faces[endfg->index]);
                    std::swap_ranges(map + fg->start, map + fg->end, map + endfg->start);

                    std::swap(fg->index, endfg->index);
                    std::swap(fg->start, endfg->start);
                    std::swap(fg->end, endfg->end);

                    faces.pop_back();
                    size_t face_size = fg->end - fg->start;
                    vbo->set_data_length(vbo->get_data_length() - face_size);

                    delete fg;
                }
                delete bg;
                bg = nullptr;
                block->graphics = nullptr;
            }
        }
        vbo->unmap();

        // re-add all block faces
        for (auto p : *blocks) {
            Block *block = get_block(p.x, p.y, p.z);
            if (true || block->id != 0) {
                size_t face_size = faces[0]->end - faces[0]->start;
                std::vector<float> data = std::vector<float>();
                size_t offset = face_size * faces.size();
                add_blockdata(&data, block, p.x, p.y, p.z, calculate_adjacent_block_info(p),
                              get_block_lighting(p.x, p.y, p.z));

                vbo->sub_data(offset, data.size(), &data);
                vbo->set_data_length(offset + data.size());
            }
        }
    }

    // VBO UTIL

    void Chunk::add_point(std::vector<float> *data, glm::vec3 v, glm::vec2 t, float b) {
        data->push_back(v.x);
        data->push_back(v.y);
        data->push_back(v.z);
        data->push_back(t.x);
        data->push_back(t.y);
        data->push_back(b);
    }

    void Chunk::add_triangle(std::vector<float> *data, glm::vec3 A, glm::vec3 B, glm::vec3 C,
                             glm::vec2 a, glm::vec2 b, glm::vec2 c, TriLightingInfo li) {
        add_point(data, A, a, li.a);
        add_point(data, C, c, li.c);
        add_point(data, B, b, li.b);
    }

    FaceGraphics *Chunk::add_quad(std::vector<float> *data, glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D,
                                  glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d, QuadLightingInfo li) {
        FaceGraphics *fg = new FaceGraphics;

        fg->start = data->size();

        add_triangle(data, A, B, D, a, b, d, {li.a, li.b, li.d});
        add_triangle(data, B, C, D, b, c, d, {li.b, li.c, li.d});

        fg->end = data->size();

        return fg;
    }

    BlockGraphics *Chunk::add_cube(std::vector<float> *data, glm::vec3 pos, float r, BlockInfo bi,
                                   AdjacentBlockInfo ai, BlockLightingInfo li) {
        glm::vec3 xyz{pos + glm::vec3(-r, -r, -r)},
                Xyz{pos + glm::vec3(r, -r, -r)},
                xYz{pos + glm::vec3(-r, r, -r)},
                XYz{pos + glm::vec3(r, r, -r)},
                xyZ{pos + glm::vec3(-r, -r, r)},
                XyZ{pos + glm::vec3(r, -r, r)},
                xYZ{pos + glm::vec3(-r, r, r)},
                XYZ{pos + glm::vec3(r, r, r)};
        glm::vec2
                pXTL{spritesheet.get_sprite_position(bi.pX.x, bi.pX.y)},
                pXTR{spritesheet.get_sprite_position(bi.pX.x + 1, bi.pX.y)},
                pXBL{spritesheet.get_sprite_position(bi.pX.x, bi.pX.y + 1)},
                pXBR{spritesheet.get_sprite_position(bi.pX.x + 1, bi.pX.y + 1)};
        glm::vec2
                nXTL{spritesheet.get_sprite_position(bi.nX.x, bi.nX.y)},
                nXTR{spritesheet.get_sprite_position(bi.nX.x + 1, bi.nX.y)},
                nXBL{spritesheet.get_sprite_position(bi.nX.x, bi.nX.y + 1)},
                nXBR{spritesheet.get_sprite_position(bi.nX.x + 1, bi.nX.y + 1)};
        glm::vec2
                pYTL{spritesheet.get_sprite_position(bi.pY.x, bi.pY.y)},
                pYTR{spritesheet.get_sprite_position(bi.pY.x + 1, bi.pY.y)},
                pYBL{spritesheet.get_sprite_position(bi.pY.x, bi.pY.y + 1)},
                pYBR{spritesheet.get_sprite_position(bi.pY.x + 1, bi.pY.y + 1)};
        glm::vec2
                nYTL{spritesheet.get_sprite_position(bi.nY.x, bi.nY.y)},
                nYTR{spritesheet.get_sprite_position(bi.nY.x + 1, bi.nY.y)},
                nYBL{spritesheet.get_sprite_position(bi.nY.x, bi.nY.y + 1)},
                nYBR{spritesheet.get_sprite_position(bi.nY.x + 1, bi.nY.y + 1)};
        glm::vec2
                pZTL{spritesheet.get_sprite_position(bi.pZ.x, bi.pZ.y)},
                pZTR{spritesheet.get_sprite_position(bi.pZ.x + 1, bi.pZ.y)},
                pZBL{spritesheet.get_sprite_position(bi.pZ.x, bi.pZ.y + 1)},
                pZBR{spritesheet.get_sprite_position(bi.pZ.x + 1, bi.pZ.y + 1)};
        glm::vec2
                nZTL{spritesheet.get_sprite_position(bi.nZ.x, bi.nZ.y)},
                nZTR{spritesheet.get_sprite_position(bi.nZ.x + 1, bi.nZ.y)},
                nZBL{spritesheet.get_sprite_position(bi.nZ.x, bi.nZ.y + 1)},
                nZBR{spritesheet.get_sprite_position(bi.nZ.x + 1, bi.nZ.y + 1)};

        BlockGraphics *bg = new BlockGraphics{{}, 0};

        if (ai.pZ) {
            FaceGraphics *fg = add_quad(data, xyZ, xYZ, XYZ, XyZ, pZBL, pZBR, pZTR, pZTL,
                                        {li.xyZ->pZ, li.xYZ->pZ, li.XYZ->pZ, li.XyZ->pZ});
            bg->fgs[bg->count] = fg;
            bg->count++;
            fg->index = this->faces.size();
            this->faces.push_back(fg);
        }
        if (ai.nZ) {
            FaceGraphics *fg = add_quad(data, xyz, Xyz, XYz, xYz, nZTL, nZBL, nZBR, nZTR,
                                        {li.xyz->nZ, li.Xyz->nZ, li.XYz->nZ, li.xYz->nZ});
            bg->fgs[bg->count] = fg;
            bg->count++;
            fg->index = this->faces.size();
            this->faces.push_back(fg);
        }

        if (ai.nY) {
            FaceGraphics *fg = add_quad(data, xyZ, XyZ, Xyz, xyz, nYBL, nYBR, nYTR, nYTL,
                                        {li.xyZ->nY, li.XyZ->nY, li.Xyz->nY, li.xyz->nY});
            bg->fgs[bg->count] = fg;
            bg->count++;
            fg->index = this->faces.size();
            this->faces.push_back(fg);
        }
        if (ai.pY) {
            FaceGraphics *fg = add_quad(data, xYZ, xYz, XYz, XYZ, pYTR, pYTL, pYBL, pYBR,
                                        {li.xYZ->pY, li.xYz->pY, li.XYz->pY, li.XYZ->pY});
            bg->fgs[bg->count] = fg;
            bg->count++;
            fg->index = this->faces.size();
            this->faces.push_back(fg);
        }

        if (ai.nX) {
            FaceGraphics *fg = add_quad(data, xyZ, xyz, xYz, xYZ, nXTL, nXBL, nXBR, nXTR,
                                        {li.xyZ->nX, li.xyz->nX, li.xYz->nX, li.xYZ->nX});
            bg->fgs[bg->count] = fg;
            bg->count++;
            fg->index = this->faces.size();
            this->faces.push_back(fg);
        }
        if (ai.pX) {
            FaceGraphics *fg = add_quad(data, XyZ, XYZ, XYz, Xyz, pXBL, pXBR, pXTR, pXTL,
                                        {li.XyZ->pX, li.XYZ->pX, li.XYz->pX, li.Xyz->pX});
            bg->fgs[bg->count] = fg;
            bg->count++;
            fg->index = this->faces.size();
            this->faces.push_back(fg);
        }
        return bg;
    }

    void Chunk::add_blockdata(std::vector<float> *data, Block *block, int x, int y, int z,
                              AdjacentBlockInfo ai, BlockLightingInfo li) {
        if (block->id != 0) {
            BlockInfo info;
            switch (block->id) {
                case 3:
                    info = Stone;
                    break;
                case 2:
                    info = Grass;
                    break;
                case 1:
                default:
                    info = Dirt;
                    break;
            }
            block->graphics = add_cube(data, glm::vec3(x, y, z), 0.5f, info, ai, li);
        } else {
            block->graphics = nullptr;
        }
    }

    AdjacentBlockInfo Chunk::calculate_adjacent_block_info(LocalPos p) {
        int x = p.x;
        int y = p.y;
        int z = p.z;
        return {
                this->is_air(x + 1, y, z),
                this->is_air(x - 1, y, z),
                this->is_air(x, y + 1, z),
                this->is_air(x, y - 1, z),
                this->is_air(x, y, z + 1),
                this->is_air(x, y, z - 1)
        };
    }

}
