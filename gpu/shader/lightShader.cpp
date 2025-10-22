#include "lightShader.h"

LightShader::LightShader() {}
LightShader::~LightShader() {}

VsOutput LightShader::vertexShader(
    const std::map<uint32_t, BindingDescription>& bindingMap,
    const std::map<uint32_t, BufferObject*>& bufferMap,
    const uint32_t& index
) {
    VsOutput output;

    //取出Attribute数值
    math::vec4f position = getVector(bindingMap, bufferMap, 0, index);
    //转换为齐次坐标
    position.w = 1.0f;

    //math::vec4f color = getVector(bindingMap, bufferMap, 1, index);
    math::vec3f normal = getVector(bindingMap, bufferMap, 1, index);
    math::vec2f uv = getVector(bindingMap, bufferMap, 2, index);

    output.mPosition = mProjectionMatrix * mViewMatrix * mModelMatrix * position;
    //output.mColor = color;
    // 添加法线矩阵，这样法线就会跟随模型旋转而变化
    // 变换法线使用模型矩阵的逆矩阵的转置；法线为方向向量，齐次坐标 w 必须为 0
    output.mNormal = math::transpose(math::inverse(mModelMatrix)) * math::vec4f(normal, 0.0f);
    output.mUV = uv;

    return output;
}

void LightShader::fragmentShader(
    const VsOutput& input,
    FsOutput& output,
    const std::map<uint32_t, Texture*>& textures
) {
    output.mPixelPos.x = static_cast<int>(input.mPosition.x);
    output.mPixelPos.y = static_cast<int>(input.mPosition.y);
    output.mDepth = input.mPosition.z;

    //Lambert光照模型算法

    //取出法线
    auto noraml = math::normalize(input.mNormal);
    auto lightDirection = math::normalize(mDirectionalLight.direction);

    //取出texture
    auto iter = textures.find(mDiffuseTexture);
    auto texture = iter->second;

    //计算颜色
    math::vec4f texColor = texture->getColor(input.mUV.x, input.mUV.y);

    //计算漫反射光
    math::vec4f diffuseColor;
    float diffuse = math::dot(noraml, -lightDirection);
    diffuse = std::clamp(diffuse, 0.0f, 1.0f);
    diffuseColor = texColor * diffuse * math::vec4f(mDirectionalLight.color, 1.0f);

    //计算环境光
    math::vec4f envColor;
    envColor = math::vec3f(texColor) * mEnvLight;

    output.mColor = vectorToRGBA(diffuseColor + envColor);
}
