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
private:
    GPU();

public:
    ~GPU();

    static GPU* getInstance();

    //接收外界传入的bmp对应的内存指针以及窗体的宽/高
    void initSurface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

    //清除画布内容
    void clear();

    //绘制基本图形
    void drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color);
    void drawLine(const Point& p0, const Point& p1);
    void drawTriangle(const Point& p0, const Point& p1, const Point& p2);

    //绘制图片
    void drawImage(const Image* image);
    void drawImageWithAlpha(const Image* image, const uint32_t& alpha);

    //设置颜色混合
    void setBlending(bool enable);

    //设置纹理
    void setTexture(const Image* image);

private:
    RGBA sampleNearest(const math::vec2f& uv);

private:
    static GPU* mInstance;
    FrameBuffer* mFrameBuffer{ nullptr };
    
    //纹理贴图
    const Image* mImage{ nullptr };

    //blending
    bool mEnableBlending{ false };
};