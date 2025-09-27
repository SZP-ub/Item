#include <stdio.h>
#include "telephon.h"

int main(int argc, char* argv[]) {
    teleDirectory directory;
    memset(&directory, 0, sizeof(teleDirectory));

    FILE* fptr = fopen("Telephon.txt", "rb");
    if (fptr != NULL) {
        fread(&directory, sizeof(teleDirectory), 1, fptr);
        fclose(fptr);
        fptr = NULL;
    } else {
        printf("未找到电话本文件，将创建新电话本。\n");
    }


    while (1) {
        show_menu();

        printf("请输入您的操作：\n");
        int input = 0;
        get_input(&input);
        execute_function(input, &directory);
    }

    return 0;
}
