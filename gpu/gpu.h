#pragma once 
#include "../global/base.h"
#include "frameBuffer.h"
#include "../application/application.h"
#include "../application/image.h"
#include "dataStructures.h"
#include "bufferObject.h"
#include "vao.h"

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

    //打印状态机
    void printVAO(const uint32_t& vaoId);

    uint32_t genBuffer();
    void deleteBuffer(const uint32_t& bufferId);
    void bindBuffer(const uint32_t& bufferType, const uint32_t& bufferId);
    void bufferData(const uint32_t& bufferType, size_t dataSize, void* data);

    uint32_t genVertexArray();
    void deleteVertexArray(const uint32_t& vaoId);
    void bindVertexArray(const uint32_t& vaoId);
    void vertexAttributePointer(
        const uint32_t& binding,
        const uint32_t& itemSize,
        const uint32_t& stride,
        const uint32_t& offset);

private:
    static GPU* mInstance;
    FrameBuffer* mFrameBuffer{ nullptr };
    
    //VBO相关/EBO也存在内部
    uint32_t mCurrentVBO{ 0 };
    uint32_t mCurrentEBO{ 0 };
    uint32_t mBufferCounter{ 0 };
    std::map<uint32_t, BufferObject*> mBufferMap;

    //VAO相关
    uint32_t mCurrentVAO{ 0 };
    uint32_t mVaoCounter{ 0 };
    std::map<uint32_t, VertexArrayObject*> mVaoMap;
};