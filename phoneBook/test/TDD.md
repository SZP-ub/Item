下面给出基于你当前项目（zhishidian / src/foo.c、src/foo.h、hal/storage.c 等）的、面向 Unity 测试框架的完整 TDD（Test-Driven Development）流程说明、每个迭代应该做的事情、推荐的测试用例清单、实际可复制的文件模板（CMake、Unity 集成、测试文件、mock HAL），以及如何在本地/CI 上运行。你可以把模板直接放到仓库中开始按 TDD 循环练习。

先给出整体流程要点，然后贴出可直接复制的文件模板和常用命令。

一、TDD 总体流程（红 / 绿 / 重构）—— 每次新增功能按此循环

1. 红（Red） — 写一个失败的单元测试（明确期望行为）
    - 新增一个测试函数（Unity 格式），只测一个行为/场景（Arrange-Act-Assert）。
    - 运行测试，确认测试失败（未实现或返回错误）——这保证测试确实能捕捉缺陷。

2. 绿（Green） — 实现最小代码使测试通过
    - 只实现通过该测试所需的最小逻辑，不追求完美。
    - 重新运行测试，直到通过。

3. 重构（Refactor） — 在测试保护下优化实现
    - 改善代码可读性、移除重复、改进接口、修复潜在问题，确保所有测试仍通过。

4. 提交与文档
    - 每完成一个“红-绿-重构”小循环，就提交（小、原子提交），并在 docs/ 或 ADR 记录重要设计决策与接口变动。

二、如何在 C 项目中实现 TDD（关键实践）

- 使用 Unity（ThrowTheSwitch/Unity）作为测试框架。使用 CMake 将测试集成到构建系统（add_test）。
- 隔离外部依赖（HAL、文件系统、时钟等）：用 Mock（在测试构建中替换 hal 实现）或依赖注入（把 ops 结构注入业务层）。
- 每个测试文件尽量独立、快速、确定性（no timing, no random, no network）。
- 在本地启用 Sanitizers（ASAN/UBSAN）与编译器警告，捕捉 UB 与内存问题。

三、TDD 实战迭代示例（按优先级的循环 / 小任务）
下面给出一组推荐的 TDD 循环（你可以按顺序实现）——每一项都是一个“红-绿-重构”循环。

迭代 A — 初始化与加载

- 测试 A1（红）: contact_init() 在无存储文件/空存储时成功返回（或能初始化内部数组）。
- 实现 A1（绿）: 实现 contact_init() 调用 storage_init() 并零初始化 contacts。
- 重构 A1: 抽出 load/persist 函数。

迭代 B — 添加联系人（正常路径）

- 测试 B1（红）: 添加新联系人返回 success，之后可通过 find 找回（基本用例）。
- 实现 B1（绿）: 实现 contact_add 最小路径，持久化写入 storage_write。
- 重构 B1: 检查错误码、去重逻辑。

迭代 C — 添加联系人（边界/错误）

- 测试 C1: 添加当容量满时返回错误码。
- 测试 C2: 当 storage_write 返回错误时，contact_add 返回持久化错误。
- 实现/重构：实现容量检查、处理 storage 错误、写相应错误码并记录日志。

迭代 D — 查询与多个结果

- 测试 D1: contact_find 对不存在的名字返回 not found。
- 测试 D2: 名称匹配边界（长度、大小写差异）——定义行为并测试。
- 实现/重构：改进匹配规则（精确/模糊）。

迭代 E — 删除/修改联系人

- 测试 E1: 删除后 find 返回 not found。
- 测试 E2: 修改后能正确返回新字段。
- 实现/重构相应 API。

迭代 F — 并发/重入（如果适用）

- 测试 F1: 多线程环境下（或模拟）对 contact_add 的互斥性（使用 ThreadSanitizer 做扩展测试）。
- 实现/重构：加锁/使用原子操作。

每个迭代都应包含：1个或多个测试、实现最小功能、重构、提交、在 docs/ 更新 API 描述与测试要点。

四、具体测试用例表（优先级从高到低）
基础（必须）

- init 成功（无存储或文件损坏）
- add 单个联系人成功并能 find
- add 超容量返回错误
- find 不存在返回错误
- add 时 storage_write 失败 -> 返回持久化错误

强化（高优先级）

