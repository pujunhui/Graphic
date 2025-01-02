#pragma once 
#include "../global/base.h"
#include "dataStructures.h"


/*
* class Raster:
* 对外提供静态函数接口，传入离散的图元点，返回光栅化后的像素数组
*/
class Raster
{
public:
    Raster();
    ~Raster();

    static void rasterizeLine(
        std::vector<Point>& results,
        const Point& v0,
        const Point& v1
    );

    static void interpolantLine(
        const Point& v0,
        const Point& v1,
        Point& target
    );

    static void rasterizeTriangle(
        std::vector<Point>& results,
        const Point& v0,
        const Point& v1,
        const Point& v2
    );

    static void interpolantTriangle(
        const Point& v0,
        const Point& v1,
        const Point& v2,
        Point& target
    );

    //独立的lerp函数
    /*
    * 颜色从c0过度到c1，weight可视为进度
    * c = c0 * (1 - weight) + c1 * weight
    */
    static RGBA lerpRGBA(const RGBA& c0, const RGBA& c1, float weight);

    static RGBA lerpRGBA(
        const RGBA& c0, const RGBA& c1, const RGBA& c2,
        float weight0, float weight1, float weight2
    );

    static math::vec2f lerpUV(
        const math::vec2f& uv0, const math::vec2f& uv1, const math::vec2f& uv2,
        float weight0, float weight1, float weight2
    );
};
