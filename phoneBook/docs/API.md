# 电话本项目 API 文档

## 函数列表

### `void add_contact(teleDirectory* directory)`

- **功能**：向电话本添加一个联系人
- **参数**:
    - `directory`: 指向电话本结构体的指针
- **返回值**：无
- **示例**:

    ```c
    add_contact(&directory);
    ```

### void remove_contact(teleDirectory\* directory)

- **功能**：根据姓名删除联系人
- **参数**：
    - `directory`：指向电话本结构体的指针
- **返回值**：无

### void search_contact(teleDirectory\* directory)

- **功能**：查找联系人并显示其信息
- **参数**：
    - `directory`：指向电话本结构体的指针
- **返回值**：无

### void show_contact(teleDirectory\* directory)

- **功能**：显示所有联系人
- **参数**：
    - `directory`：指向电话本结构体的指针
- **返回值**：无
