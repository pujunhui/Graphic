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
    std::fill_n(mFrameBuffer->mDepthBuffer, pixelSize, 1.0f);
}

void GPU::drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color) {
    if (x >= mFrameBuffer->mWidth || y >= mFrameBuffer->mHeight) {
        return;
    }

    //从窗口左下角开始计算
    uint32_t pixelPos = y * mFrameBuffer->mWidth + x;

    RGBA result = color;
    if (mEnableBlending) {
        //进行颜色混合
        auto src = color;
        auto dst = mFrameBuffer->mColorBuffer[pixelPos];
        float weight = static_cast<float>(src.mA) / 255.0f;
        result = math::lerp(dst, src, weight);
    }
    mFrameBuffer->mColorBuffer[pixelPos] = result;
}

void GPU::drawLine(const Point& p0, const Point& p1) {
    std::vector<Point> pixels;
    Raster::rasterizeLine(pixels, p0, p1);

    for (auto &p : pixels) {
        drawPoint(p.x, p.y, p.color);
    }
}

void GPU::drawTriangle(const Point& p0, const Point& p1, const Point& p2) {
    std::vector<Point> pixels;
    Raster::rasterizeTriangle(pixels, p0, p1, p2);

    RGBA resultColor;
    for (auto &p : pixels) {
        if (mImage) {
            resultColor = sampleNearest(p.uv);
        }
        else {
            resultColor = p.color;
        }
        drawPoint(p.x, p.y, resultColor);
    }
}

void GPU::drawImage(const Image* image) {
    for (uint32_t i = 0; i < image->mHeight; i++) {
        for (uint32_t j = 0; j < image->mWidth; j++) {
            drawPoint(j, i, image->mData[i * image->mWidth + j]);
        }
    }
}

void GPU::drawImageWithAlpha(const Image* image, const uint32_t& alpha) {
    RGBA color;
    for (uint32_t i = 0; i < image->mHeight; i++) {
        for (uint32_t j = 0; j < image->mWidth; j++) {
            color = image->mData[i * image->mWidth + j];
            color.mA = alpha;
            drawPoint(j, i, color);
        }
    }
}

void GPU::setBlending(bool enable) {
    mEnableBlending = enable;
}

void GPU::setTexture(const Image* image) {
    mImage = image;
}

RGBA GPU::sampleNearest(const math::vec2f& uv) {
    auto myUV = uv;

    //四舍五入到最近的整数
    // u = 0 对应 x = 0，u = 1，对应 x = width -1
    // v = 0 对应 y = 0，v = 1，对应 y = height -1
    int x = std::round(myUV.x * (mImage->mWidth - 1));
    int y = std::round(myUV.y * (mImage->mHeight - 1));

    int position = y * mImage->mWidth + x;
    return mImage->mData[position];
}