- add 已有名字（定义行为：允许/拒绝）
- find 名字长度边界（最大长度）
- persistent 数据在重启后能 load（init 后 find）

高级（可选）

- 模拟多线程并发（TSAN）
- fuzz 输入（非法字符串、非常长名字等）
- 模拟存储部分写入或 IO 错误序列

五、如何组织测试（目录/命名）

- test/test_contact.c — 每个模块一个测试文件（contact 对应 foo）
- test/mock_storage.c — Mock HAL：可注入不同返回值以测试错误路径
- test/CMakeLists.txt 或项目根 CMakeLists 集成测试目标

六、Unity 集成 + CMake 示例（模板文件）
下面提供的是一套可直接放入仓库的文件：CMakeLists.txt、Unity 集成示例、测试文件、Mock HAL 文件。请把这些文件复制到项目并按命令运行。

```cmake name=CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(zhishidian C)

set(CMAKE_C_STANDARD 11)
option(ENABLE_SANITIZERS "Enable sanitizers" ON)

if(ENABLE_SANITIZERS)
  set(SANITIZE_FLAGS "-fsanitize=address,undefined -fno-omit-frame-pointer")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZE_FLAGS} -g -O1")
endif()

# ---- source libraries ----
add_library(libfoo STATIC src/foo.c)
target_include_directories(libfoo PUBLIC ${CMAKE_SOURCE_DIR}/src)

# HAL real implementation (for app)
add_library(halstorage STATIC hal/storage.c)
target_include_directories(halstorage PUBLIC ${CMAKE_SOURCE_DIR}/hal)

# app executable
add_executable(zhishidian src/main.c)
target_link_libraries(zhishidian PRIVATE libfoo halstorage)

# ---- Unity test framework (FetchContent) ----
include(FetchContent)
FetchContent_Declare(
  unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG v2.5.2
)
FetchContent_MakeAvailable(unity)

# ---- Tests ----
enable_testing()
# Build test executable using src/foo.c and a mock HAL override in test/mock_storage.c
add_executable(test_contact
  test/test_contact.c
  ${unity_SOURCE_DIR}/src/unity.c
  src/foo.c
  test/mock_storage.c
)
target_include_directories(test_contact PRIVATE
  ${unity_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/hal
  ${CMAKE_SOURCE_DIR}/test
)
# link order: no need to link halstorage (we override functions by mock)
add_test(NAME test_contact COMMAND test_contact)
```

```c name=test/test_contact.c
#include "unity.h"
#include "foo.h"
#include <string.h>

/* Unity requires setUp/tearDown even if empty */
void setUp(void) {}
void tearDown(void) {}

/* Red: write failing test first */
void test_contact_add_and_find_success(void) {
    contact_init();

    contact_t c;
    memset(&c, 0, sizeof(c));
    strncpy(c.name, "Alice", sizeof(c.name)-1);
    strncpy(c.phone, "10086", sizeof(c.phone)-1);

    int rc = contact_add(&c);
    TEST_ASSERT_EQUAL_INT(0, rc);

    contact_t out;
    rc = contact_find("Alice", &out);
    TEST_ASSERT_EQUAL_INT(0, rc);
    TEST_ASSERT_EQUAL_STRING("10086", out.phone);
}

/* Test storage write failure handling */
void test_contact_add_storage_fail_returns_error(void) {
    /* instruct mock storage to fail writes */
    mock_storage_set_write_fail(1);

    contact_init();

    contact_t c;
    memset(&c, 0, sizeof(c));
    strncpy(c.name, "Bob", sizeof(c.name)-1);
    strncpy(c.phone, "22222", sizeof(c.phone)-1);

    int rc = contact_add(&c);
    TEST_ASSERT_LESS_THAN_INT(0, rc); /* expect negative error */

    /* reset mock behavior */
    mock_storage_set_write_fail(0);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_contact_add_and_find_success);
    RUN_TEST(test_contact_add_storage_fail_returns_error);
    return UNITY_END();
}
```

