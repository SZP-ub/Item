# 电话本项目开发文档

- 时间：25-09-27

## 项目结构2

- main.c //程序入口，主循环
- telephon.h // 结构体定义、函数声明 -
- telephon.c //主要功能实现

## 主要数据结构

```c
typedef struct Contact {
    char name[16];
    char telephon[16];
}Contact;
```

## 编译运行

```sh
gcc -g main.c telephon.c -o main.out
./main.out
```

## 主要流程

1. 程序启动，尝试读取本地文件加载联系人数据
2. 显示菜单，等待用户输入操作
3. 根据用户选择执行添加、删除、修改、查找、显示等功能
4. 退出时保存数据到文件

## 主要函数说明

- `void show_menu(void);`:显示主菜单
- `void get_input(int* input);`:获取用户输入
- `void execute_function(int input, teleDirectory* directory);`:根据输入执行操作
- `void add_contact(teleDirectory* directory);`:添加联系人
- `void remove_contact(teleDirectory* directory);`:删除联系人
- `void revise_contact(teleDirectory* directory);`:修改联系人
- `void search_contact(teleDirectory* directory);`:查找联系人
- `void show_contact(teleDirectory* directory);`:显示所有联系人
- `void exit_contact(teleDirectory* directory);`:保存并退出
