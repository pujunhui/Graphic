# 三维空间变换与 MVP 矩阵数学推导

在 3D 渲染管线中，任何一个三维模型的顶点想要显示在电脑 2D 屏幕上，都必须依次乘上三个大名鼎鼎的 $4\times 4$ 矩阵：**Model（模型矩阵） $\rightarrow$ View（视图矩阵） $\rightarrow$ Projection（投影矩阵）**，简称 **MVP 变换**。

很多初学者一上来就被满屏的 $\cos\theta$ 、叉积转置、 $-\frac{f+n}{f-n}$ 搞得头晕脑胀。其实，MVP 变换完全等价于现实生活中**“在摄影棚里拍电影的三个步骤”**。先看懂下面这个 30 秒片场直觉，所有矩阵的作用就能一目了然。

---

## 0. 30 秒核心直觉：片场拍电影三部曲

想象你是一名电影导演，要把一把在木工车间做好的椅子拍进电影画面里：

### 第一步：Model 模型变换 = “摆道具”（局部空间 $\rightarrow$ 世界空间）
- 椅子刚在木工车间（建模软件 Blender/Maya）做好时，是以椅子腿中心为原点 $(0,0,0)$ 的（这叫**局部/模型空间**）。
- 现在你要把椅子搬进巨大的摄影棚（**世界空间**）：先把它放大 1.5 倍（**Scale 缩放**），再转个 $45^\circ$ 朝向窗户（**Rotate 旋转**），最后搬到摄影棚东北角 $(10, 0, 5)$ 的位置（**Translate 平移**）。
- 这一套“缩放 $\rightarrow$ 旋转 $\rightarrow$ 平移”打包成的矩阵，就是 **Model 矩阵** $M$ 。

### 第二步：View 视图变换 = “架摄像机”（世界空间 $\rightarrow$ 观察空间）
- 道具摆好了，现在你在摄影棚里架起一台摄像机。为了方便后面拍照，我们要把坐标系换成**“以摄像机镜头为原点 $(0,0,0)$ 、镜头永远正对着 $-Z$ 轴前方”**的坐标系（这叫**观察空间 View Space**）。
- **⚡ 核心顿悟点（为什么 View 矩阵是相机动作的逆矩阵？）**：
  - 在画面里，**“摄像机往左走 5 米”**，拍出来的效果和**“摄像机原地不动，全世界所有桌椅道具一起往右退 5 米”**是 100% 一模一样的！
  - **“摄像机往左歪头 $30^\circ$ ”**，完全等价于**“摄像机不动，全世界所有道具一起往右歪 $30^\circ$ ”**！
  - 所以：**视图矩阵 $V$ 根本不是去动摄像机，而是把摄像机的平移和旋转完全反过来（求逆矩阵），施加给全世界的所有顶点！**

### 第三步：Projection 投影变换 = “装镜头捏橡皮泥”（观察空间 $\rightarrow$ 裁剪空间）
- 人的眼睛和摄像机看出去的视野，是一个**近处截面小、远处截面大的金字塔台（平截头体 / 视锥体 Frustum）**。
- **投影矩阵 $P$ 干了一件极其暴力的事**：它像捏橡皮泥一样，把这个“近小远大的金字塔台”，强行挤压成一个 $[-1, 1]^3$ 的**标准正方体盒子（NDC）**！
- **为什么挤压就能产生“近大远小”？**
  - 因为近处的框本来就小，不用怎么挤；而**远处的大框被强行挤缩到了和近处一样大**！站在大框里的远处物体跟着框一起被猛烈缩小，所以在画面上就变小了！

```mermaid
flowchart LR
    A["局部/模型空间<br/>(木工车间做椅子)"] -->|"1. Model 矩阵 M<br/>(摆道具: 缩放→旋转→平移)"| B["世界空间<br/>(摄影棚全局坐标)"]
    B -->|"2. View 矩阵 V<br/>(架相机: 全世界反着平移旋转)"| C["观察空间<br/>(以镜头为原点)"]
    C -->|"3. Projection 矩阵 P<br/>(捏金字塔视锥体)"| D["4D 齐次裁剪空间<br/>(w = -z)"]
    D -->|"透视除法 (/w)<br/>(近大远小生效)"| E["NDC 空间<br/>([-1, 1]³ 标准正方体)"]
    E -->|"视口矩阵 M_screen<br/>(拉伸至像素宽高)"| F["2D 屏幕空间<br/>([0, W] × [0, H])"]
    style A fill:#f1f5f9,stroke:#64748b,color:#0f172a
    style B fill:#dbeafe,stroke:#0284c7,color:#0f172a
    style C fill:#e0e7ff,stroke:#4f46e5,color:#0f172a
    style D fill:#fef3c7,stroke:#d97706,color:#0f172a
    style E fill:#dcfce7,stroke:#16a34a,color:#0f172a
    style F fill:#fee2e2,stroke:#dc2626,color:#0f172a
```

