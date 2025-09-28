#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "telephon.h"
#include "unity.h"
#include "unity_internals.h"

int  add_called, remove_called, revise_called, search_called, show_called, exit_called;
char last_output[256];

void add_contact(teleDirectory* d) {
    add_called = 1;
}

void revise_contact(teleDirectory* d) {
    remove_called = 1;
}

void remove_contact(teleDirectory* d) {
    remove_called = 1;
}

void search_contact(teleDirectory* d) {
    search_called = 1;
}

void show_contact(teleDirectory* d) {
    show_called = 1;
}

void exit_contact(teleDirectory* d) {
    exit_called = 1;
}

int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(last_output, sizeof(last_output), fmt, args);
    va_end(args);
    return strlen(last_output);
}

void setUp(void) {
    add_called = remove_called = revise_called = search_called = show_called = exit_called = 0;
    memset(last_output, 0, sizeof(last_output));
}

void test_execute_add(void) {
    teleDirectory dir = {0};
    execute_function(ADD, &dir);
    TEST_ASSERT_TRUE(add_called);
    TEST_ASSERT_TRUE(show_called);
    TEST_ASSERT_EQUAL_STRING("请添加联系人\n", last_output);
}

void test_execute_remove(void) {
    teleDirectory dir = {0};
    execute_function(REMOVE, &dir);
    TEST_ASSERT_TRUE(remove_called);
    TEST_ASSERT_TRUE(show_called);
    TEST_ASSERT_EQUAL_STRING("请删除联系人\n", last_output);
}

void test_execute_default(void) {
    teleDirectory dir = {0};
    execute_function(99, &dir);
    TEST_ASSERT_EQUAL_STRING("您输入的操作暂不支持，请期待下一个版本\n", last_output);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_execute_add);
    RUN_TEST(test_execute_remove);
    RUN_TEST(test_execute_default);
    return UNITY_END();
}
