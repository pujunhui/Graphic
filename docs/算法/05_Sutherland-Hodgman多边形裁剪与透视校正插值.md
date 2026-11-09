# Sutherland-Hodgman 视锥体裁剪与透视校正插值

在 3D 软光栅管线中，当物体移动穿过相机近平面或视锥体四周时，若不进行裁剪，会导致严重的两大问题：
1. **除零与背后翻转**：顶点处于相机后方时深度 $w \le 0$，透视除法除以负数导致图元在屏幕前方反向投影、畸变翻转。
2. **算力浪费与数值溢出**：若不加限制，屏幕外数千像素外的顶点会形成巨大的包围盒，导致 CPU 遍历数以亿计的无效像素，帧率断崖下跌。

本模块系统解析在齐次裁剪空间下基于 **Sutherland-Hodgman 算法**的多边形裁剪与缝补剖分，以及解决屏幕空间非线性插值畸变的**透视校正插值（Perspective-Correct Interpolation）**。

---

## 1. 齐次裁剪空间的 7 个超平面定义

裁剪发生在**透视除法之前**的 4D 齐次裁剪空间 $(x, y, z, w)$ 中。
根据透视投影条件，视锥体内可见区域必须满足：
$$-w \le x \le w, \quad -w \le y \le w, \quad -w \le z \le w, \quad w > 0$$

将上述 7 个不等式统一化为四维超平面点积判定形式 $\mathbf{P} \cdot \mathbf{Plane}_i \ge 0$：
```cpp
std::vector<math::vec4f> clipPlanes = {
    // 1. W > 0 边界（防止位于相机背后）
    math::vec4f( 0.0f,  0.0f,  0.0f,  1.0f), // dot(P, Plane) = w >= 0
    // 2. Near 近裁剪平面 (-w <= z)
    math::vec4f( 0.0f,  0.0f,  1.0f,  1.0f), // z + w >= 0
    // 3. Far 远裁剪平面 (z <= w)
    math::vec4f( 0.0f,  0.0f, -1.0f,  1.0f), // -z + w >= 0
    // 4. Left 左视锥面 (-w <= x)
    math::vec4f( 1.0f,  0.0f,  0.0f,  1.0f), // x + w >= 0
    // 5. Right 右视锥面 (x <= w)
    math::vec4f(-1.0f,  0.0f,  0.0f,  1.0f), // -x + w >= 0
    // 6. Top 上视锥面 (y <= w)
    math::vec4f( 0.0f, -1.0f,  0.0f,  1.0f), // -y + w >= 0
    // 7. Bottom 下视锥面 (-w <= y)
    math::vec4f( 0.0f,  1.0f,  0.0f,  1.0f)  // y + w >= 0
};
```
点 $\mathbf{P}$ 在平面内侧的充要条件简化为单次四维向量点积：
$$\text{inside}(\mathbf{P}) \iff \mathbf{P} \cdot \mathbf{Plane} \ge 0$$

---

## 2. Sutherland-Hodgman 多边形逐平面裁剪算法

算法对上述 7 个裁剪平面执行级联流水线过滤：前一个平面输出的多边形顶点列表，作为后一个平面的输入。

```
输入多边形 ──> [ W平面裁剪 ] ──> [ Near平面 ] ──> ... ──> [ Bottom平面 ] ──> 最终可见多边形
```

### 2.1 边的四种相交情况
遍历多边形所有有向边 $S(\text{last}) \to P(\text{current})$：
1. **情况 1（都在外侧：外 $\to$ 外）**：无任何顶点输出。
2. **情况 2（由内向外：内 $\to$ 外）**：输出线段与平面的交点 $I$。
3. **情况 3（由外向内：外 $\to$ 内）**：先输出交点 $I$，随后输出终点 $P$。
4. **情况 4（都在内侧：内 $\to$ 内）**：直接输出终点 $P$。

### 2.2 超平面交点与属性线性插值（`Clipper::intersect`）
设线段端点为 $S$ 和 $P$，其到超平面的代数有向距离分别为：
$$d_S = S \cdot \mathbf{Plane}, \qquad d_P = P \cdot \mathbf{Plane}$$
在相交边界处，代数距离为 0。根据相似性，交点在线段上的比例权重为：
$$t = \frac{d_S}{d_S - d_P} \quad (t \in [0, 1])$$
根据权重 $t$，对所有顶点属性（位置、颜色、法线、UV）进行线性插值：
$$\text{IntersectVertex} = \text{lerp}(S, P, t)$$

