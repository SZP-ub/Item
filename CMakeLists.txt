cmake_minimum_required(VERSION 3.10) # 指定 CMake 的最低版本要求
project(phoneBook C) # 定义项目名称和语言为 C

set(CMAKE_C_STANDARD 17) # 设置 C 语言标准为 C17
set(CMAKE_C_STANDARD_REQUIRED ON) # 强制要求使用指定的 C 标准
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 生成 compile_commands.json，方便 clangd 等工具使用

# ================== 构建类型 ==================
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug) # 默认构建类型为 Debug
endif()

# ================== 主程序源码 ==================
file(GLOB SRC_FILES src/*.c) # 查找 src 目录下所有 .c 文件
include_directories(include) # 添加 include 目录为头文件搜索路径

# add_executable(${PROJECT_NAME} ${SRC_FILES}) # 生成主程序可执行文件

# 从 SRC_FILES 中排除 main.c,TDD阶段专属
list(REMOVE_ITEM SRC_FILES "${CMAKE_SOURCE_DIR}/src/main.c")

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra) # 添加编译警告选项

# ================== 单元测试部分 ==================
# 假设 Unity 和 CMock 源码在 /home/i/Unity/src 和 /home/i/CMock/src
include_directories(/home/i/Unity/src /home/i/CMock/src) # 添加 Unity 和 CMock
                                                         # 头文件路径
set(UNITY_SRC /home/i/Unity/src/unity.c) # Unity 源文件路径
set(CMOCK_SRC /home/i/CMock/src/cmock.c) # CMock 源文件路径

file(GLOB TEST_SRC test/test_*.c) # 查找 test 目录下所有以 test_ 开头的 .c 测试文件

add_executable(run_tests ${TEST_SRC} ${UNITY_SRC} ${CMOCK_SRC} ${SRC_FILES}
)# 生成测试可执行文件，包含测试代码、Unity、CMock 和主程序源码
target_link_libraries(run_tests m) # 链接数学库（如测试中用到数学函数）
target_compile_options(run_tests PRIVATE -Wall -Wextra) # 添加编译警告选项

# ================== 测试自动化 ==================
enable_testing() # 启用 CMake 的测试功能
add_test(NAME all_tests COMMAND run_tests) # 添加测试命令，运行 run_tests 可执行文件

add_custom_command(
  TARGET run_tests
  POST_BUILD
  COMMAND run_tests
  COMMENT "Running unit tests...") # 每次构建 run_tests 后自动运行测试
