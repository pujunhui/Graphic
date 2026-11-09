# 显存抽象与顶点数组状态机（VAO / VBO / EBO）

在早期的 OpenGL 1.x 固定管线时代，开发者使用 `glBegin()` / `glEnd()` 逐个提交顶点数据，CPU 与显卡之间存在极大的总线带宽瓶颈。现代图形 API（OpenGL 3.3+、Vulkan、DirectX 11/12）引入了**缓冲对象（Buffer Objects）**与**顶点数组对象（Vertex Array Objects）**的显存状态机架构。本项目在 CPU 软光栅中完整模拟了该架构。

---

## 1. 显存缓冲对象（`BufferObject`）：VBO 与 EBO

`BufferObject`（`gpu/bufferObject.h`）模拟了显卡板载显存的一块独立分配内存：
- **按需分配与复用**：内部维护 `byte* mBuffer`。当外部调用 `setBufferData(size, data)` 时，若当前内存为空或不足以容纳数据，则重新动态分配堆内存，并通过 `memcpy` 将 CPU 客户端数据深拷贝入虚拟显存。
- **角色解耦（Binding Targets）**：
  在 `GPU` 单例中，统一使用 `mBufferMap` 管理所有分配的缓冲对象，通过绑定目标区分用途：
  - **`ARRAY_BUFFER`（顶点缓冲对象，VBO）**：存储顶点属性流（位置、法线、切线、纹理坐标、顶点色）。
  - **`ELEMENT_ARRAY_BUFFER`（索引缓冲对象，EBO / IBO）**：存储构成多边形图元的顶点索引下标（`uint32_t` 数组）。采用索引绘制可以实现公共顶点的显存复用（例如一个立方体仅需存储 8 个空间顶点几何坐标，而非 36 个冗余顶点）。

---

## 2. 顶点数组对象（`VertexArrayObject`，VAO）

### 2.1 为什么需要 VAO？
VBO 只是一块没有格式定义的无类型原始二进制字节流（`byte*`）。着色器如何知道：
- 属性 0（位置）从哪个 VBO 读？每次读几个 `float`？相邻顶点间隔几个字节？
- 属性 1（法线）的字节偏移量是多少？

**VAO 本身不存储任何顶点数据，它是一个“属性解析元数据字典”**，记录着着色器输入属性与各 VBO 之间的映射配置。每次切换绘制不同的模型时，只需一行 `sgl->bindVertexArray(vaoId)`，便能瞬间恢复该模型的所有属性解析配置。

### 2.2 属性描述结构体（`BindingDescription`）
每个属性插槽记录如下字段（`gpu/dataStructures.h`）：
```cpp
struct BindingDescription {
    uint32_t mVboId;    // 数据所在的 VBO ID
    size_t   mItemSize; // 属性分量数（如 vec3 为 3, vec2 为 2）
    size_t   mStride;   // 相邻顶点之间的总步长（以字节为单位）
    size_t   mOffset;   // 该属性在单个顶点数据块内的起始字节偏移
};
```

---

## 3. 两种典型的顶点内存布局对比

### 3.1 分离式顶点缓冲（Separate VBOs，如 `10_vao.cpp`）
每种属性独立存放在单独的 VBO 中：
```cpp
// 1. 位置 VBO: Stride = 3 * sizeof(float), Offset = 0
sgl->bindBuffer(ARRAY_BUFFER, positionVbo);
sgl->vertexAttributePointer(0, 3, 3 * sizeof(float), 0);

// 2. 颜色 VBO: Stride = 4 * sizeof(float), Offset = 0
sgl->bindBuffer(ARRAY_BUFFER, colorVbo);
sgl->vertexAttributePointer(1, 4, 4 * sizeof(float), 0);

// 3. 纹理坐标 VBO: Stride = 2 * sizeof(float), Offset = 0
sgl->bindBuffer(ARRAY_BUFFER, uvVbo);
sgl->vertexAttributePointer(2, 2, 2 * sizeof(float), 0);
```

### 3.2 交叉式顶点缓冲（Interleaved VBO，如 `14_depthTest` ~ `19_model`）
同一个顶点的所有属性在内存中打包为一个连续的 `struct Vertex`：
```cpp
struct Vertex {
    math::vec3f position;  // Offset = 0
    math::vec3f normal;    // Offset = 12
    math::vec2f texCoords; // Offset = 24
};                         // Stride = 32 字节

// 绑定同一个 VBO，步长统一设为 sizeof(Vertex)，通过 offset 区分字段
sgl->bindBuffer(ARRAY_BUFFER, interleavedVbo);
sgl->vertexAttributePointer(0, 3, sizeof(Vertex), 0);
sgl->vertexAttributePointer(1, 3, sizeof(Vertex), sizeof(float) * 3);
sgl->vertexAttributePointer(2, 2, sizeof(Vertex), sizeof(float) * 6);
```

### 3.3 为什么工程实践强烈推崇交叉式布局？
- **CPU/GPU 缓存命中率（Cache Locality）极高**：
  在处理单个顶点时，其位置、法线和 UV 几乎总是被着色器同时访问。交叉式存储使得这些数据紧挨在一起，CPU 只需一次读取即可将整个顶点载入 L1/L2 Cache Line，彻底避免了在多个分离内存区域之间来回跳跃导致的 Cache Miss。

---

## 4. 着色器属性动态提取机制（`Shader::getVector`）

在顶点着色器执行时，`Shader::getVector` 依据传入的 `attributeLocation`（属性插槽）与当前顶点索引 `index`，动态完成内存物理地址推算与安全提取：
1. 从当前绑定的 VAO 中查出插槽对应的 `BindingDescription`；
2. 从 `mBufferMap` 中找到对应的 VBO 内存指针 `buffer`；
3. 计算该顶点属性的精确字节偏移量：
   $$\text{DataOffset} = \text{stride} \times \text{index} + \text{offset}$$
4. 通过 `memcpy(&result, buffer + DataOffset, itemSize * sizeof(float))` 将原始浮点流灌入 `math::vec4f`，供着色器进行 MVP 矩阵乘法变换。
