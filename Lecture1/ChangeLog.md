# 更改

## 格式调整

1. 将缩进从原来的2个空格调整至8个空格；
2. 将函数调用后的空格去除；
3. if, while等控制条件间增加了空格；
4. if else语句统一加了大括号，无论是否只有一条语句；
5. 调整了函数声明的格式，使其符合现代C语言的习惯；
6. 调整了指针变量的格式，统一让所有*与变量名贴合在一起；
7. 双目运算符，如+、-、*等，运算符与操作数中间增加空格；
8. typedef struct _export_list后增加空格，并调整了其成员的对齐方式；

## 代码更改

1. const char* usage前增加了static关键字；

## TODO

1. 将error函数拆开，这个函数做了两件事：打印错误信息，释放资源
2. 去除文件中的全局变量