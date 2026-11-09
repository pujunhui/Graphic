# 背面剔除与深度测试（Z-Buffer）

在三维空间中，不同物体之间相互重叠、多边形自身存在背向相机的一面。若不加判定直接按绘制顺序覆盖，会导致背后的物体遮挡住前面的物体（反向穿模）。解决可见性问题的两大核心机制分别是**背面剔除（Back-Face Culling）**与**深度测试（Depth Testing）**。

---

## 1. 背面剔除（Back-Face Culling）

### 1.1 原理与管线优势
对于闭合不透明的 3D 网格（如球体、立方体、人体模型），相机永远不可能看到背向自己的面片。
如果在光栅化之前将背面的三角形丢弃，可以**立刻减少约 50% 的光栅化遍历、片元着色器执行与深度读写操作**。

### 1.2 投影后左手系与法向量 Z 符号判据
给定三角形在投影后空间的三个顶点 $V_0, V_1, V_2$，计算边向量及其叉积法向量：
$$\mathbf{e}_1 = V_1 - V_0, \quad \mathbf{e}_2 = V_2 - V_0, \quad \mathbf{n} = \mathbf{e}_1 \times \mathbf{e}_2$$

> **关键坐标系手性转换**：
> 观察空间是右手系（相机朝向 $-Z$），但透视投影矩阵中包含 $P_{3,2} = -1$，变换后的裁剪空间与 NDC 空间**变成了左手坐标系**（$Z$ 轴深度向屏幕内部延伸，相机视线看向 $+Z$）。

在左手系下：
- 若顶点在屏幕上呈现**逆时针（`FRONT_FACE_CCW`）**，用右手定则公式算出的代数叉积 $n_z = e_{1x} e_{2y} - e_{1y} e_{2x} > 0$。
- 若顶点在屏幕上呈现**顺时针（`FRONT_FACE_CW`）**，则 $n_z < 0$。

### 1.3 状态机判定对照表（`Clipper::cullFace`）

| 剔除配置 (`cullFace`) | 正面定义 (`frontFace`) | 目标保留哪个面 | 保留条件（返回 `true`） |
| :--- | :--- | :--- | :--- |
| **`BACK_FACE`（剔除背面）** | `FRONT_FACE_CCW`（逆时针） | 正面 | `normal.z > 0` |
| **`BACK_FACE`（剔除背面）** | `FRONT_FACE_CW`（顺时针） | 正面 | `normal.z < 0` |
| **`FRONT_FACE`（剔除正面）** | `FRONT_FACE_CCW`（逆时针） | 背面 | `normal.z < 0` |
| **`FRONT_FACE`（剔除正面）** | `FRONT_FACE_CW`（顺时针） | 背面 | `normal.z > 0` |

---

## 2. 深度测试（Z-Buffer Depth Test）

背面剔除只能处理单个闭合网格自身的背离面，无法解决**多个独立物体之间相互遮挡、穿插**的问题。

### 2.1 传统画家算法（Painter's Algorithm）的失效
画家算法尝试在 CPU 侧先将场景中所有多边形按距离相机远近排序，先画最远的多边形，最后画最近的。
- **缺点与失效场景**：
  1. 多边形相互环形穿插（如 A 遮挡 B，B 遮挡 C，C 遮挡 A）时，无法定义排序次序。
  2. 每帧对数十万个三角面片在 CPU 端执行全局排序，开销极大。

### 2.2 Z-Buffer 算法思想
Z-Buffer 是硬件 GPU 与软光栅解决遮挡问题的标准方案：
- 为每个屏幕像素分配额外的存储空间——**深度缓冲（`float* mDepthBuffer`）**，尺寸与颜色缓冲完全一致（$W \times H$）。
- 深度缓冲记录着**当前该像素位置所渲染物体的最小深度值（即离相机最近的距离）**。

### 2.3 深度映射范围与每帧重置
- 经过透视除法与视口变换矩阵（`math::screenMatrix`）后，NDC 的 $z_{\text{ndc}} \in [-1, 1]$ 被线性映射到 $[0.0, 1.0]$：
  $$z_{\text{screen}} = 0.5 \cdot z_{\text{ndc}} + 0.5$$
  - $z = 0.0$：对应视锥体近平面（离相机最近）；
  - $z = 1.0$：对应视锥体远平面（离相机最远）。
- 在每帧渲染前的 `GPU::clear()` 中：
  ```cpp
  std::fill_n(mFrameBuffer->mDepthBuffer, pixelSize, 1.0f);
  ```
  将所有像素的深度值重置为最远极限值 `1.0f`。

