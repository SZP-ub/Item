#include "telephon.h"
#include <stdio.h>
#include <string.h>

void show_menu(void) {
    printf("--------------------------\n");
    printf("电话本系统v1.0\n");
    printf("--------------------------\n");
    printf("1、添加联系人\n");
    printf("2、删除联系人\n");
    printf("3、修改联系人\n");
    printf("4、查找联系人\n");
    printf("5、显示联系人\n");
    printf("6、退出电话本\n");
    printf("--------------------------\n");
}

void get_input(int* input) {
    while (1) {
        if (scanf("%d", input) == 1 && *input >= 1 && *input <= 6) {
            break;
        }
        char buf[128] = {0};
        fgets(buf, sizeof(buf), stdin);
        printf("输入无效，请重新输入\n");
    }
}

void execute_function(int input, teleDirectory* directory) {
    switch (input) {
        case ADD:

            printf("请添加联系人\n");
            add_contact(directory);
            show_contact(directory);
            break;
        case REMOVE:

            printf("请删除联系人\n");
            remove_contact(directory);
            show_contact(directory);
            break;
        case REVISE:

            printf("请修改联系人\n");
            revise_contact(directory);
            show_contact(directory);
            break;
        case SEARCH:

            printf("请查找联系人\n");
            search_contact(directory);
            show_contact(directory);
            break;
        case SHOW:

            printf("显示联系人\n");
            show_contact(directory);
            break;
        case EXIT:

            printf("欢迎再次使用该系统\n");
            exit_contact(directory);
            break;
        default:

            printf("您输入的操作暂不支持，请期待下一个版本\n");
            system("pause");
            break;
    }
}

void add_contact(teleDirectory* directory) {
    if (directory->cnt >= MAX_CONTACTS) {
        printf("电话本已满，无法添加新联系人\n");
        return;
    }
    printf("请输入联系人姓名：");
    char name[16] = {0};
    scanf("%15s", name);
    printf("请输入联系人电话：");
    char telephon[16] = {0};
    scanf(" %s", telephon);
    strcpy(directory->contacts[directory->cnt].name, name);
    strcpy(directory->contacts[directory->cnt].telephon, telephon);
    directory->cnt++;
}

void show_contact(teleDirectory* directory) {
    printf("--------------------------\n");
    printf("姓名\t电话\n");
    printf("--------------------------\n");
    for (int i = 0; i < directory->cnt; ++i) {
        printf("%s\t%s\n", directory->contacts[i].name, directory->contacts[i].telephon);
    }
}

void revise_contact(teleDirectory* directory) {
    printf("请输入要修改的联系人姓名：");
    char name[16] = {0};
    scanf("%15s", name);
    printf("请输入要修改的联系人电话：");
    char telephon[16] = {0};
    scanf("%s", telephon);
    int flag = -1;
    for (int i = 0; i < directory->cnt; ++i) {
        if (strcmp(directory->contacts[i].name, name) == 0) {
            printf("请输入新的联系人电话：");
            char telephon[16] = {0};
            scanf("%15s", telephon);
            strcpy(directory->contacts[i].telephon, telephon);
            flag = 0;
            printf("联系人已修改\n");
            break;
        }
    }
    if (flag == -1) {
        printf("请输入正确的联系人\n");
    }
}


void remove_contact(teleDirectory* directory) {
    printf("请输入要删除联系人的姓名：");
    char name[16] = {0};
    scanf("%15s", name);
    int flag = -1;
    for (int i = 0; i < directory->cnt; ++i) {
        if (strcmp(directory->contacts[i].name, name) == 0) {
            for (int j = i; j < directory->cnt - 1; ++j) {
                directory->contacts[j] = directory->contacts[j + 1];
            }
            directory->cnt--;
            flag = 0;
            printf("联系人已删除。\n");
            break;
        }
    }
    if (flag == -1) {
        printf("请输入正确的联系人\n");
    }
}

void search_contact(teleDirectory* directory) {
    printf("请输入要查找联系人的姓名：");
    char name[16] = {0};
    scanf("%15s", name);
    int flag = -1;
    for (int i = 0; i < directory->cnt; ++i) {
        if (strcmp(directory->contacts[i].name, name) == 0) {
            printf("所要查找联系人电话为：%s\n", directory->contacts[i].telephon);
            flag = 0;
            break;
        }
    }
    if (flag == -1) {
        printf("请输入正确的联系人\n");
    }
}

void exit_contact(teleDirectory* directory) {
    FILE* fptr = fopen("Telephon.txt", "wb");
    if (fptr == NULL) {
        perror("fptr");
        return;
    }
    fwrite(directory, sizeof(teleDirectory), 1, fptr);
    fclose(fptr);
    fptr = NULL;
    exit(0);
}