```c name=test/mock_storage.c
#include "storage.h"
#include <string.h>
#include <stdio.h>

/* This mock implements the same functions as hal/storage.c but allows test control */
static int write_fail = 0;
void mock_storage_set_write_fail(int v) { write_fail = v; }

/* Provide the symbols with same names as real HAL so linking mock overrides real one */
int storage_init(void) {
    /* pretend storage file exists; ok */
    return STORAGE_OK;
}
int storage_read(uint32_t addr, void *buf, size_t len) {
    /* for tests, return zeroed region */
    memset(buf, 0, len);
    return STORAGE_OK;
}
int storage_write(uint32_t addr, const void *buf, size_t len) {
    if (write_fail) return STORAGE_ERR;
    /* do nothing (in-memory mock) */
    return STORAGE_OK;
}
```

说明：

- mock*storage.c 定义了 storage*\* 符号，因在 test 可执行文件链接时被包含，故会覆盖 hal/storage.c 的实现（链接时使用 mock）。
- 在更复杂的项目中，推荐使用依赖注入（在业务层传入 storage_ops），更灵活，也便于在运行时切换实现。

七、Writing tests — 实际 TDD 操作步骤（示例：实现 contact_add）

1. Red（写测试）
    - 在 test/test_contact.c 增加 test_contact_add_and_find_success。
    - 运行：
        ```sh
        mkdir -p build && cd build
        cmake -DENABLE_SANITIZERS=ON ..
        cmake --build .
        ctest --output-on-failure
        ```
    - 预期：测试失败（函数 contact_init / contact_add 未实现或行为不符合预期）。记录错误信息。

2. Green（实现最少代码）
    - 在 src/foo.c 添加最小实现：contact_init 初始化内部数组，contact_add 将第一个空槽填入数据并返回 0，contact_find 做线性搜索。
    - 重新运行 ctest，直到测试通过。

3. Refactor（优化）
    - 把持久化逻辑封装到 save_contacts()，把错误码统一为 enum，整理注释。
    - 重新运行测试确认仍通过。

4. 扩展测试（边界/错误）
    - 写 test_contact_add_storage_fail_returns_error（如上），运行测试，模拟写失败（mock_storage_set_write_fail(1)），确保 contact_add 返回明确的错误码。
    - 实现对应错误处理逻辑：当 storage_write 返回错误时，contact_add 撤销内存更改或返回错误码。

八、如何处理 HAL mock：两种常用做法

1. 链接替换（示例中使用）
    - 在测试可执行中直接包含 mock/mock_storage.c（或将其作为测试库），链接时 mock 覆盖真实实现。
2. 依赖注入（推荐在复杂项目）
    - 定义一个 ops 结构体（指向 storage_init/read/write 的函数指针），业务层内部使用该 ops，而在生产中注入 real_storage_ops，在测试中注入 mock_ops。

例如（简要）：

```c
typedef struct {
  int (*init)(void);
  int (*read)(uint32_t, void*, size_t);
  int (*write)(uint32_t, const void*, size_t);
} storage_ops_t;

void foo_set_storage_ops(const storage_ops_t *ops);
```

九、运行 Sanitizers 与调试常见崩溃

- 在 CMake 开启 ENABLE_SANITIZERS=ON（示例）。
- 如果测试运行时 ASAN 报错，优先修复内存越界/未初始化问题。
- 可以用 valgrind 进一步排查（在不启用 ASAN 时）。

十、CI 集成建议（GitHub Actions 简述）

- 在 .github/workflows/ci.yml 中做：
    - matrix: [gcc, clang] 构建
    - cmake -DENABLE_SANITIZERS=ON
    - build, ctest
    - run cppcheck/clang-tidy
    - upload coverage（lcov + codecov）
- 强制 PR 必须通过 CI。

十一、常见 TDD 误区与建议

- 不要把太多断言和场景塞入一个测试；一条测试只检验一个行为（便于定位错误）。
- 测试与实现分离：避免在测试中直接调用私有静态函数（尽量通过公开接口或 test seam）。
- 测试要稳定，不应依赖外部环境（时间、随机等）。
- 小步快走：每次只改一小部分、提交一小步，便于回滚与审查。

十二、练习计划（四周示例）

- 周 1：搭建 CMake + Unity 环境，完成第 1 个 TDD 循环（init、add、find 基本用例）。
- 周 2：完成持久化错误处理、边界情况（满表、超长字符串）、扩展测试用例。
- 周 3：实现删除/修改、多线程或接口兼容，加入静态分析修复警告。
- 周 4：编写 docs/TESTING.md、在 GitHub Actions 上部署 CI，整理 ADR。

