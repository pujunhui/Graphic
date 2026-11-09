# 三维空间变换与 MVP 矩阵数学推导

三维渲染的核心任务，就是将定义在模型局部空间中的几何网格，通过一系列刚体与投影变换，投影并映射到二维屏幕像素网格上。这套变换链条通称为 **MVP 变换**。

```
[ 模型空间 Local/Model ]
         ↓ 模型矩阵 M (缩放 S → 旋转 R → 平移 T)
[ 世界空间 World ]
         ↓ 视图矩阵 V (对齐相机原点与正交基)
[ 观察空间 View/Eye ]
         ↓ 投影矩阵 P (透视视锥体/正交盒压缩)
[ 齐次裁剪空间 Clip ]
         ↓ 透视除法 (/w)
[ 归一化设备坐标 NDC ] ([-1, 1]³ 标准立方体)
         ↓ 视口屏幕变换 M_screen
[ 屏幕空间 Screen ] (像素坐标 [0, W] × [0, H])
```

---

## 1. 模型矩阵（Model Matrix）：仿射变换组合

模型矩阵负责将物体放置在世界中的特定位置、朝向与大小：
$$M = T \cdot R \cdot S$$

### 1.1 缩放矩阵（Scale）
沿着自身局部坐标系 $X, Y, Z$ 轴分别缩放 $s_x, s_y, s_z$ 倍：
$$S(s_x, s_y, s_z) = \begin{bmatrix} s_x & 0 & 0 & 0 \\ 0 & s_y & 0 & 0 \\ 0 & 0 & s_z & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}$$

### 1.2 平移矩阵（Translation）
三维平移无法用 $3\times 3$ 线性矩阵表达，必须引入齐次坐标（第四维 $w=1$）：
$$T(t_x, t_y, t_z) = \begin{bmatrix} 1 & 0 & 0 & t_x \\ 0 & 1 & 0 & t_y \\ 0 & 0 & 1 & t_z \\ 0 & 0 & 0 & 1 \end{bmatrix}$$

### 1.3 罗德里格斯任意轴旋转矩阵（Rodrigues' Rotation）
设单位旋转轴为 $\mathbf{u} = (u_x, u_y, u_z)^T$（$|\mathbf{u}| = 1$），绕该轴旋转角度 $\theta$（弧度）。根据罗德里格斯公式，任意向量 $\mathbf{v}$ 旋转后的结果为：
$$\mathbf{v}_{\text{rot}} = \mathbf{v} \cos\theta + (\mathbf{u} \times \mathbf{v}) \sin\theta + \mathbf{u} (\mathbf{u} \cdot \mathbf{v}) (1 - \cos\theta)$$
将其整理为 $4\times 4$ 齐次矩阵（令 $c = \cos\theta, s = \sin\theta$）：
$$R(\mathbf{u}, \theta) = \begin{bmatrix} 
c + (1-c)u_x^2 & (1-c)u_x u_y - s u_z & (1-c)u_x u_z + s u_y & 0 \\
(1-c)u_x u_y + s u_z & c + (1-c)u_y^2 & (1-c)u_y u_z - s u_x & 0 \\
(1-c)u_x u_z - s u_y & (1-c)u_y u_z + s u_x & c + (1-c)u_z^2 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}$$
在代码 `math::rotate` 中，计算 `src * Rotate`，确保在物体当前的局部坐标轴系下完成旋转。

---

## 2. 视图矩阵（View Matrix）：相机视角对齐

在世界空间中，相机位于 $\mathbf{eye}$，看向目标点 $\mathbf{center}$，上方参考向量为 $\mathbf{top}$。
将相机移动至世界原点，并让相机视线对齐到负 $Z$ 轴的过程，相当于对世界中所有物体施加相机变换的**逆变换**：
$$V = R_{\text{view}} \cdot T(-\mathbf{eye})$$

构造相机的正交局部基（右手坐标系）：
1. 观察方向单位向量：$\mathbf{f} = \text{normalize}(\mathbf{center} - \mathbf{eye})$（相机注视方向）
2. 相机右向量：$\mathbf{r} = \text{normalize}(\mathbf{f} \times \mathbf{top})$
3. 相机实际真实上向量：$\mathbf{u} = \text{normalize}(\mathbf{r} \times \mathbf{f})$

由于旋转矩阵是正交矩阵，其逆矩阵等于其转置矩阵：
$$R_{\text{view}} = \begin{bmatrix} \mathbf{r} & \mathbf{u} & -\mathbf{f} \end{bmatrix}^{-1} = \begin{bmatrix} r_x & r_y & r_z & 0 \\ u_x & u_y & u_z & 0 \\ -f_x & -f_y & -f_z & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}$$
与平移矩阵 $T(-\mathbf{eye})$ 复合相乘后，得到著名的 **LookAt 视图矩阵**：
$$V = \begin{bmatrix} 
r_x & r_y & r_z & -\mathbf{r} \cdot \mathbf{eye} \\
u_x & u_y & u_z & -\mathbf{u} \cdot \mathbf{eye} \\
-f_x & -f_y & -f_z & \mathbf{f} \cdot \mathbf{eye} \\
0 & 0 & 0 & 1
\end{bmatrix}$$

