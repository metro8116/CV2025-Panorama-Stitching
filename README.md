# OpenCV Panorama Stitching | 全景图拼接

## 项目简介
项目基于 **C++** 和 **OpenCV** 库，实现了多张图像的自动特征点检测、配准、透视变换与融合，最终生成一张宽视角的无黑边全景图。

## 核心算法 (Algorithms)

1.  **特征检测 (Feature Detection)**: 使用 **SIFT** 算法提取图像的关键点与描述子（具有尺度和旋转不变性）。
2.  **特征匹配 (Matching)**: 使用 **FLANN** 快速近似最近邻算法，并结合 Lowe's Ratio Test 剔除错误匹配。
3.  **变换估计 (Homography)**: 基于 **RANSAC** 算法计算单应性矩阵 (H)，鲁棒地处理离群点。
4.  **图像融合 (Blending)**: 
    - 使用 `warpPerspective` 进行图像扭曲。
    - 采用**像素最大值融合**策略消除拼接处的黑线。
5.  **自动裁剪 (Auto Cropping)**: 计算非零像素区域的最小外接矩形，自动切除无效的黑色背景。

## 文件结构 (Files)

```text
├── main.cpp                # 核心 C++ 源代码
├── images/                 # 输入的测试图片序列 (S1.jpg - S7.jpg)
├── panorama_cropped.jpg    # (生成的) 最终拼接结果
└── README.md
```

## 编译与运行 (Build & Run)

### 依赖环境
- OpenCV 4.x
- C++ 编译器 (GCC/Clang/MSVC)

### 运行步骤
1.  确保 `main.cpp` 中的图片读取路径正确（默认读取 S1.jpg 至 S7.jpg）。
2.  编译代码：
    ```bash
    g++ main.cpp -o panorama `pkg-config --cflags --libs opencv4`
    ```
3.  运行程序：
    ```bash
    ./panorama
    ```
4.  程序将显示拼接窗口，并保存结果至 `panorama_cropped.jpg`。