---

`foo.h/foo.c`最小实现(green步骤)：

```c name=src/foo.h
#ifndef FOO_H
#define FOO_H

#include <stddef.h>

#define CONTACT_NAME_MAX 32
#define CONTACT_PHONE_MAX 16

typedef struct {
    char name[CONTACT_NAME_MAX];
    char phone[CONTACT_PHONE_MAX];
} contact_t;

/* Initialize contact module (load from storage if available).
 * Returns 0 on success, negative on error.
 */
int contact_init(void);

/* Add a contact.
 * Returns:
 *   0  - success
 *  <0  - error (see implementation for codes)
 */
int contact_add(const contact_t *c);

/* Find a contact by exact name match.
 * If found and out != NULL, copies the contact into *out.
 * Returns 0 on success, negative if not found or error.
 */
int contact_find(const char *name, contact_t *out);

#endif /* FOO_H */
```

```c name=src/foo.c
#include "foo.h"
#include "storage.h"
#include <string.h>

/*
 Minimal "green" implementation for TDD:
 - Maintains an in-memory fixed-size contact array.
 - On init, attempts to call storage_init() and storage_read() (ignore read error).
 - contact_add stores the first empty slot, persists the entire array via storage_write().
 - contact_find does a linear exact-name search.
 - Return codes:
     0   success
    -1   invalid argument or storage init failed
    -2   storage persist failed
    -3   contact list full
    -4   not found
*/

#define MAX_CONTACTS 16
#define STORAGE_BASE_ADDR 0  /* in demo storage uses file offset 0 */

static contact_t contacts[MAX_CONTACTS];

int contact_init(void) {
    /* clear in-memory table */
    for (int i = 0; i < MAX_CONTACTS; ++i) {
        contacts[i].name[0] = '\0';
        contacts[i].phone[0] = '\0';
    }

    if (storage_init() != STORAGE_OK) {
        return -1;
    }

    /* try to read persisted contacts; ignore partial/format errors for this minimal implementation */
    (void) storage_read(STORAGE_BASE_ADDR, contacts, sizeof(contacts));
    return 0;
}

int contact_add(const contact_t *c) {
    if (!c || c->name[0] == '\0') return -1;

    for (int i = 0; i < MAX_CONTACTS; ++i) {
        if (contacts[i].name[0] == '\0') {
            /* copy safely and ensure null-termination */
            strncpy(contacts[i].name, c->name, CONTACT_NAME_MAX - 1);
            contacts[i].name[CONTACT_NAME_MAX - 1] = '\0';
            strncpy(contacts[i].phone, c->phone, CONTACT_PHONE_MAX - 1);
            contacts[i].phone[CONTACT_PHONE_MAX - 1] = '\0';

            /* persist entire table */
            if (storage_write(STORAGE_BASE_ADDR, contacts, sizeof(contacts)) != STORAGE_OK) {
                /* rollback in-memory entry on persist failure */
                contacts[i].name[0] = '\0';
                contacts[i].phone[0] = '\0';
                return -2;
            }
            return 0;
        }
    }

    return -3; /* table full */
}

int contact_find(const char *name, contact_t *out) {
    if (!name) return -1;

    for (int i = 0; i < MAX_CONTACTS; ++i) {
        if (contacts[i].name[0] != '\0' &&
            strncmp(contacts[i].name, name, CONTACT_NAME_MAX) == 0) {
            if (out) *out = contacts[i];
            return 0;
        }
    }
    return -4; /* not found */
}
```

我已经为你生成了 src/foo.h 和 src/foo.c 的最小实现（green 步骤）。实现要点：

- 使用固定大小数组（MAX_CONTACTS）存储联系人，便于在单元测试中可预测行为。
- contact_init() 会调用 storage_init()，并尝试加载持久化数据（若失败不会阻止初始化，便于测试）。
- contact_add() 在找到第一个空槽后写入并调用 storage_write() 持久化；持久化失败时会回滚该槽并返回错误。
- contact_find() 做线性精确匹配并可把结果拷贝到调用者提供的 out 结构。

下一步（建议的短期动作）：

