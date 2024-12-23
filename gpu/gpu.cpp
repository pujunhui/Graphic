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

        result = Raster::lerpRGBA(dst, src, weight);
    }

    mFrameBuffer->mColorBuffer[pxielPos] = result;
}

void GPU::drawLine(const Point& p1, const Point& p2) {
    std::vector<Point> pixels;
    Raster::rasterizeLine(pixels, p1, p2);

    for (auto& p : pixels) {
        drawPoint(p.x, p.y, p.color);
    }
}

void GPU::drawTriangle(const Point& p1, const Point& p2, const Point& p3) {
    std::vector<Point> pixels;
    Raster::rasterizeTriangle(pixels, p1, p2, p3);

    RGBA resultColor;
    for (auto& p : pixels) {
        if (mImage) {
            resultColor = mEnabelBilinear ? sampleBilinear(p.uv) : sampleNearest(p.uv);
        }
        else {
            resultColor = p.color;
        }
        drawPoint(p.x, p.y, resultColor);
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

void GPU::setBilinear(bool enable) {
    mEnabelBilinear = enable;
}

void GPU::setTexture(Image* image) {
    mImage = image;
}

void GPU::setTextureWrap(uint32_t wrap) {
    mWrap = wrap;
}

RGBA GPU::sampleNearest(const math::vec2f& uv) {
    auto myUV = uv;

    checkWrap(myUV.x);
    checkWrap(myUV.y);

    //四舍五入到最近整数
    //u = 0 对应 x = 0, u = 1 对应 x = width - 1
    //v = 0 对应 y = 0, v = 1 对应 y = height - 1
    int x = std::round(myUV.x * (mImage->mWidth - 1));
    int y = std::round(myUV.y * (mImage->mHeight - 1));

    int position = y * mImage->mWidth + x;
    return mImage->mData[position];
}

RGBA GPU::sampleBilinear(const math::vec2f& uv) {
    RGBA resultColor;

    auto myUV = uv;

    checkWrap(myUV.x);
    checkWrap(myUV.y);

    float x = myUV.x * static_cast<float>(mImage->mWidth - 1);
    float y = myUV.y * static_cast<float>(mImage->mHeight - 1);

    int left = std::floor(x);
    int right = std::ceil(x);
    int bottom = std::floor(y);
    int top = std::ceil(y);

    //对上下差值，得到左右
    float yScale = 0.0f;
    if (top == bottom) {
        yScale = 1.0f;
    }
    else {
        yScale = (y - static_cast<float>(bottom)) / static_cast<float>(top - bottom);
    }

    int positionLeftTop = top * mImage->mWidth + left;
    int positionLeftBottom = bottom * mImage->mWidth + left;
    int positionRightTop = top * mImage->mWidth + right;
    int positionRightBottom = bottom * mImage->mWidth + right;

    RGBA leftColor = Raster::lerpRGBA(mImage->mData[positionLeftBottom], mImage->mData[positionLeftTop], yScale);
    RGBA rightColor = Raster::lerpRGBA(mImage->mData[positionRightBottom], mImage->mData[positionRightTop], yScale);

    //对左右差值，得到结果
    float xScale = 0.0f;
    if (right == left) {
        xScale = 1.0f;
    }
    else {
        xScale = (x - static_cast<float>(left)) / static_cast<float>(right - left);
    }

    resultColor = Raster::lerpRGBA(leftColor, rightColor, xScale);

    return resultColor;
}

void GPU::checkWrap(float& n) {
    if (n > 1.0f || n < 0.0f) {
        n = FRACTION(n);
        auto m = FRACTION(n + 1);
        switch (mWrap)
        {
        case TEXTURE_WRAP_REPEAT:
            n = m;
            break;
        case TEXTURE_WRAP_MIRROR:
            n = 1.0f - m;
            break;
        default:
            break;
        }
    }
}
