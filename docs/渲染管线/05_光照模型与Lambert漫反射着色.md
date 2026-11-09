# 经典光照模型与 Lambert 漫反射着色

真实的物理世界中，物体由于受到光线的照射才呈现出明暗立体感与空间质感。在不引入实时光线追踪（Ray Tracing）的情况下，实时渲染使用局部经验光照模型模拟光子反射。本项目在 `LightShader` 中实现了一套经典的 **Lambert 漫反射 + 基础环境光** 逐像素光照管线。

---

## 1. 光照模型的三大组成部分（Phong / Blinn-Phong 思想）

在经典经验光照模型中，物体表面的最终光照颜色分解为三个独立项的叠加：
$$I_{\text{total}} = I_{\text{ambient}} + I_{\text{diffuse}} + I_{\text{specular}}$$
1. **环境光项（Ambient Light）**：模拟光线在场景中经过无数次漫反射后的间接光能底色，确保背光面不会呈现死黑。
2. **漫反射项（Diffuse Light）**：光线射向粗糙物体表面后向所有方向均匀散射的能量，产生明暗渐变。
3. **高光反射项（Specular Light）**：光滑表面朝视线方向产生的镜面高光反射斑点。

本项目重点实现并优化了漫反射项与环境光项的着色。

---

## 2. 朗伯余弦定律（Lambert's Cosine Law）

### 2.1 物理原理
理想的漫反射表面（Lambertian Surface）具有微观粗糙结构。当一束平行光射向表面时：
- 光线垂直入射时（表面法线 $\mathbf{n}$ 与光线反方向平行），单位受光面积接收到的光通量密度最大。
- 随着光线斜射角度增大，受光面积被拉长，单位面积分配到的光子变少，表面变暗。
- 接收到的辐射照度与表面法向量 $\mathbf{n}$ 与光线入射方向的反方向 $\mathbf{l}$ 之间的夹角余弦成正比：
  $$I_{\text{diffuse}} \propto \cos\theta = \hat{\mathbf{n}} \cdot \hat{\mathbf{l}}$$

```
                 ^ 法线 n
                 │
                 │   / 光源方向 l (从表面指向光源)
                 │  / 
                 │θ/
─────────────────┴────────────────── 表面
```

### 2.2 向量定义与方向修正
在代码 `LightShader.h` 中，平行光结构体定义为：
```cpp
struct DirectionalLight {
    math::vec3f color;
    math::vec3f direction; // 光线行进方向 (从光源指向物体表面)
};
```
> **核心注意点**：
> `mDirectionalLight.direction` 是光线的行进方向（从光源指向表面，即 $-\mathbf{l}$）。
> 在片元着色器中计算点积时，夹角 $\theta$ 是**表面外法线**与**指向光源的连线**之间的夹角。因此必须对光照方向取反：
> $$\hat{\mathbf{l}} = -\text{normalize}(\text{mDirectionalLight.direction})$$
> 点积公式变为：
> $$\text{diffuse} = \mathbf{n} \cdot (-\hat{\mathbf{l}})$$

### 2.3 背光面截断（Clamping）
当光线从多边形背面照射过来时，$\theta > 90^\circ$，点积结果为负数。由于漫反射不可能产生负亮度，必须用 `std::clamp` 将其截断：
$$\text{diffuseFactor} = \max(\hat{\mathbf{n}} \cdot (-\hat{\mathbf{l}}), 0.0f) = \text{std::clamp}(\text{diffuse}, 0.0f, 1.0f)$$

---

## 3. 着色流程（Gouraud 着色 vs Phong 着色）

| 着色技术 | 计算发生的阶段 | 特点 |
| :--- | :--- | :--- |
| **Gouraud 着色（逐顶点光照）** | 顶点着色器（Vertex Shader） | 仅在顶点计算光照，光栅化线性插值颜色。性能极高，但高光会被平滑拉伸消失，低多边形网格明暗突变生硬。 |
| **Phong 着色（逐片元光照，本项目采用）** | 片元着色器（Fragment Shader） | 顶点着色器仅变换法线；在光栅化后，**每个片元像素独立对法线归一化并执行光照公式**。明暗过渡丝滑细腻。 |

### 3.1 `LightShader::vertexShader` 阶段
顶点着色器仅负责将法线变换至世界空间（使用逆转置矩阵且齐次坐标 $w=0$）：
```cpp
output.mNormal = math::transpose(math::inverse(mModelMatrix)) * math::vec4f(normal, 0.0f);
```

### 3.2 `LightShader::fragmentShader` 阶段
1. **法线与光照重新归一化**：
   经过三角形重心插值后，单位法线的模长已小于 1，必须重新单位化：
   ```cpp
   auto normal = math::normalize(input.mNormal);
   auto lightDir = math::normalize(mDirectionalLight.direction);
   ```
2. **纹理漫反射颜色采样**：
   ```cpp
   math::vec4f texColor = texture->getColor(input.mUV.x, input.mUV.y);
   ```
3. **漫反射与环境光调制合成**：
   ```cpp
   // 漫反射光: 纹理色 * 漫反射系数 * 光源色
   float diffuse = std::clamp(math::dot(normal, -lightDir), 0.0f, 1.0f);
   math::vec4f diffuseColor = texColor * diffuse * math::vec4f(mDirectionalLight.color, 1.0f);

   // 环境光: 纹理色 * 环境光色
   math::vec4f envColor = math::vec3f(texColor) * mEnvLight;

   // 最终输出色彩
   output.mColor = vectorToRGBA(diffuseColor + envColor);
   ```

---

## 4. 立方体网格法线构造经验（为何是 36 个顶点？）

在 `18_light.cpp` 中定义了一个单位立方体。
几何学上立方体仅有 8 个顶点，但在渲染管线中：
- 同一个角点对于前面（$+Z$）、顶面（$+Y$）和右面（$+X$）这三个面来说，**法线向量完全不同且相互正交**（分别为 $(0,0,1)$、$(0,1,0)$、$(1,0,0)$）。
- 如果强行让 8 个顶点共享索引，重心插值会把三个不同面的法线平均化为对角线方向，导致立方体看起来像一个光照圆滑的球体，棱角完全丢失（硬表面特征消失）。
- **因此硬表面几何体必须“拆点”**：立方体 6 个面，每个面由 2 个三角形共 6 个独立顶点组成，共计声明 **36 个互不共享法线的独立顶点**，确保每个面上的法向量严格垂直于该平面，展现出清晰分明的明暗硬边棱角。
