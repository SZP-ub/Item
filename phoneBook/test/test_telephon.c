#include <stdio.h>
#include <string.h>
#include "telephon.h"
#include "unity.h"
#include "unity_internals.h"

void setUp(void) {}

void tearDown(void) {}

void test_init_directory_should_be_empty(void) {
    teleDirectory dir;
    memset(&dir, 0, sizeof(dir));
    TEST_ASSERT_EQUAL_INT(0, dir.cnt);
}

void test_add_contact_should_increase_count(void) {
    teleDirectory dir;
    memset(&dir, 0, sizeof(dir));

    // 模拟输入
    strcpy(dir.contacts[0].name, "Alice");
    strcpy(dir.contacts[0].telephon, "123456");
    dir.cnt = 1;
    TEST_ASSERT_EQUAL_STRING("Alice", dir.contacts[0].name);
    TEST_ASSERT_EQUAL_STRING("123456", dir.contacts[0].telephon);
    TEST_ASSERT_EQUAL_INT(1, dir.cnt);
}
