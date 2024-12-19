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

    RGBA result = color;
    if (mEnabelBlending) {
        //加入blending
        auto src = color;
        auto dst = mFrameBuffer->mColorBuffer[pxielPos];
        float weight = static_cast<float>(src.mA) / 255.0f;

        result.mR = static_cast<float>(src.mR) * weight + static_cast<float>(dst.mR) * (1.0f - weight);
        result.mG = static_cast<float>(src.mG) * weight + static_cast<float>(dst.mG) * (1.0f - weight);
        result.mB = static_cast<float>(src.mB) * weight + static_cast<float>(dst.mB) * (1.0f - weight);
        result.mA = static_cast<float>(src.mA) * weight + static_cast<float>(dst.mA) * (1.0f - weight);
    }

    mFrameBuffer->mColorBuffer[pxielPos] = result;
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

void GPU::drawImage(Image* image) {
    for (uint32_t y = 0; y < image->mHeight; y++) {
        for (uint32_t x = 0; x < image->mWidth; x++) {
            drawPoint(x, y, image->mData[y * image->mWidth + x]);
        }
    }
}

void GPU::drawImageWithAlpha(Image* image, const uint32_t& alpha) {
    RGBA color;
    for (uint32_t y = 0; y < image->mHeight; y++) {
        for (uint32_t x = 0; x < image->mWidth; x++) {
            color = image->mData[y * image->mWidth + x];
            color.mA = alpha;
            drawPoint(x, y, color);
        }
    }
}

void GPU::setBlending(bool enable) {
    mEnabelBlending = enable;
}