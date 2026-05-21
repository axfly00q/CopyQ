# CopyQ 16.0.0 Glass Edition

Windows 11 毛玻璃深色主题定制版本，为现代 Windows 系统优化的高颜值剪贴板管理工具。

## ✨ 核心特性

### UI 设计
- 🎨 **Windows 11 毛玻璃效果** — 原生 DWM 模糊与云母材质效果，与系统深度融合
- 🌙 **深色主题配色** — 暗蓝调半透明背景，保护眼睛、降低功耗
- 🔍 **系统蓝搜索高亮** — 采用 Windows 11 标准蓝色 (#0A84FF)，快速定位内容

### 交互优化
- 🧹 **工具栏精简** — 隐藏不常用按钮，界面简洁清爽
- 📌 **托盘菜单智能隐藏** — 无内容时自动隐藏分隔线
- 🖱️ **鼠标滚轮修复** — 悬停于内容区时可直接滚动，操作更顺畅

## 📦 快速开始

### 便携式安装（推荐）
1. 下载 `CopyQ-16.0.0-glass-portable.zip`
2. 解压到任意目录
3. 运行 `copyq.exe`
4. ✅ 开箱即用，已包含预配置文件

### 系统安装
1. 从 [官方网站](https://hluk.github.io/CopyQ/) 下载标准安装程序
2. 将本项目的配置文件复制到：
   - **Windows**: `%APPDATA%\copyq\copyq.ini`
   - **Linux**: `~/.config/copyq/copyq.ini`

## 🎯 配置亮点

| 配置项 | 值 | 说明 |
|-------|-----|------|
| 背景色 | `rgba(18, 22, 34, 180)` | 深蓝半透明 |
| 交替背景 | `rgba(18, 22, 34, 120)` | 浅透明用于行区分 |
| 选中背景 | `rgba(160, 165, 180, 110)` | 淡蓝选中态 |
| 前景色 | `#C8D0E0` | 冷白文字 |
| 搜索高亮 | `#0A84FF` | Windows 11 标准蓝 |
| 工具栏 | `rgba(18, 22, 34, 200)` | 深色背景 |

## 📁 预置标签页
- **&clipboard** — 主剪贴板（系统同步）
- **剪贴板(&C)** — 中文标签演示

## 🔧 技术细节

### 源码改动
基于官方 CopyQ 16.0.0，在以下文件增加了定制化：

- `src/gui/mainwindow.cpp` — 毛玻璃 paintEvent、WM_MOUSEWHEEL 路由、WinRT API 集成
- `src/gui/mainwindow.h` — paintEvent 虚函数声明
- `src/gui/menuitems.cpp` — 中文菜单标签
- `src/gui/traymenu.cpp` — 托盘菜单分隔线逻辑

### 构建环境
- C++17 标准
- Qt 6.11.0
- MinGW 13.1.0 (Windows)
- CMake 3.28+

## 🛠️ 从源码构建

```bash
# 配置 CMake
cmake -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=./install \
  -DWITH_TESTS=ON .

# 编译
cmake --build build

# 安装到 build/install
cmake --install build

# 打包便携版本
cd build/install
7z a ../copyq-portable.zip .
```

## 📝 使用提示

### 导入现有配置
如果已有旧版 CopyQ 配置：
1. 备份原 `copyq.ini`
2. 将本版本的 `copyq.ini` 复制覆盖
3. 重启 CopyQ
4. 可选：手动恢复某些自定义设置

### 修改配置
编辑 `copyq.ini` 文件：
```ini
[Theme]
bg=rgba(18, 22, 34, 180)      # 主背景
fg=#C8D0E0                      # 文字颜色
find_bg=#0A84FF                 # 搜索高亮
```

### 插件管理
预置已禁用 ItemEncrypt、ItemSync 等（防止加载缓慢），可在 GUI 中重新启用。

## ⚙️ 系统需求

- **Windows**: Windows 10 Build 14393+ 或 Windows 11（推荐）
- **屏幕**: 1920x1080 或更高分辨率（毛玻璃效果最佳体验）
- **DWM**: 必须启用（Windows 11 默认启用）

## 📜 许可证

本项目基于官方 CopyQ 源码，遵循 [GPL-3.0 License](gpl.txt)。

UI 定制化部分在同一许可证下发布。

## 🔗 相关链接

- [CopyQ 官方仓库](https://github.com/hluk/CopyQ)
- [官方文档](https://hluk.github.io/CopyQ/)
- [本定制版源码](https://github.com/axfly00q/CopyQ)

## 💡 已知事项

- 毛玻璃效果仅在 Windows 10 1903+ 和 Windows 11 下工作
- 部分主题引擎（如 GTK）下 Linux 版本效果有限，建议使用标准主题
- 首次启动可能需要 2-3 秒加载配置，属正常行为

## 🤝 反馈

欢迎提交 Issue 或 Pull Request！

---

**当前版本**: CopyQ 16.0.0 Glass Edition  
**构建日期**: 2026-05-21  
**维护者**: @axfly00q
