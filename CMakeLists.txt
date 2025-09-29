cmake_minimum_required(VERSION 3.10)
project(phoneBook C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ================== 构建类型 ==================
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# ================== 选项：是否只进行TDD ==================
option(TDD "Enable TDD mode (only build tests, not main program)" OFF)

# ================== 主程序源码 ==================
file(GLOB SRC_FILES src/*.c)
include_directories(include)

# ================== 主程序目标 ==================
if(NOT TDD)
  add_executable(${PROJECT_NAME} ${SRC_FILES})
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# ================== 单元测试部分 ==================
include_directories(/home/i/Unity/src /home/i/CMock/src)
set(UNITY_SRC /home/i/Unity/src/unity.c)
set(CMOCK_SRC /home/i/CMock/src/cmock.c)
file(GLOB TEST_SRC test/test_*.c)

# 自动排除 main.c，避免测试目标重复定义 main
set(SRC_LIB ${SRC_FILES})
list(REMOVE_ITEM SRC_LIB "${CMAKE_SOURCE_DIR}/src/main.c")

add_executable(run_tests ${TEST_SRC} ${UNITY_SRC} ${CMOCK_SRC} ${SRC_LIB})
target_link_libraries(run_tests m)
target_compile_options(run_tests PRIVATE -Wall -Wextra)

# ================== 测试自动化 ==================
enable_testing()
add_test(NAME all_tests COMMAND run_tests)

add_custom_command(
  TARGET run_tests
  POST_BUILD
  COMMAND run_tests
  COMMENT "Running unit tests...")

# ================== 输出当前模式 ==================
if(TDD)
  message(STATUS "TDD模式：只编译测试目标，不编译主程序")
else()
  message(STATUS "常规模式：编译主程序和测试目标")
endif()
