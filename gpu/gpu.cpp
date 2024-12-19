#include "gpu.h"
#include "raster.h"

//初始化GPU的静态变量
GPU* GPU::mInstance = nullptr;
GPU* GPU::getInstance() {
    //如果mInstance已经实例化了（new出来了），就直接返回
    //否则需要先new出来，再返回
    if (mInstance == nullptr) {
        mInstance = new GPU();
    }

    return mInstance;
}

GPU::GPU() {

}

GPU::~GPU() {
    if (mFrameBuffer) {
        delete mFrameBuffer;
    }
}

void GPU::initSurface(const uint32_t& width, const uint32_t& height, void* buffer) {
    mFrameBuffer = new FrameBuffer(width, height, buffer);
}

void GPU::clear() {
    size_t pixelSize = mFrameBuffer->mWidth * mFrameBuffer->mHeight;
    std::fill_n(mFrameBuffer->mColorBuffer, pixelSize, RGBA(0, 0, 0, 0));
}

void GPU::drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color) {
    if (x >= mFrameBuffer->mWidth || y >= mFrameBuffer->mHeight) {
        return;
    }

    //从窗口左下角开始计算
    uint32_t pxielPos = y * mFrameBuffer->mWidth + x;
    mFrameBuffer->mColorBuffer[pxielPos] = color;
}

void GPU::drawLine(const Point& p1, const Point& p2) {
    std::vector<Point> pixels;
    Raster::rasterizeLine(pixels, p1, p2);
    
    for (auto p : pixels) {
        drawPoint(p.x, p.y, p.color);
    }
}

void GPU::drawTriangle(const Point& p1, const Point& p2, const Point& p3) {
    std::vector<Point> pixels;
    Raster::rasterizeTriangle(pixels, p1, p2, p3);

    for (auto p : pixels) {
        drawPoint(p.x, p.y, p.color);
    }
}