---

## 1. 第一步：模型矩阵（Model Matrix）—— 摆道具

把物体放进世界，遵循 **“先缩放 $S$ $\rightarrow$ 再自转 $R$ $\rightarrow$ 最后平移到位 $T$ ”** 的顺序（因为矩阵左乘向量，所以从右往左写）：

$$
M = T \cdot R \cdot S
$$

### 1.1 缩放矩阵（Scale）与平移矩阵（Translation）
这两个矩阵最直观——对角线上放缩放倍数，最后一列放平移距离（原理详见 [00_仿射变换起源与二维齐次坐标推导.md](file:///d:/Graphic/Graphic/docs/数学/00_仿射变换起源与二维齐次坐标推导.md)）：

$$
S(s_x, s_y, s_z) = \begin{bmatrix}
s_x & 0 & 0 & 0 \\
0 & s_y & 0 & 0 \\
0 & 0 & s_z & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}, \qquad T(t_x, t_y, t_z) = \begin{bmatrix}
1 & 0 & 0 & t_x \\
0 & 1 & 0 & t_y \\
0 & 0 & 1 & t_z \\
0 & 0 & 0 & 1
\end{bmatrix}
$$

### 1.2 基础单轴三维旋转矩阵（绕 X、Y、Z 轴）
**超简记忆技巧**：绕哪个轴转，哪个轴对应的那一行和那一列就填 $1$ 和 $0$ （保持不动），剩下的 $2\times 2$ 格子直接套用二维旋转矩阵 $[[\cos, -\sin], [\sin, \cos]]$ （唯独绕 $Y$ 轴因为右手系次序，负号换到左下角）：

1. **绕 $X$ 轴转 $\alpha$ （点头 Pitch，第 1 行第 1 列为 1）**：

$$
R_x(\alpha) = \begin{bmatrix}
\mathbf{1} & 0 & 0 & 0 \\
0 & \cos\alpha & -\sin\alpha & 0 \\
0 & \sin\alpha & \cos\alpha & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
$$

2. **绕 $Y$ 轴转 $\beta$ （摇头 Yaw，第 2 行第 2 列为 1）**：

$$
R_y(\beta) = \begin{bmatrix}
\cos\beta & 0 & \sin\beta & 0 \\
0 & \mathbf{1} & 0 & 0 \\
-\sin\beta & 0 & \cos\beta & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
$$

3. **绕 $Z$ 轴转 $\gamma$ （歪头 Roll，第 3 行第 3 列为 1）**：

$$
R_z(\gamma) = \begin{bmatrix}
\cos\gamma & -\sin\gamma & 0 & 0 \\
\sin\gamma & \cos\gamma & 0 & 0 \\
0 & 0 & \mathbf{1} & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
$$

> **任意轴旋转（罗德里格斯公式 Rodrigues' Rotation）**：如果旋转轴不是 X/Y/Z，而是空间任意一根单位向量 $\mathbf{u}(u_x, u_y, u_z)$ ，代码 `math::rotate` 直接套用罗德里格斯通用公式 $\mathbf{v}_{\text{rot}} = \mathbf{v} \cos\theta + (\mathbf{u} \times \mathbf{v}) \sin\theta + \mathbf{u} (\mathbf{u} \cdot \mathbf{v}) (1 - \cos\theta)$ 展开成的 $4\times 4$ 矩阵（展开式附于本文末尾附录）。

---

## 2. 第二步：视图矩阵（View Matrix / LookAt）—— 架摄像机

假设摄像机架在世界坐标 $\mathbf{eye}$ ，镜头看着目标点 $\mathbf{center}$ ，头顶朝向参考方向 $\mathbf{top}(0,1,0)$ 。

根据第 0 节的直觉：**要把摄像机归零到原点并对齐坐标轴，只需让全世界所有物体反着平移 $-\mathbf{eye}$ ，再反着旋转**：

$$
V = R_{\text{view}} \cdot T(-\mathbf{eye})
$$

