/*
    数据展示：字符界面
    业务功能：c语言语法，数据类型
    数据存储：文件

        一个项目的核心是数据->数据的结构->电话本：
            联系人的信息
            联系人的数组
            联系人的数量

        具体实现：利用结构体及其数组
            联系人信息->结构体
            联系人数量->结构体数组

        分析业务流程：
            显示操作菜单
            获得用户输入的操作
            根据用户输入做出相应的操作
*/


#ifndef TELEPHON_H_H
#define TELEPHON_H_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CONTACTS 100

typedef enum { ADD = 1, REMOVE, REVISE, SEARCH, SHOW, EXIT } Status;

typedef struct Contact {
    char name[16];
    char telephon[16];
} Contact;

typedef struct teleDirectory {
    Contact contacts[MAX_CONTACTS];
    int     cnt;
} teleDirectory;

void show_menu(void);
void get_input(int* input);
void execute_function(int input, teleDirectory* directory);
void add_contact(teleDirectory* directory);
void remove_contact(teleDirectory* directory);
void revise_contact(teleDirectory* directory);
void search_contact(teleDirectory* directory);
void show_contact(teleDirectory* directory);
void exit_contact(teleDirectory* directory);

#endif /* end of include guard: TELEPHON_H_H */