### 2.3 凸多边形三角剖分（Triangle Fan 缝补）
一个三角形被视锥体削去一个角后，可能变成四边形或五边形（最多多达六边形）。
裁剪完成后，以输出顶点列表的第 0 个顶点为扇心（Triangle Fan），按顺序剖分为若干独立的三角形：
```cpp
for (uint32_t c = 0; c < results.size() - 2; c++) {
    outputs.push_back(results[0]);
    outputs.push_back(results[c + 1]);
    outputs.push_back(results[c + 2]);
}
```
保证输出给后续管线的始终是标准的三角形图元。

---

## 3. 透视校正插值（Perspective-Correct Interpolation）

### 3.1 核心痛点：屏幕空间仿射插值的失真
透视投影除以深度 $w = -z_{\text{view}}$ 是非线性运算。
三维空间中物体表面上等距分布的条纹（如棋盘格），投影到屏幕上之后，由于“近大远小”，近处的纹理像素跨度大，远处的跨度小。
**若直接在屏幕空间中对属性 $I$（如 UV、法线）依据屏幕二维重心坐标做线性插值，会导致纹理在表面上发生严重扭曲与抖动（仿射纹理映射缺陷，PS1 经典拉伸波纹）。**

```
三维空间（线性等分）:  |───|───|───|───|───|
屏幕空间（近大远小）:  |───────|─────|───|──|─|  (非线性分布！)
```

### 3.2 严格数学推导：为什么 $1/w$ 与 $I/w$ 在屏幕空间是线性的？
设三维空间中直线（或平面）的参数方程为：
$$X = X_0 + s \Delta X, \quad Z = Z_0 + s \Delta Z$$
某个属性沿表面的线性分布为：$I(s) = I_0 + s \Delta I$。

在透视投影中，屏幕横坐标为 $x = \dfrac{X}{-Z}$，齐次深度分量 $w = -Z$。
考察屏幕空间中 $\dfrac{1}{w}$ 的表达式：
$$\frac{1}{w} = \frac{1}{-Z_0 - s \Delta Z}$$
反解参数 $s$：
$$s = \frac{-(Z_0 + w)}{\Delta Z}$$
将 $s$ 代入投影公式，经过代数整理可证明：
$$\frac{1}{w(x)} = \left(1 - t_{\text{screen}}\right) \frac{1}{w_0} + t_{\text{screen}} \frac{1}{w_1}$$
$$\frac{I(x)}{w(x)} = \left(1 - t_{\text{screen}}\right) \frac{I_0}{w_0} + t_{\text{screen}} \frac{I_1}{w_1}$$
**重要定理**：
**虽然属性 $I$ 和深度 $Z$ 本身在屏幕空间非线性，但倒数 $\dfrac{1}{w}$ 以及加权属性 $\dfrac{I}{w}$ 在屏幕空间中严格呈线性关系！**

### 3.3 软光栅三步执行流程

1. **透视除法阶段（`GPU::perspectiveDivision`）**：
   计算顶点的倒数 $1/w$ 并暂存：
   ```cpp
   vsOutput.mOneOverW = 1.0f / vsOutput.mPosition.w;
   // 将所有待插值属性乘以 1/w
   vsOutput.mColor  *= vsOutput.mOneOverW;
   vsOutput.mNormal *= vsOutput.mOneOverW;
   vsOutput.mUV     *= vsOutput.mOneOverW;
   ```
2. **光栅化屏幕重心插值阶段（`Raster::rasterizeTriangle1`）**：
   在屏幕二维包围盒内，使用屏幕重心坐标权重 $(\alpha, \beta, \gamma)$，直接对 $\dfrac{1}{w}$ 以及 $\dfrac{I}{w}$ 进行常规线性加权：
   $$\overline{\left(\frac{1}{w}\right)} = \alpha \frac{1}{w_0} + \beta \frac{1}{w_1} + \gamma \frac{1}{w_2}$$
   $$\overline{\left(\frac{I}{w}\right)} = \alpha \frac{I_0}{w_0} + \beta \frac{I_1}{w_1} + \gamma \frac{I_2}{w_2}$$
3. **片元着色透视恢复阶段（`GPU::perspectiveRecover`）**：
   进入片元着色器前，将插值后的属性除以插值后的 $\dfrac{1}{w}$，还原出三维空间中真实的几何属性：
   $$I_{\text{correct}} = \frac{\overline{\left(\dfrac{I}{w}\right)}}{\overline{\left(\dfrac{1}{w}\right)}}$$
   彻底消除了纹理由于透视形变产生的弯曲，实现了与硬件 GPU 完全一致的精确纹理映射与平滑着色。