### 2.1 三次叉乘搭出摄像机自己的“右下前”三根轴
就像伸出你的右手比划一样，利用向量叉乘（详见 [01_向量与列主序矩阵基础.md](file:///d:/Graphic/Graphic/docs/数学/01_向量与列主序矩阵基础.md)）现场算出摄像机自己的三个正交坐标轴：
1. **视线前向轴 $\mathbf{f}$ （Forward）**：从眼睛指向目标， $\mathbf{f} = \text{normalize}(\mathbf{center} - \mathbf{eye})$ ；
2. **相机右向轴 $\mathbf{r}$ （Right）**：前向叉乘头顶上方， $\mathbf{r} = \text{normalize}(\mathbf{f} \times \mathbf{top})$ ；
3. **相机正上轴 $\mathbf{u}$ （Up）**：右向叉乘前向（修正头顶使其严格垂直于视线）， $\mathbf{u} = \text{normalize}(\mathbf{r} \times \mathbf{f})$ 。

### 2.2 正交矩阵求逆的作弊捷径：逆矩阵 = 转置矩阵！
把相机的三根轴 $\mathbf{r}, \mathbf{u}, -\mathbf{f}$ 竖着摆成三列，是从相机转到世界；现在我们要**反过来（求逆）**从世界转到相机。
因为 $\mathbf{r}, \mathbf{u}, -\mathbf{f}$ 两两垂直且长度为 1（**正交矩阵**），数学上有一个天大的福利：**正交矩阵的逆矩阵，直接等于把它的列横过来写成行（转置 $R^{-1} = R^T$ ）！**

所以只需把 $\mathbf{r}, \mathbf{u}, -\mathbf{f}$ **横着填进矩阵的前三行**，再乘上反向平移 $T(-\mathbf{eye})$ ，就得到了大名鼎鼎的 **LookAt 矩阵**：

$$
V = \begin{bmatrix}
r_x & r_y & r_z & 0 \\
u_x & u_y & u_z & 0 \\
-f_x & -f_y & -f_z & 0 \\
0 & 0 & 0 & 1
\end{bmatrix} \begin{bmatrix}
1 & 0 & 0 & -\text{eye}_x \\
0 & 1 & 0 & -\text{eye}_y \\
0 & 0 & 1 & -\text{eye}_z \\
0 & 0 & 0 & 1
\end{bmatrix} = \begin{bmatrix}
r_x & r_y & r_z & -\mathbf{r} \cdot \mathbf{eye} \\
u_x & u_y & u_z & -\mathbf{u} \cdot \mathbf{eye} \\
-f_x & -f_y & -f_z & \mathbf{f} \cdot \mathbf{eye} \\
0 & 0 & 0 & 1
\end{bmatrix}
$$

---

## 3. 第三步：透视投影矩阵（Perspective Matrix）—— 捏视锥体

**千万别死背这个大矩阵！** 你只需要看懂下面矩阵公式里用括号标出的 **4 个关键机关**各自在干什么（代码 `math::perspective`）：

$$
P = \begin{bmatrix}
\underbrace{\dfrac{1}{\text{aspect} \cdot \tan(\text{fovy}/2)}}_{\text{① 水平视野缩放}} & 0 & 0 & 0 \\
[14pt]
0 & \underbrace{\dfrac{1}{\tan(\text{fovy}/2)}}_{\text{② 垂直视野缩放}} & 0 & 0 \\
[14pt]
0 & 0 & \underbrace{-\dfrac{f + n}{f - n}}_{\text{③ 深度系数 } A} & \underbrace{-\dfrac{2 f n}{f - n}}_{\text{③ 深度系数 } B} \\
[14pt]
0 & 0 & \underbrace{\mathbf{-1}}_{\text{④ 备份深度 } w = -z} & 0
\end{bmatrix}
$$

### 机关 4（最核心的一格）：第 4 行第 3 列的 `-1`
- 当这个矩阵乘以顶点 $(x, y, z, 1)^T$ 时，第 4 行算出来的齐次分量是：

$$
w_{\text{clip}} = 0 \cdot x + 0 \cdot y + (-1) \cdot z + 0 \cdot 1 = -z
$$

- 因为在观察空间里，摄像机看向 $-Z$ 轴，所有在镜头前面的物体 $z$ 都是负数，所以 $-z$ **刚好就是物体离镜头的正数距离（深度）**！
- **这一格 `-1` 的唯一目的，就是把物体的真实距离 $-z$ 悄悄备份到 $w$ 分量里**，好让下一步“透视除法（全员除以 $w$ ）”实现近大远小（ $x / (-z), y / (-z)$ ）！

### 机关 1 与 机关 2：控制视野张角（FOV）与长宽比（Aspect）
- $\text{fovy}$ 是摄像机上下张开的视野角度（类似眼睛睁得多大）：
  - $\text{fovy}$ 越大（广角镜头） $\rightarrow \tan(\text{fovy}/2)$ 越大 $\rightarrow$ 倒数越小 $\rightarrow$ 拍进来的东西越多，单个物体显得越小。
- 第一格多除以一个窗口宽高比 $\text{aspect} = \frac{W}{H}$ ，是为了防止在宽屏显示器上画面被横向拉扁。

### 机关 3：把深度从 $[-n, -f]$ 塞进 $[-1, 1]$
- 第三行的两个系数 $A = -\frac{f + n}{f - n}$ 和 $B = -\frac{2 f n}{f - n}$ ，只负责一件事：当后面除以 $w = -z$ 之后，刚好能把近平面（ $z = -n$ ）映射成 $-1$ ，把远平面（ $z = -f$ ）映射成 $+1$ （具体两行联立方程求解见文末附录）。

---

## 4. 最后一步：视口变换（Screen Mapping）—— 贴上显示器

经过透视除法后，所有可见顶点的 $x, y, z$ 都落在了 $[-1, 1]$ 的标准小正方体（NDC）里。
最后乘以一个简单的**视口矩阵 `math::screenMatrix`**，把 $[-1, 1]$ 拉伸平移到显示器真实的像素宽高 $[0, W] \times [0, H]$ 和深度缓冲 $[0, 1]$ ：

- **怎么把 $[-1, 1]$ 变成 $[0, W]$ ？** 先加 $1$ 变成 $[0, 2]$ ，再除以 $2$ 变成 $[0, 1]$ ，最后乘以 $W$ 变成 $[0, W]$ ！写成公式就是 $\frac{W}{2} x + \frac{W}{2}$ ：

$$
M_{\text{screen}} = \begin{bmatrix}
\dfrac{W}{2} & 0 & 0 & \dfrac{W}{2} \\
[6pt]
0 & \dfrac{H}{2} & 0 & \dfrac{H}{2} \\
[6pt]
0 & 0 & \dfrac{1}{2} & \dfrac{1}{2} \\
[6pt]
0 & 0 & 0 & 1
\end{bmatrix}
$$

至此，一个 3D 顶点就彻底变成了屏幕上的像素坐标 $(x_{\text{screen}}, y_{\text{screen}})$ ！

---

## 5. 进阶附录：公式详细代数推导备查（初学可跳过）

### 附录 A：相似三角形与透视投影系数 $A, B$ 的详细求解
1. **相似三角形投影**：在观察空间中，点 $P(x, y, z)$ 连线到镜头原点，与近平面（距离为 $n$ ，坐标 $z = -n$ ）相交于 $P'(x', y', -n)$ 。由相似三角形对应边成比例：

$$
\frac{x'}{x} = \frac{n}{-z} \implies x' = \frac{n \cdot x}{-z}, \qquad y' = \frac{n \cdot y}{-z}
$$

2. **归一化到** $[-1, 1]$ ：近平面半高为 $t = n \tan(\text{fovy}/2)$ ，半宽为 $r = t \cdot \text{aspect}$ 。将 $x'$ 除以半宽 $r$ 、 $y'$ 除以半高 $t$ ，即消去分子上的 $n$ ，得到机关 1 和机关 2 的系数。
3. **求解深度系数** $A, B$ ：设 $z_{\text{clip}} = A z + B$ ，除以 $w = -z$ 后得到 $z_{\text{ndc}} = \frac{A z + B}{-z}$ 。
   代入近平面（ $z = -n$ 时 $z_{\text{ndc}} = -1$ ）与远平面（ $z = -f$ 时 $z_{\text{ndc}} = +1$ ）：

$$
\begin{cases}
\dfrac{A(-n) + B}{n} = -1 \implies -A n + B = -n \\[8pt]
\dfrac{A(-f) + B}{f} = 1 \implies -A f + B = f
\end{cases}
$$

   两式相减消去 $B$ ，即可解出 $A = -\dfrac{f + n}{f - n}$ ， $B = -\dfrac{2 f n}{f - n}$ 。

### 附录 B：罗德里格斯任意轴旋转 $4\times 4$ 展开矩阵
设单位旋转轴为 $\mathbf{u} = (u_x, u_y, u_z)^T$ ，旋转角为 $\theta$ （记 $c = \cos\theta, s = \sin\theta$ ）：

$$
R(\mathbf{u}, \theta) = \begin{bmatrix}
c + (1-c)u_x^2 & (1-c)u_x u_y - s u_z & (1-c)u_x u_z + s u_y & 0 \\
(1-c)u_x u_y + s u_z & c + (1-c)u_y^2 & (1-c)u_y u_z - s u_x & 0 \\
(1-c)u_x u_z - s u_y & (1-c)u_y u_z + s u_x & c + (1-c)u_z^2 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
$$