- 把这两个文件放到项目的 src/ 目录下（如果你已经有 src/foo.c 请替换或备份原文件）。
- 运行你已有的 Unity 测试（或之前示例的 test/test_contact.c + test/mock_storage.c）。例如用之前的 CMake 配置：
  mkdir build && cd build
  cmake -DENABLE_SANITIZERS=ON ..
  cmake --build .
  ctest --output-on-failure
- 观察测试结果：按 TDD 流程，如果测试失败，修正实现或测试；如果通过，进入重构阶段（例如把持久化地址和 MAX_CONTACTS 抽成配置、增加边界检查和日志）。

如果你希望，我可以接着：

- 生成或调整一个与此实现配套的 mock_storage.c（用于 Unity 测试），或者
- 将此实现改为依赖注入风格以便更易于模拟（更推荐用于较大项目），或者
- 直接将测试（test/test_contact.c）和 CMake 测试集成文件贴出供你复制并立即运行。

## 具体实现：

下面我把整个项目的“大纲 → TDD（红-绿-重构）迭代计划”写清楚，并给出第一轮（Red）所需的测试文件和 mock（Unity 框架），以及对应的 Green 最小实现要点。你可以把这些文件复制到仓库并按流程执行：写测试（Red）→ 运行（确认失败）→ 实现最少代码使测试通过（Green）→ 重构（Refactor）→ 提交并进入下一条迭代，直到完成所有大纲功能。

我先给出：

1. 项目大纲（模块/函数列表 + 签名 + 说明）
2. 优先级迭代（每次迭代的 Red / Green / Refactor 明细）
3. 第 1 个 Red 测试实现（Unity 测试文件 + mock_storage），以及如何运行测试与预期行为（你需要本地运行）
4. 下一步说明

一、项目大纲（分层 + 模块 + 函数列表）

- 目标：通讯录（PhoneBook）功能，分层为 app / business / hal

目录建议（示例）

```
zhishidian/
├── CMakeLists.txt
├── src/            # 应用层 + 业务逻辑头/实现
│   ├── main.c
│   ├── foo.h       # 业务层头（contact 接口）
│   └── foo.c       # 业务层实现（contact）
├── hal/            # 硬件抽象层：存储、日志、时间等
│   ├── storage.h
│   └── storage.c
├── test/           # 单元测试（Unity）
│   ├── test_contact_red1.c
│   └── mock_storage.c
├── docs/
└── README.md
```

核心模块与函数（建议签名与说明）：

A. 硬件抽象层 — hal/storage.h

```c
int storage_init(void);
int storage_read(uint32_t addr, void *buf, size_t len);
int storage_write(uint32_t addr, const void *buf, size_t len);
int storage_erase(void); // 可选：清空或重置存储
```

说明：返回 STORAGE_OK/ STORAGE_ERR（枚举或宏），在主机上可用文件模拟，在目标板上实现实际 flash/EEPROM。

B. 业务层 — src/foo.h（contact 接口）

```c
int contact_init(void);                    // 初始化模块，加载持久化数据
int contact_add(const contact_t *c);       // 添加联系人，0 成功，负数错误
int contact_find(const char *name, contact_t *out); // 精确查询
int contact_delete(const char *name);      // 删除联系人
int contact_update(const char *name, const contact_t *new_val); // 修改已有联系人
int contact_count(void);                   // 返回当前联系人数
int contact_list(contact_t *buf, size_t max, size_t *actual); // 列出联系人
```

数据类型：

```c
#define CONTACT_NAME_MAX 32
#define CONTACT_PHONE_MAX 16
typedef struct { char name[CONTACT_NAME_MAX]; char phone[CONTACT_PHONE_MAX]; } contact_t;
```

C. 应用层 — src/main.c

- 提供 CLI / 菜单调用 contact\_ 开头接口，不直接与 storage 打交道。

错误码建议（enum 或宏）

- 0 成功
- -1 参数错误
- -2 持久化失败
- -3 容量已满
- -4 未找到

二、TDD 迭代计划（优先级与每次的 Red/Green/Refactor）

总体原则：每个迭代聚焦一组独立行为（单一职责），写最小测试（单个断言组合），用 mock 隔离外部依赖（storage）。

迭代 0：准备工作

- 将 Unity 集成到 CMake（FetchContent 或 submodule）
- 添加 test 可执行和 mock_storage（用于替换 hal 实现）

