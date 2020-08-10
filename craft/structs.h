//
// Created by Will on 2020-01-21.
//

#ifndef TEST_PROJECT_STRUCTS_H
#define TEST_PROJECT_STRUCTS_H

#include "uint.h"

namespace craft {
    struct LocalPos {
        uint x;
        uint y;
        uint z;
    };

    LocalPos operator + (const LocalPos& a, const LocalPos& b) {
        return {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    struct FaceGraphics {
        size_t start;
        size_t end;
        size_t index;
    };

    struct BlockGraphics {
        FaceGraphics *fgs[6];
        size_t count;
    };

    struct LightPoint {
        float pX;
        float nX;
        float pY;
        float nY;
        float pZ;
        float nZ;
    };

    struct BlockLightingInfo {
        LightPoint *xyz;
        LightPoint *Xyz;
        LightPoint *xYz;
        LightPoint *XYz;
        LightPoint *xyZ;
        LightPoint *XyZ;
        LightPoint *xYZ;
        LightPoint *XYZ;
    };

    struct QuadLightingInfo {
        float a;
        float b;
        float c;
        float d;
    };

    struct TriLightingInfo {
        float a;
        float b;
        float c;
    };

    struct AdjacentBlockInfo {
        bool pX;
        bool nX;
        bool pY;
        bool nY;
        bool pZ;
        bool nZ;
    };

    struct Point2D {
        int x;
        int y;
    };

    struct BlockInfo {
        Point2D pX;
        Point2D nX;
        Point2D pY;
        Point2D nY;
        Point2D pZ;
        Point2D nZ;
    };

    static BlockInfo Grass{
            {3, 1},
            {3, 1},
            {3, 2},
            {3, 0},
            {3, 1},
            {3, 1}
    };
    static BlockInfo Dirt{
            {3, 0},
            {3, 0},
            {3, 0},
            {3, 0},
            {3, 0},
            {3, 0}
    };
    static BlockInfo Stone{
            {3, 3},
            {3, 3},
            {3, 3},
            {3, 3},
            {3, 3},
            {3, 3}
    };


    struct Block {
        uint id;
        uint light_level;
        BlockGraphics *graphics;
    };
}

#endif //TEST_PROJECT_STRUCTS_H
