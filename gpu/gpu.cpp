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

void GPU::printVAO(const uint32_t& vaoId) {
    auto iter = mVaoMap.find(vaoId);
    if (iter != mVaoMap.end()) {
        iter->second->print();
    }
}

uint32_t GPU::genBuffer() {
    mBufferCounter++;
    mBufferMap.insert(std::make_pair(mBufferCounter, new BufferObject()));

    return mBufferCounter;
}

void GPU::deleteBuffer(const uint32_t& bufferId) {
    auto iter = mBufferMap.find(bufferId);
    if (iter != mBufferMap.end()) {
        delete iter->second;
    }
    else {
        return;
    }

    mBufferMap.erase(iter);
}

void GPU::bindBuffer(const uint32_t& bufferType, const uint32_t& bufferId) {
    if (bufferType == ARRAY_BUFFER) {
        mCurrentVBO = bufferId;
    }
    else if (bufferType == ELEMENT_ARRAY_BUFFER) {
        mCurrentEBO = bufferId;
    }
}

void GPU::bufferData(const uint32_t& bufferType, size_t dataSize, void* data) {
    uint32_t bufferId = 0;
    if (bufferType == ARRAY_BUFFER) {
        bufferId = mCurrentVBO;
    }
    else if (bufferType == ELEMENT_ARRAY_BUFFER) {
        bufferId = mCurrentEBO;
    }
    else {
        assert(false);
    }

    auto iter = mBufferMap.find(bufferId);
    if (iter == mBufferMap.end()) {
        assert(false);
    }

    BufferObject* bufferObject = iter->second;
    bufferObject->setBufferData(dataSize, data);
}

uint32_t GPU::genVertexArray() {
    mVaoCounter++;
    mVaoMap.insert(std::make_pair(mVaoCounter, new VertexArrayObject()));

    return mVaoCounter;
}

void GPU::deleteVertexArray(const uint32_t& vaoId) {
    auto iter = mVaoMap.find(vaoId);
    if (iter == mVaoMap.end()) {
        delete iter->second;
    }
    else {
        return;
    }

    mVaoMap.erase(iter);
}

void GPU::bindVertexArray(const uint32_t& vaoId) {
    mCurrentVAO = vaoId;
}

void GPU::vertexAttributePointer(
    const uint32_t& binding,
    const uint32_t& itemSize,
    const uint32_t& stride,
    const uint32_t& offset) {
    auto iter = mVaoMap.find(mCurrentVAO);
    if (iter == mVaoMap.end()) {
        assert(false);
    }

    auto vao = iter->second;
    vao->set(binding, mCurrentVBO, itemSize, stride, offset);
}
