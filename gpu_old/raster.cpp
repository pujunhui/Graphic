#include "raster.h"
#include "../math/math.h"

Raster::Raster()
{
}

Raster::~Raster()
{
}

//bersenham直线绘制算法
void Raster::rasterizeLine(std::vector<Point>& results, const Point& v0, const Point& v1) {
    Point start = v0;
    Point end = v1;

    // 垂直线
    if (v0.x == v1.x) {
        if (start.y > end.y) {
            auto tmp = start;
            start = end;
            end = tmp;
        }
        int length = end.y - start.y;
        Point point;
        for (int y = start.y; y < end.y; y++) {
            point.x = start.x;
            point.y = y;
            float weight = (float)(y - start.y) / length;
            point.color = math::lerp(start.color, end.color, weight);
            results.push_back(point);
        }
        return;
    }

    // 水平线
    if (v0.y == v1.y) {
        if (start.x > end.x) {
            auto tmp = start;
            start = end;
            end = tmp;
        }
        int length = end.x - start.x;
        Point point;
        for (int x = start.x; x < end.x; x++) {
            point.x = x;
            point.y = start.y;
            float weight = (float)(x - start.x) / length;
            point.color = math::lerp(start.color, end.color, weight);
            results.push_back(point);
        }
        return;
    }

    // 一般情况使用Bresenham算法

    //1 保证x方向是从小到大的
    if (start.x > end.x) {
        auto tmp = start;
        start = end;
        end = tmp;
    }

    results.push_back(start);

    //2 保证y方向也是从小到大，如果需要翻转，必须记录
    bool flipY = false;
    if (start.y > end.y) {
        start.y *= -1.0f;
        end.y *= -1.0f;
        flipY = true;
    }

    //3 保证斜率在0~1之间，如果需要调整，必须记录
    int deltaX = static_cast<int>(end.x - start.x);
    int deltaY = static_cast<int>(end.y - start.y);

    bool swapXY = false;
    if (deltaX < deltaY) {
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
        std::swap(deltaX, deltaY);
        swapXY = true;
    }

    //4 brensenham
    int currentX = static_cast<int>(start.x);
    int currentY = static_cast<int>(start.y);

    int resultX = 0;
    int resultY = 0;

    Point currentPoint;
    int p = 2 * deltaY - deltaX;

    for (int i = 0; i < deltaX; i++) {
        if (p >= 0) {
            currentY += 1;
            p -= 2 * deltaX;
        }

        currentX += 1;
        p += 2 * deltaY;

        //处理新xy，flip and swap
        resultX = currentX;
        resultY = currentY;
        if (swapXY) {
            std::swap(resultX, resultY);
        }

        if (flipY) {
            resultY *= -1;
        }

        //产生新顶点
        currentPoint.x = resultX;
        currentPoint.y = resultY;

        //这里不能传入start和end，因为currentPoint点是需要绘制的点，它一定在原始直线v0,v1上，而start,end是从v0,v1变换而来
        interpolantLine(v0, v1, currentPoint);

        results.push_back(currentPoint);
    }
}

void Raster::rasterizeTriangle(std::vector<Point>& results, const Point& v0, const Point& v1, const Point& v2) {
    //获取三角形最小包围矩形
    int minX = static_cast<int>(std::min(v0.x, std::min(v1.x, v2.x)));
    int maxX = static_cast<int>(std::max(v0.x, std::max(v1.x, v2.x)));
    int minY = static_cast<int>(std::min(v0.y, std::min(v1.y, v2.y)));
    int maxY = static_cast<int>(std::max(v0.y, std::max(v1.y, v2.y)));

    math::vec2f pv0, pv1, pv2;
    Point result;
    for (int i = minX; i <= maxX; ++i) {
        for (int j = minY; j <= maxY; ++j) {
            pv0 = math::vec2f(v0.x - i, v0.y - j);
            pv1 = math::vec2f(v1.x - i, v1.y - j);
            pv2 = math::vec2f(v2.x - i, v2.y - j);

            auto crossl = math::cross(pv0, pv1);
            auto cross2 = math::cross(pv1, pv2);
            auto cross3 = math::cross(pv2, pv0);

            //为了包含边缘像素点，允许叉积为0，否则会导致相邻三角形连接处会出现未填充点
            bool negativeAll = crossl <= 0 && cross2 <= 0 && cross3 <= 0;
            bool positiveAll = crossl >= 0 && cross2 >= 0 && cross3 >= 0;

            if (negativeAll || positiveAll) {
                result.x = i;
                result.y = j;
                //利用重心插值算法对三角形中的点进行颜色插值
                interpolantTriangle(v0, v1, v2, result);

                results.push_back(result);
            }
        }
    }
}

void Raster::interpolantLine(const Point& v0, const Point& v1, Point& target) {
    float weight = 1.0f;
    int deltaX = static_cast<int>(v1.x - v0.x);
    int deltaY = static_cast<int>(v1.y - v0.y);
    if (abs(deltaX) >= abs(deltaY)) {
        //用x做比例
        weight = (float)(target.x - v0.x) / deltaX;
    }
    else {
        //用y做比例
        weight = (float)(target.y - v0.y) / deltaY;
    }
    target.color = math::lerp(v0.color, v1.color, weight);
}

void Raster::interpolantTriangle(const Point& v0, const Point& v1, const Point& v2, Point& target) {
    auto e1 = math::vec2f(v1.x - v0.x, v1.y - v0.y);
    auto e2 = math::vec2f(v2.x - v0.x, v2.y - v0.y);
    float sumArea = std::abs(math::cross(e1, e2));

    auto pv0 = math::vec2f(v0.x - target.x, v0.y - target.y);
    auto pv1 = math::vec2f(v1.x - target.x, v1.y - target.y);
    auto pv2 = math::vec2f(v2.x - target.x, v2.y - target.y);

    //计算v0的权重
    float vOArea = std::abs(math::cross(pv1, pv2));
    float v1Area = std::abs(math::cross(pv0, pv2));
    float v2Area = std::abs(math::cross(pv0, pv1));

    float weight0 = vOArea / sumArea;
    float weight1 = v1Area / sumArea;
    float weight2 = v2Area / sumArea;

    //对于颜色的插值
    target.color = math::lerp(v0.color, v1.color, v2.color, weight0, weight1, weight2);
    //对于UV坐标的插值
    target.uv = math::lerp(v0.uv, v1.uv, v2.uv, weight0, weight1, weight2);
}
