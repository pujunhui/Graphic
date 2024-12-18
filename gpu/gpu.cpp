#include "gpu.h"
#include "raster.h"

//��ʼ��GPU�ľ�̬����
GPU* GPU::mInstance = nullptr;
GPU* GPU::getInstance() {
    //���mInstance�Ѿ�ʵ�����ˣ�new�����ˣ�����ֱ�ӷ���
    //������Ҫ��new�������ٷ���
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

    //�Ӵ������½ǿ�ʼ����
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