迭代 1（基础载入/添加/查询） — 高优先级

- Red:
    - test: contact_init 成功（存储不存在时也返回成功）
    - test: contact_add 正常添加并可用 contact_find 查到
- Green:
    - 实现 contact_init/contact_add/contact_find 的最小逻辑（内存表 + 调用 storage_init/storage_write）
- Refactor:
    - 将持久化逻辑抽成 save/load 函数，错误码统一

迭代 2（持久化与恢复） — 高优先级

- Red:
    - test: 添加联系人后重启（重新调用 contact_init）仍能 find（持久化有效）
- Green:
    - 在 contact_init 中实现从 storage_read 恢复数据
- Refactor:
    - 支持部分写入检测（可选），并明确存储格式版本号

迭代 3（边界与错误处理） — 中优先级

- Red:
    - test: 添加超容量返回 -3（表满）
    - test: 当 storage_write 返回失败，contact_add 返回 -2 且不留下半写数据
- Green:
    - 实现容量检查与写入失败回滚
- Refactor:
    - 把错误码常量化/注释完善

迭代 4（删除与修改） — 中优先级

- Red:
    - test: 删除后 find 返回未找到
    - test: 更新后 find 返回新值
- Green:
    - 实现 contact_delete / contact_update
- Refactor:
    - 考虑使用 tombstone 标记或压缩持久化存储

迭代 5（列表/分页/排序） — 可选

- Red:
    - test: contact_list 返回正确数目和顺序
- Green:
    - 实现 list、可选择排序策略
- Refactor:
    - 考虑内存/性能优化

迭代 6（并发 / 线程安全） — 可选

- Red:
    - test: 在多线程中访问 contact_add/contact_find 不产生数据竞争（可用 ThreadSanitizer）
- Green:
    - 在业务层加锁或使用互斥机制
- Refactor:
    - 提供选项编译时开关（SINGLE_THREADED）

迭代 7（输入验证 / 安全） — 可选

- Red:
    - test: 非法输入（超长名字、NULL 指针）被正确拒绝
- Green:
    - 在 API 检查参数并返回 -1

迭代 8（CLI 与集成） — 最后

- 将所有接口暴露到 main.c 菜单，做手动验收测试

三、第一次迭代的 Red 步骤（具体：测试文件 + mock）

下面我把第 1 次 Red 的测试文件和 mock_storage.c 给出（Unity 风格）。把它们放到 test/ 下并通过 CMake 将 Unity 源包含进来（之前我给过 CMake 示例）。先写这些测试（Red），你运行它们应该看到失败（因为你可能还没有完全实现对应行为或实现不符合预期）。然后基于失败信息实现最少代码（Green）。

```c name=test/test_contact_red1.c
#include "unity.h"
#include "foo.h"
#include <string.h>

/* Unity required hooks */
void setUp(void) {}
void tearDown(void) {}

/* Test 1: 初始化应成功（在没有文件/存储时也能 init） */
void test_contact_init_returns_success(void) {
    int rc = contact_init();
    TEST_ASSERT_EQUAL_INT(0, rc);
}

/* Test 2: 添加并查询（基本路径） */
void test_contact_add_and_find_success(void) {
    contact_init();

    contact_t c;
    memset(&c, 0, sizeof(c));
    strncpy(c.name, "Alice", sizeof(c.name)-1);
    strncpy(c.phone, "10086", sizeof(c.phone)-1);

    int rc = contact_add(&c);
    TEST_ASSERT_EQUAL_INT(0, rc);

    contact_t out;
    rc = contact_find("Alice", &out);
    TEST_ASSERT_EQUAL_INT(0, rc);
    TEST_ASSERT_EQUAL_STRING("10086", out.phone);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_contact_init_returns_success);
    RUN_TEST(test_contact_add_and_find_success);
    return UNITY_END();
}
```

