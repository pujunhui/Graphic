#pragma once 
#include "../global/base.h"
#include "dataStructures.h"
#include "bufferObject.h"
#include "texture.h"
#include <algorithm>

class Shader {
public:
    Shader();
    ~Shader();
    virtual VsOutput vertexShader(
        //VAO当中的bindingMap
        const std::map<uint32_t, BindingDescription>& bindingMap,

        //VBO当中的bindingMap
        const std::map<uint32_t, BufferObject*>& bufferMap,

        //当前要处理的顶点index
        const uint32_t& index
    ) = 0;

    virtual void fragmentShader(
        const VsOutput& input,
        FsOutput& output, 
        const std::map<uint32_t, Texture*>& textures
    ) = 0;

    //tool functions
public:
    //你要先看在VertexShader函数中如何使用，再看逻辑
    math::vec4f getVector(
        const std::map<uint32_t, BindingDescription>& bindingMap,
        const std::map<uint32_t, BufferObject*>& bufferMap,
        const uint32_t& attributeLocation, //当前属性的编号
        const uint32_t& index //当前顶点编号
    );

    RGBA vectorToRGBA(const math::vec4f& v) {
        //防止颜色越界
        math::vec4f c = v;
        c.x = std::clamp(c.x, 0.0f, 1.0f);
        c.y = std::clamp(c.y, 0.0f, 1.0f);
        c.z = std::clamp(c.z, 0.0f, 1.0f);
        c.w = std::clamp(c.w, 0.0f, 1.0f);

        RGBA color;
        color.mR = static_cast<byte>(c.x * 255.0f);
        color.mG = static_cast<byte>(c.y * 255.0f);
        color.mB = static_cast<byte>(c.z * 255.0f);
        color.mA = static_cast<byte>(c.w * 255.0f);

        return color;
    }
        
};