### 2.4 逐片元测试逻辑（`GPU::depthTest`）
当光栅化生成一个片元并计算出其实际插值屏幕深度 `output.mDepth` 时：
```cpp
bool GPU::depthTest(const FsOutput& output) {
    uint32_t pixelPos = output.mPixelPos.y * mFrameBuffer->mWidth + output.mPixelPos.x;
    float oldDepth = mFrameBuffer->mDepthBuffer[pixelPos];

    switch (mDepthFunc) {
    case DEPTH_LESS: // 默认模式：较近的通过
        if (output.mDepth < oldDepth) {
            mFrameBuffer->mDepthBuffer[pixelPos] = output.mDepth; // 更新深度缓冲
            return true;                                          // 允许写入颜色
        }
        return false;                                             // 丢弃片元
    case DEPTH_GREATER:
        if (output.mDepth > oldDepth) {
            mFrameBuffer->mDepthBuffer[pixelPos] = output.mDepth;
            return true;
        }
        return false;
    default:
        return false;
    }
}
```
通过逐像素的比对与就地更新，完全摆脱了多边形预排序的需求，任何相互穿插的复杂几何体均能展现出物理上精确的遮挡剪裁。

---

## 3. 非线性深度精度分布与 Z-Fighting

### 3.1 深度映射的非线性数学本质
在透视投影中，裁剪空间的 $z_{\text{clip}}$ 与 $w_{\text{clip}}$ 计算公式为：
$$z_{\text{clip}} = -\frac{f + n}{f - n} z_{\text{view}} - \frac{2fn}{f - n}, \quad w_{\text{clip}} = -z_{\text{view}}$$
透视除法后得到的 NDC 深度为：
$$z_{\text{ndc}} = \frac{z_{\text{clip}}}{w_{\text{clip}}} = \frac{f + n}{f - n} + \frac{2fn}{f - n} \cdot \frac{1}{z_{\text{view}}}$$
映射到 $[0.0, 1.0]$ 的深度缓冲值 $z_{\text{depth}}$ 与观察空间距离具有反比例关系：
$$z_{\text{depth}} = \alpha + \frac{\beta}{z_{\text{view}}}$$

### 3.2 深度精度分布对照表（以 $n = 0.1, f = 100$ 为例）

| 观察空间深度 $-z_{\text{view}}$ | NDC 深度 $z_{\text{ndc}}$ | 深度缓冲值 $z_{\text{depth}} \in [0, 1]$ | 精度分布区间占比特征 |
| :--- | :--- | :--- | :--- |
| **0.1（近平面）** | -1.0000 | **0.0000** | ──┐ |
| **0.2** | -0.9800 | 0.0100 |   │ 极高精度区间 |
| **0.5** | -0.9200 | 0.0400 |   │ （近处物体） |
| **1.0** | -0.8182 | **0.0909** | ──┘ |
| **2.0** | -0.6364 | 0.1818 | |
| **5.0** | -0.2727 | 0.3636 | |
| **10.0** | 0.1818 | **0.5909** | ── 前 10% 的世界深度占用了近 60% 精度空间 |
| **20.0** | 0.5455 | 0.7727 | |
| **50.0** | 0.8182 | 0.9091 | |
| **100.0（远平面）** | 1.0000 | **1.0000** | ── 低精度区间（远处物体） |

> **关键认知**：
> 观察空间前 10% 的距离（0.1 到 10.0）占据了整个 32 位浮点深度缓冲近 60% 的数值动态区间；而后 90% 的广袤距离（10.0 到 100.0）只能共用剩下的 40% 数值空间。这与人眼感知和近处细节渲染高度契合，但也为远景遮挡埋下了隐患。

### 3.3 深度冲突（Z-Fighting）成因与工程应对

#### 1. 产生原因
当两个几何平面在世界空间中距离非常近（如贴在墙壁上的海报、共面地面贴花、远处的重叠山峦），由于远处浮点精度离散化间隔较大，插值计算的微小浮点误差会导致两个多边形的片元深度值在各个像素上交替比对方更小，画面呈现出剧烈闪烁的杂乱条纹现象，即 **Z-Fighting**。

#### 2. 工程优化方案
1. **适度拉大近裁剪面（Near Plane）**：
   - 绝不要将近平面设为极小值（如 `0.001`）。
   - 将近平面从 `0.01` 提升到 `0.1` 甚至 `1.0`，能将全场景深度精度提升数个数量级。
2. **多边形偏移（Polygon Offset）**：
   - 在渲染贴花或描边时，微调投影矩阵深度偏移量（或片元着色器添加微小深度偏置 $\epsilon$），强制让前景图元深度略微提前。
3. **现代 Reverse-Z 技术**：
   - 将近平面映射为 $1.0$，远平面映射为 $0.0$。结合 IEEE 754 浮点数在接近 0 处指数分布更密集的特性，Reverse-Z 可以抹平远近精度差距，实现近乎线性的精度分布。