```c name=test/mock_storage.c
#include "storage.h"
#include <string.h>
#include <stdio.h>

/* Mock-controlled flags */
static int mock_write_fail = 0;

/* optional control API for tests */
void mock_storage_set_write_fail(int v) { mock_write_fail = v; }

int storage_init(void) {
    /* Pretend storage exists; no-op for tests */
    return STORAGE_OK;
}

int storage_read(uint32_t addr, void *buf, size_t len) {
    /* zero buffer to simulate empty storage */
    if (buf && len) memset(buf, 0, len);
    return STORAGE_OK;
}

int storage_write(uint32_t addr, const void *buf, size_t len) {
    if (mock_write_fail) return STORAGE_ERR;
    /* no-op: keep in-memory only */
    (void)addr; (void)buf; (void)len;
    return STORAGE_OK;
}
```

注意：

- mock*storage.c 中定义的 storage*\* 会在测试构建时被链接到测试可执行（替换真实 hal/storage.c）。如果你用 CMake，把 test/mock_storage.c 编译进 test 可执行即可。
- 我额外提供了 mock_storage_set_write_fail()，以便后续测试让写入失败。

如何运行（本地）：

1. 确保 CMakeLists.txt 包含 Unity FetchContent 并将 test 可执行链接 Unity 源（我之前给出示例 CMake）。
2. 在项目根：

```sh
mkdir -p build
cd build
cmake -DENABLE_SANITIZERS=ON ..
cmake --build .
ctest --output-on-failure        # 或直接 ./test/test_contact_red1
```

预期：测试会失败（如果你的 foo.c 还未实现或行为不一致）。这就进入 Red 阶段的目标：测试应该能捕获未实现或错误行为。

四、Green（基于上面 Red 的最小实现说明）
Green 阶段目标：在最小改动下通过上述两个测试。你已经有了一个最小实现示例（我之前给出的 src/foo.h / src/foo.c），它应能通过这两个测试（如果 test 可执行正确链接 mock_storage 并且没有残留真实 storage 实现冲突）。

Green 最小实现要点回顾：

- contact_init(): 清空内部数组并调用 storage_init()，忽略 storage_read 错误（最小实现）。
- contact_add(): 找到第一个空槽，拷贝数据并调用 storage_write()，若 storage_write 失败则回滚并返回错误。
- contact_find(): 精确匹配名字并返回结果。
- 错误码：0 成功，负数错误（-1 参数错误，-2 持久化失败，-3 表满，-4 未找到）。

五、Refactor 建议（在 Green 通过后进行）

- 把持久化的 load/save 提取为静态辅助函数（load_contacts(), save_contacts()）。
- 将错误码定义在头文件并注释清楚。
- 把 MAX_CONTACTS、STORAGE_BASE_ADDR 等宏放到配置头或 CMake 可配置选项。
- 考虑把业务层从直接调用全局 storage 改为依赖注入（storage_ops）以更灵活地 mock。

六、后续迭代（你接下来要做的步骤简表）

- 完成 Red-Write tests 的下一条：持久化恢复测试（迭代 2）
    - 写测试：add -> re-init (模拟读取已存在数据) -> find 成功
    - 运行（应失败）→ 修改 contact_init 中的 storage_read 恢复逻辑 → 运行直到通过
- 写边界测试（容量满、写失败回滚等）
- 实现 delete/update/list 的 Red→Green→Refactor 循环
- 把 mock 改成更完善的 mock（支持在内存保存数据），或采用依赖注入
- 添加 CI（GitHub Actions）运行 Unity 测试、静态分析、Sanitizer

我已经为你准备好了第 1 次 Red 所需的测试与 mock 文件（上面以文件块形式给出）。接下来你可以：

- 把这些文件放到 test/ 目录；确保 CMake 以 mock 文件编译到测试可执行（覆盖 hal/storage.c）
- 运行测试，观察失败信息（Red），然后把此前给你的最小实现（src/foo.h / src/foo.c）放入 src/（Green），重新运行测试应通过
- 通过后进行重构（Refactor），然后进入下一轮迭代

我现在已经完成：

- 列出项目大纲、函数与签名、完整迭代计划（按优先级）
- 给出第 1 次 Red 所需的 Unity 测试文件和 mock_storage.c（可复制使用）
- 指导如何运行并说明 Green 的最小实现要点与 Refactor 建议

下一步（你无需确认）请把测试文件和 mock 放到 repo 的 test/ 下并运行测试（ctest 或直接运行测试可执行）。把运行结果（失败的输出）贴过来，我会根据失败信息帮你定位并给出具体的 Green 实现修改建议，直到测试全部通过并完成整个项目大纲。