---

## 3. 透视投影矩阵（Perspective Projection）

透视投影旨在模拟人眼的“近大远小”规律，将视锥体（Frustum）平截头体压缩挤压为一个标准的长方体。

### 3.1 几何相似三角形与近大远小原理
在观察空间（右手系，相机看向 $-Z$）中，设近平面距离相机原点为 $n$（即近平面方程为 $z = -n$），远平面距离为 $f$（$z = -f$）。
对于视锥体内任意一点 $P(x, y, z)$，从相机原点连线投影到近平面 $z = -n$ 上的交点 $P'(x', y', -n)$，由相似三角形几何关系：
$$\frac{x'}{x} = \frac{-n}{z} \implies x' = \frac{n x}{-z}, \qquad \frac{y'}{y} = \frac{-n}{z} \implies y' = \frac{n y}{-z}$$
分母中的 $-z > 0$ 正好是点距离相机的真实深度。物体距离越远（$-z$ 越大），投影后的坐标 $x', y'$ 越小，天然呈现出近大远小。

### 3.2 齐次坐标与透视除法解耦
为了让投影变换能统一用 $4\times 4$ 矩阵乘法表示，将除法推迟到**透视除法（Perspective Division）**阶段，让矩阵将深度值 $-z$ 保存在齐次分量 $w$ 中：
$$P_{\text{clip}} = \begin{pmatrix} x_{\text{clip}} \\ y_{\text{clip}} \\ z_{\text{clip}} \\ w_{\text{clip}} \end{pmatrix} = \begin{pmatrix} \dots \\ \dots \\ \dots \\ -z \end{pmatrix}$$
随后执行透视除法：
$$x_{\text{ndc}} = \frac{x_{\text{clip}}}{-z}, \quad y_{\text{ndc}} = \frac{y_{\text{clip}}}{-z}, \quad z_{\text{ndc}} = \frac{z_{\text{clip}}}{-z}$$

### 3.3 OpenGL 风格透视投影矩阵标准推导
给定垂直视场角 $\text{fovy}$，宽高比 $\text{aspect} = \frac{W}{H}$，近远平面 $n, f$：
- 近平面半高：$t = n \cdot \tan\left(\frac{\text{fovy}}{2}\right)$
- 近平面半宽：$r = t \cdot \text{aspect}$
- 映射至 NDC 区间 $[-1, 1]$：
  $$x_{\text{ndc}} = \frac{x'}{r} = \frac{n x}{-z \cdot r} = \frac{x}{-z} \cdot \frac{1}{\text{aspect} \cdot \tan(\text{fovy}/2)}$$
  $$y_{\text{ndc}} = \frac{y'}{t} = \frac{n y}{-z \cdot t} = \frac{y}{-z} \cdot \frac{1}{\tan(\text{fovy}/2)}$$

对于深度分量 $z$，设 $z_{\text{clip}} = A z + B$，要求在近平面 $z = -n$ 时映射为 $z_{\text{ndc}} = -1$，远平面 $z = -f$ 时映射为 $z_{\text{ndc}} = 1$：
$$\begin{cases} \dfrac{A(-n) + B}{n} = -1 \implies -A n + B = -n \\[8pt] \dfrac{A(-f) + B}{f} = 1 \implies -A f + B = f \end{cases}$$
联立解得：
$$A = -\frac{f + n}{f - n}, \qquad B = -\frac{2 f n}{f - n}$$

由此得到完整的 **OpenGL 透视投影矩阵**（`math::perspective`）：
$$P = \begin{bmatrix} 
\dfrac{1}{\text{aspect} \cdot \tan(\text{fovy}/2)} & 0 & 0 & 0 \\[8pt]
0 & \dfrac{1}{\tan(\text{fovy}/2)} & 0 & 0 \\[8pt]
0 & 0 & -\dfrac{f + n}{f - n} & -\dfrac{2 f n}{f - n} \\[8pt]
0 & 0 & -1 & 0
\end{bmatrix}$$

---

## 4. 视口屏幕变换矩阵（Screen Mapping）

透视除法后的坐标处于 **归一化设备坐标空间（NDC）**，范围为 $[-1, 1]^3$。
视口矩阵 `math::screenMatrix` 将该标准化立方体线性拉伸并平移至实际物理屏幕窗口与深度缓冲范围：
- $X \in [-1, 1] \longrightarrow [0, W]$
- $Y \in [-1, 1] \longrightarrow [0, H]$
- $Z \in [-1, 1] \longrightarrow [0, 1]$（用于深度测试 Z-Buffer）

$$M_{\text{screen}} = \begin{bmatrix} 
\dfrac{W}{2} & 0 & 0 & \dfrac{W}{2} \\[6pt]
0 & \dfrac{H}{2} & 0 & \dfrac{H}{2} \\[6pt]
0 & 0 & \dfrac{1}{2} & \dfrac{1}{2} \\[6pt]
0 & 0 & 0 & 1
\end{bmatrix}$$

顶点经过 $M_{\text{screen}}$ 变换后，其 $(x, y)$ 分量即为最终在光栅化阶段寻址帧缓冲像素阵列的离散坐标。
