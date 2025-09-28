# PhoneBook 项目

## 项目简介

PhoneBook 是一个用 C 语言实现的简易通讯录管理程序。该项目支持通讯录的基本操作，如添加、查询、修改、删除联系人信息。适合 C 初学者学习数据结构、文件操作和基本模块化编程。项目采用模块化设计，主逻辑在 `main.c`，核心功能在 `telephon.c` 和 `telephon.h`，便于后续嵌入式开发扩展。

## 功能特性

- 添加联系人信息（姓名、电话等）
- 查询联系人
- 修改联系人信息
- 删除联系人
- 联系人信息持久化存储（文件读写）
- 界面菜单式交互

## 目录结构

```
phoneBook/
├── docs/           # 项目相关文档
├── src/            # 源代码目录
│   ├── main.c      # 程序主入口
│   ├── telephon.c  # 通讯录核心功能模块
│   └── telephon.h  # 通讯录模块头文件
```

## 快速开始

### 依赖环境

- C 语言编译器（GCC 或 Clang，支持 C11 标准）
- 支持 Linux、Windows、MacOS 等平台

### 编译方式

#### 使用 gcc 手动编译：

```shell
cd phoneBook/src
gcc -o phonebook main.c telephon.c
```

#### 推荐使用 CMake（适合嵌入式/车企项目自动化管理）：

1. 在项目根目录新建 `CMakeLists.txt`（可参考下文模板）
2. 编译和运行：
    ```shell
    mkdir build
    cd build
    cmake ..
    make
    ./phonebook
    ```

### 运行程序

```shell
./phonebook
```

根据菜单提示操作即可。

## 单元测试

> 如有测试代码，可在 `test/` 或 `src/` 下添加测试文件，推荐用 CMocka/Unity 等框架。  
> 建议在 CMakeLists.txt 中集成自动测试（见下文示例）。

## 代码结构简述

- `main.c`：实现主菜单和用户交互逻辑
- `telephon.c`：实现联系人添加、查询、修改、删除等核心功能
- `telephon.h`：声明通讯录相关数据结构和接口

## 文档与设计

- 项目设计说明、API 文档请见 `docs/` 目录
- 详细注释见各源代码文件

## 贡献指南

欢迎反馈 bug 或提出优化建议！如需贡献代码，请遵循以下流程：

1. Fork 本仓库，创建新分支
2. 提交代码并附详细 commit message
3. 保证通过所有编译与测试
4. 提交 Pull Request 并说明变更内容

如有贡献规范、代码风格要求请参考 `CONTRIBUTING.md`

## 常见问题 FAQ

- 编译错误：请确认已安装 gcc/clang，且源文件在同一目录下
- 程序运行崩溃：请检查输入数据格式，或使用 valgrind 排查内存问题
- 功能扩展：如需适配嵌入式环境，请联系维护者或查阅 docs/ 相关说明

## 许可证

本项目采用 MIT License 开源，详情请见 LICENSE 文件。

## 联系方式

项目维护者：[@SZP-ub](https://github.com/SZP-ub)
如有疑问或合作意向请通过 Issue 或 Pull Request 联系。

---

## CMakeLists.txt 示例（推荐自动化管理）

```cmake
cmake_minimum_required(VERSION 3.14)
project(phonebook C)
set(CMAKE_C_STANDARD 11)

add_executable(phonebook src/main.c src/telephon.c)
# 可扩展测试目标和静态分析等
```
