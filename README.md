# ⛅ WeatherForecast 天气预报桌面客户端

> 基于 Qt 6 / C++17 的 Windows 桌面天气应用，支持 IP 自动定位、城市搜索、6 天预报、温度曲线、本地缓存与系统托盘。

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Qt](https://img.shields.io/badge/Qt-6.5.3-green)](https://www.qt.io/)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue)](https://en.cppreference.com/w/cpp/17)
[![Stars](https://img.shields.io/github/stars/yiyinideshang/Qt_WeatherForecast?style=social)](https://github.com/yiyinideshang/Qt_WeatherForecast)

---

# 📖 项目概述

WeatherForecast 是一个基于 Qt 框架的 Windows 桌面天气客户端，调用 [t.weather.itboy.net](http://t.weather.itboy.net) 免费天气 API 获取实时气象数据。

项目从单文件 Widget 逐步重构为 ApiClient / DataCache / ChartWidget / Widget 四层架构，涵盖网络通信、JSON 解析、SQLite 缓存、自定义 QPainter 绘制、系统托盘等典型桌面应用技术栈。

# ✨ 功能特性

| 功能模块 | 说明 |
|---------|------|
| IP 自动定位 | 首次启动通过 ipinfo.io 自动定位当前城市，无需手动配置 |
| 城市搜索 | 内置 2800+ 城市数据库，支持中文名搜索及"市"/"县"后缀自动纠错 |
| 最近访问 | 点击搜索框弹出最近使用的城市下拉列表，选中即自动跳转 |
| 六天预报 | 昨天 / 今天 / 明天 + 后三天，含天气图标、温度范围、AQI、风力风向 |
| 温度曲线 | QPainter 自定义绘制高低温折线图，虚线区分历史、实线表示预报 |
| 本地缓存 | SQLite 存储，LRU 策略保留最近 5 个城市，2 小时过期，断网可加载快照 |
| 窗口持久化 | QSettings 保存上次城市和窗口位置，下次启动自动恢复 |
| 系统托盘 | 托盘图标常驻，支持最小化到托盘 / 单击双击恢复窗口 |
| 网络容错 | 请求失败立即弹窗提示，后台每 30 秒静默重试，最多 10 次（5 分钟） |

# 🧱 架构设计

```
┌─────────────────────────────────────────────┐
│  Widget（视图 + 控制器）                      │
│  - UI 布局与用户交互                           │
│  - 调度 ApiClient / DataCache / ChartWidget  │
├─────────────────────────────────────────────┤
│  ApiClient        │  DataCache              │
│  (网络请求层)      │  (SQLite 缓存层)          │
│  - HTTP GET       │  - 读写天气预报数据       │
│  - JSON 解析      │  - LRU 淘汰 / 过期失效    │
│  - 30s×10 重试    │  - 断网加载本地快照       │
├───────────────────┴─────────────────────────┤
│  ChartWidget（温度曲线绘制）                   │
│  - QPainter 自定义绘制                        │
│  - 高低温双曲线                               │
└─────────────────────────────────────────────┘
```

# 🗂️ 项目结构

```
WeatherForecast/
├── main.cpp                  # 程序入口
├── widget.h / .cpp / .ui     # 主窗口（视图 + 控制器）
├── apiclient.h / .cpp        # 网络请求与 JSON 解析
├── datacache.h / .cpp        # SQLite 本地缓存层
├── chartwidget.h / .cpp      # 温度曲线绘制控件
├── weatherdata.h             # 数据模型（Today / Day）
├── weatherTool.h             # 城市编码映射工具
├── WeatherForecast.pro       # qmake 构建配置
├── res.qrc                   # Qt 资源文件
├── .gitignore
├── LICENSE
├── Cmake_WeatherForecast/    # CMake 构建副本
├── res/                      # 图标资源
├── city_weather/             # 2800+ 城市数据库
└── docs/                     # 文档与截图
```

# 🖼️ 截图

| 主界面与温度曲线 | 六天预报与详情 |
|:---:|:---:|
| <img src="docs/展示1.png" width="300"> | <img src="docs/展示2.png" width="300"> |
| **城市搜索下拉列表** | **系统托盘** |
| <img src="docs/展示3.png" width="300"> | <img src="docs/展示4.png" width="300"> |

# 🛠️ 构建方式

## qmake

```bash
cd build
qmake ..\WeatherForecast.pro
mingw32-make -j4 release
```

> 若 SQLite 驱动未加载，将 `{Qt目录}/plugins/sqldrivers/qsqlite.dll` 复制到 exe 同级的 `sqldrivers/` 目录。

## CMake

```bash
cd Cmake_WeatherForecast/WeatherForecast/build
cmake .. -G "MinGW Makefiles"
cmake --build . --config Release
```

# 📬 联系方式

- **GitHub**: [yiyinideshang](https://github.com/yiyinideshang)
- **项目地址**: [github.com/yiyinideshang/Qt_WeatherForecast](https://github.com/yiyinideshang/Qt_WeatherForecast)
- **Email**: 2779271357@qq.com

---

Built with ❤️ and Qt
