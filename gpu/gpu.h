#pragma once 
#include "../global/base.h"
#include "frameBuffer.h"
#include "../application/application.h"
#include "../application/image.h"

#define sgl GPU::getInstance()

/*
*class GPU:
* 模拟GPU的绘图行为以及算法等
*/
class GPU {
public:
    static GPU* getInstance();
    ~GPU();

    //接收外界传入的bmp对应的内存指针以及窗体的宽/高
    void initSurface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

    //清除画布内容
    void clear();

    //传入像素位置，绘制成某种颜色
    void drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color);

    void drawLine(const Point& p1, const Point& p2);

    void drawTriangle(const Point& p1, const Point& p2, const Point& p3);

    void drawImage(Image* image);

    void drawImageWithAlpha(Image* image, const uint32_t& alpha);

    //设置状态
    void setBlending(bool enable);

private:
    static GPU* mInstance;

    GPU();

    bool mEnabelBlending{ false };

    FrameBuffer* mFrameBuffer{ nullptr };
};